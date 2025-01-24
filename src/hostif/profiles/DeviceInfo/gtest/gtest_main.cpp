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
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "XrdkCentralComRFCStore.h"
#include "XrdkCentralComBSStore.h"

#include "hostIf_msgHandler.h"
#include "http_server.h"
#include "request_handler.h"
#include "rdk_debug.h"
#include "waldb.h"


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

std::mutex mtx_httpServerThreadDone;
std::condition_variable cv_httpServerThreadDone;
bool httpServerThreadDone = false;
GThread *HTTPServerThread = NULL;
char *HTTPServerName = (char *)"HTTPServerThread";
GError *httpError = NULL;
T_ARGLIST argList = {{'\0'}, 0};

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

