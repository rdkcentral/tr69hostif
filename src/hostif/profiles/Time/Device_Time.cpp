/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
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

/**
 * @file Device_Time.c
 *
 * @brief DeviceTime API Implementation.
 *
 * This is the implementation of the DeviceTime API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */


/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "Device_Time.h"
#include "safec_lib.h"
#include <cstring>
#include <string>
#include <fstream>
#include <cstdio> 
#include <errno.h> 
#include <sys/stat.h>
#include <unistd.h>

#define TIME_ZONE_LENGTH 8
#define CHRONY_ENABLE_FILE "/opt/secure/RFC/chrony/chronyd_enabled" 
#define NTP_MINPOLL_FILE "/opt/secure/RFC/chrony/ntp_minpoll"
#define NTP_MAXPOLL_FILE "/opt/secure/RFC/chrony/ntp_maxpoll"
#define NTP_SERVER1_DIRECTIVE_FILE "/opt/secure/RFC/chrony/ntp_server1_directive"
#define NTP_SERVER2_DIRECTIVE_FILE "/opt/secure/RFC/chrony/ntp_server2_directive"
#define NTP_SERVER3_DIRECTIVE_FILE "/opt/secure/RFC/chrony/ntp_server3_directive"
#define NTP_SERVER4_DIRECTIVE_FILE "/opt/secure/RFC/chrony/ntp_server4_directive"
#define NTP_SERVER5_DIRECTIVE_FILE "/opt/secure/RFC/chrony/ntp_server5_directive"
#define NTP_MAXSTEP_FILE "/opt/secure/RFC/chrony/ntp_maxstep"
#define NTP_MAXSTEP_DEFAULT "1.0,3"

GHashTable* hostIf_Time::ifHash = NULL;
GMutex hostIf_Time::m_mutex;
GHashTable* hostIf_Time::m_notifyHash = NULL;
XBSStore* hostIf_Time::m_bsStore;

/****************************************************************************************************************************************************/
// Device.DeviceInfo Profile. Getters:
/****************************************************************************************************************************************************/
hostIf_Time::hostIf_Time(int dev_id):
    dev_id(dev_id),
    bCalledLocalTimeZone(false),
    bCalledCurrentLocalTime(false),
    bCalledCurrentUTCTime(false)
{
    backupLocalTimeZone[0]='\0';
    backupCurrentLocalTime[0]='\0';
    backupCurrentUTCTime[0]='\0';
    m_bsStore = XBSStore::getInstance();
}

hostIf_Time* hostIf_Time::getInstance(int dev_id)
{
    hostIf_Time* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_Time *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_Time(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create MoCA Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_Time::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_Time::closeInstance(hostIf_Time *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_Time::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        GList* current = tmp_list;

        while(current)
        {
            hostIf_Time* pDev = (hostIf_Time *)current->data;
            current = current->next;
            closeInstance(pDev);
        }

        g_list_free(tmp_list);
    }
}

void hostIf_Time::getLock()
{
    g_mutex_init(&hostIf_Time::m_mutex);
    g_mutex_lock(&hostIf_Time::m_mutex);
}

void hostIf_Time::releaseLock()
{
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] Unlocking mutex...  \n", __FUNCTION__, __LINE__);
    g_mutex_unlock(&hostIf_Time::m_mutex);
}

GHashTable* hostIf_Time::getNotifyHash()
{
    if(m_notifyHash)
    {
        return m_notifyHash;
    }
    else
    {
        return m_notifyHash = g_hash_table_new(g_str_hash, g_str_equal);
    }
}

hostIf_Time::~hostIf_Time()
{
    if(m_notifyHash)
    {
        g_hash_table_destroy(m_notifyHash);
    }
}

