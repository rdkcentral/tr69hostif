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
 * @file hostIf_msgHandler.cpp
 * @brief The header file provides HostIf message handler information APIs.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#include <mutex>
#include <chrono>
#include <atomic>
#include <time.h>
#include "hostIf_main.h"
#include "hostIf_msgHandler.h"
#include "hostIf_utils.h"
#include "hostIf_dsClient_ReqHandler.h"
#include "hostIf_DeviceClient_ReqHandler.h"
#include "hostIf_XrdkCentralT2_ReqHandler.h"
#include "safec_lib.h"

#ifdef USE_XRESRC
#include "hostIf_XREClient_ReqHandler.h"
#endif /* USE_XRESRC */
#ifdef USE_MoCA_PROFILE
#include "hostIf_MoCAClient_ReqHandler.h"
#endif /* USE_MoCA_PROFILE */
#include "hostIf_EthernetClient_ReqHandler.h"
#include "hostIf_IPClient_ReqHandler.h"
#include "hostIf_TimeClient_ReqHandler.h"
#ifdef USE_WIFI_PROFILE
#include "hostIf_WiFi_ReqHandler.h"
#endif /* USE_WIFI_PROFILE */
#ifdef USE_DHCPv4_PROFILE
#include "hostIf_DHCPv4Client_ReqHandler.h"
#endif /* WITH_DHCP_PROFILE*/
#ifdef USE_STORAGESERVICE_PROFILE
#include "hostIf_StorageSrvc_ReqHandler.h"
#endif /* USE_STORAGESERVICE_PROFILE */
#ifdef USE_INTFSTACK_PROFILE
#include "hostIf_InterfaceStackClient_ReqHandler.h"
#endif /* USE_INTFSTACK_PROFILE */
#ifdef SNMP_ADAPTER_ENABLED
#include "hostIf_SNMPClient_ReqHandler.h"
#endif
#include "x_rdk_req_handler.h"
#include "rdk_otlp_instrumentation.h"
extern GHashTable* paramMgrhash;
extern T_ARGLIST argList;
static std::mutex get_handler_mutex;
static std::mutex set_handler_mutex;
static int getCount = 0;
static int setCount = 0;
static std::atomic<int> getCountSinceBoot{0};
static time_t bootTimeSec = 0;
static std::atomic<int> setCountSinceBoot{0};
static time_t bootTimeSecSet =0;
static std::atomic<bool> loggedGet200Within1Min {false};
static std::atomic<bool> loggedGet1000Within5Min {false};
static std::atomic<bool> loggedSet200Within1Min {false};
static std::atomic<bool> loggedSet1000Within5Min {false};

#define PARAM_VALUE_STR_SIZE 128


void paramValueToString(const HOSTIF_MsgData_t *stMsgData, char *paramValueStr, size_t strSize)
{
    if (!stMsgData || !paramValueStr || strSize == 0) {
        if (paramValueStr && strSize > 0)
            snprintf(paramValueStr, strSize, "<invalid input>");
        return;
    }

    switch (stMsgData->paramtype) {
        case hostIf_StringType:
        case hostIf_DateTimeType:
            snprintf(paramValueStr, strSize, "%s", (const char*)stMsgData->paramValue);
            break;
        case hostIf_IntegerType: {
            int val = 0;
            memcpy(&val, stMsgData->paramValue, sizeof(val));
            snprintf(paramValueStr, strSize, "%d", val);
            break;
        }
        case hostIf_UnsignedIntType: {
            unsigned int val = 0;
            memcpy(&val, stMsgData->paramValue, sizeof(val));
            snprintf(paramValueStr, strSize, "%u", val);
            break;
        }
        case hostIf_BooleanType: {
            bool val = false;
            memcpy(&val, stMsgData->paramValue, sizeof(val));
            snprintf(paramValueStr, strSize, "%s", val ? "true" : "false");
            break;
        }
        case hostIf_UnsignedLongType: {
            unsigned long val = 0;
            memcpy(&val, stMsgData->paramValue, sizeof(val));
            snprintf(paramValueStr, strSize, "%lu", val);
            break;
        }
        default:
            snprintf(paramValueStr, strSize, "<unknown or unsupported type>");
            break;
    }
}

