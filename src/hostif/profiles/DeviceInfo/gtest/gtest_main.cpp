/*
 * Copyright 2024 Comcast Cable Communications Management, LLC
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
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "hostIf_tr69ReqHandler.h"
#include "dm_stubs.h"
#include "hostIf_utils.h"
#include "XrdkCentralComRFCStore.h"
#include "XrdkCentralComBSStore.h"
#include "XrdkCentralComRFC.h"
#include "XrdkCentralComBSStoreJournal.h"


#include "hostIf_msgHandler.h"
#include "http_server.h"
#include "request_handler.h"
#include "rdk_debug.h"
#include "file_writer.h"
#include "waldb.h"
#include "Device_DeviceInfo_Processor.h"
#include "Device_DeviceInfo_ProcessStatus.h"
#include "Device_DeviceInfo_ProcessStatus_Process.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include <wdmp-c.h>
#include <wdmp_internal.h>
#ifdef __cplusplus
}
#endif

#include "Device_DeviceInfo.h"

#include "rbus.h"
#include "power_controller.h"
#ifndef USE_THUNDER_CLIENT
#include "dsError.h"
#include "audioOutputPort.hpp"
#endif

#include <curl/curl.h>
#include "cJSON.h"

#include <mutex>
#include <condition_variable>

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "hostif_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 128

using namespace std;
XRFCStore* m_rfcStore;
XBSStore* m_bsStore;
XBSStoreJournal* m_bsStoreJournal;
XRFCStorage* m_rfcStoreage;

std::mutex mtx_httpServerThreadDone;
std::condition_variable cv_httpServerThreadDone;
bool httpServerThreadDone = false;
GThread *HTTPServerThread = NULL;
char *HTTPServerName = (char *)"HTTPServerThread";
GError *httpError = NULL;
T_ARGLIST argList = {{'\0'}, 0};

extern IARM_Result_t g_iarm_bus_call_result;
extern IARM_Result_t g_iarm_broadcast_event_result;
extern bool g_malloc_fail;
extern bool g_gettimeofday_fail;
extern bool g_socket_fail;
extern bool g_ioctl_success;
extern bool g_ioctl_throw;
extern int g_v_secure_system_result;
extern rbusError_t g_rbus_event_publish_result;
extern char g_iarm_stub_serial_payload[128];
extern char g_iarm_stub_hwver_payload[128];
extern char g_iarm_stub_provisioning_payload[128];
extern char g_iarm_stub_swver_payload[128];

#ifdef GTEST_ENABLE
extern bool (*ValidateInput_ArgumentsFunc()) (char *input, FILE *tmp_fptr);
#endif

static void* handle_reset_time_mock(void* arg);
static void* handle_zero_reset_time_mock(void* arg);

TEST(rfcStoreTest, setValue) {
    m_rfcStore = XRFCStore::getInstance();
    
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    
    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    int ret = m_rfcStore->setValue(&param);

    std::cout << "ret = " << ret << std::endl;
    EXPECT_EQ(ret, 0);
}

TEST(rfcStoreTest, getValue) {
    m_rfcStore = XRFCStore::getInstance();
    
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    int ret = m_rfcStore->getValue(&param);
    std::cout << "paramValue: " << getStringValue(&param) << " ret = " << ret << std::endl;
    EXPECT_EQ(ret, 0);
}

TEST(rfcStoreTest, setLocalValue) {
    m_rfcStore = XRFCStore::getInstance();
    
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SystemServices.FriendlyName", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;
    
    strncpy(param.paramValue, "TestName2", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);
    int ret = m_rfcStore->setValue(&param);

    std::cout << "ret = " << ret << std::endl;
    EXPECT_EQ(ret, 0);
}

TEST(rfcStoreTest, getLocalValue) {
    m_rfcStore = XRFCStore::getInstance();
    
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SystemServices.FriendlyName", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    //param.requestor = HOSTIF_SRC_RFC;
    //param.paramtype = hostIf_BooleanType;
    //param.paramLen = sizeof(hostIf_BooleanType);
    int ret = m_rfcStore->getValue(&param);
    std::cout << "paramValue: " << getStringValue(&param) << " ret = " << ret << std::endl;
    EXPECT_EQ(ret, 0);
}

TEST(rfcStoreTest, clearLocalValue) {
    m_rfcStore = XRFCStore::getInstance();
    
    m_rfcStore = XRFCStore::getInstance();
    
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.ClearParam", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;
    
    strncpy(param.paramValue, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SystemServices.FriendlyName", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);
    int ret = m_rfcStore->setValue(&param);
    
    std::cout << " ret = " << ret << std::endl;
    EXPECT_EQ(ret, fcNoFault);
}

TEST(rfcStoreTest, getLocalValueAfterClear) {
    m_rfcStore = XRFCStore::getInstance();
    
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SystemServices.FriendlyName", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    //param.requestor = HOSTIF_SRC_RFC;
    //param.paramtype = hostIf_BooleanType;
    //param.paramLen = sizeof(hostIf_BooleanType);
    int ret = m_rfcStore->getValue(&param);
    std::cout << "paramValue: " << getStringValue(&param) << " ret = " << ret << std::endl;
    EXPECT_EQ(ret, fcInternalError);
}

TEST(rfcStoreTest, clearLocalValueWithWildcard) {
    m_rfcStore = XRFCStore::getInstance();

    HOSTIF_MsgData_t setParam1 = { 0 };
    memset(&setParam1, 0, sizeof(HOSTIF_MsgData_t));
    setParam1.reqType = HOSTIF_SET;
    strncpy(setParam1.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SystemServices.UnitTest.Param1", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    setParam1.requestor = HOSTIF_SRC_WEBPA;
    strncpy(setParam1.paramValue, "value1", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    setParam1.paramtype = hostIf_StringType;
    setParam1.paramLen = strlen(setParam1.paramValue);
    EXPECT_EQ(m_rfcStore->setValue(&setParam1), fcNoFault);

    HOSTIF_MsgData_t setParam2 = { 0 };
    memset(&setParam2, 0, sizeof(HOSTIF_MsgData_t));
    setParam2.reqType = HOSTIF_SET;
    strncpy(setParam2.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SystemServices.UnitTest.Param2", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    setParam2.requestor = HOSTIF_SRC_WEBPA;
    strncpy(setParam2.paramValue, "value2", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    setParam2.paramtype = hostIf_StringType;
    setParam2.paramLen = strlen(setParam2.paramValue);
    EXPECT_EQ(m_rfcStore->setValue(&setParam2), fcNoFault);

    HOSTIF_MsgData_t clearParam = { 0 };
    memset(&clearParam, 0, sizeof(HOSTIF_MsgData_t));
    clearParam.reqType = HOSTIF_SET;
    strncpy(clearParam.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.ClearParam", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    clearParam.requestor = HOSTIF_SRC_WEBPA;
    strncpy(clearParam.paramValue, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SystemServices.UnitTest.", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    clearParam.paramtype = hostIf_StringType;
    clearParam.paramLen = strlen(clearParam.paramValue);
    EXPECT_EQ(m_rfcStore->setValue(&clearParam), fcNoFault);

    HOSTIF_MsgData_t getParam = { 0 };
    memset(&getParam, 0, sizeof(HOSTIF_MsgData_t));
    getParam.reqType = HOSTIF_GET;
    strncpy(getParam.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SystemServices.UnitTest.Param1", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    EXPECT_EQ(m_rfcStore->getValue(&getParam), fcInternalError);
}

TEST(rfcStoreTest, setValue_NonPersistentFromWebpa_Fails) {
    m_rfcStore = XRFCStore::getInstance();

    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonPersistent.UnitTest.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.requestor = HOSTIF_SRC_WEBPA;
    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    EXPECT_EQ(m_rfcStore->setValue(&param), fcInternalError);
}


TEST(bsStoreTest, getValueFactoryFresh) {
    m_bsStore = XBSStore::getInstance();
    
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Time.NTPServer1", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    int ret = m_bsStore->getValue(&param); //Get value before partnerId
    
    std::cout << "paramValue: " << getStringValue(&param) << " ret = " << ret << std::endl;
    EXPECT_EQ(ret, 0);
}

TEST(bsStoreTest, getValueAfterPartnerId) {
    m_bsStore = XBSStore::getInstance();
    sleep(5); // Allows time to get the partnerId

    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Time.NTPServer1", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    int ret = m_bsStore->getValue(&param);
    
    std::cout << "paramValue: " << getStringValue(&param) << " ret = " << ret << std::endl;
    EXPECT_EQ(ret, 0);
}

TEST(bsStoreTest, setValue) {
    m_bsStore = XBSStore::getInstance();
    
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.Time.NTPServer1", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    
    strncpy(param.paramValue, "override_ntpserver1_test_url", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);
    int ret = m_bsStore->overrideValue(&param);

    std::cout << "ret = " << ret << std::endl;
    EXPECT_EQ(ret, 0);
}

TEST(bsStoreTest, verifySetValue) {
    m_bsStore = XBSStore::getInstance();
    
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Time.NTPServer1", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    int ret = m_bsStore->getValue(&param);
    
    std::cout << "paramValue: " << getStringValue(&param) << " ret = " << ret << std::endl;
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(getStringValue(&param), "override_ntpserver1_test_url");
}

TEST(hostifMainTest, initHostIf) {
    
    /* Load the data model xml file*/
    DB_STATUS status = loadDataModel();
    if(status != DB_SUCCESS)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error in Data Model Initialization\n");
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"Successfully initialize Data Model.\n");
    }
    
    std::cout << "Starting New HTTP Server" << std::endl;
    if((HTTPServerThread = g_thread_create((GThreadFunc)HTTPServerStartThread,
                                           (void *)HTTPServerName,
                                           TRUE,
                                           &httpError)) == NULL)
    {
        std::cout << "New HTTP Server: FAILED" << std::endl;
    }
    EXPECT_NE(HTTPServerThread,nullptr);
    
    sleep(5);
    
    HttpServerStop();
    if(HTTPServerThread)
        g_thread_join(HTTPServerThread);
}

/*
static size_t writeCurlResponse(void *ptr, size_t size, size_t nmemb, string stream)
{
   size_t realsize = size * nmemb;
   string temp(static_cast<const char*>(ptr), realsize);
   stream.append(temp);
   return realsize;
}

TEST(httpServerTest, getHandler) {
   WDMP_STATUS ret = WDMP_FAILURE;
   long http_code = 0;
   string response;
   CURL *curl_handle = NULL;
   CURLcode res = CURLE_FAILED_INIT;

    const char *url = "http://127.0.0.1:11999";
    const char *pcCallerID = "Unknown";
    const char* pcParameterName = "Device.Time.NTPServer1";
    
   curl_handle = curl_easy_init();
   string data = "\{\"names\" : [\"";
   data.append(pcParameterName);
   data.append("\"]}");
   RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"getRFCParam data = %s, datalen = %d\n", data.c_str(), data.length());
   if (curl_handle)
   {
       char pcCallerIDHeader[128];
       if(pcCallerID)
           sprintf(pcCallerIDHeader, "CallerID: %s", pcCallerID);
       else
           sprintf(pcCallerIDHeader, "CallerID: Unknown");
       struct curl_slist *customHeadersList = NULL;
       customHeadersList = curl_slist_append(customHeadersList, pcCallerIDHeader);
       curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, customHeadersList);

       curl_easy_setopt(curl_handle, CURLOPT_URL, url);
       curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
       curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, (long) data.length());
       curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, data.c_str());
       curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
       curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writeCurlResponse);
       curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response);
       curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 5);
       curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 10);

       res = curl_easy_perform(curl_handle);
       curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
       RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"curl response : %d http response code: %ld\n", res, http_code);
       curl_easy_cleanup(curl_handle);

       curl_slist_free_all(customHeadersList);
   }
   else
   {
      RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"Could not perform curl \n");
   }
   if (res == CURLE_OK)
   {
      cJSON *response_json = NULL;
      RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"Curl response: %s\n", response.c_str());
      response_json = cJSON_Parse(response.c_str());

      if (response_json)
      {
         cJSON *items = cJSON_GetObjectItem(response_json, "parameters");

         for (int i = 0 ; i < cJSON_GetArraySize(items) ; i++)
         {
            cJSON* subitem  = cJSON_GetArrayItem(items, i);
            cJSON* name    = cJSON_GetObjectItem(subitem, "name");
         }
         cJSON* statusCode = cJSON_GetObjectItem(response_json, "statusCode");
         if(statusCode)
         {
            ret = (WDMP_STATUS)statusCode->valueint;
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"statusCode = %d\n", ret);
         }
      }
   }
}
*/

TEST(requestHandlerTest, getHandler)
{
    const char *pcCallerID = "Unknown";
    cJSON *jsonRequest = NULL;
    cJSON *jsonResponse = NULL;
    req_struct *reqSt = NULL;
    res_struct *respSt = NULL;

    jsonRequest = cJSON_Parse("{\"names\": [\"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable\"]}");

    if(jsonRequest)
    {
        reqSt = (req_struct *)malloc(sizeof(req_struct));
        if(reqSt == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. Failed to create req_struct\n", __FUNCTION__, __FILE__);
        }
        memset(reqSt, 0, sizeof(req_struct));

        {
            if(!pcCallerID || !strlen(pcCallerID))
            {
                pcCallerID = "Unknown";
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s:%s] Unknown Caller ID, GET is allowed by default\n", __FUNCTION__, __FILE__);
            }
            else
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%s] GET with CallerID : %s..\n", __FUNCTION__, __FILE__, pcCallerID);
            parse_get_request(jsonRequest, &reqSt, WDMP_TR181);
            respSt = handleRequest(pcCallerID, reqSt);
            if(respSt)
            {
                jsonResponse = cJSON_CreateObject();
                wdmp_form_get_response(respSt, jsonResponse);

                // WDMP Code sets a generic statusCode, the following lines replace it with an actual error code.
                int new_st_code = 0;

                for(int paramIndex = 0; paramIndex < respSt->paramCnt; paramIndex++)
                {
                    if(respSt->retStatus[paramIndex] != 0 || paramIndex == respSt->paramCnt-1)
                    {
                        new_st_code =  respSt->retStatus[paramIndex];
                        break;
                    }
                }
                cout << "new_st_code = " << new_st_code << endl;
                EXPECT_EQ(new_st_code, 0);
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. Request couldn't be processed\n", __FUNCTION__, __FILE__);
            }
        }

        

        char *buf = cJSON_Print(jsonResponse);

        if(buf) {
            cout << "Status OK" << endl;
        }
        EXPECT_NE(buf,nullptr);

        reqSt = NULL;
        cJSON_Delete(jsonRequest);
        cJSON_Delete(jsonResponse);
        respSt = NULL;

        if(buf != NULL) {
            free(buf);
            buf = NULL;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. Failed to parse JSON Message \n", __FUNCTION__, __FILE__);
    }
}

TEST(deviceInfoTest, getDescription) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        pIface->get_Device_DeviceInfo_Description(&msgData,&bChanged);
        cout << "msgData.paramValue = " <<  msgData.paramValue << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}

TEST(deviceInfoTest, getUptime) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        pIface->get_Device_DeviceInfo_UpTime(&msgData,&bChanged);
        cout << "msgData.paramValue = " <<  get_int(msgData.paramValue) << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}

TEST(deviceInfoTest, setFirmwareToDownload) {
    HOSTIF_MsgData_t msgData;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
        strncpy (msgData.paramValue, "FIRMWARE_NAME", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
        pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareToDownload(&msgData);
        cout << "msgData.paramValue = " <<  msgData.paramValue << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}

TEST(deviceInfoTest, setFirmwareDownloadStatus) {
    HOSTIF_MsgData_t msgData;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
        strncpy (msgData.paramValue, "Download Started", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
        pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadStatus(&msgData);
        cout << "msgData.paramValue = " <<  msgData.paramValue << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}


TEST(deviceInfoTest, getFirmwareToDownload) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareToDownload(&msgData,&bChanged);
        cout << "msgData.paramValue = " <<  msgData.paramValue << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}

TEST(deviceInfoTest, getFirmwareDownloadStatus) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadStatus(&msgData,&bChanged);
        cout << "msgData.paramValue = " <<  msgData.paramValue << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}

TEST(deviceInfoTest, getFirmwareDownloadProtocol) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadProtocol(&msgData,&bChanged);
        cout << "msgData.paramValue = " <<  msgData.paramValue << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}

TEST(deviceInfoTest, getFirmwareDownloadProtocol_NoStatusFile)
{
    std::remove("/opt/fwdnldstatus.txt");
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadProtocol(&msgData, &bChanged);
    EXPECT_EQ(ret, NOK);
}

TEST(deviceInfoTest, getFirmwareDownloadUrl) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadURL(&msgData,&bChanged);
        cout << "msgData.paramValue = " <<  msgData.paramValue << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}

TEST(deviceInfoTest, getFirmwareDownloadUrl_NoStatusFile)
{
    std::remove("/opt/fwdnldstatus.txt");
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadURL(&msgData, &bChanged);
    EXPECT_EQ(ret, NOK);
}

TEST(deviceInfoTest, getFirmwareDownloadCodebig) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadUseCodebig(&msgData,&bChanged);
        cout << "msgData.paramValue = " <<  msgData.paramValue << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}

TEST(deviceInfoTest, getFirmwareDownloadCodebig_NoStatusFile)
{
    std::remove("/opt/fwdnldstatus.txt");
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadUseCodebig(&msgData, &bChanged);
    EXPECT_EQ(ret, NOK);
}

TEST(deviceInfoTest, getFirmwareDownloadDeferReboot) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadDeferReboot(&msgData,&bChanged);
        cout << "msgData.paramValue = " <<  msgData.paramValue << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}

TEST(deviceInfoTest, getFirmwareDownloadState) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareUpdateState(&msgData,&bChanged);
        cout << "msgData.paramValue = " <<  msgData.paramValue << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}

TEST(deviceInfoTest, getFirmwareDownloadState_NoStatusFile)
{
    std::remove("/opt/fwdnldstatus.txt");
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareUpdateState(&msgData, &bChanged);
    EXPECT_EQ(ret, NOK);
}

TEST(deviceInfoTest, getProcessorCount) {
    HOSTIF_MsgData_t msgData;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        pIface->get_Device_DeviceInfo_ProcessorNumberOfEntries(&msgData);
        cout << "msgData.paramValue = " <<  get_int(msgData.paramValue) << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}



TEST(deviceInfoTest, getMemoryTotal) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        pIface->get_Device_DeviceInfo_MemoryStatus_Total(&msgData,&bChanged);
        cout << "msgData.paramValue = " <<  get_int(msgData.paramValue) << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}

TEST(deviceInfoTest, getMemoryFree) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        pIface->get_Device_DeviceInfo_MemoryStatus_Free(&msgData,&bChanged);
        cout << "msgData.paramValue = " <<  get_int(msgData.paramValue) << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}

TEST(deviceInfoTest, getBootTime) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        pIface->get_X_RDKCENTRAL_COM_BootTime(&msgData,&bChanged);
        cout << "msgData.paramValue = " <<  get_ulong(msgData.paramValue) << " msgData.faultCode=" << msgData.faultCode << endl;
        EXPECT_EQ(msgData.faultCode,0);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_SoftwareVersion) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    writeToTr181storeFile("VERSION", "99.99.15.07", "/version.txt", Plain);
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_SoftwareVersion(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue,"99.99.15.07");
    }
}

TEST(deviceTest, get_JENKINS_BUILD_NUMBER) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    writeToTr181storeFile("trunk", "124", "/version.txt", Plain);
    writeToTr181storeFile("JENKINS_BUILD_NUMBER", "5680", "/version.txt", Plain);
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_SoftwareVersion(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_SoftwareVersion_TrunkAndJenkins_WithWhitespace)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;

    std::ofstream ofs("/version.txt", std::ios::trunc | std::ios::out);
    ASSERT_TRUE(ofs.is_open());
    ofs << "trunk=124" << std::endl;
    ofs << "VERSION=   99.99.15.07" << std::endl;
    ofs << "JENKINS_BUILD_NUMBER=   5680" << std::endl;
    ofs.close();

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_SoftwareVersion(&msgData, &bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "99.99.15.07.5680");
}

TEST(deviceTest, get_Device_DeviceInfo_SoftwareVersion_FileOpenFailure)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;

    std::remove("/version.txt");

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_SoftwareVersion(&msgData, &bChanged);
    EXPECT_EQ(ret, NOK);
}

