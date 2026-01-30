/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2019 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <ctype.h>
#include <curl/curl.h>
#include "Device_DeviceInfo.h"
#include "XrdkCentralComBSStore.h"
#include "hostIf_utils.h"
#include "rfcapi.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <libgen.h>       // For dirname()
#define BS_STORE_KEY "BS_STORE_FILENAME"
#define BS_JOURNAL_KEY "BS_JOURNAL_FILENAME"
#define RFC_PROPERTIES_FILE "/etc/rfc.properties"
#define BS_JSON_FILE "/etc/partners_defaults.json"
#define BS_JSON_DEVICE_FILE "/etc/partners_defaults_device.json"
#define TR181_PARTNER_ID_KEY "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId"
#define BS_CLEAR_DB_START "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDB"
#define BS_CLEAR_DB_END "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDBEnd"
#define RFC_SERVICE_LOCK "/tmp/.rfcServiceLock"
#define BSP_COMPLETE "/opt/bspcomplete.ini"
#define BSP_COMPLETE_TMP "/tmp/bspcomplete"
#define RFC_DIRECTORY "/opt/RFC"
#define AUTH_SERVICE_PARODUS_RESTART "/tmp/authservice_parodus_restart"
#define MAX_FILENAME_LENGTH 256

#define CURL_EASY_SETOPT(CURL , CURLoption , Value)\
    if (curl_easy_setopt(CURL , CURLoption , Value) != CURLE_OK ) {\
                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: curl_easy_setopt setopt failed ...\n", __FUNCTION__);\
        }
/*CID:121745 - checked return */


XBSStore* XBSStore::xbsInstance = NULL;
XBSStoreJournal* XBSStore::xbsJournalInstance = NULL;
recursive_mutex XBSStore::mtx;
thread XBSStore::partnerIdThread;
bool XBSStore::m_stopped = false;
mutex XBSStore::mtx_stopped;
condition_variable XBSStore::cv;
mutex XBSStore::g_instance_mutex;

// Function to check if a directory or file exists
bool static fileExists(const std::string &path) {
    struct stat info;
    return (stat(path.c_str(), &info) == 0);
}

// Function to extract the parent directory
std::string static getParentDirectory(const std::string& path) {
    char pathCopy[PATH_MAX];
    strncpy(pathCopy, path.c_str(), sizeof(pathCopy));
    pathCopy[sizeof(pathCopy) - 1] = '\0';
    return std::string(dirname(pathCopy));
}

// Function to create a new file
void createFile(const char* path) {
    std::ofstream ofs(path);  // This creates the file if it doesn't exist, or truncates if it does
    if (ofs) {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"File created at: %s\n", path);
    } else {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF,"Error creating file at: %s\n", path);
    }
}

// Function to create a directory
bool createDirectory(const char* path) {
   // 0755 permissions: rwxr-xr-x (user: read/write/execute, group: read/execute, others: read/execute)
   if (mkdir(path, 0755) == 0) {
       RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF,"Directory created: %s\n", path);
       return true;
   } else {
       RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF,"Error creating directory\n");
       return false;
   }
}

bool createBspCompleteFiles()
{
    // Check if the RFC directory exists, if not, create it
    if (!fileExists(RFC_DIRECTORY)) {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF,"Directory %s does not exist. Creating it...\n", RFC_DIRECTORY);
        if (!createDirectory(RFC_DIRECTORY)) {
            return false;  // Exit if we fail to create the directory
        }
    } else {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF,"Directory %s already exists.\n", RFC_DIRECTORY);
    }

    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"Creating BSP Complete File %s\n", BSP_COMPLETE);
    createFile(BSP_COMPLETE);
    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"Creating BSP Complete File %s in tmp Directory\n", BSP_COMPLETE_TMP);
    createFile(BSP_COMPLETE_TMP);
    return true;
}