int hostIf_GetMsgHandler(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    int ret = NOK;
    getCount++;
    if (getCount % 10 == 0) {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d] GET called %d times\n",__FUNCTION__, __LINE__, getCount);
    }
    std::lock_guard<std::mutex> lock(get_handler_mutex);
     // On first call, record boot time
    if (bootTimeSec == 0) {
        struct timespec ts;
        clock_gettime(CLOCK_BOOTTIME, &ts);
        bootTimeSec = ts.tv_sec;
    }
    getCountSinceBoot++;

    // Calculate seconds since boot
    struct timespec now;
    clock_gettime(CLOCK_BOOTTIME, &now);
    long secondsSinceBoot = now.tv_sec - bootTimeSec;

    // Log if getCount reaches 200 before 1 minute from boot
    if ( !loggedGet200Within1Min && getCountSinceBoot >= 200 && secondsSinceBoot <= 60) {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
            "[%s:%d] GET count reached 200 within 1 minute after boot (actual: %ld seconds)\n",
            __FUNCTION__, __LINE__, secondsSinceBoot);
        #ifdef T2_EVENT_ENABLED
        t2CountNotify("TR69HOSTIF_GET_200_WITHIN_1MIN", 1);
        #endif
        loggedGet200Within1Min = true;
    }

    // Log if getCount reaches 1000 before 5 minutes from boot
    if ( !loggedGet1000Within5Min && getCountSinceBoot >= 1000 && secondsSinceBoot <= 300) {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
            "[%s:%d] GET count reached 1000 within 5 minutes after boot (actual: %ld seconds)\n",
            __FUNCTION__, __LINE__, secondsSinceBoot);
        #ifdef T2_EVENT_ENABLED
        t2CountNotify("TR69HOSTIF_GET_1000_WITHIN_5MIN", 1);
        #endif
        loggedGet1000Within5Min =true;
    }
    // Start child span (retrieves parent context from TraceContextManager)
    rdk_otlp_start_child_span(stMsgData->paramName, "get");
    try
    {
        /* Find the respective manager and forward the request*/
        msgHandler *pMsgHandler = HostIf_GetMgr(stMsgData);

        if(pMsgHandler)
        {
            auto startTime = std::chrono::high_resolution_clock::now();
            ret = pMsgHandler->handleGetMsg(stMsgData);
            auto endTime = std::chrono::high_resolution_clock::now();
            auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
            char paramValueStr[PARAM_VALUE_STR_SIZE] = {0};
            paramValueToString(stMsgData, paramValueStr, sizeof(paramValueStr));
            

            // Calculate time taken in microseconds
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
                "[%s:%d] ret: %d, paramName: %s, paramValue: %s, timeTaken: %ld us\n",
                __FUNCTION__, __LINE__, ret,
                stMsgData->paramName,
                paramValueStr,
                timeTaken);
            
            // Record metric - convert microseconds to seconds
            double duration_seconds = timeTaken / 1000000.0;
	    rdk_otlp_metrics_record_parameter_operation(stMsgData->paramName, "get", duration_seconds);
           // Telemetry and debug log if processing time > 5 second (1,000,000 us)
            if (timeTaken > 5000000) {
                // Debug log
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
                    "[%s:%d] Slow GET detected: paramName: %s, timeTaken: %ld ms\n",
                    __FUNCTION__, __LINE__, stMsgData->paramName, timeTaken/1000);
                #ifdef T2_EVENT_ENABLED
                // Telemetry: report paramName
                t2ValNotify("TR69HOSTIF_GET_TIMEOUT_PARAM", stMsgData->paramName);
                #endif
                
            }
        }
        
    }
    catch (const std::exception& e)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s:%d] Exception caught %s\n", __FUNCTION__, __LINE__, e.what());
    }
    // Finish child span
    rdk_otlp_finish_child_span();
    return ret;
}