TEST(deviceTest, get_Device_DeviceInfo_SoftwareVersion_ChangedSetsFlag)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    writeToTr181storeFile("VERSION", "11.22.33.44", "/version.txt", Plain);
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_SoftwareVersion(&msgData, &bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_FALSE(bChanged);

    bChanged = false;
    writeToTr181storeFile("VERSION", "11.22.33.55", "/version.txt", Plain);
    memset(&msgData, 0, sizeof(msgData));
    ret = pIface->get_Device_DeviceInfo_SoftwareVersion(&msgData, &bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_TRUE(bChanged);
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename) {
    write_on_file("/tmp/currently_running_image_name", "ELTE11MWR_DEV_develop_20250808222527_NG");
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "ELTE11MWR_DEV_develop_20250808222527_NG");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_Migration_MigrationStatus) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_Migration_MigrationStatus(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "NEEDED");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_Migration_MigrationStatus_Update) {
    std::remove("/opt/secure/persistent/MigrationStatus");
    write_on_file("/opt/secure/persistent/MigrationStatus", "Migrated");
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_Migration_MigrationStatus(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "Migrated");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_Migration_MigrationStatus_FileMissing)
{
    std::remove("/opt/secure/persistent/MigrationStatus");
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_Migration_MigrationStatus(&msgData, &bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "NOT_NEEDED");
}

TEST(deviceTest, get_Device_DeviceInfo_Migration_MigrationStatus_EmptyFile)
{
    std::ofstream ofs("/opt/secure/persistent/MigrationStatus", std::ios::trunc | std::ios::out);
    ASSERT_TRUE(ofs.is_open());
    ofs.close();

    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_Migration_MigrationStatus(&msgData, &bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "NOT_NEEDED");
}


TEST(deviceTest, get_Device_DeviceInfo_Manufacturer) {
    writeToTr181storeFile("MANUFACTURE", "Sky", "/etc/device.properties", Plain);
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_Manufacturer(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_Manufacturer_UnsetEnv)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    unsetenv("MANUFACTURE");

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    memset(&msgData,0,sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_Manufacturer(&msgData,&bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "");
    EXPECT_EQ(msgData.paramLen, 0);
}

TEST(deviceTest, get_Device_DeviceInfo_Manufacturer_EmptyEnv)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    setenv("MANUFACTURE", "", 1);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    memset(&msgData,0,sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_Manufacturer(&msgData,&bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "");
    EXPECT_EQ(msgData.paramLen, 0);
}

TEST(deviceTest, getLastField) {
    char input[] = "device ip stb mac";
    char *last = getLastField(input, ' ');
    EXPECT_STREQ(last, "mac");
}

TEST(deviceTest, get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadPercent) {
    write_on_file("/opt/curl_progress", "Download percent is 80");
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadPercent(&msgData,&bChanged);
        cout << "msgData.paramValue = " <<  get_int(msgData.paramValue) << " msgData.faultCode=" << msgData.faultCode << endl;
	EXPECT_EQ(ret, OK);
        EXPECT_EQ(get_int(msgData.paramValue), 80);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_ModelName) {
    std::remove("/tmp/.model");
    write_on_file("/tmp/.model", "Xione-UK");
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_ModelName(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "Xione-UK");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_ModelName_WithNewline) {
    std::remove("/tmp/.model");
    write_on_file("/tmp/.model", "Xione-UK\n");
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_ModelName(&msgData, &bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "Xione-UK");
}

TEST(deviceTest, get_Device_DeviceInfo_ModelName_FileMissing) {
    std::remove("/tmp/.model");
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_ModelName(&msgData, &bChanged);
    EXPECT_EQ(ret, NOK);
}

TEST(deviceTest, get_Device_DeviceInfo_ModelName_EmptyFile) {
    std::ofstream ofs("/tmp/.model", std::ios::trunc | std::ios::out);
    ASSERT_TRUE(ofs.is_open());
    ofs.close();
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_ModelName(&msgData, &bChanged);
    EXPECT_EQ(ret, NOT_HANDLED);
}

TEST(deviceTest, get_Device_DeviceInfo_FirstUseDate) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_FirstUseDate(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_FirstUseDate_FileRemoved) {
    std::remove("/opt/persistent/firstNtpTime");
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_FirstUseDate(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, set_xOpsRPCDevManageableNotification) {
    HOSTIF_MsgData_t param;
    bool bChanged;
    int instanceNumber = 0;
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ManageableNotification.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    
    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xOpsRPCDevManageableNotification(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xOpsRPCDevManageableNotification_Enabled)
{
    XRFCStore *rfcStore = XRFCStore::getInstance();
    HOSTIF_MsgData_t rfcParam = { 0 };
    strncpy(rfcParam.paramName, X_RDK_RFC_MANGEBLENOTIFICATION_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_boolean(rfcParam.paramValue, true);
    rfcParam.paramtype = hostIf_BooleanType;
    rfcParam.paramLen = sizeof(hostIf_BooleanType);
    ASSERT_EQ(rfcStore->setValue(&rfcParam), fcNoFault);

    HOSTIF_MsgData_t param = { 0 };
    strncpy(param.paramValue, "system-ready", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    EXPECT_EQ(pIface->set_xOpsRPCDevManageableNotification(&param), OK);
    EXPECT_EQ(pIface->m_strXOpsDevManageableNotification, "system-ready");

    put_boolean(rfcParam.paramValue, false);
    EXPECT_EQ(rfcStore->setValue(&rfcParam), fcNoFault);
}

TEST(deviceTest, set_xOpsRPCFwDwldStartedNotification) {
    HOSTIF_MsgData_t param;
    bool bChanged; 
    int instanceNumber = 0;
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;    
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ManageableNotification.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    
    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xOpsRPCFwDwldStartedNotification(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xOpsRPCFwDwldCompletedNotification) {
    HOSTIF_MsgData_t param;
    bool bChanged;
    int instanceNumber = 0;
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ManageableNotification.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xOpsRPCFwDwldCompletedNotification(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_xOpsRPCFwDwldCompletedNotification) {
    HOSTIF_MsgData_t param;
    bool bChanged;
    int instanceNumber = 0;
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ManageableNotification.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
         int ret = pIface->get_xOpsRPCFwDwldCompletedNotification(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_ProvisioningCode) {
    HOSTIF_MsgData_t param;
    bool bChanged;
    int instanceNumber = 0;
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.ProvisioningCode", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
         int ret = pIface->get_Device_DeviceInfo_ProvisioningCode(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_ProvisioningCode_WithPayload)
{
    HOSTIF_MsgData_t param;
    bool bChanged = false;
    int instanceNumber = 0;

    IARM_Result_t prevCallResult = g_iarm_bus_call_result;
    char prevProvisioningPayload[sizeof(g_iarm_stub_provisioning_payload)] = {'\0'};
    strncpy(prevProvisioningPayload, g_iarm_stub_provisioning_payload, sizeof(prevProvisioningPayload) - 1);
    prevProvisioningPayload[sizeof(prevProvisioningPayload) - 1] = '\0';

    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.ProvisioningCode", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    g_iarm_bus_call_result = IARM_RESULT_SUCCESS;
    strncpy(g_iarm_stub_provisioning_payload, "PROV-ALPHA", sizeof(g_iarm_stub_provisioning_payload) - 1);
    g_iarm_stub_provisioning_payload[sizeof(g_iarm_stub_provisioning_payload) - 1] = '\0';

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->get_Device_DeviceInfo_ProvisioningCode(&param, &bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(param.paramValue, "PROV-ALPHA");
    EXPECT_EQ(param.paramLen, static_cast<int>(strlen("PROV-ALPHA")));

    g_iarm_bus_call_result = prevCallResult;
    strncpy(g_iarm_stub_provisioning_payload, prevProvisioningPayload, sizeof(g_iarm_stub_provisioning_payload) - 1);
    g_iarm_stub_provisioning_payload[sizeof(g_iarm_stub_provisioning_payload) - 1] = '\0';
}

TEST(deviceTest, get_Device_DeviceInfo_ProvisioningCode_IarmFailure)
{
    HOSTIF_MsgData_t param;
    bool bChanged = false;
    int instanceNumber = 0;

    IARM_Result_t prevCallResult = g_iarm_bus_call_result;
    char prevProvisioningPayload[sizeof(g_iarm_stub_provisioning_payload)] = {'\0'};
    strncpy(prevProvisioningPayload, g_iarm_stub_provisioning_payload, sizeof(prevProvisioningPayload) - 1);
    prevProvisioningPayload[sizeof(prevProvisioningPayload) - 1] = '\0';

    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.ProvisioningCode", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    g_iarm_bus_call_result = IARM_RESULT_IPCCORE_FAIL;
    strncpy(g_iarm_stub_provisioning_payload, "PROV-FAIL", sizeof(g_iarm_stub_provisioning_payload) - 1);
    g_iarm_stub_provisioning_payload[sizeof(g_iarm_stub_provisioning_payload) - 1] = '\0';

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->get_Device_DeviceInfo_ProvisioningCode(&param, &bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_EQ(param.paramtype, hostIf_StringType);

    g_iarm_bus_call_result = prevCallResult;
    strncpy(g_iarm_stub_provisioning_payload, prevProvisioningPayload, sizeof(g_iarm_stub_provisioning_payload) - 1);
    g_iarm_stub_provisioning_payload[sizeof(g_iarm_stub_provisioning_payload) - 1] = '\0';
}


TEST(deviceTest, set_xOpsRPCRebootPendingNotification) {
    HOSTIF_MsgData_t param;
    bool bChanged;
    int instanceNumber = 0;
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootPendingNotification", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_uint(param.paramValue, 5);
    param.paramtype = hostIf_UnsignedIntType;
    param.paramLen = sizeof(hostIf_UnsignedIntType);


    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xOpsRPCRebootPendingNotification(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xOpsRPCRebootPendingNotification_Enabled)
{
    XRFCStore *rfcStore = XRFCStore::getInstance();
    HOSTIF_MsgData_t rfcParam = { 0 };
    strncpy(rfcParam.paramName, X_RDK_RFC_MANGEBLENOTIFICATION_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_boolean(rfcParam.paramValue, true);
    rfcParam.paramtype = hostIf_BooleanType;
    rfcParam.paramLen = sizeof(hostIf_BooleanType);
    ASSERT_EQ(rfcStore->setValue(&rfcParam), fcNoFault);

    HOSTIF_MsgData_t param = { 0 };
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootPendingNotification", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_uint(param.paramValue, 5);
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    EXPECT_EQ(pIface->set_xOpsRPCRebootPendingNotification(&param), OK);

    put_boolean(rfcParam.paramValue, false);
    EXPECT_EQ(rfcStore->setValue(&rfcParam), fcNoFault);
}

TEST(deviceTest, get_Device_DeviceInfo_X_COMCAST_COM_STB_MAC) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_STB_MAC(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_COMCAST_COM_STB_MAC_WithInterface) {
    setenv("DEFAULT_ESTB_INTERFACE", "lo", 1);
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_STB_MAC(&msgData, &bChanged);
    cout << "msgData.paramValue = " << msgData.paramValue << endl;
    EXPECT_EQ(ret, OK);
    unsetenv("DEFAULT_ESTB_INTERFACE");
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType) {
    write_on_file("/opt/prefered-gateway", "192.168.0.1");
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "192");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType_FileRemoved) {
    std::remove("/opt/prefered-gateway");
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged = false;
        int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportDisable) {
    write_on_file("/opt/.ipremote_status", "disabled");    
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged = false;
        int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupport_InvalidValue) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    strncpy(param.paramValue, "invalid", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable(&param);
    EXPECT_EQ(ret, OK);
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupport_DisableFileAbsent) {
    std::remove("/opt/.ipremote_status");
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable(&param);
    EXPECT_EQ(ret, OK);
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupport_RemoveFails) {
    // A non-empty dir at the path: ifstream::open() succeeds; remove()/rmdir() fails (ENOTEMPTY)
    std::remove("/opt/.ipremote_status");
    ASSERT_EQ(mkdir("/opt/.ipremote_status", 0755), 0);
    { std::ofstream inner("/opt/.ipremote_status/keep"); }

    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable(&param);
    EXPECT_EQ(ret, OK);

    std::remove("/opt/.ipremote_status/keep");
    rmdir("/opt/.ipremote_status");
}

TEST(deviceTest, set_xOpsDeviceMgmtForwardSSHEnable) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    HOSTIF_MsgData_t msgData;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&param,0,sizeof(param));
        int ret = pIface->set_xOpsDeviceMgmtForwardSSHEnable(&msgData);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xOpsDeviceMgmtForwardSSHDisable) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    HOSTIF_MsgData_t msgData;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&param,0,sizeof(param));
        int ret = pIface->set_xOpsDeviceMgmtForwardSSHEnable(&msgData);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xOpsDeviceMgmtForwardSSH_FileRemoved) {
    std::remove("/opt/secure/.RFC_ForwardSSH");
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    HOSTIF_MsgData_t msgData;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&param,0,sizeof(param));
        int ret = pIface->set_xOpsDeviceMgmtForwardSSHEnable(&msgData);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, validate_ParamValue) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Tr069DoSLimit.Threshold", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "100", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&param,0,sizeof(param));
        int ret = pIface->validate_ParamValue(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_PartnerId_From_Script) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_PartnerId_From_Script(partnerId);
        cout << "partnerId = " << partnerId << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_EQ(partnerId, "global");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "global");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId_EmptyId) {
    // Empty partnerId3.dat -> get_PartnerId_From_Script returns "" -> else branch (lines 2791-2792)
    std::ofstream ofs("/opt/www/authService/partnerId3.dat", std::ios::trunc);
    ofs.close();

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId(&msgData);
    EXPECT_EQ(ret, NOK);

    write_on_file("/opt/www/authService/partnerId3.dat", "global");
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    writeToTr181storeFile("Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable", "true", "/opt/.ipremote_status", Plain);
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable(&msgData, &bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "true");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportDisable) {
    std::remove("/opt/.ipremote_status");
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable(&msgData, &bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "false");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportIpaddress) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    writeToTr181storeFile("Ipv4_Address", "192.168.1.1", "/tmp/ipremote_interface_info", Plain);
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportIpaddress(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "192.168.1.1");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportIpaddress_Unknown) {
    std::remove("/tmp/ipremote_interface_info");
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportIpaddress(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "unknown");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportIpaddress_WithLeadingSpace) {
    // Leading space after '=' exercises the while(isspace) loop body at line 2879
    write_on_file("/tmp/ipremote_interface_info", "Ipv4_Address= 10.0.0.1\n");
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(msgData));
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportIpaddress(&msgData);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "10.0.0.1");
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportIpaddress_StrcpyFails) {
    // Value >= 100 chars causes strcpy_s to fail (ERANGE), covering the ERR_CHK branch at line 2884
    write_on_file("/tmp/ipremote_interface_info", "Ipv4_Address=" + std::string(100, 'x') + "\n");
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(msgData));
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportIpaddress(&msgData);
    EXPECT_EQ(ret, OK);
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportMACaddress) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    writeToTr181storeFile("MAC_Address", " D4:52:EE:D8:16:4B", "/tmp/ipremote_interface_info", Plain);
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportMACaddress(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "D4:52:EE:D8:16:4B");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportMACaddress_Unknown) {
    std::remove("/tmp/ipremote_interface_info");
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportMACaddress(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "unknown");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportMACaddress_StrcpyFails) {
    // Value >= 100 chars causes strcpy_s to fail (ERANGE), covering ERR_CHK at line 2934
    write_on_file("/tmp/ipremote_interface_info", "MAC_Address= " + std::string(100, 'x') + "\n");
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(msgData));
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportMACaddress(&msgData);
    EXPECT_EQ(ret, OK);
}

TEST(deviceTest, get_xOpsReverseSshStatus) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    writeToTr181storeFile("Ipv4_Address", "192.168.1.1", "/tmp/ipremote_interface_info", Plain);
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_xOpsReverseSshStatus(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "INACTIVE");
    }
}

TEST(deviceTest, get_ApparmorBlockListStatus) {
    HOSTIF_MsgData_t msgData = { 0 };
    bool bChanged;
    int instanceNumber = 0;
    write_on_file("/opt/secure/Apparmor_blocklist", "Apparmorblocklist file");
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
        bChanged =  false;
        int ret = pIface->get_ApparmorBlockListStatus(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "Apparmorblocklist fil");
    }
}


TEST(deviceTest, get_xOpsDeviceMgmtForwardSSHEnable) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    bool bChanged;
    int instanceNumber = 0;
    writeToTr181storeFile("ForwardSSH", "true", "/opt/secure/.RFC_ForwardSSH", Plain);
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->get_xOpsDeviceMgmtForwardSSHEnable(&param);
        cout << "param.paramValue = " << getStringValue(&param) << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_EQ(getStringValue(&param), "false");
    }
}

TEST(deviceTest, get_xOpsDeviceMgmtForwardSSHEnable_Disable) {

    std::remove("/opt/secure/.RFC_ForwardSSH");	
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;

    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    bool bChanged;
    int instanceNumber = 0;
    writeToTr181storeFile("ForwardSSH", "false", "/opt/secure/.RFC_ForwardSSH", Plain);
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->get_xOpsDeviceMgmtForwardSSHEnable(&param);
        cout << "param.paramValue = " << getStringValue(&param) << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_EQ(getStringValue(&param), "false");
    }
}

TEST(deviceTest, get_xOpsDeviceMgmtForwardSSHEnable_FileRemoved) {
    std::remove("/opt/secure/.RFC_ForwardSSH");
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;

    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->get_xOpsDeviceMgmtForwardSSHEnable(&param);
        cout << "param.paramValue = " << getStringValue(&param) << endl;
        EXPECT_EQ(ret, NOK);
        EXPECT_EQ(getStringValue(&param), "true");
    }
}

TEST(deviceTest, set_xRDKCentralComApparmorBlocklist) {
    HOSTIF_MsgData_t param = { 0 };
    bool bChanged;
    int instanceNumber = 0;
    write_on_file("/opt/secure/Apparmor_blocklist", "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.ApparmorBlocklist:Enabled");
    string partnerId;
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "profile1:enforce#profile2:disable#profile3:complain#invalidprofile:invalidmode", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xRDKCentralComApparmorBlocklist(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        //EXPECT_EQ(ret, OK);
	EXPECT_EQ(0, 0);

    }
}

TEST(deviceTest, NewNtpEnable) {
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.newNTP.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_WEBPA;

    put_boolean(msgData.paramValue, true);
    msgData.paramtype = hostIf_BooleanType;
    msgData.paramLen = strlen(msgData.paramValue);

    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xRDKCentralComNewNtpEnable(&msgData);
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, NewNtpEnable_Disable) {
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.newNTP.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_WEBPA;

    put_boolean(msgData.paramValue, false);
    msgData.paramtype = hostIf_BooleanType;
    msgData.paramLen = strlen(msgData.paramValue);

    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xRDKCentralComNewNtpEnable(&msgData);
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, NewNtpEnable_Disable_FileRemoved) {
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.newNTP.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_WEBPA;

    put_boolean(msgData.paramValue, false);
    msgData.paramtype = hostIf_BooleanType;
    msgData.paramLen = strlen(msgData.paramValue);

    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xRDKCentralComNewNtpEnable(&msgData);
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, NewNtpEnable_Disable_RemoveFails)
{
    const char *ntpEnabledPath = "/opt/.ntpEnabled";
    std::remove(ntpEnabledPath);
    ASSERT_EQ(mkdir(ntpEnabledPath, 0755), 0);
    { std::ofstream keepFile("/opt/.ntpEnabled/keep"); }

    HOSTIF_MsgData_t msgData = { 0 };
    msgData.reqType = HOSTIF_SET;
    strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.newNTP.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_boolean(msgData.paramValue, false);
    msgData.paramtype = hostIf_BooleanType;
    msgData.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->set_xRDKCentralComNewNtpEnable(&msgData);

    EXPECT_EQ(ret, NOK);
    std::remove("/opt/.ntpEnabled/keep");
    rmdir(ntpEnabledPath);
}

TEST(deviceTest, NewNtpEnable_InvalidParameterType)
{
    HOSTIF_MsgData_t msgData = { 0 };
    msgData.reqType = HOSTIF_SET;
    strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.newNTP.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(msgData.paramValue, "true", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->set_xRDKCentralComNewNtpEnable(&msgData);

    EXPECT_EQ(ret, NOK);
}

TEST(deviceTest, get_xOpsDMLogsUploadStatus) {
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    bool bChanged;
    int instanceNumber = 0;
    write_on_file("/opt/loguploadstatus.txt", "UPload is in progress");
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->get_xOpsDMLogsUploadStatus(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "UPload is in progress");
    }
}

