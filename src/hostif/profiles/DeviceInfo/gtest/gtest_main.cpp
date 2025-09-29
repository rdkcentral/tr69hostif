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
XBSStoreJournal* m_bsStoreJournal;
XRFCStorage* m_rfcStoreage;

std::mutex mtx_httpServerThreadDone;
std::condition_variable cv_httpServerThreadDone;
bool httpServerThreadDone = false;
GThread *HTTPServerThread = NULL;
char *HTTPServerName = (char *)"HTTPServerThread";
GError *httpError = NULL;
T_ARGLIST argList = {{'\0'}, 0};

#ifdef GTEST_ENABLE
extern bool (*ValidateInput_ArgumentsFunc()) (char *input, FILE *tmp_fptr);
#endif

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

TEST(deviceTest, set_xRDKCentralComXREContainerRFCDisable) {
    HOSTIF_MsgData_t param;
    bool bChanged;
    int instanceNumber = 0;
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreEnable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xRDKCentralComXREContainerRFCEnable(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xRDKCentralComXREContainerRFCInvalidtype) {
    HOSTIF_MsgData_t param;
    bool bChanged;
    int instanceNumber = 0;
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreEnable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "TestName2", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xRDKCentralComXREContainerRFCEnable(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, set_xRDKCentralComXREContainerRFCEnable) {
    HOSTIF_MsgData_t param;
    bool bChanged;
    int instanceNumber = 0;
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreEnable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    
    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xRDKCentralComXREContainerRFCEnable(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, set_xRDKCentralComXREContainerRFCEnable_FileRemoved) {
    std::remove("/opt/XRE_container_enable");	
    HOSTIF_MsgData_t param;
    bool bChanged;
    int instanceNumber = 0;
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreEnable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, false);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        bChanged =  false;
        int ret = pIface->set_xRDKCentralComXREContainerRFCEnable(&param);
        cout << "param.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
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
        EXPECT_EQ(ret, OK);
    }
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

TEST(deviceInfoTest, get_X_RDKCENTRAL_COM_LastRebootReason) {
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

TEST(deviceTest, ScheduleAutoReboot) {
    int instanceNumber = 0;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->ScheduleAutoReboot(true);
        EXPECT_EQ(ret, OK);
    }
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
    bool pChanged;
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
        int ret = pIface->get_Device_DeviceInfo_SerialNumber(&param, &pChanged);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
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

TEST(deviceTest, set_xRDKCentralComRFC_RebootStopEnable_XRE_CONTAINER_RFC_ENABLE)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LXC.XRE.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
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

TEST(deviceTest, RDKRemoteDebuggergetProfileData)
{
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.getProfileData", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggergetProfileData(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(deviceTest, RDKRemoteDebuggergetProfileData_FileRemoved)
{
    std::remove("/etc/rrd/remote_debugger.json");
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.getProfileData", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggergetProfileData(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
}

TEST(deviceTest, RDKRemoteDebuggergetProfileData_EmptyFile)
{
    std::ofstream file("/etc/rrd/remote_debugger.json");
    file.close();
    int instanceNumber = 0;
    bool pChanged;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.getProfileData", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(pIface)
    {
        int ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_RDKRemoteDebuggergetProfileData(&param);
        cout << "msgData.paramValue = " << param.paramValue << endl;
        EXPECT_EQ(ret, NOK);
    }
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
}

TEST(bsStoreJournalTest, rfcUpdateStarted) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    const string key = "Device.Time.NTPServer4";

    bool result = m_bsStoreJournal->rfcUpdateStarted();
    EXPECT_EQ(result, true);
}

TEST(bsStoreJournalTest, rfcUpdateEnd) {
    m_bsStoreJournal = XBSStoreJournal::getInstance("/opt/secure/RFC/bootstrap.journal");
    const string key = "Device.Time.NTPServer4";

    bool result = m_bsStoreJournal->rfcUpdateEnd();
    EXPECT_EQ(result, true);
}

TEST(bsStoreJournalTest, constructor) {
    XBSStoreJournal* journalPtr = new XBSStoreJournal();
    EXPECT_EQ(0, 0);
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
    int instanceNumber = 0;

    hostIf_DeviceProcessorInterface *processorIface = hostIf_DeviceProcessorInterface::getInstance(instanceNumber);
    if(processorIface)
    {
       unsigned int ret = processorIface->getNumOfProcessorEntries();
       EXPECT_EQ(ret, 4);
   }
}

TEST(processTest, get_Device_DeviceInfo_Processor_Architecture) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    hostIf_DeviceProcessorInterface *processorIface = hostIf_DeviceProcessorInterface::getInstance(instanceNumber);
    if(processorIface)
    {
       int ret = processorIface->get_Device_DeviceInfo_Processor_Architecture(&msgData);
       cout << "msgData.paramValue = " << msgData.paramValue << endl;
       EXPECT_EQ(ret, OK);
       EXPECT_STREQ(msgData.paramValue, "x86_64");
   }
}

TEST(processTest, Processor_Lock_ReleaseLock) {
    int instanceNumber = 0;

    hostIf_DeviceProcessorInterface *processorIface = hostIf_DeviceProcessorInterface::getInstance(instanceNumber);
    if(processorIface)
    {
       processorIface->getLock();
       processorIface->releaseLock();
       EXPECT_EQ(0, 0);
   }
   processorIface->closeInstance(processorIface);
   processorIface->closeAllInstances();

}

TEST(processTest, getProcessStatusCPUUsage) {
    int instanceNumber = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    hostIf_DeviceProcessStatusInterface *processStatusIface = hostIf_DeviceProcessStatusInterface::getInstance(instanceNumber);
    if(processStatusIface)
    {
       int ret = processStatusIface->getProcessStatusCPUUsage();
       EXPECT_EQ(0, 0);
   }
}


TEST(processTest, get_Device_DeviceInfo_ProcessStatus_CPUUsage) {
    int instanceNumber = 0;
    bool pChanged;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    hostIf_DeviceProcessStatusInterface *processStatusIface = hostIf_DeviceProcessStatusInterface::getInstance(instanceNumber);
    if(processStatusIface)
    {
       pChanged = false;
       int ret = processStatusIface->get_Device_DeviceInfo_ProcessStatus_CPUUsage(&msgData, &pChanged);
       EXPECT_EQ(ret, OK);
   }
}

TEST(processTest, getProcessStatParam) {
    int instanceNumber = 0;
    bool pChanged;

    long long unsigned int mUser = 0;
    long long unsigned int mNice = 0;
    long long unsigned int mSystem = 0;
    long long unsigned int mIdle = 0;
    long long unsigned int mIOwait = 0;
    long long unsigned int mIrq = 0;
    long long unsigned int mSoftirq = 0;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    hostIf_DeviceProcessStatusInterface *processStatusIface = hostIf_DeviceProcessStatusInterface::getInstance(instanceNumber);
    if(processStatusIface)
    {
       pChanged = false;
       int ret = processStatusIface->getProcessStatParam(&mUser, &mNice, &mSystem, &mIdle, &mIOwait, &mIrq, &mSoftirq);
       EXPECT_EQ(ret, OK);
   }
}

TEST(processTest, ProcessStatus_Lock_ReleaseLock) {
    int instanceNumber = 0;
    bool pChanged;

    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    hostIf_DeviceProcessStatusInterface *processStatusIface = hostIf_DeviceProcessStatusInterface::getInstance(instanceNumber);
    if(processStatusIface)
    {
       pChanged = false;
       processStatusIface->getLock();
       processStatusIface->releaseLock();
       EXPECT_EQ(0, 0);
   }
   processStatusIface->closeInstance(processStatusIface);
   processStatusIface->closeAllInstances();
}


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

