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
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include "string.h"
#include "stdio.h"
#include <stddef.h>
#include <stdlib.h>
#include <sstream>
#include "hostIf_utils.h"
#include "safec_lib.h"
#include "cJSON.h"
#include <secure_wrapper.h>
#include <curl/curl.h>

#if defined (RDK_DEVICE_EMU)
#define INTERFACE_ETH          "eth0"
#else
#define INTERFACE_ETH          "eth1"
#endif

static bool gGatewayConnStatus = false;
#ifndef NEW_HTTP_SERVER_DISABLE
static bool legacyRFC = false;
#endif

const char* ntp_time_received_file="/tmp/timeReceivedNTP";
const char *webpa_start_tm_file ="/tmp/webpa/start_time";

EntryExitLogger::EntryExitLogger (const char* func, const char* file) :
    func (func), file (file)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entry: %s [%s]\n", func, file);
}

EntryExitLogger::~EntryExitLogger ()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Exit: %s [%s]\n", func, file);
}

using namespace std;

const char * getStringFromEnum( EnumStringMapper stbMapperArray[], int stbMapperArraySize, int enumCode )
{
    register int i = 0;
    for ( i = 0; i != stbMapperArraySize; i++ )
    {
        if ( stbMapperArray[i].enumCode == enumCode )
        {
            return stbMapperArray[i].enumString;
        }
    }

    return NULL;
}

int getEnumFromString( EnumStringMapper stbMapperArray[], int stbMapperArraySize, const char * inputStr )
{
    register int i = 0;
    for ( i = 0; i != stbMapperArraySize; i++ )
    {
        if (!strncasecmp(stbMapperArray[i].enumString, inputStr, strlen(inputStr)))
        {
            return stbMapperArray[i].enumCode;
        }
    }

    return -1;
}

#define MAX_NUM_LEN 10
bool matchComponent(const char* pParam, const char *pKey, const char **pSetting, int &instanceNo)
{
    bool ret = false;
    int str_len = strlen(pKey);
    ret = (strncasecmp(pParam,pKey,str_len)==0?true:false);

    if(ret)
    {
        const char *tmp_ptr;
        int tmp_len;
        if((pParam[str_len] == '.') &&
                (tmp_ptr = strchr(pParam+str_len+1,'.')) &&
                (tmp_len = tmp_ptr - (pParam + str_len + 1)) < MAX_NUM_LEN)
        {
            char tmp_buff[MAX_NUM_LEN];
            memset(tmp_buff,0,MAX_NUM_LEN);
            strncpy(tmp_buff,pParam+str_len+1,tmp_len);
            instanceNo = atoi(tmp_buff);
            *pSetting = (tmp_ptr + 1);
        }
        else
            ret = false;
    }

    return ret;

}

std::string int_to_string(int d)
{
    char tmp_buff[10];
    memset(tmp_buff,0,10);
    sprintf(tmp_buff,"%d",d);
    return std::string(tmp_buff);
}

std::string uint_to_string(uint d)
{
    std::stringstream ss;
    ss << d;
    return ss.str();
}

std::string ulong_to_string(unsigned long d)
{
    std::stringstream ss;
    ss << d;
    return ss.str();
}

int get_int(const char* ptr)
{
    int *ret = (int *)ptr;
    return *ret;
}

void put_int(char *ptr, int val)
{
    int *tmp = (int *)ptr;
    *tmp = val;
}

uint get_uint(char *ptr)
{
    uint *ret = (uint *)ptr;
    return *ret;
}

void put_uint(char *ptr, uint val)
{
    uint *tmp = (uint *)ptr;
    *tmp = val;
}



int get_ulong(const char* ptr)
{
    unsigned long *ret = (unsigned long *)ptr;
    return *ret;
}

void put_ulong(char *ptr, unsigned long val)
{
    unsigned long *tmp = (unsigned long *)ptr;
    *tmp = val;
}


bool get_boolean(const char *ptr)
{
    bool *ret = (bool *)ptr;
    return *ret;
}

void put_boolean(char *ptr, bool val)
{
    bool *tmp = (bool *)ptr;
    *tmp = val;
}

std::string bool_to_string(bool value)
{
    if (value == true) {
        return "true";
    } else if (value == false) {
        return "false";
    }
    return "";
}

int string_to_int(const char *value)
{
    char *end;
    long ret = strtol(value, &end, 10);
    return (int)ret;
}