int hostIf_Time::get_Device_Time_LocalTimeZone(HOSTIF_MsgData_t *stMsgData, bool *pChanged )
{
    struct timeval time_now;
    struct tm *newtime = NULL;
    

    char tmp[_BUF_LEN_64];

    gettimeofday(&time_now, NULL);

    newtime = localtime((const time_t*)&(time_now.tv_sec));

    if(0 == strftime(tmp,sizeof(tmp),"%Z", newtime))
    {
        return NOK;
    }

    if(bCalledLocalTimeZone && pChanged && strncmp(tmp, backupLocalTimeZone,_BUF_LEN_64))
    {
        *pChanged = true;
    }

    bCalledLocalTimeZone = true;
    strncpy(stMsgData->paramValue,tmp,sizeof(stMsgData->paramValue) -1);
    stMsgData->paramValue[sizeof(stMsgData->paramValue) - 1] = '\0';
    strncpy(backupLocalTimeZone,tmp,sizeof(backupLocalTimeZone) -1);
    backupLocalTimeZone[sizeof(backupLocalTimeZone) -1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    return OK;
}
int hostIf_Time::get_Device_Time_Enable(HOSTIF_MsgData_t *, bool *pChanged )
{
    return NOK;
}
int hostIf_Time::get_Device_Time_Status(HOSTIF_MsgData_t *, bool *pChanged )
{
    return NOK;
}
int hostIf_Time::get_Device_Time_NTPServer1(HOSTIF_MsgData_t *, bool *pChanged )
{
    return NOK;
}
int hostIf_Time::get_Device_Time_NTPServer2(HOSTIF_MsgData_t *, bool *pChanged )
{
    return NOK;
}
int hostIf_Time::get_Device_Time_NTPServer3(HOSTIF_MsgData_t *, bool *pChanged )
{
    return NOK;
}
int hostIf_Time::get_Device_Time_NTPServer4(HOSTIF_MsgData_t *, bool *pChanged )
{
    return NOK;
}
int hostIf_Time::get_Device_Time_NTPServer5(HOSTIF_MsgData_t *, bool *pChanged )
{
    return NOK;
}


int hostIf_Time::get_Device_Time_CurrentLocalTime(HOSTIF_MsgData_t *stMsgData, bool *pChanged )
{
    time_t rawtime;
    struct tm * timeinfo;
    errno_t rc = -1;
    char buffer [_BUF_LEN_64] = {'\0'};
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FILE__, __FUNCTION__);
    char timeZoneTmp[7];
    memset(timeZoneTmp, 0, sizeof(timeZoneTmp));

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    strftime(buffer,_BUF_LEN_64-1,  "%Y-%m-%dT%H:%M:%S", timeinfo);
    strftime(timeZoneTmp, sizeof(timeZoneTmp),  "%z", timeinfo);
    snprintf(buffer + strlen(buffer),(sizeof(buffer) - strlen(buffer)), ".%.6d%s", timeinfo->tm_sec, timeZoneTmp);

    if(bCalledCurrentLocalTime && pChanged && strncmp(buffer, backupCurrentLocalTime, _BUF_LEN_64 ))
    {
        *pChanged = true;
    }

    bCalledCurrentLocalTime = true;
    rc=strcpy_s(stMsgData->paramValue,sizeof(stMsgData->paramValue), buffer);
    ERR_CHK(rc);
    rc=strcpy_s(backupCurrentLocalTime,sizeof(backupCurrentLocalTime),buffer);
    ERR_CHK(rc);

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] buffer : %s stMsgData->paramValue: %s\n", __FILE__, __FUNCTION__, buffer, stMsgData->paramValue);

    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FILE__, __FUNCTION__);
    return OK;
}

int hostIf_Time::get_xRDKCentralComBootstrap(HOSTIF_MsgData_t *stMsgData)
{
   return m_bsStore->getValue(stMsgData);
}

int hostIf_Time::set_xRDKCentralComBootstrap(HOSTIF_MsgData_t * stMsgData)
{
    return  m_bsStore->overrideValue(stMsgData);
}

int hostIf_Time::set_Device_Time_Enable(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}
int hostIf_Time::set_Device_Time_NTPServer1(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}
int hostIf_Time::set_Device_Time_NTPServer2(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}
int hostIf_Time::set_Device_Time_NTPServer3(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}
int hostIf_Time::set_Device_Time_NTPServer4(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}
int hostIf_Time::set_Device_Time_NTPServer5(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}
int hostIf_Time::set_Device_Time_LocalTimeZone(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}

int hostIf_Time::get_Device_Time_CurrentUTCTime(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    time_t rawtime;
    struct tm * timeinfo;
    errno_t rc = -1;
    char buffer [_BUF_LEN_64] = {'\0'};
    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s:%s] Entering..\n", __FILE__, __FUNCTION__);
    char timeZoneTmp[7];
    memset(timeZoneTmp, 0, sizeof(timeZoneTmp));

    time (&rawtime);
    timeinfo = gmtime (&rawtime);

    strftime(buffer, _BUF_LEN_64-1, "%Y-%m-%d %H:%M:%S", timeinfo);

    if(bCalledCurrentUTCTime && pChanged && strncmp(buffer, backupCurrentUTCTime, _BUF_LEN_64 ))
    {
        *pChanged = true;
    }

    bCalledCurrentUTCTime = true;
    rc=strcpy_s(stMsgData->paramValue, sizeof(stMsgData->paramValue), buffer);
    ERR_CHK(rc);
    rc=strcpy_s(backupCurrentUTCTime, sizeof(backupCurrentUTCTime), buffer);
    ERR_CHK(rc);

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] buffer : %s stMsgData->paramValue: %s\n", __FILE__, __FUNCTION__, buffer, stMsgData->paramValue);

    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s:%s] Exiting..\n", __FILE__, __FUNCTION__);
    return OK;
}