int hostIf_SetMsgHandler(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    setCount++;
     if (setCount % 10 == 0) {
         RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d] SET called %d times\n",__FUNCTION__, __LINE__, setCount);
     }

    std::lock_guard<std::mutex> lock(set_handler_mutex);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

     if (bootTimeSecSet == 0) {
        struct timespec ts;
        clock_gettime(CLOCK_BOOTTIME, &ts);
        bootTimeSecSet = ts.tv_sec;
    }
    setCountSinceBoot++;

    // Calculate seconds since boot for SET
    struct timespec now;
    clock_gettime(CLOCK_BOOTTIME, &now);
    long secondsSinceBoot = now.tv_sec - bootTimeSecSet;

    // Log if setCount reaches 200 before 1 minute from boot
    if (!loggedSet200Within1Min && setCountSinceBoot >= 200 && secondsSinceBoot <= 60) {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
            "[%s:%d] SET count reached 200 within 1 minute after boot (actual: %ld seconds)\n",
            __FUNCTION__, __LINE__, secondsSinceBoot);
        #ifdef T2_EVENT_ENABLED
        t2CountNotify("TR69HOSTIF_SET_200_WITHIN_1MIN", 1);
        #endif
        loggedSet200Within1Min = true ;
    }

    // Log if setCount reaches 1000 before 5 minutes from boot
    if (!loggedSet1000Within5Min && setCountSinceBoot >= 1000 && secondsSinceBoot <= 300) {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
            "[%s:%d] SET count reached 1000 within 5 minutes after boot (actual: %ld seconds)\n",
            __FUNCTION__, __LINE__, secondsSinceBoot);
        #ifdef T2_EVENT_ENABLED
        t2CountNotify("TR69HOSTIF_SET_1000_WITHIN_5MIN", 1);
        #endif
        loggedSet1000Within5Min = true;
    }

   // Start child span (retrieves parent context from TraceContextManager)
    rdk_otlp_start_child_span(stMsgData->paramName, "set");
    /* Find the respective manager and forward the request*/
    msgHandler *pMsgHandler = HostIf_GetMgr(stMsgData);

    if(pMsgHandler)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        ret = pMsgHandler->handleSetMsg(stMsgData);
        auto endTime = std::chrono::high_resolution_clock::now();
        auto timeTakenset = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

        char paramValueStr[128] = {0};
        paramValueToString(stMsgData, paramValueStr, sizeof(paramValueStr));

        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
                "[%s:%d] ret: %d, paramName: %s, paramValue: %s, timeTaken: %ld us\n",
                __FUNCTION__, __LINE__, ret,
                stMsgData->paramName,
                paramValueStr,
                timeTakenset);
        
        // Record metric - convert microseconds to seconds
        double duration_seconds = timeTakenset / 1000000.0;
	rdk_otlp_metrics_record_parameter_operation(stMsgData->paramName, "set", duration_seconds);
       // Telemetry and debug log if processing time > 5 seconds (5,000,000 us)
        if (timeTakenset > 5000000) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
                "[%s:%d] Slow SET detected: paramName: %s, timeTaken: %ld ms\n",
                __FUNCTION__, __LINE__, stMsgData->paramName, timeTakenset/1000);
            #ifdef T2_EVENT_ENABLED
            t2ValNotify("TR69HOSTIF_SET_TIMEOUT_PARAM", stMsgData->paramName);
            #endif
            
        }
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    rdk_otlp_finish_child_span();
    return ret;
}