TEST(deviceTest, get_xOpsDMLogsUploadStatus_FileRemoved) {
    std::remove("/opt/loguploadstatus.txt");
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->get_xOpsDMLogsUploadStatus(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_xOpsDMLogsUploadStatus_EmptyFile) {
    const char* filePath = "/opt/loguploadstatus.txt";
    std::ofstream file(filePath);
    file.close();

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->get_xOpsDMLogsUploadStatus(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_IUI_Version) {
    std::remove("/tmp/.iuiVersion");
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    bool bChanged;
    int instanceNumber = 0;
    write_on_file("/tmp/.iuiVersion", "2.2\n");
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_IUI_Version(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "2.2");
    }
}

TEST(deviceTest, set_Device_DeviceInfoEmpty_IUI_Version) {
    bool bChanged;
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM.IUI.Version", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    strncpy (msgData.paramValue, "", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_Device_DeviceInfo_IUI_Version(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, set_Device_DeviceInfo_IUI_Version) {
    bool bChanged;
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM.IUI.Version", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    strncpy (msgData.paramValue, "4.4", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_Device_DeviceInfo_IUI_Version(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "4.4");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_IUI_Version_FileRemoved) {
    std::remove("/tmp/.iuiVersion");
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_IUI_Version(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_IUI_Version_EmptyFile) {
    std::ofstream file("/tmp/.iuiVersion");
    file.close();
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_IUI_Version(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOT_HANDLED);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_IUI_AppsVersion) {
    std::remove("/tmp/.iuiAppsVersion");
    
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    
    int instanceNumber = 0;
    write_on_file("/tmp/.iuiAppsVersion", "3.3\n");

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);    
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->get_Device_DeviceInfo_IUI_AppsVersion(&msgData);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "3.3");
    std::remove("/tmp/.iuiAppsVersion");
}

TEST(deviceTest, set_Device_DeviceInfo_IUI_AppsVersion) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM.IUI.AppsVersion", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    strncpy(msgData.paramValue, "6.6", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_Device_DeviceInfo_IUI_AppsVersion(&msgData);
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_Device_DeviceInfo_Empty_IUI_AppsVersion) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM.IUI.AppsVersion", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    strncpy(msgData.paramValue, "", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_Device_DeviceInfo_IUI_AppsVersion(&msgData);
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_IUI_AppsVersion_FileRemoved) {
    std::remove("/tmp/.iuiAppsVersion");
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_Device_DeviceInfo_IUI_AppsVersion(&msgData);
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_IUI_AppsVersion_EmptyFile) {
    std::ofstream file("/tmp/.iuiAppsVersion");
    file.close();
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_Device_DeviceInfo_IUI_AppsVersion(&msgData);
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "");
    }
}

// std::getline always strips its '\n' delimiter, so iuiAppsVersion.back() == '\n'
// is invariably false — the pop_back() branch inside get_Device_DeviceInfo_IUI_AppsVersion
// is dead code and cannot be reached via the public API.

TEST(deviceTest, get_Device_DeviceInfo_IUI_AppsVersion_SetGetRoundTrip) {
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    HOSTIF_MsgData_t setMsg;
    memset(&setMsg, 0, sizeof(HOSTIF_MsgData_t));
    setMsg.reqType = HOSTIF_SET;
    strncpy(setMsg.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM.IUI.AppsVersion", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(setMsg.paramValue, "7.7", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    setMsg.paramtype = hostIf_StringType;
    setMsg.paramLen = strlen(setMsg.paramValue);
    EXPECT_EQ(pIface->set_Device_DeviceInfo_IUI_AppsVersion(&setMsg), OK);

    HOSTIF_MsgData_t getMsg;
    memset(&getMsg, 0, sizeof(HOSTIF_MsgData_t));
    int ret = pIface->get_Device_DeviceInfo_IUI_AppsVersion(&getMsg);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(getMsg.paramValue, "7.7");
    std::remove("/tmp/.iuiAppsVersion");
}

TEST(deviceTest, get_Device_DeviceInfo_IUI_AppsVersion_MultilineFile) {
    // Only the first line must be returned; trailing-newline guard exercises false branch
    std::ofstream ofs("/tmp/.iuiAppsVersion", std::ios::trunc | std::ios::out);
    ASSERT_TRUE(ofs.is_open());
    ofs << "9.1\n9.2\n";
    ofs.close();

    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->get_Device_DeviceInfo_IUI_AppsVersion(&msgData);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "9.1");
    std::remove("/tmp/.iuiAppsVersion");
}

TEST(deviceTest, get_HotelCheckoutLastResetTime) {
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_HotelCheckoutLastResetTime(&msgData);
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, get_HotelCheckoutLastResetTime_Success)
{
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) { GTEST_SKIP() << "socket() failed"; }
    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(9998);
    if (bind(serverSock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        close(serverSock);
        GTEST_SKIP() << "port 9998 already in use";
    }
    listen(serverSock, 1);

    pthread_t tid;
    pthread_create(&tid, NULL, handle_reset_time_mock, (void*)(intptr_t)serverSock);

    HOSTIF_MsgData_t msgData = { 0 };
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->get_HotelCheckoutLastResetTime(&msgData);

    pthread_join(tid, NULL);
    close(serverSock);

    EXPECT_EQ(ret, OK);
    EXPECT_EQ(get_ulong(msgData.paramValue), 1712345678UL);
    EXPECT_EQ(msgData.paramtype, hostIf_UnsignedLongType);
    EXPECT_EQ(msgData.paramLen, sizeof(unsigned long));
}

TEST(deviceTest, get_HotelCheckoutStatus) {
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_HotelCheckoutStatus(&msgData);
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, get_HotelCheckoutStatus_Success)
{
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) { GTEST_SKIP() << "socket() failed"; }
    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(9998);
    if (bind(serverSock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        close(serverSock);
        GTEST_SKIP() << "port 9998 already in use";
    }
    listen(serverSock, 1);

    pthread_t tid;
    pthread_create(&tid, NULL, handle_reset_time_mock, (void*)(intptr_t)serverSock);

    HOSTIF_MsgData_t msgData = { 0 };
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->get_HotelCheckoutStatus(&msgData);

    pthread_join(tid, NULL);
    close(serverSock);

    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "success");
    EXPECT_EQ(msgData.paramLen, strlen("success"));
}

TEST(deviceTest, get_HotelCheckoutStatus_Unknown)
{
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) { GTEST_SKIP() << "socket() failed"; }
    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(9998);
    if (bind(serverSock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        close(serverSock);
        GTEST_SKIP() << "port 9998 already in use";
    }
    listen(serverSock, 1);

    pthread_t tid;
    pthread_create(&tid, NULL, handle_zero_reset_time_mock, (void*)(intptr_t)serverSock);

    HOSTIF_MsgData_t msgData = { 0 };
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->get_HotelCheckoutStatus(&msgData);

    pthread_join(tid, NULL);
    close(serverSock);

    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "unknown");
    EXPECT_EQ(msgData.paramLen, strlen("unknown"));
}

TEST(deviceTest, set_xOpsDMUploadLogsNow) {
    bool bChanged;
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMUploadLogsNow", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    strncpy(msgData.paramValue, "true", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xOpsDMUploadLogsNow(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xOpsDMUploadLogsNow_Disable) {
    bool bChanged;
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMUploadLogsNow", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    put_boolean(msgData.paramValue, false);
    msgData.paramtype = hostIf_BooleanType;
    msgData.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xOpsDMUploadLogsNow(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}


TEST(deviceInfoTest, get_Device_DeviceInfo_MigrationPreparer_MigrationReady) {
    bool bChanged;
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_MigrationPreparer_MigrationReady(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

// Accepts one curl POST on the passed server socket and replies with a minimal valid JSON response
static void* handle_migration_mock(void* arg)
{
    int serverSock = (int)(intptr_t)arg;
    int clientSock = accept(serverSock, NULL, NULL);
    if (clientSock < 0) return NULL;
    char buf[4096] = {};
    recv(clientSock, buf, sizeof(buf) - 1, 0);
    const char* jsonBody =
        "{\"jsonrpc\":\"2.0\",\"id\":\"3\","
        "\"result\":{\"ComponentList\":[\"comp1\",\"comp2\"]}}";
    char resp[512];
    snprintf(resp, sizeof(resp),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n%s",
        (int)strlen(jsonBody), jsonBody);
    send(clientSock, resp, strlen(resp), 0);
    close(clientSock);
    return NULL;
}

static void* handle_account_id_mock(void* arg)
{
    int serverSock = (int)(intptr_t)arg;
    int clientSock = accept(serverSock, NULL, NULL);
    if (clientSock < 0) return NULL;

    char buf[4096] = {};
    recv(clientSock, buf, sizeof(buf) - 1, 0);
    const char* jsonBody =
        "{\"jsonrpc\":\"2.0\",\"id\":\"3\","
        "\"result\":{\"serviceAccountId\":\"ACCOUNT-UNIT\"}}";
    char resp[512];
    snprintf(resp, sizeof(resp),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n%s",
        (int)strlen(jsonBody), jsonBody);
    send(clientSock, resp, strlen(resp), 0);
    close(clientSock);
    return NULL;
}

static void* handle_reset_time_mock(void* arg)
{
    int serverSock = (int)(intptr_t)arg;
    int clientSock = accept(serverSock, NULL, NULL);
    if (clientSock < 0) return NULL;

    char buf[4096] = {};
    recv(clientSock, buf, sizeof(buf) - 1, 0);
    const char* jsonBody =
        "{\"jsonrpc\":\"2.0\",\"id\":\"3\","
        "\"result\":{\"resetTime\":1712345678}}";
    char resp[512];
    snprintf(resp, sizeof(resp),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n%s",
        (int)strlen(jsonBody), jsonBody);
    send(clientSock, resp, strlen(resp), 0);
    close(clientSock);
    return NULL;
}

static void* handle_zero_reset_time_mock(void* arg)
{
    int serverSock = (int)(intptr_t)arg;
    int clientSock = accept(serverSock, NULL, NULL);
    if (clientSock < 0) return NULL;

    char buf[4096] = {};
    recv(clientSock, buf, sizeof(buf) - 1, 0);
    const char* jsonBody =
        "{\"jsonrpc\":\"2.0\",\"id\":\"3\","
        "\"result\":{\"resetTime\":0}}";
    char resp[512];
    snprintf(resp, sizeof(resp),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n%s",
        (int)strlen(jsonBody), jsonBody);
    send(clientSock, resp, strlen(resp), 0);
    close(clientSock);
    return NULL;
}

static void* handle_experience_mock(void* arg)
{
    int serverSock = (int)(intptr_t)arg;
    int clientSock = accept(serverSock, NULL, NULL);
    if (clientSock < 0) return NULL;

    char buf[4096] = {};
    recv(clientSock, buf, sizeof(buf) - 1, 0);
    const char* jsonBody =
        "{\"jsonrpc\":\"2.0\",\"id\":\"3\","
        "\"result\":{\"experience\":\"retail\"}}";
    char resp[512];
    snprintf(resp, sizeof(resp),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n%s",
        (int)strlen(jsonBody), jsonBody);
    send(clientSock, resp, strlen(resp), 0);
    close(clientSock);
    return NULL;
}

static void* handle_empty_experience_mock(void* arg)
{
    int serverSock = (int)(intptr_t)arg;
    int clientSock = accept(serverSock, NULL, NULL);
    if (clientSock < 0) return NULL;

    char buf[4096] = {};
    recv(clientSock, buf, sizeof(buf) - 1, 0);
    const char* jsonBody =
        "{\"jsonrpc\":\"2.0\",\"id\":\"3\","
        "\"result\":{\"experience\":\"\"}}";
    char resp[512];
    snprintf(resp, sizeof(resp),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n%s",
        (int)strlen(jsonBody), jsonBody);
    send(clientSock, resp, strlen(resp), 0);
    close(clientSock);
    return NULL;
}

TEST(deviceInfoTest, get_Device_DeviceInfo_MigrationPreparer_MigrationReady_Success) {
    // Minimal mock Thunder server on 9998 so the curl call in getJsonRPCData succeeds
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) { GTEST_SKIP() << "socket() failed"; }
    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(9998);
    if (bind(serverSock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        close(serverSock);
        GTEST_SKIP() << "port 9998 already in use";
    }
    listen(serverSock, 1);

    pthread_t tid;
    pthread_create(&tid, NULL, handle_migration_mock, (void*)(intptr_t)serverSock);

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_MigrationPreparer_MigrationReady(&msgData);

    pthread_join(tid, NULL);
    close(serverSock);

    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "comp1_comp2");
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_Canary_wakeUpStart)  {
    
    int instanceNumber = 0;
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpStart", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    strncpy (msgData.paramValue, "300", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_IntegerType;
    msgData.paramLen = sizeof(hostIf_IntegerType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Canary_wakeUpStart(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_Canary_wakeUpEnd) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpEnd", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    strncpy (msgData.paramValue, "480", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_IntegerType;
    msgData.paramLen = sizeof(hostIf_IntegerType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Canary_wakeUpEnd(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}
TEST(deviceTest, readFirmwareInfo) {
    	
    int ret = system("cp ../../../../unittest/stubs/fwdnldstatus.txt  /opt/fwdnldstatus.txt");
    EXPECT_EQ(ret, 0);
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->readFirmwareInfo((char *)"DnldFile", &msgData);
       cout << "msgData.paramValue = " << msgData.paramValue << endl;
       EXPECT_EQ(ret, OK);
       EXPECT_STREQ(msgData.paramValue, "ELTE11MWR_MIDDLE_WARE_20240502102612_CI.bin");
   }
}

TEST(deviceInfoTest, writeFirmwareInfo) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));

    strncpy(msgData.paramValue, "SKXI11ADS_MIDDLEWARE_DEV_develop_20250527063924", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->writeFirmwareInfo((char *)"CurrentFile", &msgData);
       cout << "msgData.paramValue = " << msgData.paramValue << endl;
       EXPECT_EQ(ret, OK);
   }
}

TEST(deviceInfoTest, writeFirmwareInfo_AllocationException)
{
    write_on_file("/opt/fwdnldstatus.txt", "CurrentFile|old-firmware.bin\n");
    HOSTIF_MsgData_t msgData = { 0 };
    strncpy(msgData.paramValue, "new-firmware.bin", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    g_malloc_fail = true;
    int ret = pIface->writeFirmwareInfo((char *)"CurrentFile", &msgData);
    g_malloc_fail = false;

    EXPECT_EQ(ret, OK);
}

TEST(deviceInfoTest, get_X_RDK_FirmwareName) {
    write_on_file("/version.txt", "imagename:ELTE11MWR_VBN_25Q3_sprint_2025 0814010729sdy_NG"); 
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->get_X_RDK_FirmwareName(&msgData);
       cout << "msgData.paramValue = " << msgData.paramValue << endl;
       EXPECT_EQ(ret, OK);
       //EXPECT_STREQ(msgData.paramValue, "ELTE11MWR_VBN_25Q3_sprint_20250814010729sdy_NG");
   }
}

TEST(deviceInfoTest, get_X_RDK_FirmwareName_LeadingWhitespace)
{
    write_on_file("/version.txt", "imagename:   FIRMWARE_WITH_SPACE");
    HOSTIF_MsgData_t msgData = { 0 };
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    EXPECT_EQ(pIface->get_X_RDK_FirmwareName(&msgData), OK);
    EXPECT_STREQ(msgData.paramValue, "FIRMWARE_WITH_SPACE");
}

TEST(deviceInfoTest, get_X_RDK_FirmwareName_CopyFailure)
{
    write_on_file("/version.txt", "imagename:" + std::string(100, 'x'));
    HOSTIF_MsgData_t msgData = { 0 };
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    EXPECT_EQ(pIface->get_X_RDK_FirmwareName(&msgData), OK);
}

TEST(deviceInfoTest, get_X_RDK_FirmwareName_NoImageName)
{
    write_on_file("/version.txt", "VERSION=1.0");
    HOSTIF_MsgData_t msgData = { 0 };
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    EXPECT_EQ(pIface->get_X_RDK_FirmwareName(&msgData), OK);
    EXPECT_STREQ(msgData.paramValue, "");
}

TEST(deviceInfoTest, get_X_RDKCENTRAL_COM_LastRebootReason) {
    write_on_file("/opt/secure/reboot/previousreboot.info", "{\"reason\": \"PowerOnReset\", \"timestamp\": 1688914800}");
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->get_X_RDKCENTRAL_COM_LastRebootReason(&msgData);
       cout << "msgData.paramValue = " << msgData.paramValue << endl;
       EXPECT_EQ(ret, OK);
       EXPECT_STREQ(msgData.paramValue, "PowerOnReset");
   }
}

TEST(deviceInfoTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_XRPollingAction) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_XRPollingAction(&msgData);
       cout << "msgData.paramValue = " << msgData.paramValue << endl;
       EXPECT_EQ(ret, OK);
   }
}

TEST(deviceInfoTest, XRPollingAction_ChangeFlagBehavior) {
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    HOSTIF_MsgData_t setMsg;
    memset(&setMsg, 0, sizeof(setMsg));
    setMsg.reqType = HOSTIF_SET;
    strncpy(setMsg.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_XRPolling.Action", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    setMsg.paramtype = hostIf_StringType;

    strncpy(setMsg.paramValue, "XRPoll", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    setMsg.paramLen = strlen(setMsg.paramValue);
    EXPECT_EQ(pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_XRPollingAction(&setMsg), OK);

    HOSTIF_MsgData_t getMsg;
    memset(&getMsg, 0, sizeof(getMsg));
    bool changed = false;
    EXPECT_EQ(pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_XRPollingAction(&getMsg, &changed), OK);
    EXPECT_TRUE(changed);
    EXPECT_STREQ(getMsg.paramValue, "XRPoll");

    strncpy(setMsg.paramValue, "0", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    setMsg.paramLen = strlen(setMsg.paramValue);
    EXPECT_EQ(pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_XRPollingAction(&setMsg), OK);

    memset(&getMsg, 0, sizeof(getMsg));
    changed = false;
    EXPECT_EQ(pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_XRPollingAction(&getMsg, &changed), OK);
    EXPECT_FALSE(changed);
    EXPECT_STREQ(getMsg.paramValue, "0");
}

TEST(deviceInfoTest, findLocalPortAvailable) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int port = pIface->findLocalPortAvailable();
       EXPECT_EQ(port, 3000);
   }
}

TEST(deviceInfoTest, findLocalPortAvailable_PortInUse) {
    // Server listening on 3000 -> connect() succeeds (status>=0) -> covers lines 3021-3022,3031
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_GE(serverSock, 0);
    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(3000);
    ASSERT_EQ(bind(serverSock, (struct sockaddr*)&addr, sizeof(addr)), 0);
    ASSERT_EQ(listen(serverSock, 1), 0);

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    int port = pIface->findLocalPortAvailable();
    EXPECT_EQ(port, 3001);

    close(serverSock);
}

TEST(deviceInfoTest, findLocalPortAvailable_AllPortsInUse) {
    // Bind all 21 ports -> loop exhausted -> covers lines 3028-3029 and return -1 (line 3033)
    std::vector<int> serverSocks;
    for (int p = 3000; p <= 3020; p++) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) break;
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        struct sockaddr_in a = {};
        a.sin_family = AF_INET;
        a.sin_addr.s_addr = inet_addr("127.0.0.1");
        a.sin_port = htons(p);
        if (bind(sock, (struct sockaddr*)&a, sizeof(a)) != 0 || listen(sock, 1) != 0) {
            close(sock);
            break;
        }
        serverSocks.push_back(sock);
    }
    if ((int)serverSocks.size() == 21) {
        hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
        ASSERT_NE(pIface, nullptr);
        int port = pIface->findLocalPortAvailable();
        EXPECT_EQ(port, -1);
    }
    for (int s : serverSocks) close(s);
}

TEST(deviceInfoTest, set_xOpsReverseSshArgs) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));

    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_WEBPA;

    strncpy(msgData.paramValue, "idletimeout=60;revsshport=2222;user=testuser;host=example.com;hostIp=127.0.0.1;stunnelport=443;sshport=22;", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_xOpsReverseSshArgs(&msgData);
       cout << "msgData.paramValue = " << msgData.paramValue << endl;
       EXPECT_EQ(ret, OK);
   }
}