int hostIf_Time::set_Device_Time_Chrony_Enable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string chronyEnableStr = getStringValue(stMsgData);

    // If the value is empty, remove the file
    if (chronyEnableStr.empty() || chronyEnableStr == "false" || chronyEnableStr == "0") {
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
                "[%s:%s:%d] Chrony Enable set to false/empty, removing the file %s\n",
                __FUNCTION__, __FILE__, __LINE__,
                CHRONY_ENABLE_FILE);

        if (std::remove(CHRONY_ENABLE_FILE) != 0) {
            if (errno != ENOENT) { // Only log if it's not "file not found"
                RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                        "[%s:%s:%d] Failed to remove %s: %s\n",
                        __FUNCTION__, __FILE__, __LINE__,
                        CHRONY_ENABLE_FILE, strerror(errno));
            }
        }
        if (pChanged) *pChanged = true;
        return OK;
    }

    // Only allow "true" or "1" to enable
    if (chronyEnableStr == "true" || chronyEnableStr == "1") {
		const char* chronyDir = "/opt/secure/RFC/chrony";
        if (mkdir(chronyDir, 0755) != 0 && errno != EEXIST) {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s:%s:%d] Failed to create %s: %s\n",
                __FUNCTION__, __FILE__, __LINE__,
                chronyDir, strerror(errno));
           return NOK;
        }
        std::ofstream file(CHRONY_ENABLE_FILE);
        if (!file.is_open()) {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                    "[%s:%s:%d] Failed to open %s for writing\n",
                    __FUNCTION__, __FILE__, __LINE__, CHRONY_ENABLE_FILE);
            return NOK;
        }
        file << "true"; // Always write "true" if enabling
        file.close();
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
                "[%s:%s:%d] Successfully enabled Chrony\n", __FUNCTION__, __FILE__, __LINE__);
        if (pChanged) *pChanged = true;
        return OK;
    }

    // Unrecognized value
    RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
            "[%s:%s:%d] Invalid Chrony Enable value: %s\n",
            __FUNCTION__, __FILE__, __LINE__, chronyEnableStr.c_str());
    return NOK;
}

int hostIf_Time::get_Device_Time_Chrony_Enable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    stMsgData->paramtype = hostIf_BooleanType;

	if (access(CHRONY_ENABLE_FILE, F_OK) == 0) {
	 put_boolean(stMsgData->paramValue, true);
	} else {
	 put_boolean(stMsgData->paramValue, false);
	}

	stMsgData->paramLen = sizeof(bool);
	
    if (pChanged) *pChanged = false;
    return OK;
}


// Get handler for NTPMinpoll
int hostIf_Time::get_Device_Time_NTPMinpoll(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    stMsgData->paramtype = hostIf_UnsignedIntType;

    unsigned int minpoll = 10; // Default value
    std::ifstream file(NTP_MINPOLL_FILE);
    if (file.is_open()) {
        std::string value;
        std::getline(file, value);
        file.close();
        if (!value.empty()) {
            try {
                minpoll = static_cast<unsigned int>(std::stoul(value));
            } catch (const std::exception&) {
                minpoll = 10;
            }
        }
    }

    put_uint(stMsgData->paramValue, minpoll);
    stMsgData->paramLen = sizeof(unsigned int);

    if (pChanged) *pChanged = false;
    return OK;
}

// Set handler for NTPMinpoll
int hostIf_Time::set_Device_Time_NTPMinpoll(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
     const char* chronyDir = "/opt/secure/RFC/chrony";
    if (mkdir(chronyDir, 0755) != 0 && errno != EEXIST) {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
            "[%s:%s:%d] Failed to create %s: %s\n",
            __FUNCTION__, __FILE__, __LINE__,
            chronyDir, strerror(errno));
        return NOK;
    }

    std::string minpollStr = getStringValue(stMsgData);

    // Validate that minpollStr is a number in a valid range [4, 17] for NTP
    int minpoll = atoi(minpollStr.c_str());
    if (minpoll < 4 || minpoll > 24) {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s:%s:%d] Invalid NTPMinpoll value: %s\n",
                __FUNCTION__, __FILE__, __LINE__, minpollStr.c_str());
        return NOK;
    }

    std::ofstream file(NTP_MINPOLL_FILE);
    if (!file.is_open()) {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s:%s:%d] Failed to open %s for writing\n",
                __FUNCTION__, __FILE__, __LINE__, NTP_MINPOLL_FILE);
        return NOK;
    }
    file << minpollStr;
    file.close();

    if (pChanged) *pChanged = true;
    return OK;
}