int hostIf_GetAttributesMsgHandler(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    //printf("[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    /* Find the respective manager and forward the request*/
    msgHandler *pMsgHandler = HostIf_GetMgr(stMsgData);

    if(pMsgHandler)
        ret = pMsgHandler->handleGetAttributesMsg(stMsgData);

    //printf("[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return ret;
}

int hostIf_SetAttributesMsgHandler(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    //printf("[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Entering...\n", __FUNCTION__, __FILE__);
    /* Find the respective manager and forward the request*/
    msgHandler *pMsgHandler = HostIf_GetMgr(stMsgData);
    if (NULL != pMsgHandler)
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Valid Handler for %s", __FUNCTION__, __FILE__,stMsgData->paramName);
    }
    if(pMsgHandler)
        ret = pMsgHandler->handleSetAttributesMsg(stMsgData);

    //printf("[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return ret;
}

//------------------------------------------------------------------------------
// hostIf_Free_stMsgData: Freeing HOSTIF_MsgData_t.
//------------------------------------------------------------------------------
void hostIf_Free_stMsgData (HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering...\n", __FUNCTION__, __FILE__);
    if(stMsgData)
    {
//        delete stMsgData;
        g_free(stMsgData);
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting...\n", __FUNCTION__, __FILE__);
}


//------------------------------------------------------------------------------
// hostIf_Init_Dummy_stMsgData: Initialized Dummy test data
//------------------------------------------------------------------------------
void hostIf_Init_Dummy_stMsgData (HOSTIF_MsgData_t **stMsgData)
{
    errno_t rc = -1;
    HOSTIF_MsgData_t *stMsgDummyData = NULL;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    if(NULL == *stMsgData)
    {
        stMsgDummyData = new HOSTIF_MsgData_t();
        rc=strcpy_s(stMsgDummyData->paramName,sizeof(stMsgDummyData->paramName), TEST_STR);
        if(rc!=EOK)
        {
            ERR_CHK(rc);
        }
        stMsgDummyData->reqType = HOSTIF_GET;
        stMsgDummyData->instanceNum = 0;
        *stMsgData = stMsgDummyData;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
}

//------------------------------------------------------------------------------
// hostIf_Print_msgData: Print Msg data
//------------------------------------------------------------------------------
void hostIf_Print_msgData(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"\n****************************************************\n");
    cout << "Parameter Name : "<< stMsgData->paramName << endl;
    cout << "Parameter Request Type : "<< stMsgData->reqType << endl;
    switch (stMsgData->paramtype) {
    case hostIf_StringType:
        cout << "Parameter Value : "<< (char *)stMsgData->paramValue << endl;
        break;
    case hostIf_BooleanType:
        cout << "Parameter Value : "<< (get_boolean(stMsgData->paramValue)?"true":"false")  << endl;
        break;
    default:
        cout << "Parameter Value : "<< get_int(stMsgData->paramValue) << endl;
    }
    cout << "Parameter Length : "<< stMsgData->paramLen << endl;
    cout << "Parameter Type : "<< stMsgData->paramtype << endl;
    cout << "Parameter Instance number : "<< stMsgData->instanceNum << endl;
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"****************************************************\n");
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
}