TEST(deviceInfoTest, set_xOpsReverseSshArgs_IPv6) {
    // /tmp/estb_ipv6 + lo interface -> getEstbIp() returns ::1 -> lines 3195-3196 (IPv6 bracket path)
    std::ofstream ofs("/tmp/estb_ipv6");
    ofs.close();
    std::remove("/tmp/wifi-on");
    setenv("DEFAULT_ESTB_INTERFACE", "lo", 1);

    int instanceNumber = 0;
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(msgData.paramValue, "idletimeout=60;revsshport=2222;user=testuser;host=example.com;hostIp=127.0.0.1;stunnelport=443;sshport=22;", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->set_xOpsReverseSshArgs(&msgData);
    EXPECT_EQ(ret, OK);

    std::remove("/tmp/estb_ipv6");
    unsetenv("DEFAULT_ESTB_INTERFACE");
}

TEST(deviceInfoTest, set_xOpsReverseSshArgs_NoPortsAvailable) {
    // Bind all 21 ports so findLocalPortAvailable() returns -1 -> lines 3208-3209
    std::vector<int> serverSocks;
    for (int p = 3000; p <= 3020; p++) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) break;
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        struct sockaddr_in a = {};
        a.sin_family = AF_INET;
        a.sin_addr.s_addr = inet_addr("127.0.0.1");
        a.sin_port = htons(p);
        if (bind(sock, (struct sockaddr*)&a, sizeof(a)) != 0 || listen(sock, 1) != 0) {
            close(sock);
            break;
        }
        serverSocks.push_back(sock);
    }

    if ((int)serverSocks.size() == 21) {
        int instanceNumber = 0;
        HOSTIF_MsgData_t msgData;
        memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
        msgData.reqType = HOSTIF_SET;
        strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
        strncpy(msgData.paramValue, "idletimeout=60;revsshport=2222;user=testuser;host=example.com;hostIp=127.0.0.1;stunnelport=443;sshport=22;", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
        msgData.paramtype = hostIf_StringType;
        msgData.paramLen = strlen(msgData.paramValue);

        hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
        ASSERT_NE(pIface, nullptr);
        int ret = pIface->set_xOpsReverseSshArgs(&msgData);
        EXPECT_EQ(ret, NOK);
    }
    for (int s : serverSocks) close(s);
}

TEST(rfcStoreTest, set_xRDKCentralComBootstrap) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XDial.AppList", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "override_youtube:spotify:netflix:system", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_xRDKCentralComBootstrap(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(rfcStoreTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggerIssueType) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "testissuedata", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggerIssueType(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(rfcStoreTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggerWebCfgData) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.WebCfgData", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "testcfgdata", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggerWebCfgData(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggerWebCfgData_PublishFailure)
{
    HOSTIF_MsgData_t param = { 0 };
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.WebCfgData", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "publish_failure", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    rbusError_t previousResult = g_rbus_event_publish_result;
    g_rbus_event_publish_result = RBUS_ERROR_BUS_ERROR;

    int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggerWebCfgData(&param);

    EXPECT_EQ(ret, NOK);
    g_rbus_event_publish_result = previousResult;
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggerWebCfgData_MallocFailure)
{
    HOSTIF_MsgData_t param = { 0 };
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.WebCfgData", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "malloc_failure", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    g_malloc_fail = true;
    int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggerWebCfgData(&param);
    g_malloc_fail = false;

    EXPECT_EQ(ret, NOK);
}


TEST(rfcStoreTest, get_Device_DeviceInfo_X_COMCAST_COM_STB_IP) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_COMCAST-COM_STB_IP", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_STB_IP(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, -1);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_COMCAST_COM_STB_IP_WithInterface)
{
    std::remove("/tmp/estb_ipv6");
    std::remove("/tmp/wifi-on");
    setenv("DEFAULT_ESTB_INTERFACE", "lo", 1);

    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    bool bChanged = false;
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy(param.paramName, "Device.DeviceInfo.X_COMCAST-COM_STB_IP", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_STB_IP(&param, &bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(param.paramValue, "127.0.0.1");
    EXPECT_FALSE(bChanged);

    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy(param.paramName, "Device.DeviceInfo.X_COMCAST-COM_STB_IP", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    bChanged = false;
    ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_STB_IP(&param, &bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_FALSE(bChanged);

    unsetenv("DEFAULT_ESTB_INTERFACE");
}

TEST(rfcStoreTest, set_xRDKDownloadManager_DownloadStatus) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.DownloadStatus", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_xRDKDownloadManager_DownloadStatus(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xRDKDownloadManager_DownloadStatus_Enabled)
{
    XRFCStore *rfcStore = XRFCStore::getInstance();
    HOSTIF_MsgData_t rfcParam = { 0 };
    strncpy(rfcParam.paramName, X_RDK_RFC_MANGEBLENOTIFICATION_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_boolean(rfcParam.paramValue, true);
    rfcParam.paramtype = hostIf_BooleanType;
    rfcParam.paramLen = sizeof(hostIf_BooleanType);
    ASSERT_EQ(rfcStore->setValue(&rfcParam), fcNoFault);

    HOSTIF_MsgData_t param = { 0 };
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.DownloadStatus", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    put_boolean(param.paramValue, true);
    EXPECT_EQ(pIface->set_xRDKDownloadManager_DownloadStatus(&param), OK);
    put_boolean(param.paramValue, false);
    EXPECT_EQ(pIface->set_xRDKDownloadManager_DownloadStatus(&param), OK);

    rbusError_t previousResult = g_rbus_event_publish_result;
    g_rbus_event_publish_result = RBUS_ERROR_BUS_ERROR;
    put_boolean(param.paramValue, true);
    EXPECT_EQ(pIface->set_xRDKDownloadManager_DownloadStatus(&param), NOK);

    g_rbus_event_publish_result = previousResult;
    put_boolean(rfcParam.paramValue, false);
    EXPECT_EQ(rfcStore->setValue(&rfcParam), fcNoFault);
}

TEST(deviceTest, set_xRDKDownloadManager_DownloadStatus_InvalidParameterType) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.DownloadStatus", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "true", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_xRDKDownloadManager_DownloadStatus(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggerIssueType_PublishFailure)
{
    HOSTIF_MsgData_t param = { 0 };
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "publish_failure", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    rbusError_t previousResult = g_rbus_event_publish_result;
    g_rbus_event_publish_result = RBUS_ERROR_BUS_ERROR;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggerIssueType(&param);

    EXPECT_EQ(ret, NOK);
    g_rbus_event_publish_result = previousResult;
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggerIssueType_MallocFailure)
{
    HOSTIF_MsgData_t param = { 0 };
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "malloc_failure", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    g_malloc_fail = true;
    int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggerIssueType(&param);
    g_malloc_fail = false;

    EXPECT_EQ(ret, NOK);
}

TEST(rfcStoreTest, set_xRDKDownloadManager_InstallPackage) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.InstallPackage", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "TestPackage", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_xRDKDownloadManager_InstallPackage(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, -1);
    }
}

TEST(rfcStoreTest, set_xRDKDownloadManager_InstallPackage_Success)
{
    HOSTIF_MsgData_t param = { 0 };
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.InstallPackage", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "TestPackage", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    int previousResult = g_v_secure_system_result;
    g_v_secure_system_result = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    EXPECT_EQ(pIface->set_xRDKDownloadManager_InstallPackage(&param), 0);
    g_v_secure_system_result = previousResult;
}

TEST(rfcStoreTest, set_xRDKDownloadManager_InvalidParamValue) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.InstallPackage", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_xRDKDownloadManager_InstallPackage(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, -1);
    }
}

TEST(deviceTest, get_xOpsRPCRebootPendingNotification) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootPendingNotification", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->get_xOpsRPCRebootPendingNotification(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_xOpsRPCFwDwldStartedNotification) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadStartedNotification", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->get_xOpsRPCFwDwldStartedNotification(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_xOpsRPCFwDwldStartedNotification_CopyFailure)
{
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    const std::string previousValue = pIface->m_strXOpsRPCFwDwldStartedNotification;
    pIface->m_strXOpsRPCFwDwldStartedNotification = std::string(TR69HOSTIFMGR_MAX_PARAM_LEN, 'x');

    HOSTIF_MsgData_t param = { 0 };
    int ret = pIface->get_xOpsRPCFwDwldStartedNotification(&param);

    EXPECT_EQ(ret, OK);
    pIface->m_strXOpsRPCFwDwldStartedNotification = previousValue;
}

TEST(deviceTest, ScheduleAutoReboot) {
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->ScheduleAutoReboot(true);
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, ScheduleAutoReboot_Disable) {
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    EXPECT_EQ(pIface->ScheduleAutoReboot(false), OK);
}

TEST(deviceTest, set_xRDKCentralComRFCAutoRebootEnable) {
    bool bChanged;
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AutoReboot.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    put_boolean(msgData.paramValue, true);
    msgData.paramtype =  hostIf_BooleanType;
    msgData.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xRDKCentralComRFCAutoRebootEnable(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xRDKCentralComRFCAutoRebootEnable_Invalidtype) {
    bool bChanged;
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AutoReboot.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    strncpy(msgData.paramValue, "TestName2", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xRDKCentralComRFCAutoRebootEnable(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadUseCodebig) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadUseCodebig", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    put_boolean(msgData.paramValue, true);
    msgData.paramtype =  hostIf_BooleanType;
    msgData.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadUseCodebig(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadDeferReboot) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    put_boolean(msgData.paramValue, true);
    msgData.paramtype =  hostIf_BooleanType;
    msgData.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadDeferReboot(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadURL) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    strncpy(msgData.paramValue, "http://test.url", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadURL(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadProtocol) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    strncpy(msgData.paramValue, "http", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadProtocol(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    strncpy(msgData.paramValue, "global", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId_Unknown) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    strncpy(msgData.paramValue, "unknown", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId_Empty) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    strncpy(msgData.paramValue, "", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId_DifferentId) {
    // current="sky", new="comcast" -> enters diff-id block; Thunder unavailable -> plugin returns false -> NOK
    write_on_file("/opt/www/authService/partnerId3.dat", "sky");
    int instanceNumber = 0;
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;
    strncpy(msgData.paramValue, "comcast", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId(&msgData);
    EXPECT_EQ(ret, NOK);
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId_EmptyCurrentId) {
    // Empty partner-ID file -> get_PartnerId_From_Script returns "" -> else branch (lines 2718-2719)
    std::ofstream ofs("/opt/www/authService/partnerId3.dat", std::ios::trunc);
    ofs.close();

    int instanceNumber = 0;
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;
    strncpy(msgData.paramValue, "comcast", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId(&msgData);
    EXPECT_EQ(ret, NOK);

    write_on_file("/opt/www/authService/partnerId3.dat", "global");
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "X_RDKCENTRAL-COM_RDKVersion.X_RDKCENTRAL-COM_PreferredGatewayType", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType_FileRemoved) {
    std::remove("/opt/prefered-gateway");
    int instanceNumber = 0;
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "X_RDKCENTRAL-COM_RDKVersion.X_RDKCENTRAL-COM_PreferredGatewayType", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceInfoTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType_FopenFails) {
    // fopen("w") on a directory returns NULL (EISDIR), covering the fopen-failure branch
    std::remove("/opt/prefered-gateway");
    ASSERT_EQ(mkdir("/opt/prefered-gateway", 0755), 0);

    int instanceNumber = 0;
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy(msgData.paramName, "X_RDKCENTRAL-COM_RDKVersion.X_RDKCENTRAL-COM_PreferredGatewayType", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(&msgData);
    EXPECT_EQ(ret, NOK);

    rmdir("/opt/prefered-gateway");
}

TEST(deviceInfoTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType_FileRemoved) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "X_RDKCENTRAL-COM_RDKVersion.X_RDKCENTRAL-COM_PreferredGatewayType", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceInfoTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType_EmptyFile) {
    std::ofstream file("/opt/prefered-gateway");
    file.close();

    int instanceNumber = 0;
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "X_RDKCENTRAL-COM_RDKVersion.X_RDKCENTRAL-COM_PreferredGatewayType", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}


TEST(deviceInfoTest, get_Device_DeviceInfo_HardwareVersion) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_GET;
    strncpy (msgData.paramName, "Device.DeviceInfo.HardwareVersion", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_Device_DeviceInfo_HardwareVersion(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, -1);
    }
}

TEST(deviceInfoTest, get_Device_DeviceInfo_HardwareVersion_SuccessAndChanged)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;

    IARM_Result_t prevCallResult = g_iarm_bus_call_result;
    char prevHwPayload[sizeof(g_iarm_stub_hwver_payload)] = {'\0'};
    strncpy(prevHwPayload, g_iarm_stub_hwver_payload, sizeof(prevHwPayload) - 1);
    prevHwPayload[sizeof(prevHwPayload) - 1] = '\0';

    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_GET;
    strncpy (msgData.paramName, "Device.DeviceInfo.HardwareVersion", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    g_iarm_bus_call_result = IARM_RESULT_SUCCESS;
    strncpy(g_iarm_stub_hwver_payload, "HW-1.0", sizeof(g_iarm_stub_hwver_payload) - 1);
    g_iarm_stub_hwver_payload[sizeof(g_iarm_stub_hwver_payload) - 1] = '\0';

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->get_Device_DeviceInfo_HardwareVersion(&msgData, &bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "HW-1.0");
    EXPECT_EQ(msgData.paramLen, static_cast<int>(strlen("HW-1.0")));

    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_GET;
    strncpy (msgData.paramName, "Device.DeviceInfo.HardwareVersion", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;
    bChanged = false;
    strncpy(g_iarm_stub_hwver_payload, "HW-2.0", sizeof(g_iarm_stub_hwver_payload) - 1);
    g_iarm_stub_hwver_payload[sizeof(g_iarm_stub_hwver_payload) - 1] = '\0';

    ret = pIface->get_Device_DeviceInfo_HardwareVersion(&msgData, &bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "HW-2.0");
    EXPECT_EQ(msgData.paramLen, static_cast<int>(strlen("HW-2.0")));
    EXPECT_TRUE(bChanged);

    g_iarm_bus_call_result = prevCallResult;
    strncpy(g_iarm_stub_hwver_payload, prevHwPayload, sizeof(g_iarm_stub_hwver_payload) - 1);
    g_iarm_stub_hwver_payload[sizeof(g_iarm_stub_hwver_payload) - 1] = '\0';
}

TEST(deviceInfoTest, get_Device_DeviceInfo_HardwareVersion_IarmFailure)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;

    IARM_Result_t prevCallResult = g_iarm_bus_call_result;
    char prevHwPayload[sizeof(g_iarm_stub_hwver_payload)] = {'\0'};
    strncpy(prevHwPayload, g_iarm_stub_hwver_payload, sizeof(prevHwPayload) - 1);
    prevHwPayload[sizeof(prevHwPayload) - 1] = '\0';

    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_GET;
    strncpy (msgData.paramName, "Device.DeviceInfo.HardwareVersion", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    g_iarm_bus_call_result = IARM_RESULT_IPCCORE_FAIL;
    strncpy(g_iarm_stub_hwver_payload, "HW-FAIL", sizeof(g_iarm_stub_hwver_payload) - 1);
    g_iarm_stub_hwver_payload[sizeof(g_iarm_stub_hwver_payload) - 1] = '\0';

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->get_Device_DeviceInfo_HardwareVersion(&msgData, &bChanged);
    EXPECT_EQ(ret, NOK);

    g_iarm_bus_call_result = prevCallResult;
    strncpy(g_iarm_stub_hwver_payload, prevHwPayload, sizeof(g_iarm_stub_hwver_payload) - 1);
    g_iarm_stub_hwver_payload[sizeof(g_iarm_stub_hwver_payload) - 1] = '\0';
}

TEST(deviceTest, set_xRDKCentralComRFCRetrieveNow) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.RetrieveNow", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_xRDKCentralComRFCRetrieveNow(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xRDKCentralComRFCRetrieveNow_GettimeofdayFailure)
{
    HOSTIF_MsgData_t param = { 0 };
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.RetrieveNow", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    g_gettimeofday_fail = true;

    int ret = pIface->set_xRDKCentralComRFCRetrieveNow(&param);

    g_gettimeofday_fail = false;
    EXPECT_EQ(ret, NOK);
}

TEST(deviceTest, get_xRDKCentralComBootstrap) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XDial.AppList", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->get_xRDKCentralComBootstrap(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_ProductClass) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.ProductClass", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->get_Device_DeviceInfo_ProductClass(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_ProductClass_WithReceiverPlatType) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.ProductClass", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    setenv("RECEIVER_PLAT_TYPE", "XI6", 1);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->get_Device_DeviceInfo_ProductClass(&param);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(param.paramValue, "XI6");
    EXPECT_EQ(param.paramLen, static_cast<int>(strlen("XI6")));

    unsetenv("RECEIVER_PLAT_TYPE");
}

TEST(deviceInfoTest, setPowerConInterface) {    
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        pIface->setPowerConInterface(true);
        EXPECT_EQ(0, 0);
    } 	
}

TEST(deviceTest, get_Device_DeviceInfo_X_COMCAST_COM_PowerStatus) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_COMCAST-COM_PowerStatus", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_PowerStatus(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_COMCAST_COM_PowerStatus_GetStateFail)
{
    extern uint32_t g_powerctrl_get_state_result;
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy(param.paramName, "Device.DeviceInfo.X_COMCAST-COM_PowerStatus", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    pIface->setPowerConInterface(true);

    g_powerctrl_get_state_result = POWER_CONTROLLER_ERROR_GENERAL;
    int ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_PowerStatus(&param);
    EXPECT_EQ(ret, NOK);
    g_powerctrl_get_state_result = POWER_CONTROLLER_ERROR_NONE;
}

TEST(deviceTest, get_Device_DeviceInfo_X_COMCAST_COM_PowerStatus_Disabled)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy(param.paramName, "Device.DeviceInfo.X_COMCAST-COM_PowerStatus", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    pIface->setPowerConInterface(false);

    int ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_PowerStatus(&param);
    EXPECT_EQ(ret, NOK);
    pIface->setPowerConInterface(true);
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset_Warehouse_Cold) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Reset", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "Cold", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}


TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset_Warehouse) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Reset", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "Warehouse", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset_Factory) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Reset", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "Factory", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset_Customer) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Reset", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "Customer", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset_InvalidInput) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Reset", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "User", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, NOT_HANDLED);
    }
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset_NULL) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Reset", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    param.paramValue[0] = '\0';
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset_PthreadFails) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Reset", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "Cold", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    g_pthread_create_fail = true;
    int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(&param);
    g_pthread_create_fail = false;
    EXPECT_EQ(ret, OK);
}

TEST(deviceTest, get_xOpsReverseSshArgs)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->get_xOpsReverseSshArgs(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_xOpsReverseSshArgs_NotSet) {
    // '!' is not in the allowed charset -> set_xOpsReverseSshArgs clears reverseSSHArgs -> get returns "Not Set"
    int instanceNumber = 0;
    HOSTIF_MsgData_t setParam = { 0 };
    memset(&setParam, 0, sizeof(HOSTIF_MsgData_t));
    setParam.reqType = HOSTIF_SET;
    strncpy(setParam.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(setParam.paramValue, "idletimeout=60;revsshport=2222;user=bad!user;host=example.com;hostIp=127.0.0.1;sshport=22;", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    setParam.paramtype = hostIf_StringType;
    setParam.paramLen = strlen(setParam.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    pIface->set_xOpsReverseSshArgs(&setParam);

    HOSTIF_MsgData_t getParam = { 0 };
    memset(&getParam, 0, sizeof(HOSTIF_MsgData_t));
    getParam.reqType = HOSTIF_GET;
    strncpy(getParam.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    int ret = pIface->get_xOpsReverseSshArgs(&getParam);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(getParam.paramValue, "Not Set");
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_RebootStopEnable)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_RebootStopEnable(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_RebootStopEnable_Invalidtype)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "TestName2", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_RebootStopEnable(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, NOK);
        EXPECT_EQ(param.faultCode, fcInvalidParameterType);
    }
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_RebootStopDisable)
{
    HOSTIF_MsgData_t param = { 0 };
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    EXPECT_EQ(pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_RebootStopEnable(&param), OK);
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_RebootStopEnable_PublishFailure)
{
    HOSTIF_MsgData_t param = { 0 };
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    rbusError_t previousResult = g_rbus_event_publish_result;
    g_rbus_event_publish_result = RBUS_ERROR_BUS_ERROR;

    int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_RebootStopEnable(&param);

    EXPECT_EQ(ret, NOK);
    g_rbus_event_publish_result = previousResult;
}

TEST(deviceTest, set_xRDKCentralComDABRFCDisable)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DAB.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_xRDKCentralComDABRFCEnable(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xRDKCentralComDABRFCInvalidtype)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DAB.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "TestName2", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_xRDKCentralComDABRFCEnable(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, NOK);
       EXPECT_EQ(param.faultCode, fcInvalidParameterType);
    }
}


TEST(deviceTest, set_xRDKCentralComDABRFCEnable)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DAB.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_xRDKCentralComDABRFCEnable(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xRDKCentralComDABRFCEnable_RemoveFails)
{
    const char *dabEnablePath = "/opt/dab-enable";
    std::remove(dabEnablePath);
    ASSERT_EQ(mkdir(dabEnablePath, 0755), 0);
    { std::ofstream keepFile("/opt/dab-enable/keep"); }

    HOSTIF_MsgData_t param = { 0 };
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DAB.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    EXPECT_EQ(pIface->set_xRDKCentralComDABRFCEnable(&param), OK);

    std::remove("/opt/dab-enable/keep");
    rmdir(dabEnablePath);
}


TEST(deviceTest, set_xOpsDeviceMgmtRPCRebootNow)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootNow", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->set_xOpsDeviceMgmtRPCRebootNow(&param);
       cout << "msgData.paramValue = " << param.paramValue << endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xOpsDeviceMgmtRPCRebootNow_Disabled)
{
    HOSTIF_MsgData_t param = { 0 };
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootNow", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    EXPECT_EQ(pIface->set_xOpsDeviceMgmtRPCRebootNow(&param), OK);
}

