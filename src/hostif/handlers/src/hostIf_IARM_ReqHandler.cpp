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


#include "hostIf_main.h"
#include "hostIf_msgHandler.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_dsClient_ReqHandler.h"
#include "hostIf_DeviceClient_ReqHandler.h"
#include "hostIf_updateHandler.h"
#include "hostIf_utils.h"
#include "libIBus.h"
#include "libIARM.h"
#include "sysMgr.h"
#ifdef RDKV_TR69
#include "pwrMgr.h"
#else
#include "power_controller.h"
#include <thread>
#endif
#ifdef SNMP_ADAPTER_ENABLED
#include "hostIf_SNMPClient_ReqHandler.h"
#endif
#include "waldb.h"
#include "hostIf_NotificationHandler.h"
#include "Device_DeviceInfo.h"
#include "safec_lib.h"

#define X_RDK_RFC_DEEPSLEEP_ENABLE           "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Power.DeepSleepNotification.Enable"
#define RETRYSLEEP (300 * 1000) //Retry sleep
#ifndef RDKV_TR69
static bool IsPwrCtlInt = false;
#endif
static bool TR69_HostIf_Mgr_Init();
static bool TR69_HostIf_Mgr_Connect();
static bool TR69_HostIf_Mgr_Get_RegisterCall();
static IARM_Result_t tr69hostIfMgr_Stop(void);
static IARM_Result_t _Gettr69HostIfMgr(void *arg);
static IARM_Result_t _Settr69HostIfMgr(void *arg);
static IARM_Result_t _SetAttributestr69HostIfMgr(void *arg);
static IARM_Result_t _GetAttributestr69HostIfMgr(void *arg);
static IARM_Result_t _RegisterForEventstr69HostIfMgr(void *arg);
#ifdef RDKV_TR69
static void _hostIf_EventHandler(const char *, IARM_EventId_t, void *, size_t);
#else
static void _hostIf_EventHandler(const PowerController_PowerState_t currentState,
    const PowerController_PowerState_t newState, void* userdata);
#endif
//----------------------------------------------------------------------
// hostIf_IARM_IF_Start: This shall be use to initialize and register
// the  hostIf application to IARM bus.
//----------------------------------------------------------------------
bool hostIf_IARM_IF_Start()
{
    bool ret = false;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    if ( TR69_HostIf_Mgr_Init() &&
            TR69_HostIf_Mgr_Connect() &&
            TR69_HostIf_Mgr_Get_RegisterCall() )
    {
        ret = true;
        /* Initialize Managers */
        msgHandler *pMsgHandler;
        #ifdef RDKV_TR69
        pMsgHandler = DSClientReqHandler::getInstance();
        pMsgHandler->init();
        #endif
        pMsgHandler = DeviceClientReqHandler::getInstance();
        pMsgHandler->init();
        

#ifdef SNMP_ADAPTER_ENABLED
        pMsgHandler = SNMPClientReqHandler::getInstance();
        pMsgHandler->init();
#endif
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);

    return ret;
}
#ifndef RDKV_TR69
void hostIf_getPwrContInterface()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    /*TODO: remove this sleep after fix METROL-1045*/
    sleep(10);//added sleep wait for the WPEframework active.
    msgHandler *pMsgHandler;
    pMsgHandler = DSClientReqHandler::getInstance();
    pMsgHandler->init();

    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s:%d]: start PowerController_Init().. \n", __FUNCTION__, __LINE__);
    PowerController_Init();
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s:%d]: completed PowerController_Init().. \n", __FUNCTION__, __LINE__);
    while(true)
    {
        if(POWER_CONTROLLER_ERROR_NONE ==  PowerController_Connect())
        {
            hostIf_DeviceInfo::getInstance(0)->setPowerConInterface(true);
            IsPwrCtlInt = true;
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s] Got the powercontroller interface.. IsPwrCtlInt = %s\n", __FUNCTION__, __FILE__ , (IsPwrCtlInt?"true":"false"));
            break;
        }
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s] Retry after %d usec..\n", __FUNCTION__, __FILE__, RETRYSLEEP);
        usleep(RETRYSLEEP); //retry after RETRYSLEEP milli seconds.

    }
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s] Registering power mode change callback..\n", __FUNCTION__, __FILE__);
    PowerController_RegisterPowerModeChangedCallback(_hostIf_EventHandler, nullptr);
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s] Registered power mode change callback..\n", __FUNCTION__, __FILE__);

    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
}
#endif