void XBSStore::getAuthServicePartnerID()
{
    const std::string partnerIdPath = "/opt/www/authService/partnerId3.dat";

    // Initialize inotify
    int inotifyFd = inotify_init();
    if (inotifyFd == -1) {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s Failed to initialize inotify: %s\n", __FUNCTION__, strerror(errno));
        return;
    }

    // Extracting the parent directories dynamically
    std::string filePath(partnerIdPath);
    std::string authServiceDir = getParentDirectory(filePath);  // "/opt/www/authService"
    std::string wwwDir = getParentDirectory(authServiceDir);    // "/opt/www"
    std::string parentDir = getParentDirectory(wwwDir);         // "/opt"
    std::string targetFile = filePath.substr(filePath.find_last_of('/') + 1);  // "partnerId3.dat"

    // Add a watch on /opt for directory creation
    int wd = inotify_add_watch(inotifyFd, parentDir.c_str(), IN_CREATE);
    if (wd == -1) {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s Failed to add watch on %s: %s\n", __FUNCTION__, parentDir.c_str(), strerror(errno));
        close(inotifyFd);
        return;
    }

    const size_t eventBufSize = sizeof(struct inotify_event) + NAME_MAX + 1;
    char eventBuf[eventBufSize];

    bool foundWWW = fileExists(wwwDir);
    bool foundAuthService = fileExists(authServiceDir);
    bool partnerIdWatchAdded = false;  // Track if we already added the partnerId3.dat watch

    // Check if partnerId3.dat already exists before starting
    if (foundWWW && foundAuthService && fileExists(filePath)) {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s File %s already exists. Monitoring for modifications...\n", __FUNCTION__, targetFile.c_str());
        wd = inotify_add_watch(inotifyFd, filePath.c_str(), IN_CLOSE_WRITE);
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Watch descriptor (wd) value: %d\n", wd); 
        partnerIdWatchAdded = true;

        // Check if the BSP_COMPLETE file exists
        if (!fileExists(BSP_COMPLETE)) {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"%s File %s does not exist...creating it...\n", __FUNCTION__, BSP_COMPLETE);
            createBspCompleteFiles();
        }
    }

    // Check for existing directories at the start
    if (foundWWW && !partnerIdWatchAdded) {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s Directory %s already exists.\n", __FUNCTION__, wwwDir.c_str());

        if (foundAuthService) {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Directory %s already exists.\n", authServiceDir.c_str());
            wd = inotify_add_watch(inotifyFd, authServiceDir.c_str(), IN_CREATE | IN_CLOSE_WRITE);
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Watch descriptor (wd) value: %d\n", wd); 
            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Now monitoring %s for partnerId3.dat creation and modifications...\n", authServiceDir.c_str());
        } else {
            // Add a watch on /opt/www to monitor for authService creation
            wd = inotify_add_watch(inotifyFd, wwwDir.c_str(), IN_CREATE);
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Watch descriptor (wd) value: %d\n", wd); 
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Now monitoring %s for authService directory creation...\n", wwwDir.c_str());
        }
    }

    while (true) {
        ssize_t numRead = read(inotifyFd, eventBuf, eventBufSize);
        if (numRead == -1) {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to read events: %s\n", strerror(errno));
            break;
        }

        bool partnerFileUpdated = false;
        for (char *ptr = eventBuf; ptr < eventBuf + numRead;) {
            struct inotify_event *event = (struct inotify_event *)ptr;

            // If www is created
             if (!foundWWW && (event->mask & IN_CREATE) && (event->mask & IN_ISDIR) ) {
		char nameBuf[NAME_MAX +1] = {0};
		strncpy(nameBuf, event->name, NAME_MAX);
                if (strcmp(nameBuf, "www") == 0)
		{
                   foundWWW = true;
                   RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Directory %s created!\n", wwwDir.c_str());

                   // Immediately check if authService already exists
                   foundAuthService = fileExists(authServiceDir);
                   if (foundAuthService) {
                       RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Directory %s already exists.\n", authServiceDir.c_str());
                       wd = inotify_add_watch(inotifyFd, authServiceDir.c_str(), IN_CREATE | IN_CLOSE_WRITE);
                       RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Watch descriptor (wd) value: %d\n", wd); // Log the value of wd
                       RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Now monitoring %s for partnerId3.dat creation and modifications...\n", authServiceDir.c_str());
                    } else {
                    // Add a new watch for /opt/www/authService creation
                        wd = inotify_add_watch(inotifyFd, wwwDir.c_str(), IN_CREATE);
                        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Watch descriptor (wd) value: %d\n", wd); // Log the value of wd
                        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Now monitoring %s for authService directory creation...\n", wwwDir.c_str());
                   }
                }
	     }

            // If authService is created
            else if (foundWWW && !foundAuthService && (event->mask & IN_CREATE) && (event->mask & IN_ISDIR)) {
                char nameBuf[NAME_MAX +1];
		strncpy(nameBuf, event->name, NAME_MAX);
		nameBuf[NAME_MAX] ='\0';
		if (strcmp(nameBuf, "authService") == 0) {
		foundAuthService = true;
                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Directory %s created!\n", authServiceDir.c_str());

		// Add a new watch for partnerId3.dat
                wd = inotify_add_watch(inotifyFd, authServiceDir.c_str(), IN_CREATE | IN_CLOSE_WRITE);
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Watch descriptor (wd) value: %d\n", wd); // Log the value of wd
                RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Now monitoring %s for partnerId3.dat creation and modifications...\n", authServiceDir.c_str());
		}
            }

            // If partnerId3.dat is created
            else if (foundAuthService && !partnerIdWatchAdded && (event->mask & IN_CREATE)) {
    // Create a null-terminated copy of event->name
              char safeEventName[NAME_MAX + 1];
              strncpy(safeEventName, event->name, NAME_MAX);
              safeEventName[NAME_MAX] = '\0'; // Ensure null termination
    
              if (strcmp(safeEventName, targetFile.c_str()) == 0) {
                  RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s File %s created!\n", __FUNCTION__, safeEventName);
                  partnerIdWatchAdded = true;
                  partnerFileUpdated = true;

                   // Monitor the file for close after writing
                 RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Now monitoring %s for modifications...\n", filePath.c_str());
                 break;
              }
            }

            // If partnerId3.dat is modified
            else if (foundAuthService && partnerIdWatchAdded && (event->mask & IN_CLOSE_WRITE)) {
                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s File %s is modified!\n", __FUNCTION__, targetFile.c_str());
                partnerFileUpdated = true;
                break;
            }

            ptr += sizeof(struct inotify_event) + event->len;
        }
        if (partnerFileUpdated == true) {
            string newPartnerId = "";
            int ret = hostIf_DeviceInfo::get_PartnerId_From_Script(newPartnerId);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"%s PartnerID = %s get_PartnerId_From_Script returned %d \n", __FUNCTION__,newPartnerId.c_str(), ret);
            if(ret == OK && newPartnerId.length() > 0)
            {
                string storedPartnerId = xbsInstance->getRawValue(TR181_PARTNER_ID_KEY);
                if (newPartnerId.compare(storedPartnerId) != 0)
                {
                    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"%s partnerId has changed\n", __FUNCTION__);
                    mtx.lock();
                    xbsInstance->setRawValue(TR181_PARTNER_ID_KEY, newPartnerId.c_str(), HOSTIF_SRC_DEFAULT);
                    xbsInstance->loadFromJson();
                    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] Unlocking mutex...  \n", __FUNCTION__, __LINE__);
                    mtx.unlock();

                    // if BSP_COMPLETE exist it means there is a change in partnerID. Create /tmp/authservice_parodus_restart to restart parodus.service
                    if (fileExists(BSP_COMPLETE)) {
                        // Create a new file "/tmp/authservice_parodus_restart"
                        createFile(AUTH_SERVICE_PARODUS_RESTART);
                        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"BSP_COMPLETE exists. Created /tmp/authservice_parodus_restart.\n");
                    } 
                }

		RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"Check and create BSP_COMPLETE for parodus.service to start.\n");
                if (!fileExists(BSP_COMPLETE)) {
                    // If BSP_COMPLETE doesn't exist, create it so parodus.service can start.
                    createBspCompleteFiles();
                    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"BSP_COMPLETE did not exist. Created BSP_COMPLETE.\n");
		}		
            }
            else
            {
                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: partnerId not found. Continue monitoring for modifications...\n", __FUNCTION__);
            }
        }
    }

    close(inotifyFd);
}