// Get handler for NTPMaxpoll
int hostIf_Time::get_Device_Time_NTPMaxpoll(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    stMsgData->paramtype = hostIf_UnsignedIntType;

    unsigned int maxpoll = 12; // Default if file is empty or missing (NTP typical maxpoll default)

    std::ifstream file(NTP_MAXPOLL_FILE);
    if (file.is_open()) {
        std::string value;
        std::getline(file, value);
        file.close();

        if (!value.empty()) {
            maxpoll = static_cast<unsigned int>(atoi(value.c_str()));
        }
    }

    put_uint(stMsgData->paramValue, maxpoll);
    stMsgData->paramLen = sizeof(unsigned int);
    if (pChanged) *pChanged = false;
    return OK;
}

// Set handler for NTPMaxpoll
int hostIf_Time::set_Device_Time_NTPMaxpoll(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string maxpollStr = getStringValue(stMsgData);

    // Validate maxpoll in NTP allowed range [4, 17]
    int maxpoll = atoi(maxpollStr.c_str());
    if (maxpoll < 4 || maxpoll > 24) {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s:%s:%d] Invalid NTPMaxpoll value: %s\n",
                __FUNCTION__, __FILE__, __LINE__, maxpollStr.c_str());
        return NOK;
    }

    std::ofstream file(NTP_MAXPOLL_FILE);
    if (!file.is_open()) {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s:%s:%d] Failed to open %s for writing\n",
                __FUNCTION__, __FILE__, __LINE__, NTP_MAXPOLL_FILE);
        return NOK;
    }
    file << maxpollStr;
    file.close();

    if (pChanged) *pChanged = true;
    return OK;
}


int hostIf_Time::get_Device_Time_NTPServer1Directive(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    stMsgData->paramtype = hostIf_StringType;
    std::ifstream file(NTP_SERVER1_DIRECTIVE_FILE);
    std::string value;

    if (file.is_open()) {
        std::getline(file, value);
        file.close();
    }
    if (value.empty()) {
        value = "server";
    }
    strncpy(stMsgData->paramValue, value.c_str(), sizeof(stMsgData->paramValue)-1);
    stMsgData->paramValue[sizeof(stMsgData->paramValue)-1] = '\0';
    stMsgData->paramLen = strlen(stMsgData->paramValue);

    if (pChanged) *pChanged = false;
    return OK;
}

int hostIf_Time::set_Device_Time_NTPServer1Directive(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string directive = getStringValue(stMsgData);
    std::ofstream file(NTP_SERVER1_DIRECTIVE_FILE);
    if (!file.is_open()) {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s:%s:%d] Failed to open %s for writing: %s\n",
                __FUNCTION__, __FILE__, __LINE__,
                NTP_SERVER1_DIRECTIVE_FILE, strerror(errno));
        return NOK;
    }
    file << directive;
    file.close();

    if (pChanged) *pChanged = true;
    return OK;
}

int hostIf_Time::get_Device_Time_NTPServer2Directive(HOSTIF_MsgData_t *stMsgData, bool *pChanged) {
    stMsgData->paramtype = hostIf_StringType;
    std::ifstream file(NTP_SERVER2_DIRECTIVE_FILE);
    std::string value;
    if (file.is_open()) {
        std::getline(file, value);
        file.close();
    }
    if (value.empty()) value = "server";
    strncpy(stMsgData->paramValue, value.c_str(), sizeof(stMsgData->paramValue)-1);
    stMsgData->paramValue[sizeof(stMsgData->paramValue)-1] = '\0';
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (pChanged) *pChanged = false;
    return OK;
}

int hostIf_Time::set_Device_Time_NTPServer2Directive(HOSTIF_MsgData_t *stMsgData, bool *pChanged) {
    std::string directive = getStringValue(stMsgData);
    std::ofstream file(NTP_SERVER2_DIRECTIVE_FILE);
    if (!file.is_open()) return NOK;
    file << directive;
    file.close();
    if (pChanged) *pChanged = true;
    return OK;
}