//----------------------------------------------------------------------
//Initialization: This shall be initialized tr69 application to IARM bus.
//----------------------------------------------------------------------
static bool TR69_HostIf_Mgr_Init()
{
    IARM_Result_t err = IARM_RESULT_SUCCESS;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"#######################################################");
    err = IARM_Bus_Init(IARM_BUS_TR69HOSTIFMGR_NAME);

    if(IARM_RESULT_SUCCESS != err)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s()] Failed to 'IARM_Bus_Init(%s)', return with Error "\
                "code: (%d).\n", __FUNCTION__,IARM_BUS_TR69HOSTIFMGR_NAME, err);
        return false;
    }
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] Success 'IARM_Bus_Init(%s)'.\n", __FUNCTION__,    IARM_BUS_TR69HOSTIFMGR_NAME);
    #ifndef RDKV_TR69
    // Get powercontroller thunder client interface in separate thread
    std::thread pwrThread(hostIf_getPwrContInterface);
    if(pwrThread.joinable())
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s:%d]: created getPwrContInterface thread.. \n", __FUNCTION__, __LINE__);
        pwrThread.detach();  // Detach the thread to run independently
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s:%d]: Failed to create getPwrContInterface thread.. \n", __FUNCTION__, __LINE__);
    }
    #endif

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}
//----------------------------------------------------------------------
// Connect to IARM Bus:
//----------------------------------------------------------------------
static bool TR69_HostIf_Mgr_Connect()
{
    IARM_Result_t err = IARM_RESULT_SUCCESS;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);

    err = IARM_Bus_Connect();

    if(IARM_RESULT_SUCCESS != err)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to IARM_Bus_Connect(), return with Error code: %d\n", __FUNCTION__, err);
        IARM_Bus_Term();
        return false;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

static IARM_Result_t _RegisterForEventstr69HostIfMgr(void *arg)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    updateHandler::reset();
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return IARM_RESULT_SUCCESS;

}


//----------------------------------------------------------------------
//Register RPC call: This shall be RPC call registered to IARM bus.
//----------------------------------------------------------------------
static bool TR69_HostIf_Mgr_Get_RegisterCall()
{
    bool ret = true;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    /* Get RPC: to get the value*/
    if( (IARM_Bus_RegisterCall(IARM_BUS_TR69HOSTIFMGR_API_GetParams, _Gettr69HostIfMgr) != IARM_RESULT_SUCCESS) ||
            (IARM_Bus_RegisterCall(IARM_BUS_TR69HOSTIFMGR_API_SetParams, _Settr69HostIfMgr) != IARM_RESULT_SUCCESS) ||
            (IARM_Bus_RegisterCall(IARM_BUS_TR69HOSTIFMGR_API_SetAttributes, _SetAttributestr69HostIfMgr) != IARM_RESULT_SUCCESS) ||
            (IARM_Bus_RegisterCall(IARM_BUS_TR69HOSTIFMGR_API_GetAttributes, _GetAttributestr69HostIfMgr) != IARM_RESULT_SUCCESS) ||
            (IARM_Bus_RegisterCall(IARM_BUS_TR69HOSTIFMGR_API_RegisterForEvents, _RegisterForEventstr69HostIfMgr) != IARM_RESULT_SUCCESS))
    {
        ret = false;
    }

    /* Notification RPC:*/
    IARM_Bus_RegisterEvent(IARM_BUS_TR69HOSTIFMGR_EVENT_MAX);
    #ifdef RDKV_TR69
     IARM_Bus_RegisterEventHandler(IARM_BUS_PWRMGR_NAME,IARM_BUS_PWRMGR_EVENT_MODECHANGED, _hostIf_EventHandler);
    #endif
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return ret;
}