void XBSStore::resetCacheAndStore()
{
   m_dict.clear();
   if (!m_filename.empty())
   {
        if(remove(m_filename.c_str()) != 0)
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Error occured ,File is not removed.\n", __FUNCTION__);
        }   //CID:80489 - Checked Return
   }
   xbsJournalInstance->resetCacheAndStore();

}

// Get device specific bootstrap defaults and give precedence to these values over generic bootstrap defaults.
bool XBSStore::getPartnerDeviceConfig(cJSON* partnerConfig, const string & partnerId)
{
    ifstream ifs_json_device(BS_JSON_DEVICE_FILE);
    if(!ifs_json_device.is_open())
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Device specific file [%s] does not exist\n", __FUNCTION__, BS_JSON_DEVICE_FILE);
        return true;
    }

    ifs_json_device.seekg(0, std::ios::end);
    size_t size = ifs_json_device.tellg();
    string buffer_device(size, ' ');
    ifs_json_device.seekg(0);
    ifs_json_device.read(&buffer_device[0], size);
    const char *jsonTextDevice = buffer_device.c_str();

    cJSON *jsonDevice = cJSON_Parse(jsonTextDevice);
    if (!jsonDevice)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: json parse error for device file \n", __FUNCTION__);
        return false;
    }

    cJSON* partnerDeviceConfig = cJSON_GetObjectItem(jsonDevice, partnerId.c_str());
    if (!partnerDeviceConfig)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"%s: Unknown partner:%s, using default.\n", __FUNCTION__, partnerId.c_str() );
        partnerDeviceConfig = cJSON_GetObjectItem(jsonDevice, "default" );

        if (!partnerDeviceConfig)
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"%s: Error for partner:%s, error = [%s]\n", __FUNCTION__, partnerId.c_str(), cJSON_GetErrorPtr());
            return true;
        }
    }

    cJSON *configDeviceObject = partnerDeviceConfig->child;
    while( configDeviceObject )
    {
        char *configDeviceKey = configDeviceObject->string;
        char *configDeviceValue = configDeviceObject->valuestring;

        if (cJSON_GetObjectItem(partnerConfig, configDeviceKey)) {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"%s: key %s exist in generic json...replace entry\n", __FUNCTION__, configDeviceKey );
            cJSON_ReplaceItemInObject(partnerConfig, configDeviceKey, cJSON_CreateString(configDeviceValue));
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"%s: key %s does NOT exist generic json...add new entry\n", __FUNCTION__, configDeviceKey );
            cJSON_AddItemToObject(partnerConfig, configDeviceKey, cJSON_CreateString(configDeviceValue));
        }
        configDeviceObject = configDeviceObject->next;
    }
    return true;
}