uint string_to_uint(const char *value)
{
    char *end;
    unsigned long ret = strtoul(value, &end, 10);
    return (uint)ret;
}

unsigned long string_to_ulong(const char *value)
{
    char *end;
    unsigned long ret = strtoul(value, &end, 10);
    return ret;
}

bool string_to_bool(const char *value)
{
    bool ret = ((strcasecmp(value, "true") == 0)||(strcmp(value, "1") == 0)) ? true : false;
    return ret;
}

std::string getStringValue(HOSTIF_MsgData_t *stMsgData)
{
    switch (stMsgData->paramtype) {
    case hostIf_StringType:
        return string(stMsgData->paramValue);
    case hostIf_IntegerType:
        return int_to_string(get_int(stMsgData->paramValue));
    case hostIf_UnsignedIntType:
        return uint_to_string(get_uint(stMsgData->paramValue));
    case hostIf_BooleanType:
        return bool_to_string(get_boolean(stMsgData->paramValue));
    case hostIf_UnsignedLongType:
        return ulong_to_string(get_ulong(stMsgData->paramValue));
    case hostIf_DateTimeType:
        // we don't handle this one yet
    default:
        return "";
    }
}

void putValue(HOSTIF_MsgData_t *stMsgData, const string &value)
{
    errno_t rc = -1;
    // std::cout << "value ot be inserted is : " << value << std::endl;
    memset(stMsgData->paramValue, 0, TR69HOSTIFMGR_MAX_PARAM_LEN);

    switch (stMsgData->paramtype) {
    case hostIf_StringType:
	{
        rc=strcpy_s(stMsgData->paramValue,sizeof(stMsgData->paramValue), value.c_str());
	if(rc!=EOK)
    	{
	    ERR_CHK(rc);
    	}
        stMsgData->paramLen = strlen(value.c_str());
        break;
	}
    case hostIf_IntegerType:
        put_int(stMsgData->paramValue, string_to_int(value.c_str()));
        break;
    case hostIf_UnsignedIntType:
        put_uint(stMsgData->paramValue, string_to_uint(value.c_str()));
        break;
    case hostIf_BooleanType:
        put_boolean(stMsgData->paramValue, string_to_bool(value.c_str()));
        break;
    case hostIf_UnsignedLongType:
        put_ulong(stMsgData->paramValue, string_to_ulong(value.c_str()));
        break;
    case hostIf_DateTimeType:
        // we don't handle this one yet
    default:
        break;
    }
}

void setResetState( eSTBResetState rebootFlag)
{
    gResetState = rebootFlag;
}

eSTBResetState getResetState( void )
{
    return gResetState;
}

void triggerResetScript()
{
    int ret = -1;
    char scriptbuff[100] = {'\0'};

    switch (gResetState) {
    case ColdReset:
        /* Excute Cold Factory Reset script */
        snprintf(scriptbuff,100,"%s %s/%s", "sh", SCR_PATH, "coldfactory-reset.sh");
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Executing : %s \n",__FUNCTION__, scriptbuff);

        /*System command */
        ret = v_secure_system("%s", scriptbuff);
        if (WEXITSTATUS(ret) != 0 )
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to execute : %s. \n",__FUNCTION__, scriptbuff);
        }
        else {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Successfully executed %s Reset. \n",__FUNCTION__, scriptbuff);
            v_secure_system(REBOOT_SCR);
        }
        break;

    case FactoryReset:
        /* Excute Factory Reset script */
        snprintf(scriptbuff,100,"%s %s/%s", "sh", SCR_PATH, "factory-reset.sh");

        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Executing : %s \n",__FUNCTION__, scriptbuff);

        /*System command */
        ret = v_secure_system("%s", scriptbuff);
        if (WEXITSTATUS(ret) != 0 )
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to execute : %s. \n",__FUNCTION__, scriptbuff);
        }
        else {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Successfully executed %s Reset. \n",__FUNCTION__, scriptbuff);
            v_secure_system(REBOOT_SCR);
        }
        break;
    case WarehouseReset:
        /* Excute Warehouse Reset script */
        snprintf(scriptbuff,100,"%s %s/%s", "sh", SCR_PATH, "warehouse-reset.sh");
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Executing : %s \n",__FUNCTION__, scriptbuff);

        /*System command */
        ret = v_secure_system("%s", scriptbuff);
        if (WEXITSTATUS(ret) != 0 )
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to execute: %s. \n",__FUNCTION__, scriptbuff);
        }
        else {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Successfully executed %s Reset. \n",__FUNCTION__, scriptbuff);
        }
        break;
    case CustomerReset:
        /* Excute Customer Reset script */
        snprintf(scriptbuff,100,"%s %s/%s", "sh", SCR_PATH, "customer-reset.sh");
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Executing : %s \n",__FUNCTION__, scriptbuff);

        /*System command */
        ret = v_secure_system("%s", scriptbuff);
        if (WEXITSTATUS(ret) != 0 )
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to execute: %s. \n",__FUNCTION__, scriptbuff);
        }
        else {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Successfully executed %s Reset. \n",__FUNCTION__, scriptbuff);
            v_secure_system(REBOOT_SCR);
        }
        break;
    default:
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Invalid input for reset \n",__FUNCTION__);
        break;
    }

}