void hostIf_GetAttributesReqHandler(void *arg)
{
    int ret;
    HOSTIF_MsgData_t *stMsgData = (HOSTIF_MsgData_t *) arg;
    g_printf("[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    if(stMsgData)
    {
        ret = hostIf_SetAttributesMsgHandler(stMsgData);
        g_printf("[hostIf_GetAttributesReqHandler : hostIf_MsgHandler()] Return value : %d\n", ret);
        /*

                if(ret == OK)
                {
                    hostIf_Print_msgData(stMsgData);
        //            hostIf_Free_stMsgData(stMsgData);
                }
        */
    }
    g_printf("[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
}


static IARM_Result_t _GetAttributestr69HostIfMgr(void *arg)
{
    g_printf("[%s:%s] Entering..\n", __FUNCTION__, __FILE__);


    HOSTIF_MsgData_t *param = (HOSTIF_MsgData_t *) arg;
    if(param)
    {
        // g_printf("[%s:%s] paramName :%s \n", __FUNCTION__, __FILE__, (char *)param->paramName);
        hostIf_GetAttributesReqHandler(arg);
    }


    g_printf("[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return IARM_RESULT_SUCCESS;
}

void hostIf_SetAttributesReqHandler(void *arg)
{
    int ret;
    HOSTIF_MsgData_t *stMsgData = (HOSTIF_MsgData_t *) arg;
    g_printf("[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    if(stMsgData)
    {
        ret = hostIf_SetAttributesMsgHandler(stMsgData);
        g_printf("[*****hostIf_SetAttributesReqHandler : hostIf_MsgHandler()] Return value : %d for Parameter %s\n", ret,stMsgData->paramName);
        /*

                if(ret == OK)
                {
                    hostIf_Print_msgData(stMsgData);
        //            hostIf_Free_stMsgData(stMsgData);
                }
        */
    }
    g_printf("[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
}


static IARM_Result_t _SetAttributestr69HostIfMgr(void *arg)
{
    g_printf("[%s:%s] Entering..\n", __FUNCTION__, __FILE__);


    HOSTIF_MsgData_t *param = (HOSTIF_MsgData_t *) arg;
    if(param)
    {
        // g_printf("[%s:%s] paramName :%s \n", __FUNCTION__, __FILE__, (char *)param->paramName);
        hostIf_SetAttributesReqHandler(arg);
    }


    g_printf("[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return IARM_RESULT_SUCCESS;
}


//----------------------------------------------------------------------
//Initialization: This shall be initialized tr69 application to IARM bus.
//----------------------------------------------------------------------
void hostIf_IARM_IF_Stop()
{
    /* Uninitialized Device setting client*/
    msgHandler *pMsgHandler = DSClientReqHandler::getInstance();
    pMsgHandler->unInit();
    tr69hostIfMgr_Stop();
}
//----------------------------------------------------------------------
//Initialization: This shall be initialized tr69 application to IARM bus.
//----------------------------------------------------------------------
static IARM_Result_t tr69hostIfMgr_Stop(void)
{
    IARM_Result_t err = IARM_RESULT_SUCCESS;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering...\n", __FUNCTION__, __FILE__);
    err = IARM_Bus_Disconnect();
    if(IARM_RESULT_SUCCESS != err)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to IARM_Bus_Disconnect(), return with Error code: %d\n", __FUNCTION__, err);
    }

    err = IARM_Bus_Term();
    if(IARM_RESULT_SUCCESS != err)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to IARM_Bus_Term(), return with Error code: %d\n", __FUNCTION__, err);
    }
    #ifndef RDKV_TR69
    if (IsPwrCtlInt) 
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Registering power mode change callback..\n", __FUNCTION__, __FILE__);
        PowerController_UnRegisterPowerModeChangedCallback(_hostIf_EventHandler);
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s] Registered power mode change callback..\n", __FUNCTION__, __FILE__);

        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%d]: start PowerController_Term().. \n", __FUNCTION__, __LINE__);
        PowerController_Term();
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%d]: completed PowerController_Term().. \n", __FUNCTION__, __LINE__);
        hostIf_DeviceInfo::getInstance(0)->setPowerConInterface(false);
        IsPwrCtlInt = false;
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%d]:  setPowerConInterface flag to false. and  IsPwrCtlInt=%s\n", __FUNCTION__, __LINE__, (IsPwrCtlInt?"true":"false"));

    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s:%d]: No PowerController interface .. IsPwrCtlInt = %d\n", __FUNCTION__, __LINE__, IsPwrCtlInt);
    }
    #endif
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return err;
}