bool XBSStore::loadFromJson()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    string partnerId = "";
    hostIf_DeviceInfo::get_PartnerId_From_Script(partnerId);

    if (partnerId.length() == 0)
    {
#if defined(USE_BUILTIN_PARTNER_ID)
#define xstr(s) lstr(s)
#define lstr(s) #s
        std::string l_partnerId = xstr(BUILTIN_PARTNER_ID);
        if ( !l_partnerId.empty() && l_partnerId.compare("BUILTIN_PARTNER_ID")  ) {
            partnerId = l_partnerId;
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Setting Built-in Partner ID : %s \n", __FUNCTION__, partnerId.c_str());
            setRawValue(TR181_PARTNER_ID_KEY, partnerId.c_str(), HOSTIF_SRC_DEFAULT);
        }
        else {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Using default_boot configuration as Built-in PartnerID is not available at boot.\n", __FUNCTION__);
            partnerId = "default_boot";
        }
#else
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Using default_boot configuration as PartnerID is not available at boot.\n", __FUNCTION__);
        partnerId = "default_boot";
#endif
    }

    ifstream ifs_json(BS_JSON_FILE);
    ifs_json.seekg(0, std::ios::end);
    size_t size = ifs_json.tellg();
    string buffer(size, ' ');
    ifs_json.seekg(0);
    ifs_json.read(&buffer[0], size);
    const char *jsonText = buffer.c_str();

    cJSON *json = cJSON_Parse(jsonText);
    if (!json)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: json parse error \n", __FUNCTION__);
        return false;
    }

    cJSON* partnerConfig = cJSON_GetObjectItem(json, partnerId.c_str());
    if (!partnerConfig)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"Unknown partner:%s, using default.\n", partnerId.c_str() );
        partnerConfig = cJSON_GetObjectItem(json, "default" );

        if (!partnerConfig)
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"Error for partner:%s, error = [%s]\n", partnerId.c_str(), cJSON_GetErrorPtr());
            return false;
        }
    }
    if(partnerConfig->type == cJSON_Object) {
        if(!getPartnerDeviceConfig(partnerConfig, partnerId))
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: error reading partners_defaults_device.json \n", __FUNCTION__);
            return false;
        }

        ifstream ifs_bsini(m_filename);
        if (ifs_bsini.is_open()) {
            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: File [%s] exist. Look for any default parameter changes.\n", __FUNCTION__, m_filename.c_str());
            ifs_bsini.close();
        }
        else
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Initial Update of Bootstrap stores\n", __FUNCTION__);
            m_initialUpdate = true;
            xbsJournalInstance->setInitialUpdate(true);
        }

        cJSON *configObject = partnerConfig->child;

        //Check if any params were removed in firmware update. Do not remove params that were set by RFC/WebPA.
        bool removedEntries = false;
        for (unordered_map<string, string>::iterator it=m_dict.begin(); it!=m_dict.end();)
        {
            string key = it->first;
            if (!cJSON_GetObjectItem(partnerConfig, key.c_str()) && key.compare(TR181_PARTNER_ID_KEY) != 0 && xbsJournalInstance->getJournalSource(key) == HOSTIF_SRC_DEFAULT)
            {
                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Remove param %s from bootstrap store, size=%d\n", __FUNCTION__, key.c_str(), m_dict.size());
                removedEntries = true;
                it = m_dict.erase(it);
                xbsJournalInstance->removeRecord(key);
            }
            else
            {
                it++;
            }
        }
        if (removedEntries)
        {
            ofstream ofs(m_filename, ios::trunc | ios::out);
            if(!ofs.is_open())
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : %s \n", m_filename.c_str());
                return false;
            }
            for (unordered_map<string, string>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
            {
                ofs << it->first << '=' << it->second << endl;
            }
            ofs.flush();
            ofs.close();
            xbsJournalInstance->rfcUpdateEnd();
        }

        if (partnerId == "default_boot")
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: List of default_boot parameters configured: \n", __FUNCTION__);
        }
        while( configObject )
        {
            char *configKey = configObject->string;
            char *configValue = configObject->valuestring;
            if (partnerId == "default_boot")
            {
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"default_boot config: key=%s value=%s\n",configKey, configValue);
            }
            else
            {
                RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"bootstrap json parser: key=%s value=%s\n",configKey, configValue);
            }
            setRawValue(configKey, configValue, HOSTIF_SRC_DEFAULT);
            configObject = configObject->next;
        }
        m_initialUpdate = false;
        xbsJournalInstance->setInitialUpdate(false);
    }
    else
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"partnerConfig->type not of object type...");
    cJSON_Delete(json);

    if (partnerId != "default_boot")
    {
        hostIf_DeviceInfo::sendDeviceMgtNotification("firmware", "initial");
    }
    return true;
}