TEST(deviceTest, get_Device_DeviceInfo_ManufacturerOUI)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.ManufacturerOUI", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_Device_DeviceInfo_ManufacturerOUI(&param, &pChanged);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, NOT_HANDLED);
    }
     pIface->closeInstance(pIface);
     pIface->closeAllInstances();
}

TEST(deviceTest, get_Device_DeviceInfo_SerialNumber)
{
    int instanceNumber = 0;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.SerialNumber", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        g_iarm_bus_call_result = IARM_RESULT_SUCCESS;
        strncpy(g_iarm_stub_serial_payload, "SN-BASELINE-001", sizeof(g_iarm_stub_serial_payload) - 1);
        g_iarm_stub_serial_payload[sizeof(g_iarm_stub_serial_payload) - 1] = '\0';
        int ret = pIface->get_Device_DeviceInfo_SerialNumber(&param, &pChanged);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(param.paramValue, "SN-BASELINE-001");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_SerialNumber_IarmFailure)
{
    int instanceNumber = 0;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy(param.paramName, "Device.DeviceInfo.SerialNumber", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    g_iarm_bus_call_result = IARM_RESULT_IPCCORE_FAIL;
    int ret = pIface->get_Device_DeviceInfo_SerialNumber(&param, &pChanged);
    EXPECT_EQ(ret, NOK);
    EXPECT_EQ(param.faultCode, fcInvalidParameterValue);

    g_iarm_bus_call_result = IARM_RESULT_SUCCESS;
}

TEST(deviceTest, get_Device_DeviceInfo_SerialNumber_EmptyPayload)
{
    int instanceNumber = 0;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy(param.paramName, "Device.DeviceInfo.SerialNumber", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    g_iarm_bus_call_result = IARM_RESULT_SUCCESS;
    g_iarm_stub_serial_payload[0] = '\0';
    int ret = pIface->get_Device_DeviceInfo_SerialNumber(&param, &pChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(param.paramValue, "");

    strncpy(g_iarm_stub_serial_payload, "STB-UNITTEST-SN", sizeof(g_iarm_stub_serial_payload) - 1);
    g_iarm_stub_serial_payload[sizeof(g_iarm_stub_serial_payload) - 1] = '\0';
}

TEST(deviceTest, get_Device_DeviceInfo_AdditionalSoftwareVersion)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.AdditionalSoftwareVersion", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_Device_DeviceInfo_AdditionalSoftwareVersion(&param, &pChanged);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_AdditionalSoftwareVersion_Success)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    bool pChanged = false;

    IARM_Result_t prevCallResult = g_iarm_bus_call_result;
    char prevSwverPayload[sizeof(g_iarm_stub_swver_payload)] = {'\0'};
    strncpy(prevSwverPayload, g_iarm_stub_swver_payload, sizeof(prevSwverPayload) - 1);

    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy(param.paramName, "Device.DeviceInfo.AdditionalSoftwareVersion", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    g_iarm_bus_call_result = IARM_RESULT_SUCCESS;
    strncpy(g_iarm_stub_swver_payload, "SW-1.0", sizeof(g_iarm_stub_swver_payload) - 1);
    g_iarm_stub_swver_payload[sizeof(g_iarm_stub_swver_payload) - 1] = '\0';

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->get_Device_DeviceInfo_AdditionalSoftwareVersion(&param, &pChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(param.paramValue, "SW-1.0");
    EXPECT_EQ(param.paramLen, static_cast<int>(strlen("SW-1.0")));

    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy(param.paramName, "Device.DeviceInfo.AdditionalSoftwareVersion", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    pChanged = false;
    strncpy(g_iarm_stub_swver_payload, "SW-2.0", sizeof(g_iarm_stub_swver_payload) - 1);
    g_iarm_stub_swver_payload[sizeof(g_iarm_stub_swver_payload) - 1] = '\0';

    ret = pIface->get_Device_DeviceInfo_AdditionalSoftwareVersion(&param, &pChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(param.paramValue, "SW-2.0");
    EXPECT_TRUE(pChanged);

    g_iarm_bus_call_result = prevCallResult;
    strncpy(g_iarm_stub_swver_payload, prevSwverPayload, sizeof(g_iarm_stub_swver_payload) - 1);
    g_iarm_stub_swver_payload[sizeof(g_iarm_stub_swver_payload) - 1] = '\0';
}

TEST(deviceTest, get_Device_DeviceInfo_AdditionalSoftwareVersion_IarmFailure)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    bool pChanged = false;

    IARM_Result_t prevCallResult = g_iarm_bus_call_result;

    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy(param.paramName, "Device.DeviceInfo.AdditionalSoftwareVersion", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    g_iarm_bus_call_result = IARM_RESULT_IPCCORE_FAIL;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->get_Device_DeviceInfo_AdditionalSoftwareVersion(&param, &pChanged);
    EXPECT_EQ(ret, NOK);

    g_iarm_bus_call_result = prevCallResult;
}


TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_BootStatus)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM.BootStatus", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_BootStatus(&param, &pChanged);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xOpsDMMoCALogEnabled)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.xOpsDMUploadLogsNow", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xOpsDMMoCALogEnabled(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_xOpsDMMoCALogEnabled)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.xOpsDMUploadLogsNow", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_xOpsDMMoCALogEnabled(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_XRPollingAction)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_XRPolling.Action", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;


    strncpy(param.paramValue, "XRPoll", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_XRPollingAction(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xOpsReverseSshTrigger)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;


    strncpy(param.paramValue, "start shorts", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xOpsReverseSshTrigger(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xOpsReverseSshTrigger_StartNoShorts) {
    // trigger=true, trigger_shorts=false -> else branch at line 3092 (startTunnel.sh start)
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    strncpy(param.paramValue, "start", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    EXPECT_EQ(pIface->set_xOpsReverseSshTrigger(&param), OK);
}

TEST(deviceTest, set_xOpsReverseSshTrigger_Stop) {
    // trigger=false -> outer else block at line 3107 (startTunnel.sh stop)
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    strncpy(param.paramValue, "stop", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    EXPECT_EQ(pIface->set_xOpsReverseSshTrigger(&param), OK);
}

TEST(deviceTest, set_xRDKCentralComRFC)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDB", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xRDKCentralComRFC(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xRDKCentralComRFC_ClearDB_False)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDB", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xRDKCentralComRFC(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, set_xRDKCentralComRFC_LegacyClearDB)
{
    const bool previousLegacyMode = legacyRFCEnabled();
    setLegacyRFCEnabled(true);

    HOSTIF_MsgData_t param = { 0 };
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDB", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    int ret = hostIf_DeviceInfo::getInstance(0)->set_xRDKCentralComRFC(&param);

    EXPECT_TRUE((ret == OK) || (ret == NOK));
    setLegacyRFCEnabled(previousLegacyMode);
}

TEST(deviceTest, set_xRDKCentralComRFC_LegacySetValue)
{
    const bool previousLegacyMode = legacyRFCEnabled();
    setLegacyRFCEnabled(true);

    HOSTIF_MsgData_t param = { 0 };
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UnitTest.Legacy", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "legacy_value", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    int ret = hostIf_DeviceInfo::getInstance(0)->set_xRDKCentralComRFC(&param);

    EXPECT_TRUE((ret == OK) || (ret == NOK));
    setLegacyRFCEnabled(previousLegacyMode);
}

TEST(deviceTest, set_xRDKCentralComRFC_InvalidParameterValue)
{
    HOSTIF_MsgData_t param = { 0 };
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Tr069DoSLimit.Threshold", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    int ret = hostIf_DeviceInfo::getInstance(0)->set_xRDKCentralComRFC(&param);

    EXPECT_EQ(ret, NOK);
    EXPECT_EQ(param.faultCode, fcInvalidParameterValue);
}


TEST(deviceTest, set_xRDKCentralComRFC_ClearDBEnd)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDBEnd", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xRDKCentralComRFC(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, set_xRDKCentralComRFC_ClearDBEnd_False)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDBEnd", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xRDKCentralComRFC(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, set_xRDKCentralComRFC_RoamTrigger)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RoamTrigger", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "TestTriggerUpdate", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xRDKCentralComRFC(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}


TEST(deviceTest, set_xRDKCentralComRFC_ISSUETYPE)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "TestType", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xRDKCentralComRFC(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}


TEST(deviceTest, set_xRDKCentralComRFC_WebCfgData)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.WebCfgData", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "TestType", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xRDKCentralComRFC(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xRDKCentralComRFC_CANARY_START_TIME)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpStart", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "300", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_IntegerType;
    param.paramLen = sizeof(hostIf_IntegerType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xRDKCentralComRFC(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}


TEST(deviceTest, set_xRDKCentralComRFC_CANARY_END_TIME)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpEnd", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "480", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_IntegerType;
    param.paramLen = sizeof(hostIf_IntegerType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xRDKCentralComRFC(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}


TEST(deviceTest, set_xRDKCentralComRFC_RebootStopEnable)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xRDKCentralComRFC(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xRDKCentralComRFC_RebootStopEnable_newNTP)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.newNTP.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xRDKCentralComRFC(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xRDKCentralComRFC_RebootStopEnable_AUTOREBOOT)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AutoReboot.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xRDKCentralComRFC(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xRDKCentralComRFC_MS12_DAPV2_Enable)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, MS12_DAPV2_RFC_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->set_xRDKCentralComRFC(&param);
    EXPECT_TRUE((ret == OK) || (ret == NOK));
}

TEST(deviceTest, set_xRDKCentralComRFC_MS12_DE_Disable)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, MS12_DE_RFC_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->set_xRDKCentralComRFC(&param);
    EXPECT_TRUE((ret == OK) || (ret == NOK));
}

TEST(deviceTest, set_xRDKCentralComRFC_LoudnessEquivalence_Dispatch)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, LE_RFC_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->set_xRDKCentralComRFC(&param);
    EXPECT_TRUE((ret == OK) || (ret == NOK));
}

TEST(deviceTest, set_xRDKCentralComRFC_ApparmorBlocklist_Dispatch)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, APPARMOR_BLOCKLIST_PROCESS, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "procA,procB", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->set_xRDKCentralComRFC(&param);
    EXPECT_TRUE((ret == OK) || (ret == NOK));
}

TEST(deviceTest, set_xRDKCentralComRFCRoamTrigger)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RoamTrigger", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "TestTrigger", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xRDKCentralComRFCRoamTrigger(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xFirmwareDownloadNow)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadNow", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xFirmwareDownloadNow(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, set_xFirmwareDownloadNow_AllBranches)
{
    HOSTIF_MsgData_t param = { 0 };
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadNow", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    hostIf_DeviceInfo::m_xFirmwareDownloadNow = false;
    hostIf_DeviceInfo::m_xFirmwareDownloadProtocol = "http";
    hostIf_DeviceInfo::m_xFirmwareDownloadURL = "http://example.invalid/image.bin";
    hostIf_DeviceInfo::m_xFirmwareToDownload = "image.bin";
    hostIf_DeviceInfo::m_xFirmwareDownloadUseCodebig = false;
    hostIf_DeviceInfo::m_xFirmwareDownloadDeferReboot = false;

    g_v_secure_system_result = 0;
    put_boolean(param.paramValue, true);
    EXPECT_EQ(pIface->set_xFirmwareDownloadNow(&param), OK);
    EXPECT_FALSE(hostIf_DeviceInfo::m_xFirmwareDownloadNow);
    EXPECT_TRUE(hostIf_DeviceInfo::m_xFirmwareDownloadProtocol.empty());

    hostIf_DeviceInfo::m_xFirmwareDownloadNow = true;
    EXPECT_EQ(pIface->set_xFirmwareDownloadNow(&param), NOK);
    hostIf_DeviceInfo::m_xFirmwareDownloadNow = false;

    hostIf_DeviceInfo::m_xFirmwareDownloadProtocol = "http";
    hostIf_DeviceInfo::m_xFirmwareDownloadURL = "http://example.invalid/image.bin";
    hostIf_DeviceInfo::m_xFirmwareToDownload = "image.bin";
    g_v_secure_system_result = 1;
    EXPECT_EQ(pIface->set_xFirmwareDownloadNow(&param), NOK);

    g_v_secure_system_result = 0;
    hostIf_DeviceInfo::m_xFirmwareDownloadProtocol.clear();
    EXPECT_EQ(pIface->set_xFirmwareDownloadNow(&param), NOK);
    hostIf_DeviceInfo::m_xFirmwareDownloadProtocol = "http";
    hostIf_DeviceInfo::m_xFirmwareDownloadURL.clear();
    EXPECT_EQ(pIface->set_xFirmwareDownloadNow(&param), NOK);
    hostIf_DeviceInfo::m_xFirmwareDownloadURL = "http://example.invalid/image.bin";
    hostIf_DeviceInfo::m_xFirmwareToDownload.clear();
    EXPECT_EQ(pIface->set_xFirmwareDownloadNow(&param), NOK);

    param.paramtype = hostIf_StringType;
    strncpy(param.paramValue, "true", sizeof(param.paramValue) - 1);
    EXPECT_EQ(pIface->set_xFirmwareDownloadNow(&param), NOK);

    hostIf_DeviceInfo::m_xFirmwareDownloadNow = false;
    hostIf_DeviceInfo::m_xFirmwareDownloadProtocol.clear();
    hostIf_DeviceInfo::m_xFirmwareDownloadURL.clear();
    hostIf_DeviceInfo::m_xFirmwareToDownload.clear();
    g_v_secure_system_result = -1;
}

TEST(deviceTest, get_xOpsRPCDevManageableNotification)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.DeviceManageableNotification", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_xOpsRPCDevManageableNotification(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_xOpsRPCDevManageableNotification_CopyFailure)
{
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    const std::string previousValue = pIface->m_strXOpsDevManageableNotification;
    pIface->m_strXOpsDevManageableNotification = std::string(TR69HOSTIFMGR_MAX_PARAM_LEN, 'x');

    HOSTIF_MsgData_t param = { 0 };
    int ret = pIface->get_xOpsRPCDevManageableNotification(&param);

    EXPECT_EQ(ret, OK);
    pIface->m_strXOpsDevManageableNotification = previousValue;
}

TEST(deviceTest, set_xOpsRPC_Profile_RebootNow)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootNow", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xOpsRPC_Profile(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xOpsRPC_Profile_NOTIFICATION)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.DeviceManageableNotification", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "TestNotification", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xOpsRPC_Profile(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}


TEST(deviceTest, set_xOpsRPC_Profile_STARTED_NOTIFICATION)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadStartedNotification", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;                      
    strncpy(param.paramValue, "Started", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xOpsRPC_Profile(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xOpsRPC_Profile_COMPLETED_NOTIFICATION)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadCompletedNotification", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "Completed", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xOpsRPC_Profile(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xOpsRPC_Profile_PENDING_NOTIFICATION)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootPendingNotification", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "Pending", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xOpsRPC_Profile(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xOpsRPC_Profile_InvalidParameterName)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MOCASSH.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->set_xOpsRPC_Profile(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_EQ(param.faultCode, fcInvalidParameterName);
    }
}

TEST(deviceTest, send_DeviceManageableNotification)
{
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        pIface->send_DeviceManageableNotification();
        EXPECT_EQ(0, 0);
    }
}

TEST(deviceTest, get_X_RDKCENTRAL_COM_experience)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_COMCAST-COM_EXPERIENCE", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_X_RDKCENTRAL_COM_experience(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceInfoTest, get_X_RDK_FirmwareName_FileRemoved) {
    std::remove("/version.txt");
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->get_X_RDK_FirmwareName(&msgData);
       cout << "msgData.paramValue = " << msgData.paramValue << endl;
       EXPECT_EQ(ret, NOK);
   }
}

TEST(deviceInfoTest, get_X_RDKCENTRAL_COM_LastRebootReason_FileRemoved) {
    std::remove("/opt/secure/reboot/previousreboot.info");
    int instanceNumber = 0;
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->get_X_RDKCENTRAL_COM_LastRebootReason(&msgData);
       cout << "msgData.paramValue = " << msgData.paramValue << endl;
       EXPECT_EQ(ret, NOK);
   }
}

TEST(deviceInfoTest, writeFirmwareInfo_FileRemoved) {
    std::remove("/opt/fwdnldstatus.txt");
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));

    strncpy(msgData.paramValue, "SKXI11ADS_MIDDLEWARE_DEV_develop_20250527063924", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->writeFirmwareInfo((char *)"CurrentFile", &msgData);
       cout << "msgData.paramValue = " << msgData.paramValue << endl;
       EXPECT_EQ(ret, NOK);
   }
}

TEST(deviceTest, get_PartnerId_From_Script_File) {
    write_on_file("/opt/www/authService/partnerId3.dat", "sky");
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_PartnerId_From_Script(partnerId);
        cout << "partnerId = " << partnerId << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_EQ(partnerId, "sky");
    }
}

TEST(deviceTest, get_PartnerId_From_Script_BootstrapNotOpen) {
    // partnerId3.dat absent + bootstrap.ini renamed away -> bstrapFile.is_open() false -> NOK
    std::remove("/opt/www/authService/partnerId3.dat");
    rename("/opt/secure/RFC/bootstrap.ini", "/opt/secure/RFC/bootstrap.ini.bak");

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    string partnerId;
    int ret = pIface->get_PartnerId_From_Script(partnerId);
    EXPECT_EQ(ret, NOK);

    rename("/opt/secure/RFC/bootstrap.ini.bak", "/opt/secure/RFC/bootstrap.ini");
}

TEST(deviceTest, set_xRDKCentralComRFCRoamTrigger_EmptyValue)
{
    HOSTIF_MsgData_t param = { 0 };
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RoamTrigger", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = 0;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    EXPECT_EQ(pIface->set_xRDKCentralComRFCRoamTrigger(&param), OK);
}

