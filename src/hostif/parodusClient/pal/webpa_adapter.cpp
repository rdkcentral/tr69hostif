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
 * @file webpa_adapter.cpp
 *
 * @description This file describes the Webpa Adapter for Parodus
 *
 */
#include <stdlib.h>
#include <string.h>
#include "webpa_adapter.h"
#include "webpa_notification.h"
#include "webpa_attribute.h"
#include "webpa_parameter.h"
#include "rdk_debug.h"
#include "hostIf_NotificationHandler.h"
#include "libpd.h"
#include "safec_lib.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include <wdmp-c.h>
#ifdef __cplusplus
}
#endif
/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/

#define WEBPA_DEVICE_REBOOT_PARAM          "Device.X_CISCO_COM_DeviceControl.RebootDevice"
#define WEBPA_DEVICE_REBOOT_VALUE          "Device"
#define WEBPA_NOTIFICATION_SOURCE          "Unknown"
/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
char* webPANotifySource = NULL;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static WDMP_STATUS validate_parameter(param_t *param, int paramCount);
static void setRebootReason(param_t param, WEBPA_SET_TYPE setType);
void notificationCallBack();

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/* @brief Set The Notification configuration file
 *
 * @return Status
 */
void setInitialNotifyConfigFile(const char* nofityConfigFile)
{
    setNotifyConfigurationFile(nofityConfigFile);
}

/* @brief Set The Notification configuration file
 *
 * @return Status
 */
void registerNotifyCallback()
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"***************Inside registerNotifyCallback*****************\n");

    // Get Notification handler instance
    NotificationHandler* pIface = NULL;
    pIface = NotificationHandler::getInstance();
    if(pIface)
    {
        pIface->registerUpdateCallback(notificationCallBack);
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Unable to get Notification Handler, Error in Callback Registration\n");
    }

}
/**
 * @brief Initializes WebPA configuration file
 *
 * @return void.
 */
/**
 * @brief To turn on notification for the parameters extracted from the notifyList of the config file.
 */
void setInitialNotify()
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"***************Inside setInitialNotify*****************\n");
    int i=0;
    char notif[20] = "";
    char **notifyparameters=NULL;
    int notifyListSize =0;

    // Read Notify configuration file
    if(notifyparameters != NULL)
    {
        WDMP_STATUS *ret = NULL;
        AttrVal **attArr = NULL;

        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"notify List Size: %d\n",notifyListSize);
        ret = (WDMP_STATUS *) malloc(sizeof(WDMP_STATUS) * 1);
        attArr = (AttrVal **) malloc(sizeof(AttrVal *) * 1);
        for(i=0; i < notifyListSize; i++)
        {
            attArr[0] = (AttrVal *) malloc(sizeof(AttrVal) * 1);
            snprintf(notif, sizeof(notif), "%d", 1);
            attArr[0]->value = (char *) malloc(sizeof(char) * 20);
            strncpy(attArr[0]->value, notif, 20);
            attArr[0]->name= (char *) notifyparameters[i];
            attArr[0]->type = WAL_INT;
            RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"notifyparameters[%d]: %s\n", i, notifyparameters[i]);
            setAttributes(attArr[0], 1, NULL, (const AttrVal **) attArr, &ret);
            if(ret[0] != WDMP_SUCCESS)
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Failed to turn notification ON for parameter : %s ret: %d ",notifyparameters[i],ret[0]);
            }
            else
            {
                RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Successfully set notification ON for parameter : %s ret: %d\n",notifyparameters[i], ret[0]);
            }
            if(NULL != attArr[0] && NULL != attArr[0]->value)
            {
                free(attArr[0]->value);
                free(attArr[0]);
            }
        }
        if(NULL != ret)
        {
            free(ret);
        }
        if(NULL != attArr)
        {
            free(attArr);
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Initial Notification list is empty\n");
    }
    // Update the Notify Source (ESTB MAC) During Intialization
    webPANotifySource = getNotifySource();
    
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"**********************End of setInitial Notify************************\n");
}