string XBSStore::getRawValue(const string &key)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    mtx.lock();
    unordered_map<string,string>::const_iterator it = m_dict.find(key);
    if (it == m_dict.end()) {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%d] Unlocking mutex...  \n", __FUNCTION__, __LINE__);
        mtx.unlock();
        return "";
    }
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%d] Unlocking mutex...  \n", __FUNCTION__, __LINE__);
    mtx.unlock();
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s : Value = %s \n", __FUNCTION__, it->second.c_str());

    return it->second;
}

bool XBSStore::setRawValue(const string &key, const string &value, HostIf_Source_Type_t sourceType)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, " %s: m_initialUpdate=%d\n", __FUNCTION__, m_initialUpdate);
    std::string outputDir="/opt/secure/RFC";
    if(mkdir(outputDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0)  //creating RFC directory if doesn't exist
    {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, " %s: File already exists\n", __FUNCTION__);
    }
    if(m_initialUpdate)
    {
        ofstream ofs(m_filename, ios::out | ios::app);

        if(!ofs.is_open())
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : %s \n", m_filename.c_str());
            return false;
        }
        m_dict[key] = value;
        ofs << key << '=' << value << endl;
        ofs.flush();
        ofs.close();
    }
    else
    {
        const string &currentValue = getRawValue(key);
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Given Value : %s ---- Current value :  %s \n", value.c_str(), currentValue.c_str());

        if(strlen(currentValue.c_str()) > 0)
        {
            if(!strcmp(currentValue.c_str(), value.c_str()))
            {
                RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Property value exists, don't have to overwrite\n");
                if(sourceType != xbsJournalInstance->getJournalSource(key))
                    xbsJournalInstance->setJournalValue(key, value, sourceType); //Update the journal with the latest source so that the value will not be overriden by less favored source later.
                return true;
            }
        }

        //Keep an updated firmware value in the journal even though it is not active configuration.
        if(sourceType == HOSTIF_SRC_DEFAULT &&
                (xbsJournalInstance->getJournalSource(key) == HOSTIF_SRC_RFC || xbsJournalInstance->getJournalSource(key) == HOSTIF_SRC_WEBPA) )
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Update firmware value in journal even though it is not active..\n");
            xbsJournalInstance->setJournalValue(key, value, sourceType);
            return true;
        }
        ofstream ofs(m_filename, ios::trunc | ios::out);
        if(!ofs.is_open())
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : %s \n", m_filename.c_str());
            return false;
        }
        m_dict[key] = value;

        for (unordered_map<string, string>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
        {
            ofs << it->first << '=' << it->second << endl;
        }
        ofs.flush();
        ofs.close();
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Bootstrap config update: %s, %s, %s, %d-%s \n", key.c_str(), value.c_str(), currentValue.c_str(), sourceType, xbsJournalInstance->getUpdatedSourceString(sourceType).c_str());
    }
    xbsJournalInstance->setJournalValue(key, value, sourceType);

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return true;
}