bool hostIf_initalize_ConfigManger()
{
    bool bVal = true;
    HostIf_ParamMgr_t mgrName = HOSTIF_INVALID_Mgr;
    char param[100] = {'\0'};
    char mgr[16] = {'\0'};

    FILE *fp = NULL;

    fp = fopen(argList.confFile, "r");

    if (fp == NULL)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s]Error opening the %s file\n", __FILE__, __FUNCTION__, argList.confFile );
        bVal = false;
        exit(EXIT_FAILURE);
    }

    paramMgrhash = g_hash_table_new(g_str_hash, g_int_equal);
    if(paramMgrhash)
    {
        while (fscanf( fp, "%99s %15s", param, mgr) != EOF)
        {
            if(strcasecmp(mgr, "deviceMgr") == 0)
            {
                mgrName = HOSTIF_DeviceMgr;
            }
            else if(strcasecmp(mgr, "dsMgr") == 0)
            {
                mgrName = HOSTIF_DSMgr;
            }
            else if(strcasecmp(mgr, "telemetryMgr") == 0)
            {
                mgrName = HOSTIF_TelemetryMgr;
            }
#ifdef USE_MoCA_PROFILE
            else if(strcasecmp(mgr, "mocaMgr") == 0)
            {
                mgrName = HOSTIF_MoCAMgr;
            }
#endif /* USE_MoCA_PROFILE*/
#ifdef USE_XRESRC
            else if(strcasecmp(mgr, "xreMgr") == 0)
            {
                mgrName = HOSTIF_XREMgr;
            }
#endif /* USE_XRESRC*/
            else if(strcasecmp(mgr, "ethernetMgr") == 0)
            {
                mgrName = HOSTIF_EthernetMgr;
            }
            else if(strcasecmp(mgr, "ipMgr") == 0)
            {
                mgrName = HOSTIF_IPMgr;
            }
            else if(strcasecmp(mgr, "timeMgr") == 0)
            {
                mgrName = HOSTIF_TimeMgr;
            }
#ifdef USE_WIFI_PROFILE
            else if (strcasecmp(mgr, "wifiMgr") == 0)
            {
                mgrName = HOSTIF_WiFiMgr;
            }
#endif
#ifdef USE_DHCPv4_PROFILE
            else if(strcasecmp(mgr, "dhcpv4Mgr") == 0)
            {
                mgrName = HOSTIF_DHCPv4;
            }
#endif /* USE_DHCPv4_PROFILE */
#ifdef USE_INTFSTACK_PROFILE
            else if(strcasecmp(mgr, "ifStackMgr") == 0)
            {
                mgrName = HOSTIF_InterfaceStack;
            }
#endif /* USE_INTFSTACK_PROFILE */
#ifdef USE_STORAGESERVICE_PROFILE
            else if(strcasecmp(mgr, "storageSrvcMgr") == 0)
            {
                mgrName = HOSTIF_StorageSrvcMgr;
            }
#endif /* USE_STORAGESERVICE_PROFILE */
#ifdef SNMP_ADAPTER_ENABLED
            else if(strcasecmp(mgr, "snmpAdapterMgr") == 0)
            {
                mgrName = HOSTIF_SNMPAdapterMgr;
            }
#endif
            else if(strcasecmp(mgr, "rdkProfileMgr") == 0)
            {
                mgrName = HOSTIF_RdkProfileMgr;
            }

            if(mgrName != HOSTIF_INVALID_Mgr) {
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"param: %s \tMgr: [%s => %d]\n", param, mgr, mgrName);
                g_hash_table_insert(paramMgrhash, (gpointer)g_strdup(param), (gpointer)mgrName);
            } else {
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Invalid Hostif Manager for param %s, Skipping...\n", param);
            }
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed to initialize hash table.\n");
        bVal = false;
    }
    fclose(fp);  //CID:82865 - Resource leak
    return bVal;
}