int hostIf_Time::get_Device_Time_NTPServer3Directive(HOSTIF_MsgData_t *stMsgData, bool *pChanged) {
    stMsgData->paramtype = hostIf_StringType;
    std::ifstream file(NTP_SERVER3_DIRECTIVE_FILE);
    std::string value;
    if (file.is_open()) {
        std::getline(file, value);
        file.close();
    }
    if (value.empty()) value = "server";
    strncpy(stMsgData->paramValue, value.c_str(), sizeof(stMsgData->paramValue)-1);
    stMsgData->paramValue[sizeof(stMsgData->paramValue)-1] = '\0';
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (pChanged) *pChanged = false;
    return OK;
}

int hostIf_Time::set_Device_Time_NTPServer3Directive(HOSTIF_MsgData_t *stMsgData, bool *pChanged) {
    std::string directive = getStringValue(stMsgData);
    std::ofstream file(NTP_SERVER3_DIRECTIVE_FILE);
    if (!file.is_open()) return NOK;
    file << directive;
    file.close();
    if (pChanged) *pChanged = true;
    return OK;
}

int hostIf_Time::get_Device_Time_NTPServer4Directive(HOSTIF_MsgData_t *stMsgData, bool *pChanged) {
    stMsgData->paramtype = hostIf_StringType;
    std::ifstream file(NTP_SERVER4_DIRECTIVE_FILE);
    std::string value;
    if (file.is_open()) {
        std::getline(file, value);
        file.close();
    }
    if (value.empty()) value = "server";
    strncpy(stMsgData->paramValue, value.c_str(), sizeof(stMsgData->paramValue)-1);
    stMsgData->paramValue[sizeof(stMsgData->paramValue)-1] = '\0';
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (pChanged) *pChanged = false;
    return OK;
}

int hostIf_Time::set_Device_Time_NTPServer4Directive(HOSTIF_MsgData_t *stMsgData, bool *pChanged) {
    std::string directive = getStringValue(stMsgData);
    std::ofstream file(NTP_SERVER4_DIRECTIVE_FILE);
    if (!file.is_open()) return NOK;
    file << directive;
    file.close();
    if (pChanged) *pChanged = true;
    return OK;
}

int hostIf_Time::get_Device_Time_NTPServer5Directive(HOSTIF_MsgData_t *stMsgData, bool *pChanged) {
    stMsgData->paramtype = hostIf_StringType;
    std::ifstream file(NTP_SERVER5_DIRECTIVE_FILE);
    std::string value;
    if (file.is_open()) {
        std::getline(file, value);
        file.close();
    }
    if (value.empty()) value = "server";
    strncpy(stMsgData->paramValue, value.c_str(), sizeof(stMsgData->paramValue)-1);
    stMsgData->paramValue[sizeof(stMsgData->paramValue)-1] = '\0';
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (pChanged) *pChanged = false;
    return OK;
}

int hostIf_Time::set_Device_Time_NTPServer5Directive(HOSTIF_MsgData_t *stMsgData, bool *pChanged) {
    std::string directive = getStringValue(stMsgData);
    std::ofstream file(NTP_SERVER5_DIRECTIVE_FILE);
    if (!file.is_open()) return NOK;
    file << directive;
    file.close();
    if (pChanged) *pChanged = true;
    return OK;
}

int hostIf_Time::get_Device_Time_NTPMaxstep(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    stMsgData->paramtype = hostIf_StringType;
    std::ifstream file(NTP_MAXSTEP_FILE);
    std::string value;
    if (file.is_open()) {
        std::getline(file, value);
        file.close();
    }
    if (value.empty())
        value = NTP_MAXSTEP_DEFAULT;
    strncpy(stMsgData->paramValue, value.c_str(), sizeof(stMsgData->paramValue) - 1);
    stMsgData->paramValue[sizeof(stMsgData->paramValue) - 1] = '\0';
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (pChanged) *pChanged = false;
    return OK;
}

int hostIf_Time::set_Device_Time_NTPMaxstep(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string input = getStringValue(stMsgData);  

    //Format - makestep 1.0 3 
    size_t comma = input.find(',');
    if (comma == std::string::npos) {
        stMsgData->faultCode = fcInvalidParameterValue;
        return NOK;
    }
    // Optional: check number formats - left as exercise for stricter validation

    std::ofstream file(NTP_MAXSTEP_FILE, std::ios::trunc);
    if (!file.is_open())
        return NOK;
    file << input;
    file.close();
    if (pChanged) *pChanged = true;
    return OK;
}
/** @} */
/** @} */