faultCode_t XBSStore::getValue(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(!init())
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "Init not complete...value may not be for latest partner ID\n");
    }
    string rawValue = getRawValue(stMsgData->paramName);
    if(rawValue.length() > 0)
    {
        putValue(stMsgData, rawValue.c_str());
        stMsgData->faultCode = fcNoFault;
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "%s : Parameter Not Found in %s\n", stMsgData->paramName, m_filename.c_str());
        stMsgData->faultCode = fcInternalError;
    }
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s\n", __FUNCTION__);
    return stMsgData->faultCode;
}

bool XBSStore::clearRfcValues()
{
    ofstream ofs(m_filename, ios::trunc | ios::out);
    if(!ofs.is_open())
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : %s \n", m_filename.c_str());
        return false;
    }

    for (unordered_map<string, string>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
    {
        string key = it->first;
        string defaultValue = "";
        if(xbsJournalInstance->clearRfcAndGetDefaultValue(key, defaultValue))
        {
            RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "%s: key=%s, defaultValue=%s\n", __FUNCTION__, key.c_str(), defaultValue.c_str());
            m_dict[key] = std::move(defaultValue);
        }
    }

    for (unordered_map<string, string>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
    {
        ofs << it->first << '=' << it->second << endl;
    }
    ofs.flush();
    ofs.close();
    return true;
}