msgHandler* HostIf_GetMgr(HOSTIF_MsgData_t *stMsgHandlerData)
{
    const char *pParam = stMsgHandlerData->paramName;
    HostIf_ParamMgr_t mgrId;
    msgHandler* pRet = NULL;
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] Get proper manager for parameter :- %s \n", __FUNCTION__,stMsgHandlerData->paramName);
    if(NULL != paramMgrhash)
    {
        GList *list = g_hash_table_get_keys(paramMgrhash);
        GList *keys = list;
        while(keys) {
            char *data = (char *)keys->data;
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] DATA = %s\n",__FUNCTION__, data);
            if(strncmp(data,pParam,strlen(data)) == 0)
            {
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] pParam: %s data:%s\n", __FUNCTION__,pParam, data);
                break;
            }
            keys = keys->next;
        }
        if(keys)
        {
            gpointer item_ptr = g_hash_table_lookup(paramMgrhash, (char *)keys->data);
            if(item_ptr == NULL) {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Not able to find proper manager for param %s.\n", __FUNCTION__, pParam);
            }
            else
            {
                mgrId = (HostIf_ParamMgr_t)GPOINTER_TO_INT(item_ptr);

                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s][For Parameter path: \"%s\"; Manager :%d ] \n", __FUNCTION__, pParam, mgrId);

                switch (mgrId) {
                case HOSTIF_DSMgr:
                    pRet = DSClientReqHandler::getInstance();
                    break;
#ifdef USE_XRESRC
                case HOSTIF_XREMgr:
                    pRet = XREClientReqHandler::getInstance();
                    break;
#endif /*USE_XRESRC*/
                case HOSTIF_DeviceMgr:
                    pRet = DeviceClientReqHandler::getInstance();
                    break;
#ifdef USE_MoCA_PROFILE
                case HOSTIF_MoCAMgr:
                    pRet = MoCAClientReqHandler::getInstance();
                    break;
#endif /* USE_MoCA_PROFILE*/
                case HOSTIF_EthernetMgr:
                    pRet = EthernetClientReqHandler::getInstance();
                    break;
                case HOSTIF_TimeMgr:
                    pRet = TimeClientReqHandler::getInstance();
                    break;
                case HOSTIF_IPMgr:
                    pRet = IPClientReqHandler::getInstance();
                    break;
#ifdef USE_WIFI_PROFILE
                case HOSTIF_WiFiMgr:
                    pRet = WiFiReqHandler::getInstance();
                    break;
#endif /* USE_WIFI_PROFILE*/
#ifdef USE_DHCPv4_PROFILE
                case HOSTIF_DHCPv4:
                    pRet = DHCPv4ClientReqHandler::getInstance();
                    break;
#endif /* USE_DHCPv4_PROFILE*/
#ifdef USE_INTFSTACK_PROFILE
                case HOSTIF_InterfaceStack:
                    pRet = InterfaceStackClientReqHandler::getInstance();
                    break;
#endif /* USE_INTFSTACK_PROFILE */
#ifdef USE_STORAGESERVICE_PROFILE
                case HOSTIF_StorageSrvcMgr:
                    pRet = StorageSrvcReqHandler::getInstance();
                    break;
#endif /* USE_STORAGESERVICE_PROFILE */
#ifdef SNMP_ADAPTER_ENABLED
                case HOSTIF_SNMPAdapterMgr:
                    pRet = SNMPClientReqHandler::getInstance();
                    break;
#endif
                case HOSTIF_TelemetryMgr:
                    pRet = XRdkCentralT2::getInstance();
                    break;
                case HOSTIF_RdkProfileMgr:
                    pRet = X_rdk_req_hdlr::getInstance();
                    break;
                default:
                    ;
                }
            }
        }
        else {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s()]Not able to get Key  parameter :- %s \n", __FUNCTION__,stMsgHandlerData->paramName);
        }
        if (list)
            g_list_free(list);
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s()] paramMgrhash is Null  parameter :- %s \n", __FUNCTION__,stMsgHandlerData->paramName);
    }
    return pRet;
}