void hostIf_SetReqHandler(void *arg)
{
    int ret;
    HOSTIF_MsgData_t *stMsgData = (HOSTIF_MsgData_t *) arg;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    if(stMsgData)
    {
        stMsgData->requestor = HOSTIF_SRC_IARM;
        stMsgData->bsUpdate = HOSTIF_NONE;
        ret = hostIf_SetMsgHandler(stMsgData);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[hostIf_SetReqHandler : hostIf_MsgHandler()] Return value : %d\n", ret);
        /*

                if(ret == OK)
                {
                    hostIf_Print_msgData(stMsgData);
        //            hostIf_Free_stMsgData(stMsgData);
                }
        */
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
}


//----------------------------------------------------------------------
//Initialization: This shall be initialized tr69 application to IARM bus.
//----------------------------------------------------------------------
static IARM_Result_t _Settr69HostIfMgr(void *arg)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);


    HOSTIF_MsgData_t *param = (HOSTIF_MsgData_t *) arg;
    if(param)
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] paramName :%s \n", __FUNCTION__, __FILE__, (char *)param->paramName);
        hostIf_SetReqHandler(arg);
    }


    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return IARM_RESULT_SUCCESS;
}


//----------------------------------------------------------------------
//Initialization: This shall be initialized tr69 application to IARM bus.
//----------------------------------------------------------------------
void hostIf_GetReqHandler(void *arg)
{
    int ret;
    HOSTIF_MsgData_t *stMsgData = (HOSTIF_MsgData_t *) arg;

//    hostIf_Init_Dummy_stMsgData (&stMsgData);

    if(stMsgData)
    {
        DataModelParam dmParam = {0};
        if(!getParamInfoFromDataModel(getDataModelHandle(), stMsgData->paramName, &dmParam))
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s: Invalid parameter name %s: doesn't exist in data-model\n", __FUNCTION__, stMsgData->paramName);
        }
        stMsgData->bsUpdate = getBSUpdateEnum(dmParam.bsUpdate);
        if(dmParam.objectName)
           free(dmParam.objectName);
        if(dmParam.paramName)
           free(dmParam.paramName);
        if(dmParam.bsUpdate)
           free(dmParam.bsUpdate);
        if(dmParam.access)
           free(dmParam.access);
        if(dmParam.dataType)
           free(dmParam.dataType);
        if(dmParam.defaultValue)
           free(dmParam.defaultValue);

        stMsgData->requestor = HOSTIF_SRC_IARM;
        ret = hostIf_GetMsgHandler(stMsgData);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[hostIf_GetReqHandler : hostIf_MsgHandler()] Return value : %d\n", ret);

        if(ret == OK)
        {
//            hostIf_Print_msgData(stMsgData);
            //hostIf_Free_stMsgData(stMsgData);
        }
        else
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s()] Failed with returned Value : %d for [%s]\n",
                    __FUNCTION__, ret, stMsgData->paramName);
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
}

//----------------------------------------------------------------------
//Initialization: This shall be initialized tr69 application to IARM bus.
//----------------------------------------------------------------------
static IARM_Result_t _Gettr69HostIfMgr(void *arg)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);


    HOSTIF_MsgData_t *param = (HOSTIF_MsgData_t *) arg;
    if(param)
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] paramName :%s \n", __FUNCTION__, __FILE__, (char *)param->paramName);
    }
    hostIf_GetReqHandler(arg);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return IARM_RESULT_SUCCESS;
}


//----------------------------------------------------------------------
//_hostIf_EventHandler: This is to listen the IARM events and handles.
//----------------------------------------------------------------------