faultCode_t  XBSStore::overrideValue(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(!init())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Init Failed, can't handle the request\n");
        return fcInternalError;
    }

    if (strcasecmp(stMsgData->paramName,BS_CLEAR_DB_START) == 0)
    {
        m_rfcUpdateInProgress = true;
        xbsJournalInstance->rfcUpdateStarted();
        stMsgData->faultCode = fcNoFault;
        return stMsgData->faultCode;
    }
    else if (strcasecmp(stMsgData->paramName,BS_CLEAR_DB_END) == 0)
    {
        clearRfcValues();
        xbsJournalInstance->rfcUpdateEnd();
        m_rfcUpdateInProgress = false;
        stMsgData->faultCode = fcNoFault;
        return stMsgData->faultCode;
    }

    mtx.lock();
    if (m_dict.find(stMsgData->paramName) == m_dict.end())
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "Param does not exist in bootstrap store. Allowing override.\n");
    }

    const string &givenValue = getStringValue(stMsgData);

    if (m_rfcUpdateInProgress)
        xbsJournalInstance->resetClearRfc(stMsgData->paramName);

    if(setRawValue(stMsgData->paramName, givenValue, stMsgData->requestor))
    {
        stMsgData->faultCode = fcNoFault;
    }
    else
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Unable to Set Value for given Param\n");
        stMsgData->faultCode = fcInternalError;
    }
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%d] Unlocking mutex...  \n", __FUNCTION__, __LINE__);
    mtx.unlock();
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return stMsgData->faultCode;
}

void XBSStore::initBSPropertiesFileName()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    ifstream ifs_rfc(RFC_PROPERTIES_FILE);
    if(!ifs_rfc.is_open())
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Trying to open a non-existent file [%s] \n", __FUNCTION__, RFC_PROPERTIES_FILE);
    }
    else
    {
        string line;
        while (getline(ifs_rfc, line)) {
            size_t splitterPos = line.find('=');
            if (splitterPos < line.length()) {
                string key = line.substr(0, splitterPos);
                string value = line.substr(splitterPos+1, line.length());
                if(!strcmp(key.c_str(), BS_STORE_KEY))
                {
                    m_filename = std::move(value);
                    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Bootstrap Properties FileName = %s\n", m_filename.c_str());
                }
                else if(!strcmp(key.c_str(), BS_JOURNAL_KEY))
                {
                    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Bootstrap Journal FileName = %s\n", value.c_str());
                    xbsJournalInstance = XBSStoreJournal::getInstance(value.c_str());
                }
            }
        }
        ifs_rfc.close();

        if(m_filename.empty())
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Didn't find %s in %s\n", BS_STORE_KEY, RFC_PROPERTIES_FILE);
        }
    }
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

bool XBSStore::loadBSPropertiesIntoCache()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(m_filename.empty())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Invalid Bootstrap Properties filename, Unable to load properties\n");
        return false;
    }
    // get rid of quotes, it is quite common with properties files
    m_filename.erase(remove(m_filename.begin(), m_filename.end(), '\"'), m_filename.end());
    m_dict.clear();
    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Bootstrap Properties File :  %s \n", m_filename.c_str());

    ifstream ifs_bsini(m_filename);
    if (ifs_bsini.is_open())
    {
        string line;
        while (getline(ifs_bsini, line)) {
            size_t splitterPos = line.find('=');
            if (splitterPos < line.length()) {
                string key = line.substr(0, splitterPos);
                string value = line.substr(splitterPos+1, line.length());
                m_dict[key] = std::move(value);
            }
        }
        ifs_bsini.close();
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: bootstrap ini does not exist...\n", __FUNCTION__);
    }

    if (!loadFromJson())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: loadFromJson() failed \n", __FUNCTION__);
        return false;
    }

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return true;
}

bool XBSStore::init()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    mtx.lock();
    if(m_initDone)
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%d] Unlocking mutex...  \n", __FUNCTION__, __LINE__);
        mtx.unlock();
        return m_initDone;
    }
    initBSPropertiesFileName();

    m_initDone = loadBSPropertiesIntoCache();

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%d] Unlocking mutex...  \n", __FUNCTION__, __LINE__);
    mtx.unlock();
    return m_initDone;
}

XBSStore::XBSStore()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    m_initDone = false;
    m_initialUpdate = false;
    m_rfcUpdateInProgress = false;

    init();

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

XBSStore* XBSStore::getInstance()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    std::lock_guard<std::mutex> guard(g_instance_mutex);
    if(!xbsInstance)
    {
        xbsInstance = new XBSStore;
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Start thread getAuthServicePartnerID \n", __FUNCTION__);
        partnerIdThread = thread(getAuthServicePartnerID);
        partnerIdThread.detach();
    }

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return xbsInstance;
}

void XBSStore::stop()
{
    m_stopped = true;
    cv.notify_one();
}