void set_GatewayConnStatus( bool enabled)
{
    gGatewayConnStatus = enabled;
}

bool get_GatewayConnStatus()
{
    return gGatewayConnStatus;
}

/**
 * Returns:
 * the specified environment variable's value if it is not NULL.
 * the specified default value otherwise.
 */
char* getenvOrDefault (const char* name, const char* defaultValue)
{
    char* value = getenv (name);
    return value ? value : (char*)defaultValue;
}

#ifndef NEW_HTTP_SERVER_DISABLE
/**
 * @brief Get Current time
 *
 * @param[in] Time spec timer
 */
void getCurrentTime(struct timespec *timer)
{
    clock_gettime(CLOCK_REALTIME, timer);
}


long timeValDiff(struct timespec *starttime, struct timespec *finishtime)
{
    long msec;
    msec=(finishtime->tv_sec-starttime->tv_sec)*1000;
    msec+=(finishtime->tv_nsec-starttime->tv_nsec)/1000000;
    return msec;
}

void setLegacyRFCEnabled(bool value)
{
    legacyRFC = value;
}

bool legacyRFCEnabled()
{
    return legacyRFC;
}
#endif

HostIf_Source_Type_t getBSUpdateEnum(const char *bsUpdate)
{
    if (!bsUpdate)
        return HOSTIF_NONE;

    if (strcasecmp(bsUpdate, "allUpdate") == 0)
        return HOSTIF_SRC_ALL;
    else if (strcasecmp(bsUpdate, "rfcUpdate") == 0)
        return HOSTIF_SRC_RFC;
    else if (strcasecmp(bsUpdate, "default") == 0)
        return HOSTIF_SRC_DEFAULT;
    return HOSTIF_NONE;
}

bool isWebpaReady()
{
    return ((access("/tmp/webpa/start_time", F_OK) == 0)?true:false);
}

bool isNtpTimeFilePresent()
{
    return ((access(ntp_time_received_file, F_OK) == 0)?true:false);
}

unsigned long get_system_manageble_ntp_time()
{
    unsigned long ret = 0;
    FILE* fp = fopen(ntp_time_received_file, "r");

    if (fp == NULL) {
        return ret;
    }

    char* line = NULL;
    size_t len = 0;

    if((getline(&line, &len, fp)) != -1) {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Date : \"%s\".\n",__FUNCTION__, line);
    }
    if (NULL == line ) {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Failed to get date from file: \'%s\' file.\n ", __FUNCTION__, ntp_time_received_file);
        fclose(fp);
        return ret;
    }

    fclose(fp);

    struct tm tm = {0};
    time_t epoch = 0;
    char* s_time = NULL;

    s_time =  strptime(line, "%a %b %d %H:%M:%S %Z %Y", &tm);

    if (NULL != s_time ) {
        epoch = mktime(&tm);
        ret = (unsigned long)epoch;
    }
    else {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Failed to parse NTP Date Format from \'%s\' file.\n ", __FUNCTION__, ntp_time_received_file);
    }
    if (line) free(line);
    return ret;
}

unsigned long get_device_manageble_time()
{
    unsigned long epoch_time = 0;
    FILE *fptr = NULL;
    int counter = 0;

    do {
        if(epoch_time || counter++ >=5 )
            break;
        if ((fptr = fopen(webpa_start_tm_file, "r")) != NULL) {
            if(!fscanf(fptr,"%ld", &epoch_time)){
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF,"Error getting epoch time\n");
            }
            else{
                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"Device Manageble Time =%ld\n", epoch_time);
            }
            fclose(fptr);
        }
        if(epoch_time == 0)
            sleep(1);
    }
    while(epoch_time == 0);

    return epoch_time;
}