bool hostIf_ConfigProperties_Init()
{
    GKeyFile *key_file = NULL;
    GError *error = NULL;
    guint group = 0, key = 0;
    HostIf_ParamMgr_t mgrName = HOSTIF_INVALID_Mgr;  //CID:82970 - UNINIT

    if(argList.confFile[0] == '\0')
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] Failed to read Hostif Manager Configuration file \n", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    key_file = g_key_file_new();

    if(!key_file) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] Failed to g_key_file_new() \n", __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    if(!g_key_file_load_from_file(key_file, argList.confFile, G_KEY_FILE_KEEP_COMMENTS, &error))
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s", error->message);
        g_key_file_free(key_file);
        g_error_free(error);
        return false;
    }
    else
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d]\n", __FILE__, __FUNCTION__, __LINE__);
        paramMgrhash = g_hash_table_new(g_str_hash, g_int_equal);
        if(paramMgrhash) {
            gsize groups_id, num_keys;
            gchar **groups, **keys, *value;
            groups = g_key_file_get_groups(key_file, &groups_id);
            for(group = 0; group < groups_id; group++)
            {
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"group %lu/%lu: \t%s\n", (unsigned long)group, (unsigned long)(groups_id - 1), groups[group]);
                if(0 == strncasecmp(HOSTIF_MGR_GROUP, groups[group], strlen(groups[group])))
                {
                    keys = g_key_file_get_keys(key_file, groups[group], &num_keys, &error);
                    for(key = 0; key < num_keys; key++)
                    {
                        value = g_key_file_get_value(key_file,	groups[group],	keys[key],	&error);
                        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"\t\tkey %lu/%lu: \t%s => %s\n", (unsigned long)key, (unsigned long)(num_keys - 1), keys[key], value);

                        if(strcasecmp(value, "deviceMgr") == 0)
                        {
                            mgrName = HOSTIF_DeviceMgr;
                        }
                        else if(strcasecmp(value, "dsMgr") == 0)
                        {
                            mgrName = HOSTIF_DSMgr;
                        }
#ifdef USE_MoCA_PROFILE
                        else if(strcasecmp(value, "mocaMgr") == 0)
                        {
                            mgrName = HOSTIF_MoCAMgr;
                        }
#endif /* USE_MoCA_PROFILE*/
#ifdef USE_XRESRC
                        else if(strcasecmp(value, "xreMgr") == 0)
                        {
                            mgrName = HOSTIF_XREMgr;
                        }
#endif /* USE_XRESRC*/
                        else if(strcasecmp(value, "ethernetMgr") == 0)
                        {
                            mgrName = HOSTIF_EthernetMgr;
                        }
                        else if(strcasecmp(value, "ipMgr") == 0)
                        {
                            mgrName = HOSTIF_IPMgr;
                        }
                        else if(strcasecmp(value, "timeMgr") == 0)
                        {
                            mgrName = HOSTIF_TimeMgr;
                        }

#ifdef USE_DHCPv4_PROFILE
                        else if(strcasecmp(value, "dhcpv4Mgr") == 0)
                        {
                            mgrName = HOSTIF_DHCPv4;
                        }
#endif /* USE_DHCPv4_PROFILE*/
#ifdef USE_STORAGESERVICE_PROFILE
                        else if(strcasecmp(value, "storageSrvcMgr") == 0)
                        {
                            mgrName = HOSTIF_StorageSrvcMgr;
                        }
#endif /* USE_STORAGESERVICE_PROFILE */
#ifdef SNMP_ADAPTER_ENABLED
                        else if(strcasecmp(value, "snmpAdapterMgr") == 0)
                        {
                            mgrName = HOSTIF_SNMPAdapterMgr;
                        }
#endif
                        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"param: %s \tMgr: [%s => %d]\n", keys[key], value, mgrName);
                        g_hash_table_insert(paramMgrhash, (gpointer)g_strdup(keys[key]), (gpointer)mgrName);

                        g_free(value);
                    }
                    g_strfreev(keys);
                }
                else if(0 == strncasecmp(HOSTIF_MGR_GROUP, groups[group], strlen(groups[group])))
                {

                }
                else if (0 == strncasecmp(HOSTIF_GENERAL_CONG_GROUP, groups[group], strlen(groups[group])))
                {

                }
            }
            g_strfreev(groups);
        }
    }

    g_key_file_free(key_file);
    g_error_free(error);

    return true;
}



/** @} */
/** @} */