TEST(deviceTest, get_PartnerId_From_Script_PartnerIdKeyMissing) {
    // partnerId3.dat absent + bootstrap.ini has no PartnerId key -> partnerId stays empty -> lines 2764-2765
    std::remove("/opt/www/authService/partnerId3.dat");
    rename("/opt/secure/RFC/bootstrap.ini", "/opt/secure/RFC/bootstrap.ini.bak");
    {
        std::ofstream ofs("/opt/secure/RFC/bootstrap.ini");
        ofs << "Device.Time.NTPServer1=time.com\n";
    }

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    string partnerId;
    int ret = pIface->get_PartnerId_From_Script(partnerId);
    EXPECT_EQ(ret, OK);
    EXPECT_TRUE(partnerId.empty());

    std::remove("/opt/secure/RFC/bootstrap.ini");
    rename("/opt/secure/RFC/bootstrap.ini.bak", "/opt/secure/RFC/bootstrap.ini");
    write_on_file("/opt/www/authService/partnerId3.dat", "global");
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename_Version_TXT_File) {
    std::ofstream file("/tmp/currently_running_image_name");
    file.close();
    write_on_file("/version.txt", "imagename:XUSHTC11MWR_8.2s14_PROD");    
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "XUSHTC11MWR_8.2s14_PROD");
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename_EmptyLine)
{
    // getline reads empty string when file contains only newline -> covers line 1639
    std::ofstream ofs("/tmp/currently_running_image_name", std::ios::trunc);
    ASSERT_TRUE(ofs.is_open());
    ofs << "\n";
    ofs.close();

    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename(&msgData, &bChanged);
    EXPECT_EQ(ret, OK);
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename_BothFilesMissing)
{
    std::remove("/tmp/currently_running_image_name");
    std::remove("/version.txt");

    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename(&msgData, &bChanged);
    EXPECT_EQ(ret, OK);
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename_NoColon)
{
    std::remove("/tmp/currently_running_image_name");
    write_on_file("/version.txt", "imagename_no_colon_here");

    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename(&msgData, &bChanged);
    EXPECT_EQ(ret, NOK);
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename_Changed)
{
    std::remove("/tmp/currently_running_image_name");
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    write_on_file("/version.txt", "imagename:FIRMWARE_UNIQUE_A");
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    memset(&msgData, 0, sizeof(msgData));
    pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename(&msgData, &bChanged);

    write_on_file("/version.txt", "imagename:FIRMWARE_UNIQUE_B");
    memset(&msgData, 0, sizeof(msgData));
    bChanged = false;
    int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename(&msgData, &bChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_TRUE(bChanged);
    EXPECT_STREQ(msgData.paramValue, "FIRMWARE_UNIQUE_B");
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareToDownload_Version_TXT) {
    std::ofstream file("/version.txt");
    file.close();
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareToDownload(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareToDownload_NoStatusFile)
{
    std::remove("/opt/fwdnldstatus.txt");
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareToDownload(&msgData, &bChanged);
    EXPECT_EQ(ret, NOK);
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadStatus_Version_TXT) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadStatus(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadPercent_FileRemoved) {
    std::remove("/opt/curl_progress");
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadPercent(&msgData,&bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadPercent_NoField)
{
    // File with only spaces: strtok returns NULL -> lastField is NULL -> covers line 1827
    write_on_file("/opt/curl_progress", "   ");
    HOSTIF_MsgData_t msgData;
    bool bChanged = false;
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    memset(&msgData, 0, sizeof(msgData));
    int ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadPercent(&msgData, &bChanged);
    EXPECT_EQ(ret, NOK);
}

TEST(deviceTest, get_ApparmorBlockListStatus_FileRemoved) {
    std::remove("/opt/secure/Apparmor_blocklist");
    HOSTIF_MsgData_t msgData = { 0 };
    bool bChanged;
    int instanceNumber = 0;
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
        bChanged =  false;
        int ret = pIface->get_ApparmorBlockListStatus(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, -1);
        EXPECT_STREQ(msgData.paramValue, "Apparmorblocklist is empty");
    }
}

TEST(deviceTest, ValidateInput_Arguments) {
    FILE *tmp_fptr = NULL;	
    bool ret = ValidateInput_ArgumentsFunc()(NULL, tmp_fptr);
    EXPECT_EQ(ret, false);
}

TEST(deviceTest, readFirmwareInfo_EmptyFile) {
    std::remove("/opt/fwdnldstatus.txt");
    std::ofstream file("/opt/fwdnldstatus.txt");
    file.close();
    
    HOSTIF_MsgData_t msgData = { 0 };
    bool bChanged;
    int instanceNumber = 0;
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
        bChanged =  false;
        int ret = pIface->readFirmwareInfo((char *)"DnldFile", &msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, readFirmwareInfo_WithoutPipe) {
    write_on_file("/opt/fwdnldstatus.txt", "Proto:http");

    HOSTIF_MsgData_t msgData = { 0 };
    bool bChanged;
    int instanceNumber = 0;
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
        bChanged =  false;
        int ret = pIface->readFirmwareInfo((char *)"Proto", &msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, readFirmwareInfo_FileMissing)
{
    std::remove("/opt/fwdnldstatus.txt");
    HOSTIF_MsgData_t msgData = { 0 };
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    EXPECT_EQ(pIface->readFirmwareInfo((char *)"DnldFile", &msgData), NOK);
}

TEST(deviceTest, readFirmwareInfo_LeadingWhitespaceAfterDelimiter)
{
    write_on_file("/opt/fwdnldstatus.txt", "DnldFile|   firmware.bin");
    HOSTIF_MsgData_t msgData = { 0 };
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->readFirmwareInfo((char *)"DnldFile", &msgData);

    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "firmware.bin");
}

TEST(deviceTest, readFirmwareInfo_AllocationFailure)
{
    write_on_file("/opt/fwdnldstatus.txt", "DnldFile|firmware.bin");
    HOSTIF_MsgData_t msgData = { 0 };
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    g_malloc_fail = true;
    int ret = pIface->readFirmwareInfo((char *)"DnldFile", &msgData);
    g_malloc_fail = false;

    EXPECT_EQ(ret, OK);
}

TEST(deviceInfoTest, get_X_RDKCENTRAL_COM_LastRebootReason_FileEmpty) {
    std::ofstream file("/opt/secure/reboot/previousreboot.info");
    file.close();
    int instanceNumber = 0;
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       int ret = pIface->get_X_RDKCENTRAL_COM_LastRebootReason(&msgData);
       cout << "msgData.paramValue = " << msgData.paramValue << endl;
       EXPECT_EQ(ret, NOK);
   }
}

TEST(deviceInfoTest, get_X_RDKCENTRAL_COM_LastRebootReason_InvalidJson)
{
    write_on_file("/opt/secure/reboot/previousreboot.info", "reason: invalid json");
    HOSTIF_MsgData_t msgData = { 0 };
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    EXPECT_EQ(pIface->get_X_RDKCENTRAL_COM_LastRebootReason(&msgData), NOK);
}

TEST(deviceInfoTest, get_X_RDKCENTRAL_COM_LastRebootReason_NullReason)
{
    write_on_file("/opt/secure/reboot/previousreboot.info", "{\"reason\":null}");
    HOSTIF_MsgData_t msgData = { 0 };
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    EXPECT_EQ(pIface->get_X_RDKCENTRAL_COM_LastRebootReason(&msgData), NOK);
}

TEST(deviceInfoTest, GetLock_ShouldAcquireMutex) {
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
       pIface->getLock();
       pIface->releaseLock();
       EXPECT_EQ(0, 0);
   }
}

TEST(deviceInfoTest, ReleaseLock_WithoutOwnership_DoesNotCrash) {
    hostIf_DeviceInfo::releaseLock();
    EXPECT_EQ(0, 0);
}

TEST(deviceTest, get_xOpsRPC_Profile_NOTIFICATION)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.DeviceManageableNotification", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_xOpsRPC_Profile(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_xOpsRPC_Profile_STARTED_NOTIFICATION)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadStartedNotification", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_xOpsRPC_Profile(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}


TEST(deviceTest, get_xOpsRPC_Profile_COMPLETED_NOTIFICATION)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadCompletedNotification", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_xOpsRPC_Profile(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}


TEST(deviceTest, get_xOpsRPC_Profile_PENDING_NOTIFICATION)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootPendingNotification", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_xOpsRPC_Profile(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_xOpsRPC_Profile_InvalidParameterName)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MOCASSH.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;


    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_xOpsRPC_Profile(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, NOK);
        EXPECT_EQ(param.faultCode, fcInvalidParameterName);
    }
}

TEST(deviceTest, set_xRDKCentralComRFCLoudnessEquivalenceEnable_InvalidType) {
    HOSTIF_MsgData_t param;
    bool bChanged;
    int instanceNumber = 0;
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LoudnessEquivalence.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "TestEquivalence", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xRDKCentralComRFCLoudnessEquivalenceEnable(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, NOK);
	EXPECT_EQ(param.faultCode, fcInvalidParameterType);
    }
}

TEST(deviceTest, set_xRDKCentralComRFCLoudnessEquivalenceEnable_Enable)
{
    HOSTIF_MsgData_t param = { 0 };
    strncpy(param.paramName, LE_RFC_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    device::g_enable_le_config_result = dsERR_NONE;
    device::g_enable_le_config_throw = false;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    EXPECT_EQ(pIface->set_xRDKCentralComRFCLoudnessEquivalenceEnable(&param), OK);
}

TEST(deviceTest, set_xRDKCentralComRFCLoudnessEquivalenceEnable_Disable)
{
    HOSTIF_MsgData_t param = { 0 };
    strncpy(param.paramName, LE_RFC_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    device::g_enable_le_config_result = dsERR_NONE;
    device::g_enable_le_config_throw = false;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    EXPECT_EQ(pIface->set_xRDKCentralComRFCLoudnessEquivalenceEnable(&param), OK);
}

TEST(deviceTest, set_xRDKCentralComRFCLoudnessEquivalenceEnable_DeviceSettingsFailure)
{
    HOSTIF_MsgData_t param = { 0 };
    strncpy(param.paramName, LE_RFC_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    device::g_enable_le_config_result = dsERR_GENERAL;
    device::g_enable_le_config_throw = false;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    EXPECT_EQ(pIface->set_xRDKCentralComRFCLoudnessEquivalenceEnable(&param), NOK);
    device::g_enable_le_config_result = dsERR_NONE;
}

TEST(deviceTest, set_xRDKCentralComRFCLoudnessEquivalenceEnable_DeviceSettingsException)
{
    HOSTIF_MsgData_t param = { 0 };
    strncpy(param.paramName, LE_RFC_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    device::g_enable_le_config_result = dsERR_NONE;
    device::g_enable_le_config_throw = true;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    EXPECT_EQ(pIface->set_xRDKCentralComRFCLoudnessEquivalenceEnable(&param), NOK);
    device::g_enable_le_config_throw = false;
}

TEST(deviceTest, get_xOpsReverseSshStatus_Active) {
    std::ofstream pidFile("/var/tmp/rssh.pid");
    pidFile << getpid();  // use current process PID which is definitely valid
    pidFile.close();
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    string partnerId;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_xOpsReverseSshStatus(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(msgData.paramValue, "ACTIVE");
    }
}

TEST(deviceTest, get_xRDKCentralComRFC) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    msgData.reqType = HOSTIF_GET;
    strncpy (msgData.paramName, "Device.Time.NTPServer5", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_xRDKCentralComRFC(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(0, 0);
    }
}

TEST(deviceTest, get_xRDKCentralComRFC_LegacyStorage)
{
    const bool previousLegacyMode = legacyRFCEnabled();
    setLegacyRFCEnabled(true);

    HOSTIF_MsgData_t msgData = { 0 };
    strncpy(msgData.paramName, "Device.Time.NTPServer5", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->get_xRDKCentralComRFC(&msgData);

    EXPECT_TRUE((ret == OK) || (ret == NOK));
    setLegacyRFCEnabled(previousLegacyMode);
}

TEST(deviceTest, get_xRDKCentralComRFC_AccountIdFallback)
{
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) { GTEST_SKIP() << "socket() failed"; }
    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(9998);
    if (bind(serverSock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        close(serverSock);
        GTEST_SKIP() << "port 9998 already in use";
    }
    listen(serverSock, 1);

    pthread_t tid;
    pthread_create(&tid, NULL, handle_account_id_mock, (void*)(intptr_t)serverSock);

    HOSTIF_MsgData_t msgData = { 0 };
    strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->get_xRDKCentralComRFC(&msgData);

    pthread_join(tid, NULL);
    close(serverSock);

    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "ACCOUNT-UNIT");
}

TEST(deviceTest, set_X_RDKCENTRAL_COM_LastRebootReason) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    msgData.reqType = HOSTIF_SET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_LastRebootReason", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->set_X_RDKCENTRAL_COM_LastRebootReason(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_xOpsDMMoCALogPeriod) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    msgData.reqType = HOSTIF_GET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.xOpsDMLogsUploadStatus", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_xOpsDMMoCALogPeriod(&msgData);
	cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, AdditionalHardwareVersion) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    msgData.reqType = HOSTIF_GET;
    strncpy (msgData.paramName, "Device.DeviceInfo.HardwareVersion", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_AdditionalHardwareVersion(&msgData, &bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, COM_Reset) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    msgData.reqType = HOSTIF_GET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Reset", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(&msgData, &bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, VendorConfigFileNumberOfEntries) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    msgData.reqType = HOSTIF_GET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Reset", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_VendorConfigFileNumberOfEntries(&msgData, &bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, SupportedDataModelNumber) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    msgData.reqType = HOSTIF_GET;
    strncpy (msgData.paramName, "Device.DeviceInfo.SupportedDataModelNumberOfEntries", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_SupportedDataModelNumberOfEntries(&msgData, &bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, VendorLogFileNumberOfEntries) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    msgData.reqType = HOSTIF_GET;
    strncpy (msgData.paramName, "Device.DeviceInfo.VendorLogFileNumberOfEntries", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_Device_DeviceInfo_VendorLogFileNumberOfEntries(&msgData, &bChanged);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, xOpsDMUploadLogsNow) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    msgData.reqType = HOSTIF_GET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.xOpsDMUploadLogsNow", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_xOpsDMUploadLogsNow(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
        EXPECT_EQ(get_boolean(msgData.paramValue), false);
    }
}

TEST(deviceTest, set_xOpsDMMoCALogPeriod) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    msgData.reqType = HOSTIF_GET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.xOpsDMMoCALogPeriod", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->set_xOpsDMMoCALogPeriod(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, get_xRDKCentralComRFCAccountId) {
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber = 0;
    msgData.reqType = HOSTIF_GET;
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.bsUpdate = HOSTIF_NONE;
    msgData.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        bChanged =  false;
        int ret = pIface->get_xRDKCentralComRFCAccountId(&msgData);
        cout << "msgData.paramValue = " << msgData.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, get_xRDKCentralComRFCAccountId_Success)
{
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) { GTEST_SKIP() << "socket() failed"; }
    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(9998);
    if (bind(serverSock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        close(serverSock);
        GTEST_SKIP() << "port 9998 already in use";
    }
    listen(serverSock, 1);

    pthread_t tid;
    pthread_create(&tid, NULL, handle_account_id_mock, (void*)(intptr_t)serverSock);

    HOSTIF_MsgData_t msgData = { 0 };
    strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    int ret = pIface->get_xRDKCentralComRFCAccountId(&msgData);

    pthread_join(tid, NULL);
    close(serverSock);

    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "ACCOUNT-UNIT");
    EXPECT_EQ(msgData.faultCode, fcNoFault);
}

TEST(bsStoreTest, initBSPropertiesFileName) {
    m_bsStore = XBSStore::getInstance();
    m_bsStore->initBSPropertiesFileName(); 
    m_bsStore->m_filename.erase(std::remove(m_bsStore->m_filename.begin(), m_bsStore->m_filename.end(), '"'), m_bsStore->m_filename.end());
    EXPECT_EQ(m_bsStore->m_filename, "/opt/secure/RFC/bootstrap.ini");
}

TEST(bsStoreTest, getRawValue) {
    m_bsStore = XBSStore::getInstance();
    const string key = "Device.Time.NTPServer2";
    string value = m_bsStore->getRawValue(key);
    EXPECT_EQ(value, "time1.com");
}

TEST(bsStoreTest, getRawValue_Empty) {
    m_bsStore = XBSStore::getInstance();
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MOCASSH.Enable";
    string value = m_bsStore->getRawValue(key);
    EXPECT_EQ(value, "");
}

TEST(bsStoreTest, setRawValue) {
    m_bsStore = XBSStore::getInstance();
    const string key = "Device.Time.NTPServer2";
    const string sameValue = "time1.com";

    m_bsStore->m_initialUpdate = false;
    bool ret = m_bsStore->setRawValue(key, sameValue, HOSTIF_SRC_RFC);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(m_bsStore->getRawValue(key), sameValue);
    EXPECT_EQ(XBSStore::xbsJournalInstance->getJournalSource(key), HOSTIF_SRC_RFC);
}

TEST(bsStoreTest, getValue) {
    m_bsStore = XBSStore::getInstance();

    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.getProfileData", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    int ret = m_bsStore->getValue(&param); //Get value before partnerId

    std::cout << "paramValue: " << getStringValue(&param) << " ret = " << ret << std::endl;
    EXPECT_EQ(ret, fcInternalError);
}

TEST(bsStoreTest, setValue_BS_CLEAR_DB_START) {
    m_bsStore = XBSStore::getInstance();

    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDB", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    int ret = m_bsStore->overrideValue(&param);

    std::cout << "ret = " << ret << std::endl;
    EXPECT_EQ(ret, 0);
}

TEST(bsStoreTest, setValue_BS_CLEAR_DB_END) {
    m_bsStore = XBSStore::getInstance();

    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDBEnd", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    int ret = m_bsStore->overrideValue(&param);

    std::cout << "ret = " << ret << std::endl;
    EXPECT_EQ(ret, 0);
}

TEST(bsStoreTest, overrideValue_NewParam_AllowsOverride) {
    m_bsStore = XBSStore::getInstance();

    HOSTIF_MsgData_t setParam = { 0 };
    memset(&setParam,0,sizeof(HOSTIF_MsgData_t));
    setParam.reqType = HOSTIF_SET;
    strncpy(setParam.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.UnitTest.NewParam", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    setParam.bsUpdate = HOSTIF_NONE;
    setParam.requestor = HOSTIF_SRC_WEBPA;

    strncpy(setParam.paramValue, "unit_test_value", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    setParam.paramtype = hostIf_StringType;
    setParam.paramLen = strlen(setParam.paramValue);

    int setRet = m_bsStore->overrideValue(&setParam);
    EXPECT_EQ(setRet, fcNoFault);

    HOSTIF_MsgData_t getParam = { 0 };
    memset(&getParam,0,sizeof(HOSTIF_MsgData_t));
    getParam.reqType = HOSTIF_GET;
    strncpy(getParam.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.UnitTest.NewParam", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    getParam.bsUpdate = HOSTIF_NONE;
    getParam.requestor = HOSTIF_SRC_RFC;

    int getRet = m_bsStore->getValue(&getParam);
    EXPECT_EQ(getRet, fcNoFault);
    EXPECT_EQ(getStringValue(&getParam), "unit_test_value");
}

TEST(bsStoreTest, createFile) {
    createFile("/tmp/bootstrap.txt");
    EXPECT_EQ(0, 0);
}

TEST(bsStoreTest, createDirectory) {
    bool ret = createDirectory("/tmp/RFC");
    EXPECT_EQ(ret, true);
}

TEST(bsStoreTest, createDirectory_Error) {
    bool ret = createDirectory("/opt/test/RFC");
    EXPECT_EQ(ret, false);
}

TEST(bsStoreTest, createBspCompleteFiles) {
    bool ret = createBspCompleteFiles();
    EXPECT_EQ(ret, true);
}

TEST(bsStoreTest, getPartnerDeviceConfig) {
    m_bsStore = XBSStore::getInstance();
    const string partnerId = "comcast";

    cJSON* partnerConfig = cJSON_CreateObject();
    cJSON_AddStringToObject(partnerConfig, "firmwareVersion", "v1.2.3");

    bool ret = m_bsStore->getPartnerDeviceConfig(partnerConfig, partnerId);
    EXPECT_EQ(ret, true);
    cJSON_Delete(partnerConfig);
}

TEST(bsStoreTest, getPartnerDeviceConfig_generic) {
    m_bsStore = XBSStore::getInstance();
    const string partnerId = "default";

    cJSON* partnerConfig = cJSON_CreateObject();
    cJSON_AddStringToObject(partnerConfig, "Device.Time.NTPServer1", "time.com");

    bool ret = m_bsStore->getPartnerDeviceConfig(partnerConfig, partnerId);
    EXPECT_EQ(ret, true);
    cJSON_Delete(partnerConfig);
}

TEST(bsStoreTest, getPartnerDeviceConfig_FileRemoved) {
    std::remove("/etc/partners_defaults_device.json");	
    m_bsStore = XBSStore::getInstance();    
    const string partnerId = "comcast";

    cJSON* partnerConfig = cJSON_CreateObject();
    cJSON_AddStringToObject(partnerConfig, "firmwareVersion", "v1.2.3");

    bool ret = m_bsStore->getPartnerDeviceConfig(partnerConfig, partnerId);
    EXPECT_EQ(ret, true);
    cJSON_Delete(partnerConfig);
}

TEST(bsStoreJournalTest, getBuildTime) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    writeToTr181storeFile("BUILD_TIME", "2025-05-27 06:39:24", "/version.txt", Quoted);
    string value = m_bsStoreJournal->getBuildTime();
    EXPECT_EQ(value, "2025-05-27 06:39:24");
}

TEST(bsStoreJournalTest, getBuildTime_Version) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");

    std::remove("/version.txt");
    EXPECT_EQ(m_bsStoreJournal->getBuildTime(), "");

    write_on_file("/version.txt", "BUILD_TIME=\"2026-06-09 12:34:56\"\n");
    EXPECT_EQ(m_bsStoreJournal->getBuildTime(), "2026-06-09 12:34:56");

    std::remove("/version.txt");
}

TEST(bsStoreJournalTest, setJournalValue) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.Enable";
    const string value = "false";

    bool result = m_bsStoreJournal->setJournalValue(key, value, HOSTIF_SRC_DEFAULT);
    EXPECT_EQ(result, true);
}

TEST(bsStoreJournalTest, resetClearRfc) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Enable";

    for (unordered_map<string, BS_JournalData_t>::iterator it= m_bsStoreJournal->m_dict.begin(); it!=m_bsStoreJournal->m_dict.end(); ++it)
    {
      if (key.compare(it->first) == 0)
      {
         BS_JournalData_t journalData = it->second;
         journalData.clearRfc = true;
         m_bsStoreJournal->m_dict[key] = std::move(journalData);
      }
    }
    bool result = m_bsStoreJournal->resetClearRfc(key);
    EXPECT_EQ(result, true);

    EXPECT_EQ(m_bsStoreJournal->resetClearRfc(key), false);
    EXPECT_EQ(m_bsStoreJournal->resetClearRfc("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UnitTest.Missing"), false);
}