void processRequest(char *reqPayload,char *transactionId, char **resPayload)
{
    req_struct *reqObj = NULL;
    res_struct *resObj = NULL;
    WDMP_STATUS ret = WDMP_FAILURE;
    int paramCount = 0, i = 0, retCount=0, error = 0;
    const char *getParamList[MAX_PARAMETERNAME_LEN];
    char *param = NULL;

   RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"************** processRequest *****************\n");

    wdmp_parse_request(reqPayload,&reqObj);

    if(reqObj != NULL)
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Request:> Type : %d\n",reqObj->reqType);

        resObj = (res_struct *) malloc(sizeof(res_struct));
        memset(resObj, 0, sizeof(res_struct));

        resObj->reqType = reqObj->reqType;

        switch( reqObj->reqType )
        {

        case GET:
        {
            RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Processing GET request \n");
            resObj->paramCnt = reqObj->u.getReq->paramCnt;
            resObj->retStatus = (WDMP_STATUS *) calloc(resObj->paramCnt, sizeof(WDMP_STATUS));
            resObj->timeSpan = NULL;
            paramCount = (int)reqObj->u.getReq->paramCnt;

            for (i = 0; i < paramCount; i++)
            {
                RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Request:> paramNames[%d] = %s\n",i,reqObj->u.getReq->paramNames[i]);
                param = reqObj->u.getReq->paramNames[i];
                getParamList[i] = strdup(param); // Freed from wdmp_free_res_struct()
            }

            if(error != 1 && paramCount > 0)
            {
                resObj->u.getRes = (get_res_t *) malloc(sizeof(get_res_t));
                memset(resObj->u.getRes, 0, sizeof(get_res_t));

                // Allocate Memory based on input param count
                resObj->u.getRes->paramCnt = reqObj->u.getReq->paramCnt;
                resObj->u.getRes->paramNames = (char **) malloc(sizeof(char *) * paramCount);
                resObj->u.getRes->retParamCnt = (size_t *) calloc(paramCount, sizeof(size_t));
                resObj->u.getRes->params = (param_t **) malloc(sizeof(param_t*)*paramCount);

                // Memset to 0
                memset(resObj->u.getRes->params, 0, sizeof(param_t*)*paramCount);
                memset(resObj->u.getRes->paramNames, 0, sizeof(char *) * paramCount);
                memset(resObj->u.getRes->params, 0, sizeof(param_t*)*paramCount);
                getValues(getParamList, paramCount,&resObj->u.getRes->params,&resObj->u.getRes->retParamCnt ,&resObj->retStatus);

                for(i = 0; i < paramCount; i++)
                {
                    resObj->u.getRes->paramNames[i] = const_cast<char *>(getParamList[i]);
                    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Response:> paramNames[%d] = %s\n",i,resObj->u.getRes->paramNames[i]);
                    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Response:> retParamCnt[%d] = %zu\n",i,resObj->u.getRes->retParamCnt[i]);
                    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Response:> retStatus[%d] = %d\n",i,resObj->retStatus[i]);
                }
            }
            
        }
        break;

        case GET_ATTRIBUTES:
        {
            
            RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Processing GETATTR request \n");
            RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Request:> ParamCount = %zu\n",reqObj->u.getReq->paramCnt);
            resObj->paramCnt = reqObj->u.getReq->paramCnt;
            resObj->retStatus = (WDMP_STATUS *) malloc(sizeof(WDMP_STATUS)*resObj->paramCnt);
            resObj->timeSpan = NULL;
            paramCount = (int)reqObj->u.getReq->paramCnt;

            for (i = 0; i < paramCount; i++)
            {
                RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Request:> paramNames[%d] = %s\n",i,reqObj->u.getReq->paramNames[i]);
                if(reqObj->u.getReq->paramNames[i][(strlen(reqObj->u.getReq->paramNames[i])-1)] == '.')
                {
                    *resObj->retStatus = WDMP_ERR_WILDCARD_NOT_SUPPORTED;
                    error = 1;
                    break;
                }
            }

            if(error != 1)
            {
                resObj->u.paramRes = (param_res_t *) malloc(sizeof(param_res_t));
                memset(resObj->u.paramRes, 0, sizeof(param_res_t));

                resObj->u.paramRes->params = (param_t *) malloc(sizeof(param_t)*paramCount);
                memset(resObj->u.paramRes->params, 0, sizeof(param_t)*paramCount);

                getAttributes(const_cast<const char**>(reqObj->u.getReq->paramNames), paramCount, resObj->timeSpan, (ParamVal ***)&resObj->u.paramRes->params, &retCount, (WAL_STATUS *)&ret);
                RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"retCount : %d ret : %d\n",retCount, ret);

                for (i = 0; i < paramCount; i++)
                {
                    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Response:> params[%d].name = %s\n",i,resObj->u.paramRes->params[i].name);
                    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Response:> params[%d].value = %s\n",i,resObj->u.paramRes->params[i].value);
                    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Response:> params[%d].type = %d\n",i,resObj->u.paramRes->params[i].type);

                    resObj->retStatus[i] = ret;
                    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Response:> retStatus[%d] = %d\n",i,resObj->retStatus[i]);
                }
            }
        }
        break;
        case SET:
        case SET_ATTRIBUTES:
        {    
            RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Processing SET/SETATTR request \n");
            WDMP_STATUS *retList = (WDMP_STATUS *) calloc(resObj->paramCnt, sizeof(WDMP_STATUS));
            RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Request:> ParamCount = %zu\n",reqObj->u.setReq->paramCnt);
            resObj->paramCnt = reqObj->u.setReq->paramCnt;
            resObj->timeSpan = NULL;
            paramCount = (int)reqObj->u.setReq->paramCnt;
            resObj->retStatus = (WDMP_STATUS *) calloc(resObj->paramCnt, sizeof(WDMP_STATUS));
            resObj->u.paramRes = (param_res_t *) malloc(sizeof(param_res_t));
            memset(resObj->u.paramRes, 0, sizeof(param_res_t));

            for (i = 0; i < paramCount; i++)
            {
                RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Request:> param[%d].name = %s\n",i,reqObj->u.setReq->param[i].name);
                RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Request:> param[%d].value = %s\n",i,reqObj->u.setReq->param[i].value);
                setRebootReason(reqObj->u.setReq->param[i], WEBPA_SET);
            }

            ret = validate_parameter(reqObj->u.setReq->param, paramCount);
            if(ret == WDMP_SUCCESS)
            {
                if(reqObj->reqType == SET)
                {
                    setValues((ParamVal*)reqObj->u.setReq->param, paramCount, WEBPA_SET, resObj->timeSpan, &resObj->retStatus, transactionId);
                }
                else
                {
                    setAttributes((ParamVal*)reqObj->u.setReq->param, paramCount, resObj->timeSpan, (const AttrVal **) reqObj->u.setReq->param, &resObj->retStatus);
                }
                resObj->u.paramRes->params = (param_t *) malloc(sizeof(param_t)*paramCount);
                memset(resObj->u.paramRes->params, 0, sizeof(param_t)*paramCount);
		errno_t rc = -1;
                for (i = 0; i < paramCount; i++)
                {
                    resObj->u.paramRes->params[i].name = (char *) malloc(sizeof(char) * MAX_PARAMETERNAME_LEN);
                    if(resObj->u.paramRes->params[i].name!=NULL){
		    rc=strcpy_s(resObj->u.paramRes->params[i].name, MAX_PARAMETERNAME_LEN, reqObj->u.setReq->param[i].name);
                    if(rc!=EOK)
		    {
			ERR_CHK(rc);
		    }}
		    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Response:> params[%d].name = %s\n",i,resObj->u.paramRes->params[i].name);
                    resObj->u.paramRes->params[i].value = NULL;
                    resObj->u.paramRes->params[i].type = WDMP_STRING;
                    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Response:> retStatus[%d] = %d\n",i,resObj->retStatus[i]);
                }
            }
            else
            {
                resObj->retStatus[0] = ret;
                RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Response:> resObj->retStatus[0] = %d\n",resObj->retStatus[0]);
            }

            if(retList)
            {
                free(retList);  //18678 - Resource leak
            }
        }
        break;
        case TEST_AND_SET:
	break;
        case REPLACE_ROWS:
	break;
	case ADD_ROWS:
	break;
	case DELETE_ROW:
	break;
        }
    }
    wdmp_form_response(resObj,resPayload);

    if(resPayload) 
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Response:> Payload = %s\n", *resPayload);
    }
    if(NULL != reqObj)
    {
        wdmp_free_req_struct(reqObj);
    }
    if(NULL != resObj)
    {
        wdmp_free_res_struct(resObj);
    }
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"************** processRequest *****************\n");
}