#ifdef RDKV_TR69
static void _hostIf_EventHandler(const char *owner, IARM_EventId_t eventId, void *data, size_t len)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    if (0 == strcmp(owner, IARM_BUS_PWRMGR_NAME))
    {
        errno_t rc = -1;
        HOSTIF_MsgData_t stRfcData = {0};
        rc=strcpy_s(stRfcData.paramName,sizeof(stRfcData.paramName), X_RDK_RFC_DEEPSLEEP_ENABLE);
        if(rc!=EOK)
        {
            ERR_CHK(rc);
        }
        if((hostIf_DeviceInfo::getInstance(0)->get_xRDKCentralComRFC(&stRfcData) == OK) && (strncmp(stRfcData.paramValue, "true", sizeof("true")) == 0))
        {
            IARM_Bus_PWRMgr_EventData_t *param = (IARM_Bus_PWRMgr_EventData_t *)data;
            IARM_Bus_PWRMgr_PowerState_t curPowerState = param->data.state.curState;
            IARM_Bus_PWRMgr_PowerState_t newPowerState = param->data.state.newState;
            const char *event_time = NULL;

            if((newPowerState == IARM_BUS_PWRMGR_POWERSTATE_STANDBY_DEEP_SLEEP) &&
               (curPowerState != IARM_BUS_PWRMGR_POWERSTATE_STANDBY_DEEP_SLEEP))
            {
                std::string event_time_string = std::to_string(std::time(nullptr));
                event_time = event_time_string.c_str(); 
                NotificationHandler::getInstance()->push_device_deepsleep_notifications("device-enter-deepsleep-state", event_time);
            }
            else if((newPowerState != IARM_BUS_PWRMGR_POWERSTATE_STANDBY_DEEP_SLEEP) &&
                   (curPowerState == IARM_BUS_PWRMGR_POWERSTATE_STANDBY_DEEP_SLEEP))
            {
                std::string event_time_string = std::to_string(std::time(nullptr));
                event_time = event_time_string.c_str();
                NotificationHandler::getInstance()->push_device_deepsleep_notifications("device-exit-deepsleep-state", event_time);
            }
        }
        else 
        {
            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] RFC Parameter (%s) is disabled, so not sending DeepSleep notification. \n",
                 __FUNCTION__, X_RDK_RFC_DEEPSLEEP_ENABLE );
        }
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
}
#else
static void _hostIf_EventHandler(const PowerController_PowerState_t currentState,
    const PowerController_PowerState_t newState, void* userdata)
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    errno_t rc = -1;
    HOSTIF_MsgData_t stRfcData = {0};
    rc=strcpy_s(stRfcData.paramName,sizeof(stRfcData.paramName), X_RDK_RFC_DEEPSLEEP_ENABLE);
    if(rc!=EOK)
    {
        ERR_CHK(rc);
    }
    if((hostIf_DeviceInfo::getInstance(0)->get_xRDKCentralComRFC(&stRfcData) == OK) && (strncmp(stRfcData.paramValue, "true", sizeof("true")) == 0))
    {
            const char *event_time = NULL;

            if((newState == POWER_STATE_STANDBY_DEEP_SLEEP) &&
               (currentState != POWER_STATE_STANDBY_DEEP_SLEEP))
            {
                std::string event_time_string = std::to_string(std::time(nullptr));
                event_time = event_time_string.c_str(); 
                NotificationHandler::getInstance()->push_device_deepsleep_notifications("device-enter-deepsleep-state", event_time);
            }
            else if((newState != POWER_STATE_STANDBY_DEEP_SLEEP) &&
                   (currentState == POWER_STATE_STANDBY_DEEP_SLEEP))
            {
                std::string event_time_string = std::to_string(std::time(nullptr));
                event_time = event_time_string.c_str();
                NotificationHandler::getInstance()->push_device_deepsleep_notifications("device-exit-deepsleep-state", event_time);
            }
    }
    else 
    {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] RFC Parameter (%s) is disabled, so not sending DeepSleep notification. \n",
                 __FUNCTION__, X_RDK_RFC_DEEPSLEEP_ENABLE );
    }

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
}
#endif

/** @} */
/** @} */