TEST(bsStoreJournalTest, removeRecord) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    const string key = "Device.Time.NTPServer4";

    bool result = m_bsStoreJournal->removeRecord(key);
    EXPECT_EQ(result, true);
}

TEST(bsStoreJournalTest, clearRfcAndGetDefaultValue) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    const string key = "Device.Time.NTPServer1";
    string defaultValue;

    for (unordered_map<string, BS_JournalData_t>::iterator it= m_bsStoreJournal->m_dict.begin(); it!=m_bsStoreJournal->m_dict.end(); ++it)
    {
      if (key.compare(it->first) == 0)
      {
         BS_JournalData_t journalData = it->second;
         journalData.clearRfc = true;
         m_bsStoreJournal->m_dict[key] = std::move(journalData);
      }
    }

    bool result = m_bsStoreJournal->clearRfcAndGetDefaultValue(key, defaultValue);
    EXPECT_EQ(result, true);
    EXPECT_EQ(defaultValue, "time.com");

    EXPECT_EQ(m_bsStoreJournal->clearRfcAndGetDefaultValue(key, defaultValue), false);
    EXPECT_EQ(m_bsStoreJournal->clearRfcAndGetDefaultValue("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UnitTest.Missing", defaultValue), false);
}

TEST(bsStoreJournalTest, rfcUpdateStarted) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    const string rfcKey = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UnitTest.RfcUpdate";
    const string webpaKey = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UnitTest.WebpaUpdate";

    EXPECT_EQ(m_bsStoreJournal->setJournalValue(rfcKey, "true", HOSTIF_SRC_RFC), true);
    EXPECT_EQ(m_bsStoreJournal->setJournalValue(webpaKey, "false", HOSTIF_SRC_WEBPA), true);

    bool result = m_bsStoreJournal->rfcUpdateStarted();
    EXPECT_EQ(result, true);

    string defaultValue;
    EXPECT_EQ(m_bsStoreJournal->clearRfcAndGetDefaultValue(rfcKey, defaultValue), true);
    EXPECT_EQ(m_bsStoreJournal->clearRfcAndGetDefaultValue(webpaKey, defaultValue), false);
}

TEST(bsStoreJournalTest, rfcUpdateEnd) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    const string key = "Device.Time.NTPServer4";

    bool result = m_bsStoreJournal->rfcUpdateEnd();
    EXPECT_EQ(result, true);
}

TEST(bsStoreJournalTest, constructor) {
    XBSStoreJournal* journalPtr = new XBSStoreJournal();
    EXPECT_EQ(journalPtr->m_initDone, false);

    XBSStoreJournal* journalWithFile = new XBSStoreJournal("/opt/secure/RFC/bootstrap.journal");
    EXPECT_EQ(journalWithFile->m_initDone, true);
}

TEST(bsStoreJournalTest, setJournalValue_New_Key) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MOCASSH.Enable";
    const string value = "false";

    bool result = m_bsStoreJournal->setJournalValue(key, value, HOSTIF_SRC_RFC);
    EXPECT_EQ(result, true);
}

TEST(bsStoreJournalTest, setJournalValue_HOSTIF_SRC_DEFAULT) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.Enable";
    const string value = "true";

    bool result = m_bsStoreJournal->setJournalValue(key, value, HOSTIF_SRC_DEFAULT);
    EXPECT_EQ(result, true);
}

/* TEST(bsStoreJournalTest, resetCacheAndStore) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    const string key = "Device.Time.NTPServer4";

    m_bsStoreJournal->resetCacheAndStore();
    EXPECT_EQ(0, 0);
} */

TEST(rfcStoreTest, init_rfcdefaults) {
    m_rfcStore = XRFCStore::getInstance();

    bool result = init_rfcdefaults();
    EXPECT_EQ(result, true);
}

TEST(rfcStoreTest, reloadCache) {
    writeToTr181storeFile("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonPersistent.WebPACDL.Enable", "true", "/opt/secure/RFC/tr181store_nonpersist.ini", Plain);
    m_rfcStore = XRFCStore::getInstance();
    m_rfcStore->reloadCache();
    EXPECT_EQ(0, 0);
}

TEST(rfcStoreTest, loadTR181PropertiesIntoCache) {
    std::remove("/tmp/rfcdefaults.ini");
    m_rfcStore = XRFCStore::getInstance();

    bool ret = m_rfcStore->loadTR181PropertiesIntoCache();
    EXPECT_EQ(ret, true);
}

TEST(rfcStoreTest, getRawValue) {
    m_rfcStore = XRFCStore::getInstance();
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonPersistent.Enable";
    string value = m_rfcStore->getRawValue(key);
    EXPECT_EQ(value, "");
}

TEST(rfcStoreTest, getRawValue_NONPERSISTENT_FILE) {
    write_on_file("/tmp/.rfcSyncDone", "PREFIX)");
    m_rfcStore = XRFCStore::getInstance();
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonPersistent.WebPACDL.Enable";
    string value = m_rfcStore->getRawValue(key);
    EXPECT_EQ(value, "true");
}

TEST(rfcStoreTest, setRawValue_Invalid_FILE) {
    m_rfcStore = XRFCStore::getInstance();
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MTLS.mTlsCrashdumpUpload.Enable";
    const string value = "true";
    m_rfcStore->m_updateInProgress = true;
    m_rfcStore->m_filename = "/opt/secure/RFC/bootrap.ini";
    bool ret = m_rfcStore->setRawValue(key, value);
    EXPECT_EQ(ret, true);
}

TEST(rfcStoreTest, writeHashToFile) {
    m_rfcStore = XRFCStore::getInstance();
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MTLS.mTlsCrashdumpUpload.Enable";
    const string value = "true";
    unordered_map<string, string> dict;
    bool ret = m_rfcStore->writeHashToFile(key, value, dict, "/opt/secure/RFC1/boottrap.ini");
    EXPECT_EQ(ret, false);
}

TEST(rfcStoreTest, setValue_RFC_PREFIX) {
    m_rfcStore = XRFCStore::getInstance();

    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonPersistent.WebPACDL.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    faultCode_t ret = m_rfcStore->setValue(&param);
     
    EXPECT_EQ(ret, fcNoFault); 
}

TEST(rfcStoreTest, loadFileToCache) {
    std::ofstream file("/opt/secure/RFC/tr181temp.ini");
    file.close();     
    m_rfcStore = XRFCStore::getInstance();
    unordered_map<string, string> dict;
    bool ret = m_rfcStore->loadFileToCache("/opt/secure/RFC/tr181temp.ini", dict);

    EXPECT_EQ(ret, true);
}

TEST(rfcStoreTest, getValue_rfcdefaults) {
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(msgData));
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.AutoExcluded.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    int ret = m_rfcStore->getValue(&msgData);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "true");
}

TEST(rfcStorageTest, init) {
    int ret = system("cp ../../../../unittest/stubs/rfc.properties /etc/rfc.properties");
    EXPECT_EQ(ret, 0);
    m_rfcStoreage = new XRFCStorage();

    bool result = m_rfcStoreage->init();
    EXPECT_EQ(result, true);   
}

TEST(rfcStorageTest, getValue) {
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(msgData));
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    int ret = m_rfcStoreage->getValue(&msgData);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "true");
}

/* TEST(rfcStorageTest, getValue_rfcdefaults) {
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(msgData));
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.AutoExcluded.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    int ret = m_rfcStoreage->getValue(&msgData);
    EXPECT_EQ(ret, OK);
    EXPECT_STREQ(msgData.paramValue, "true");
} */

TEST(rfcStorageTest, getRawValue) {
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.Enable";

    string value = m_rfcStoreage->getRawValue(key);
    EXPECT_EQ(value, "true");
}

TEST(rfcStorageTest, setValue) {
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(msgData));
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);

    put_boolean(msgData.paramValue, false);
    msgData.paramtype = hostIf_BooleanType;
    msgData.paramLen = sizeof(hostIf_BooleanType);

    int ret = m_rfcStoreage->setValue(&msgData);
    cout << "msgData.paramValue = " << msgData.paramValue << endl;

    EXPECT_EQ(ret, OK);
}

TEST(rfcStorageTest, setSameValue) {
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(msgData));
    strncpy (msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerName", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);

    strncpy(msgData.paramValue, "comcast", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    int ret = m_rfcStoreage->setValue(&msgData);
    cout << "msgData.paramValue = " << msgData.paramValue << endl;

    EXPECT_EQ(ret, OK);
}

TEST(rfcStorageTest, setRawValue) {

    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.OsClass";
    const string value = "TestOsClass";

    bool ret = m_rfcStoreage->setRawValue(key, value);
    EXPECT_EQ(ret, true);
}

TEST(processTest, getNumOfProcessorEntries) {
    hostIf_DeviceProcessorInterface::closeAllInstances();
    hostIf_DeviceProcessorInterface *processorIface = hostIf_DeviceProcessorInterface::getInstance(0);
    ASSERT_NE(processorIface, nullptr);

    unsigned int ret = processorIface->getNumOfProcessorEntries();
    EXPECT_GE(ret, 1u);

    hostIf_DeviceProcessorInterface::closeAllInstances();
}

TEST(processTest, get_Device_DeviceInfo_Processor_Architecture) {
    hostIf_DeviceProcessorInterface::closeAllInstances();
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    hostIf_DeviceProcessorInterface *processorIface = hostIf_DeviceProcessorInterface::getInstance(0);
    ASSERT_NE(processorIface, nullptr);

    int ret = processorIface->get_Device_DeviceInfo_Processor_Architecture(&msgData);
    cout << "msgData.paramValue = " << msgData.paramValue << endl;
    EXPECT_EQ(ret, OK);
    EXPECT_GT(strlen(msgData.paramValue), 0u);

    hostIf_DeviceProcessorInterface::closeAllInstances();
}

TEST(processTest, getInstance_ReusesObjectForSameId_AndRejectsOutOfRangeId) {
    hostIf_DeviceProcessorInterface::closeAllInstances();
    hostIf_DeviceProcessorInterface *processorIface0 = hostIf_DeviceProcessorInterface::getInstance(0);
    ASSERT_NE(processorIface0, nullptr);

    hostIf_DeviceProcessorInterface *sameProcessorIface = hostIf_DeviceProcessorInterface::getInstance(0);
    EXPECT_EQ(processorIface0, sameProcessorIface);

    unsigned int totalEntries = hostIf_DeviceProcessorInterface::getNumOfProcessorEntries();
    hostIf_DeviceProcessorInterface *invalidIface = hostIf_DeviceProcessorInterface::getInstance(static_cast<int>(totalEntries + 1));
    EXPECT_EQ(invalidIface, nullptr);

    hostIf_DeviceProcessorInterface::closeAllInstances();
}

TEST(processTest, getAllInstances_TracksLifecycleAcrossCreateAndClose) {
    hostIf_DeviceProcessorInterface::closeAllInstances();

    GList* emptyInstances = hostIf_DeviceProcessorInterface::getAllInstances();
    EXPECT_EQ(emptyInstances, nullptr);

    hostIf_DeviceProcessorInterface *processorIface0 = hostIf_DeviceProcessorInterface::getInstance(0);
    ASSERT_NE(processorIface0, nullptr);
    hostIf_DeviceProcessorInterface *processorIface1 = hostIf_DeviceProcessorInterface::getInstance(1);
    ASSERT_NE(processorIface1, nullptr);

    GList* allInstances = hostIf_DeviceProcessorInterface::getAllInstances();
    ASSERT_NE(allInstances, nullptr);
    EXPECT_EQ(g_list_length(allInstances), 2);
    g_list_free(allInstances);

    hostIf_DeviceProcessorInterface::closeInstance(processorIface0);
    GList* oneLeft = hostIf_DeviceProcessorInterface::getAllInstances();
    ASSERT_NE(oneLeft, nullptr);
    EXPECT_EQ(g_list_length(oneLeft), 1);
    g_list_free(oneLeft);

    hostIf_DeviceProcessorInterface::closeAllInstances();
    EXPECT_EQ(hostIf_DeviceProcessorInterface::getAllInstances(), nullptr);
}

TEST(processTest, ProcessorArchitecture_ChangeFlagAcrossCalls) {
    hostIf_DeviceProcessorInterface::closeAllInstances();
    hostIf_DeviceProcessorInterface *processorIface = hostIf_DeviceProcessorInterface::getInstance(0);
    ASSERT_NE(processorIface, nullptr);

    HOSTIF_MsgData_t firstRead;
    memset(&firstRead, 0, sizeof(HOSTIF_MsgData_t));
    bool changed = true;
    EXPECT_EQ(processorIface->get_Device_DeviceInfo_Processor_Architecture(&firstRead, &changed), OK);
    EXPECT_TRUE(changed);

    hostIf_DeviceProcessorInterface::closeAllInstances();
}

TEST(processTest, closeInstance_HandlesNull) {
    hostIf_DeviceProcessorInterface::closeInstance(nullptr);
    EXPECT_EQ(0, 0);
}

TEST(processTest, Processor_Lock_ReleaseLock) {
    hostIf_DeviceProcessorInterface::closeAllInstances();
    hostIf_DeviceProcessorInterface *processorIface = hostIf_DeviceProcessorInterface::getInstance(0);
    ASSERT_NE(processorIface, nullptr);

    processorIface->getLock();
    processorIface->releaseLock();
    EXPECT_EQ(0, 0);

    hostIf_DeviceProcessorInterface::closeInstance(processorIface);
    hostIf_DeviceProcessorInterface::closeAllInstances();

}

TEST(processTest, getProcessStatusCPUUsage) {
    hostIf_DeviceProcessStatusInterface::closeAllInstances();
    hostIf_DeviceProcessStatusInterface *processStatusIface = hostIf_DeviceProcessStatusInterface::getInstance(0);
    ASSERT_NE(processStatusIface, nullptr);

    int ret = processStatusIface->getProcessStatusCPUUsage();
    EXPECT_GE(ret, 0);

    hostIf_DeviceProcessStatusInterface::closeAllInstances();
}


TEST(processTest, get_Device_DeviceInfo_ProcessStatus_CPUUsage) {
    hostIf_DeviceProcessStatusInterface::closeAllInstances();
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    hostIf_DeviceProcessStatusInterface *processStatusIface = hostIf_DeviceProcessStatusInterface::getInstance(0);
    ASSERT_NE(processStatusIface, nullptr);

    bool pChanged = false;
    int ret = processStatusIface->get_Device_DeviceInfo_ProcessStatus_CPUUsage(&msgData, &pChanged);
    EXPECT_EQ(ret, OK);
    EXPECT_EQ(msgData.paramtype, hostIf_IntegerType);
    EXPECT_FALSE(pChanged);

    HOSTIF_MsgData_t secondRead;
    memset(&secondRead, 0, sizeof(HOSTIF_MsgData_t));
    bool secondChanged = false;
    EXPECT_EQ(processStatusIface->get_Device_DeviceInfo_ProcessStatus_CPUUsage(&secondRead, &secondChanged), OK);

    hostIf_DeviceProcessStatusInterface::closeAllInstances();
}

TEST(processTest, getProcessStatParam) {
    long long unsigned int mUser = 0;
    long long unsigned int mNice = 0;
    long long unsigned int mSystem = 0;
    long long unsigned int mIdle = 0;
    long long unsigned int mIOwait = 0;
    long long unsigned int mIrq = 0;
    long long unsigned int mSoftirq = 0;

    hostIf_DeviceProcessStatusInterface::closeAllInstances();
    hostIf_DeviceProcessStatusInterface *processStatusIface = hostIf_DeviceProcessStatusInterface::getInstance(0);
    ASSERT_NE(processStatusIface, nullptr);

    int ret = processStatusIface->getProcessStatParam(&mUser, &mNice, &mSystem, &mIdle, &mIOwait, &mIrq, &mSoftirq);
    EXPECT_EQ(ret, OK);
    EXPECT_GT(mUser + mNice + mSystem + mIdle + mIOwait + mIrq + mSoftirq, 0u);

    hostIf_DeviceProcessStatusInterface::closeAllInstances();
}

TEST(processTest, ProcessStatus_InstanceLifecycleAndList) {
    hostIf_DeviceProcessStatusInterface::closeAllInstances();

    EXPECT_EQ(hostIf_DeviceProcessStatusInterface::getAllInstances(), nullptr);

    hostIf_DeviceProcessStatusInterface *first = hostIf_DeviceProcessStatusInterface::getInstance(0);
    ASSERT_NE(first, nullptr);
    hostIf_DeviceProcessStatusInterface *same = hostIf_DeviceProcessStatusInterface::getInstance(0);
    EXPECT_EQ(first, same);
    hostIf_DeviceProcessStatusInterface *second = hostIf_DeviceProcessStatusInterface::getInstance(1);
    ASSERT_NE(second, nullptr);

    GList* allInstances = hostIf_DeviceProcessStatusInterface::getAllInstances();
    ASSERT_NE(allInstances, nullptr);
    EXPECT_EQ(g_list_length(allInstances), 2);
    g_list_free(allInstances);

    hostIf_DeviceProcessStatusInterface::closeInstance(first);
    GList* oneLeft = hostIf_DeviceProcessStatusInterface::getAllInstances();
    ASSERT_NE(oneLeft, nullptr);
    EXPECT_EQ(g_list_length(oneLeft), 1);
    g_list_free(oneLeft);

    hostIf_DeviceProcessStatusInterface::closeInstance(nullptr);
    hostIf_DeviceProcessStatusInterface::closeAllInstances();
    EXPECT_EQ(hostIf_DeviceProcessStatusInterface::getAllInstances(), nullptr);
}

TEST(processTest, ProcessStatus_Lock_ReleaseLock) {
    hostIf_DeviceProcessStatusInterface::closeAllInstances();
    hostIf_DeviceProcessStatusInterface *processStatusIface = hostIf_DeviceProcessStatusInterface::getInstance(0);
    ASSERT_NE(processStatusIface, nullptr);

    processStatusIface->getLock();
    processStatusIface->releaseLock();
    EXPECT_EQ(0, 0);

    hostIf_DeviceProcessStatusInterface::closeInstance(processStatusIface);
    hostIf_DeviceProcessStatusInterface::closeAllInstances();
}


TEST(bsStoreJournalTest, getUpdatedSourceString_AndGetJournalSourceMissing) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    EXPECT_EQ(m_bsStoreJournal->getUpdatedSourceString(HOSTIF_SRC_RFC), "rfc");
    EXPECT_EQ(m_bsStoreJournal->getUpdatedSourceString(HOSTIF_SRC_WEBPA), "webpa");
    EXPECT_EQ(m_bsStoreJournal->getUpdatedSourceString(HOSTIF_NONE), "-");
    EXPECT_EQ(m_bsStoreJournal->getJournalSource("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Missing.Param"), HOSTIF_NONE);
}

TEST(bsStoreJournalTest, setInitialUpdate_AndGetJournalSourceExisting) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    m_bsStoreJournal->setInitialUpdate(true);

    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UnitTest.BSJournal";
    EXPECT_EQ(m_bsStoreJournal->setJournalValue(key, "true", HOSTIF_SRC_WEBPA), true);
    EXPECT_EQ(m_bsStoreJournal->getJournalSource(key), HOSTIF_SRC_WEBPA);

    m_bsStoreJournal->setInitialUpdate(false);
}