void notificationCallBack()
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"************** notificationCallBack *****************\n");

    GAsyncQueue* notificationQueue;
    NotifyData* notifyMsg;

    ParamNotify* param = NULL;

    bool sentNotification = false;

    if (NULL == (notificationQueue = NotificationHandler::getInstance()->GetNotificationQueue()))
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Notification Queue is Not Initialized\n");
        return;
    }
    else if (NULL == (notifyMsg = (NotifyData*) g_async_queue_timeout_pop (notificationQueue, 1000)))
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Notification Queue is Empty\n");
        return;
    }
    else if (NULL == notifyMsg->data)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Notify data is NULL!!\n");
        goto freeNotifyMsg;
    }
    else if (NULL == (param = notifyMsg->data->notify))
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "ParamNotify is NULL.. !!\n");
        goto freeNotifyMsgData;
    }
    else if (notifyMsg->type < 0 || notifyMsg->type >= NOTIFY_TYPE_MAX)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Invalid Notify type %d \n", notifyMsg->type);
    }
    else if (NULL == param->notifyDest || NULL == param->notifyPayload)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Source is NULL or Dest is NULL or Payload is NULL \n");
    }
    else
    {
        sendNotification(param->notifyPayload, getNotifySource(), param->notifyDest);
        sentNotification = true;
    }
    if (sentNotification == false)
        WAL_FREE(param->notifyPayload); // sendNotification frees notifyPayload. if sendNotification was not called, free notifyPayload

    WAL_FREE(param->notifyDest);
    WAL_FREE(param);