std::string get_security_token() {
    std::string sToken = "";
    char pSecurityOutput[256] = {0};
    FILE *pSecurity = v_secure_popen("r", "/usr/bin/WPEFrameworkSecurityUtility");
    if(pSecurity) {
        if (fgets(pSecurityOutput, 256, pSecurity) != NULL) {
            cJSON* root = cJSON_Parse(pSecurityOutput);
            if (root) {
                cJSON *res = cJSON_GetObjectItem(root, "success");
                if(cJSON_IsTrue(res) == 1) {
                   cJSON* token = cJSON_GetObjectItem(root, "token");
                   if (token != NULL && token->type == cJSON_String && token->valuestring != NULL) {
                       RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Security Token retrieved successfully\n", __FUNCTION__);
                       sToken = token->valuestring;
                    }
                 }
                 else {
                    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF,"%s: Security Token retrieval failed!\n", __FUNCTION__);
                 }
                 cJSON_Delete(root);
             }
             else {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] json parse error\n", __FUNCTION__);
             }
        }
	v_secure_pclose(pSecurity);
     }
     
     else {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF,"%s: Failed to open security utility\n", __FUNCTION__);
     }
    

     return sToken;
}

size_t static writeCurlResponse(void *ptr, size_t size, size_t nmemb, string stream);
size_t static writeCurlResponse(void *ptr, size_t size, size_t nmemb, string stream)
{
    size_t realsize = size * nmemb;
    string temp(static_cast<const char*>(ptr), realsize);
    stream.append(temp);
    return realsize;
}

string getJsonRPCData(std::string postData)
{
    std::string tokenheader;
    string response;
	
    CURL *curl = curl_easy_init();
    if(curl)
    {
        std::string sToken = get_security_token();
	tokenheader = "Authorization: Bearer " + sToken;

	struct curl_slist *list = NULL;
	list = curl_slist_append(list, tokenheader.c_str());
        list = curl_slist_append(list, "Content-Type: application/json");

        if(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list) != CURLE_OK){
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s:%d curl setup failed for CURLOPT_HTTPHEADER\n", __FUNCTION__, __LINE__);
            curl_easy_cleanup(curl);
            curl_slist_free_all(list);
            return response;
        }
        if(curl_easy_setopt(curl, CURLOPT_POST, 1L) != CURLE_OK){
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s:%d curl setup failed for CURLOPT_POST\n", __FUNCTION__, __LINE__);
            curl_easy_cleanup(curl);
            curl_slist_free_all(list);
            return response;
        }
        if(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str()) != CURLE_OK){
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s:%d curl setup failed for CURLOPT_POSTFIELDS\n", __FUNCTION__, __LINE__);
            curl_easy_cleanup(curl);
            curl_slist_free_all(list);
            return response;
        }
        if(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCurlResponse) != CURLE_OK){
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s:%d curl setup failed for CURLOPT_WRITEFUNCTION\n", __FUNCTION__, __LINE__);
            curl_easy_cleanup(curl);
            curl_slist_free_all(list);
            return response;
        }
        if(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response) != CURLE_OK){
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s:%d curl setup failed for CURLOPT_WRITEDATA\n", __FUNCTION__, __LINE__);
            curl_easy_cleanup(curl);
            curl_slist_free_all(list);
            return response;
        }
        if(curl_easy_setopt(curl, CURLOPT_URL, JSONRPC_URL) != CURLE_OK){
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s:%d: curl setup failed for CURLOPT_URL\n", __FUNCTION__, __LINE__);
            curl_easy_cleanup(curl);
            curl_slist_free_all(list);
            return response;
        }
        if(curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5) != CURLE_OK){
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s:%d: curl setup failed for CURLOPT_CONNECTTIMEOUT\n", __FUNCTION__, __LINE__);
            curl_easy_cleanup(curl);
            curl_slist_free_all(list);
            return response;
        }
        if(curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10) != CURLE_OK){
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s:%d: curl setup failed for CURLOPT_TIMEOUT\n", __FUNCTION__, __LINE__);
            curl_easy_cleanup(curl);
            curl_slist_free_all(list);
            return response;
        }
        
        CURLcode res = curl_easy_perform(curl);
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: curl response : %d http response code: %ld\n", __FUNCTION__, res, http_code);
        curl_easy_cleanup(curl);
        curl_slist_free_all(list);

	return response;
    }
    else
    {
	RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: curl init failed\n", __FUNCTION__);
	return response;
    }
}

/** @} */
/** @} */