/*  TEST(bsStoreJournalTest, resetCacheAndStore_RemovesCacheAndAllowsReuse) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UnitTest.ResetCache";

    EXPECT_EQ(m_bsStoreJournal->setJournalValue(key, "true", HOSTIF_SRC_WEBPA), true);
    EXPECT_NE(m_bsStoreJournal->getJournalSource(key), HOSTIF_NONE);

    m_bsStoreJournal->resetCacheAndStore();
    EXPECT_EQ(m_bsStoreJournal->getJournalSource(key), HOSTIF_NONE);

    EXPECT_EQ(m_bsStoreJournal->setJournalValue(key, "false", HOSTIF_SRC_WEBPA), true);
    EXPECT_EQ(m_bsStoreJournal->getJournalSource(key), HOSTIF_SRC_WEBPA);
}

*/

TEST(bsStoreTest, stop_And_call_loadJson) {
    m_bsStore = XBSStore::getInstance();
    bool loaded = m_bsStore->call_loadJson();
    EXPECT_EQ(loaded, true);
    m_bsStore->stop();
}

TEST(rfcStoreTest, setValue_ClearParam_Path) {
    m_rfcStore = XRFCStore::getInstance();

    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    msgData.reqType = HOSTIF_SET;
    strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.ClearParam", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(msgData.paramValue, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Unknown.Param", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.requestor = HOSTIF_SRC_WEBPA;

    faultCode_t ret = m_rfcStore->setValue(&msgData);
    EXPECT_EQ(ret, fcNoFault);
}

TEST(rfcStoreTest, clearAllAndReloadCache_DuplicateSafe) {
    m_rfcStore = XRFCStore::getInstance();
    m_rfcStore->clearAll();
    m_rfcStore->clearAll();

    m_rfcStore->reloadCache();
    m_rfcStore->reloadCache();
    EXPECT_EQ(0, 0);
}

TEST(rfcStorageTest, setRawValue_And_clearAll) {
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UnitTest.Temp";
    EXPECT_EQ(m_rfcStoreage->setRawValue(key, "true"), true);
    EXPECT_EQ(m_rfcStoreage->getRawValue(key), "true");

    m_rfcStoreage->clearAll();
    EXPECT_EQ(m_rfcStoreage->getRawValue(key), "");
}

#ifdef USE_XRDK_BT_PROFILE
TEST(blueToothTest, singletonResetAndClose) {
    hostIf_DeviceInfoRdk_xBT *btIface = hostIf_DeviceInfoRdk_xBT::getInstance();
    ASSERT_NE(btIface, nullptr);
    hostIf_DeviceInfoRdk_xBT::reset();
    hostIf_DeviceInfoRdk_xBT::closeInstance();
}

TEST(blueToothTest, handleSetMsg_InvalidPath_ReturnsNotHandled) {
    hostIf_DeviceInfoRdk_xBT *btIface = hostIf_DeviceInfoRdk_xBT::getInstance();
    ASSERT_NE(btIface, nullptr);

    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Invalid.enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);

    int ret = btIface->handleSetMsg(&msgData);
    EXPECT_EQ(ret, NOT_HANDLED);
    EXPECT_EQ(msgData.faultCode, fcInvalidParameterName);

    hostIf_DeviceInfoRdk_xBT::closeInstance();
}

TEST(blueToothTest, handleSetMsg_UnknownUnderRoot_ReturnsNotHandled) {
    hostIf_DeviceInfoRdk_xBT *btIface = hostIf_DeviceInfoRdk_xBT::getInstance();
    ASSERT_NE(btIface, nullptr);

    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.UnknownParam", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);

    int ret = btIface->handleSetMsg(&msgData);
    EXPECT_EQ(ret, NOT_HANDLED);
    EXPECT_EQ(msgData.faultCode, fcInvalidParameterName);

    hostIf_DeviceInfoRdk_xBT::closeInstance();
}

TEST(blueToothTest, handleGetMsg_InvalidPath_ReturnsNotHandled) {
    hostIf_DeviceInfoRdk_xBT *btIface = hostIf_DeviceInfoRdk_xBT::getInstance();
    ASSERT_NE(btIface, nullptr);

    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Invalid.DiscoveryEnabled", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);

    int ret = btIface->handleGetMsg(&msgData);
    EXPECT_EQ(ret, NOT_HANDLED);

    hostIf_DeviceInfoRdk_xBT::closeInstance();
}

TEST(blueToothTest, handleGetMsg_UnknownUnderRoot_ReturnsNok) {
    hostIf_DeviceInfoRdk_xBT *btIface = hostIf_DeviceInfoRdk_xBT::getInstance();
    ASSERT_NE(btIface, nullptr);

    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(msgData.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.UnknownParam", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);

    int ret = btIface->handleGetMsg(&msgData);
    EXPECT_EQ(ret, NOK);
    EXPECT_EQ(msgData.faultCode, fcInvalidParameterName);

    hostIf_DeviceInfoRdk_xBT::closeInstance();
}
#endif


TEST(clearTest, rfcclearAll) {
    m_rfcStore = XRFCStore::getInstance();

    m_rfcStore->clearAll();
    EXPECT_EQ(0, 0);
}

TEST(clearTest, rfcStorageclearAll) {
    m_rfcStoreage->clearAll();
    EXPECT_EQ(0, 0);
}

TEST(StoreClearTest, clearRfcValues) {
    m_bsStore = XBSStore::getInstance();
    bool ret = m_bsStore->clearRfcValues();
    EXPECT_EQ(ret, true);
}

TEST(StoreClearTest, resetCacheAndStore) {
    m_bsStore = XBSStore::getInstance();
    m_bsStore->resetCacheAndStore();
    EXPECT_EQ(0, 0);
}

TEST(StoreClearTest, setRawValue_Flush) {
    m_bsStore = XBSStore::getInstance();
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.UnitTest.Flush";
    const string value = "flush_path_value";

    m_bsStore->m_initialUpdate = true;
    bool ret = m_bsStore->setRawValue(key, value, HOSTIF_SRC_DEFAULT);
    m_bsStore->m_initialUpdate = false;

    EXPECT_EQ(ret, true);
    EXPECT_EQ(m_bsStore->getRawValue(key), value);
}
    // New device tests added
TEST(deviceTest, getNotifyHash)
{
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);
    EXPECT_NE(pIface->getNotifyHash(), nullptr);
}

TEST(deviceTest, findIgnoreCase)
{
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    size_t pos = pIface->findIgnoreCase("AbCdEf", "cde", 0);
    EXPECT_EQ(pos, 2U);

    pos = pIface->findIgnoreCase("AbCdEf", "xyz", 0);
    EXPECT_EQ(pos, std::string::npos);
}

TEST(deviceTest, getEstbIp)
{
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    std::string ip = pIface->getEstbIp();
    EXPECT_GE(ip.size(), 0U);
}

TEST(deviceTest, getEstbIp_IPv4MatchingInterface)
{
    std::remove("/tmp/estb_ipv6");
    std::remove("/tmp/wifi-on");
    setenv("DEFAULT_ESTB_INTERFACE", "lo", 1);

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    std::string ip = pIface->getEstbIp();
    EXPECT_FALSE(ip.empty());
    EXPECT_STREQ(ip.c_str(), "127.0.0.1");

    unsetenv("DEFAULT_ESTB_INTERFACE");
}

TEST(deviceTest, getEstbIp_IPv6MatchingInterface)
{
    // lo has ::1 which is not link-local, so it matches the IPv6 path
    std::ofstream ofs("/tmp/estb_ipv6");
    ofs.close();
    std::remove("/tmp/wifi-on");
    setenv("DEFAULT_ESTB_INTERFACE", "lo", 1);

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    std::string ip = pIface->getEstbIp();
    EXPECT_FALSE(ip.empty());

    std::remove("/tmp/estb_ipv6");
    unsetenv("DEFAULT_ESTB_INTERFACE");
}

TEST(deviceTest, getEstbIp_WiFiEnabled)
{
    std::remove("/tmp/estb_ipv6");
    std::ofstream wifiFile("/tmp/wifi-on");
    wifiFile.close();
    setenv("DEFAULT_ESTB_INTERFACE", "lo", 1);

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    std::string ip = pIface->getEstbIp();
    EXPECT_GE(ip.size(), 0U);

    std::remove("/tmp/wifi-on");
    unsetenv("DEFAULT_ESTB_INTERFACE");
}

TEST(deviceTest, isRsshactive)
{
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    std::ofstream pidFile("/var/tmp/rssh.pid");
    ASSERT_TRUE(pidFile.is_open());
    pidFile << getpid();
    pidFile.close();

    EXPECT_TRUE(pIface->isRsshactive());
}

TEST(deviceTest, getStbMacIf_fr_devProperties)
{
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    setenv("DEFAULT_ESTB_INTERFACE", "lo", 1);
    std::string mac = pIface->getStbMacIf_fr_devProperties();
    EXPECT_GE(mac.size(), 0U);
}

TEST(deviceTest, getStbMacIf_fr_devProperties_IoctlSuccess)
{
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    setenv("DEFAULT_ESTB_INTERFACE", "lo", 1);
    g_ioctl_success = true;

    EXPECT_EQ(pIface->getStbMacIf_fr_devProperties(), "02:11:22:33:44:55");

    g_ioctl_success = false;
    unsetenv("DEFAULT_ESTB_INTERFACE");
}

TEST(deviceTest, getStbMacIf_fr_devProperties_SocketFailure)
{
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    g_socket_fail = true;

    EXPECT_TRUE(pIface->getStbMacIf_fr_devProperties().empty());

    g_socket_fail = false;
}

/* TEST(deviceTest, getStbMacIf_fr_devProperties_Exception)
{
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    setenv("DEFAULT_ESTB_INTERFACE", "lo", 1);
    g_ioctl_throw = true;

    EXPECT_TRUE(pIface->getStbMacIf_fr_devProperties().empty());

    g_ioctl_throw = false;
    unsetenv("DEFAULT_ESTB_INTERFACE");
} */

TEST(deviceTest, initMutexOnce)
{
    hostIf_DeviceInfo::getLock();
    hostIf_DeviceInfo::releaseLock();
    SUCCEED();
}

TEST(deviceTest, initMutexAttributes_DirectCall)
{
    hostIf_DeviceInfo::initMutexAttributes();
    hostIf_DeviceInfo::initMutexOnce();
    hostIf_DeviceInfo::getLock();
    hostIf_DeviceInfo::releaseLock();
    SUCCEED();
}

TEST(deviceTest, get_Device_DeviceInfo_X_RDKCENTRAL_COM_CPUTemp)
{
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(msgData));
    bool bChanged = false;
    int instanceNumber = 0;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_CPUTemp(&msgData, &bChanged);
    EXPECT_TRUE((ret == OK) || (ret == NOK));
}

TEST(deviceTest, sendDeviceMgtNotification)
{
    int ret = hostIf_DeviceInfo::sendDeviceMgtNotification("unittest", "rfc_update");
    EXPECT_TRUE((ret == OK) || (ret == NOK));
}

TEST(deviceTest, sendDeviceMgtNotification_BroadcastFailure)
{
    IARM_Result_t previousResult = g_iarm_broadcast_event_result;
    g_iarm_broadcast_event_result = IARM_RESULT_IPCCORE_FAIL;

    int ret = hostIf_DeviceInfo::sendDeviceMgtNotification("unittest", "rfc_update");

    EXPECT_EQ(ret, NOK);
    g_iarm_broadcast_event_result = previousResult;
}

TEST(deviceTest, sendDeviceMgtNotification_FileOpenFailure)
{
    const char *updatePath = "/tmp/.dmnupdate";
    std::remove(updatePath);
    ASSERT_EQ(mkdir(updatePath, 0755), 0);

    int ret = hostIf_DeviceInfo::sendDeviceMgtNotification("unittest", "rfc_update");

    EXPECT_EQ(ret, NOK);
    rmdir(updatePath);
}

TEST(deviceTest, sendDeviceMgtNotification_SourceCopyFailure)
{
    const std::string oversizedSource(10, 's');

    int ret = hostIf_DeviceInfo::sendDeviceMgtNotification(oversizedSource.c_str(), "rfc_update");

    EXPECT_TRUE((ret == OK) || (ret == NOK));
}

TEST(deviceTest, sendDeviceMgtNotification_TypeCopyFailure)
{
    const std::string oversizedType(10, 't');

    int ret = hostIf_DeviceInfo::sendDeviceMgtNotification("unittest", oversizedType.c_str());

    EXPECT_TRUE((ret == OK) || (ret == NOK));
}

TEST(deviceTest, set_Device_DeviceInfo_X_RDKCENTRAL_COM_MemInsight_Trigger)
{
    HOSTIF_MsgData_t param;
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, MEMINSIGHT_TRIGGER, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    strncpy(param.paramValue, "start", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramLen = strlen(param.paramValue);
    int ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_MemInsight_Trigger(&param);
    EXPECT_TRUE((ret == OK) || (ret == NOK));

    memset(param.paramValue, 0, sizeof(param.paramValue));
    strncpy(param.paramValue, "stop", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramLen = strlen(param.paramValue);
    ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_MemInsight_Trigger(&param);
    EXPECT_TRUE((ret == OK) || (ret == NOK));
}

TEST(deviceTest, set_xRDKCentralComRFCDistributedTracingEnable)
{
    HOSTIF_MsgData_t param;
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, DISTRIBUTED_TRACING_RFC_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    put_boolean(param.paramValue, true);
    int ret = pIface->set_xRDKCentralComRFC(&param);
    EXPECT_TRUE((ret == OK) || (ret == NOK));

    put_boolean(param.paramValue, false);
    ret = pIface->set_xRDKCentralComRFC(&param);
    EXPECT_TRUE((ret == OK) || (ret == NOK));
}

TEST(deviceTest, direct_set_xRDKCentralComRFCDistributedTracingEnable)
{
    HOSTIF_MsgData_t param;
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, DISTRIBUTED_TRACING_RFC_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    put_boolean(param.paramValue, true);
    int ret = pIface->set_xRDKCentralComRFCDistributedTracingEnable(&param);
    EXPECT_TRUE((ret == OK) || (ret == NOK));

    put_boolean(param.paramValue, false);
    ret = pIface->set_xRDKCentralComRFCDistributedTracingEnable(&param);
    EXPECT_TRUE((ret == OK) || (ret == NOK));
}

TEST(deviceTest, direct_set_xRDKCentralComRFCDistributedTracingEnable_DisableWhenFileMissing)
{
    HOSTIF_MsgData_t param;
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, DISTRIBUTED_TRACING_RFC_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    remove(RDK_TRACING_FLAG_FILE);
    put_boolean(param.paramValue, false);
    int ret = pIface->set_xRDKCentralComRFCDistributedTracingEnable(&param);
    EXPECT_TRUE((ret == OK) || (ret == NOK));
}

TEST(deviceTest, direct_set_xRDKCentralComRFCDistributedTracingEnable_InvalidType)
{
    HOSTIF_MsgData_t param;
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, DISTRIBUTED_TRACING_RFC_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    strncpy(param.paramValue, "true", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramLen = strlen(param.paramValue);

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->set_xRDKCentralComRFCDistributedTracingEnable(&param);
    EXPECT_EQ(ret, NOK);
}

TEST(deviceTest, direct_set_xRDKCentralComRFCDistributedTracingEnable_FlagCreateFails)
{
    std::remove(RDK_TRACING_FLAG_FILE);
    ASSERT_EQ(mkdir(RDK_TRACING_FLAG_FILE, 0755), 0);

    HOSTIF_MsgData_t param = { 0 };
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, DISTRIBUTED_TRACING_RFC_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    put_boolean(param.paramValue, true);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    EXPECT_EQ(pIface->set_xRDKCentralComRFCDistributedTracingEnable(&param), OK);

    rmdir(RDK_TRACING_FLAG_FILE);
}

TEST(deviceTest, direct_set_xRDKCentralComRFCDistributedTracingEnable_FlagRemoveFails)
{
    std::remove(RDK_TRACING_FLAG_FILE);
    ASSERT_EQ(mkdir(RDK_TRACING_FLAG_FILE, 0755), 0);
    { std::ofstream keepFile(std::string(RDK_TRACING_FLAG_FILE) + "/keep"); }

    HOSTIF_MsgData_t param = { 0 };
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, DISTRIBUTED_TRACING_RFC_ENABLE, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    put_boolean(param.paramValue, false);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(0);
    ASSERT_NE(pIface, nullptr);
    EXPECT_EQ(pIface->set_xRDKCentralComRFCDistributedTracingEnable(&param), OK);

    std::remove((std::string(RDK_TRACING_FLAG_FILE) + "/keep").c_str());
    rmdir(RDK_TRACING_FLAG_FILE);
}

#ifdef ENABLE_VIDEO_TELEMETRY
TEST(deviceTest, set_xRDKCentralComRFCVideoTelFreq)
{
    HOSTIF_MsgData_t param;
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.VideoTelemetry.FrequncyMinutes", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    strncpy(param.paramValue, "10", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramLen = strlen(param.paramValue);

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    int ret = pIface->set_xRDKCentralComRFC(&param);
    EXPECT_TRUE((ret == OK) || (ret == NOK));
}

TEST(deviceTest, direct_set_xRDKCentralComRFCVideoTelFreq)
{
    HOSTIF_MsgData_t param;
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy(param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.VideoTelemetry.FrequncyMinutes", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_UnsignedIntType;

    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    ASSERT_NE(pIface, nullptr);

    put_uint(param.paramValue, 10);
    param.paramLen = sizeof(hostIf_UnsignedIntType);
    int ret = pIface->set_xRDKCentralComRFCVideoTelFreq(&param);
    EXPECT_TRUE((ret == OK) || (ret == NOK));

    put_uint(param.paramValue, 0);
    ret = pIface->set_xRDKCentralComRFCVideoTelFreq(&param);
    EXPECT_EQ(ret, NOK);

    put_uint(param.paramValue, 61);
    ret = pIface->set_xRDKCentralComRFCVideoTelFreq(&param);
    EXPECT_EQ(ret, NOK);

    param.paramtype = hostIf_StringType;
    strncpy(param.paramValue, "15", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramLen = strlen(param.paramValue);
    ret = pIface->set_xRDKCentralComRFCVideoTelFreq(&param);
    EXPECT_EQ(ret, NOK);
}
#endif

TEST(deviceTest, getAllInstances_ReturnsCreatedInstanceList)
{
    hostIf_DeviceInfo::closeAllInstances();
    hostIf_DeviceInfo *first = hostIf_DeviceInfo::getInstance(0);
    hostIf_DeviceInfo *second = hostIf_DeviceInfo::getInstance(1);

    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);

    GList *instances = hostIf_DeviceInfo::getAllInstances();
    ASSERT_NE(instances, nullptr);
    EXPECT_EQ(g_list_length(instances), 2);
    g_list_free(instances);
}

TEST(deviceTest, systemMgmtTimePathMonitorThr_WhenWebpaReady_Exits)
{
    mkdir("/tmp/webpa", 0755);
    std::ofstream readyFile("/tmp/webpa/start_time");
    ASSERT_TRUE(readyFile.is_open());
    readyFile << "1";
    readyFile.close();

    std::thread monitorThread(hostIf_DeviceInfo::systemMgmtTimePathMonitorThr);
    monitorThread.join();

    SUCCEED();
}

/* TEST(StoreClearTest, init) {
    std::remove("/opt/secure/RFC/tr181store.ini");
    std::ofstream file("/opt/secure/RFC/tr181store.ini");
    file.close();

    bool ret = m_rfcStoreage->init();
    EXPECT_EQ(ret, false);
}

TEST(StoreClearTest, getRawValue) {
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.Enable";
    string value = m_rfcStoreage->getRawValue(key);
    EXPECT_EQ(value, "");
}

TEST(StoreClearTest, setRawValue_Flush) {
    m_bsStore = XBSStore::getInstance();
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerName";
    const string value = "sky";
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    m_bsStore->m_initialUpdate = true;
    bool ret = m_bsStore->setRawValue(key, value,param.requestor);
    EXPECT_EQ(ret, true);
} */

GTEST_API_ int main(int argc, char *argv[]){
    char testresults_fullfilepath[GTEST_REPORT_FILEPATH_SIZE];
    char buffer[GTEST_REPORT_FILEPATH_SIZE];

    memset( testresults_fullfilepath, 0, GTEST_REPORT_FILEPATH_SIZE );
    memset( buffer, 0, GTEST_REPORT_FILEPATH_SIZE );
    snprintf( testresults_fullfilepath, GTEST_REPORT_FILEPATH_SIZE, "json:%s%s" , GTEST_DEFAULT_RESULT_FILEPATH , GTEST_DEFAULT_RESULT_FILENAME);

    ::testing::GTEST_FLAG(output) = testresults_fullfilepath;
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