freeNotifyMsgData:
    WAL_FREE(notifyMsg->data);
freeNotifyMsg:
    WAL_FREE(notifyMsg);
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/**
 * @brief validate_parameter validates parameter values
 *
 * @param[in] param arry if parameters
 * @param[in] paramCount input cid
 */
static WDMP_STATUS validate_parameter(param_t *param, int paramCount)
{
    int i = 0;
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"------------ validate_parameter ----------\n");
    for (i = 0; i < paramCount; i++)
    {
        // If input parameter is wildcard ending with "." then send error as wildcard is not supported for TEST_AND_SET
        if(param[i].name[(strlen(param[i].name)-1)] == '.')
        {
            RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Wildcard SET/SET-ATTRIBUTES is not supported \n");
            return WDMP_ERR_WILDCARD_NOT_SUPPORTED;
        }
        // Prevent SET of CMC/CID through WebPA
        if(strcmp(param[i].name, PARAM_CID) == 0 || strcmp(param[i].name, PARAM_CMC) == 0)
        {
            RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Invalid Input parameter - CID/CMC value cannot be set \n");
            return WDMP_ERR_SET_OF_CMC_OR_CID_NOT_SUPPORTED;
        }

        if(param[i].name == NULL || param[i].value == NULL)
        {
            RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Parameter value is null\n");
            return WDMP_ERR_VALUE_IS_NULL;
        }
    }
    return WDMP_SUCCESS;
}
/**
 * @brief Set Last Reboot Reason
 *
 * @param[in] param arry if parameters
 * @param[in] WebPa Set Type
 */
static void setRebootReason(param_t param, WEBPA_SET_TYPE setType)
{
    WDMP_STATUS *retReason = NULL;
    param_t *rebootParam = NULL;
    // Detect device reboot through WEBPA and log message for device reboot through webpa
    if(strcmp(param.name, WEBPA_DEVICE_REBOOT_PARAM) == 0 && strcmp(param.value, WEBPA_DEVICE_REBOOT_VALUE) == 0)
    {
        rebootParam = (param_t *) malloc(sizeof(param_t));
        retReason = (WDMP_STATUS *) malloc(sizeof(WDMP_STATUS));  //CID:18217 - sizeof_mismatch

        RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"RDKB_REBOOT : Reboot triggered through WEBPA\n");
        rebootParam[0].name = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_LastRebootReason");
        rebootParam[0].value = strdup("webpa-reboot");
        rebootParam[0].type = WDMP_STRING;

        setValues((ParamVal*)rebootParam, 1, setType, NULL, &retReason,NULL);
        if(retReason[0] != WDMP_SUCCESS)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Failed to set Reason with status %d\n",retReason[0]);
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Successfully set Reason with status %d\n",retReason[0]);
        }
        WAL_FREE(retReason);
        WAL_FREE(rebootParam);
    }

}
/**
 * @brief Get Current time
 *
 * @param[in] Time spec timer
 */
void getCurrentTime(struct timespec *timer)
{
    clock_gettime(CLOCK_REALTIME, timer);
}
