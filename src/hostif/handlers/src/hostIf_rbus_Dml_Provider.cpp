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


/** @} */
/** @} */

#include <stdlib.h>
#ifdef __cplusplus
extern "C"
{
#include "rbus.h"
#endif
#ifdef __cplusplus
}
#endif

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_msgHandler.h"
#include "waldb.h"
//#include "rbus.h"
#include "exception.hpp"
#include "illegalArgumentException.hpp"


#define MAX_NUM_PARAMETERS 2048
#define MAX_PARAMETER_LENGTH 512

static const char *EXCLUDE_PARAMLIST[] = { "Device.X_RDKCENTRAL-COM_T2.ReportProfiles", \
                                           "Device.X_RDKCENTRAL-COM_T2.ReportProfilesMsgPack"
                                         };

#define EXCLUDE_LIST_SIZE (sizeof((EXCLUDE_PARAMLIST))/sizeof((EXCLUDE_PARAMLIST)[0]))

rbusError_t TR_Dml_GetHandler(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t* opts);
rbusError_t TR_Dml_EventSubHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char* eventName, rbusFilter_t filter, int32_t interval, bool* autoPublish);
rbusHandle_t rbusHandle = NULL;

static bool isInExclusionList(const char* param) {
    unsigned int i = 0 ;
    bool isMatch = false ;
    for (i = 0; i < EXCLUDE_LIST_SIZE ; i++) {
        if (strncmp(EXCLUDE_PARAMLIST[i], param, MAX_PARAMETER_LENGTH) == 0) {
            isMatch = true ;
            break ;
        }
    }
    return isMatch ;
}

static void convert2hostIfDataType(char *ParamDataType, HostIf_ParamType_t* pParamType)
{
    if(!strcmp(ParamDataType,"string"))
        *pParamType = hostIf_StringType;
    else if(!strcmp(ParamDataType,"unsignedInt"))
        *pParamType = hostIf_UnsignedIntType;
    else if(!strcmp(ParamDataType,"int"))
        *pParamType = hostIf_IntegerType;
    else if(!strcmp(ParamDataType,"unsignedLong"))
        *pParamType = hostIf_UnsignedLongType;
    else if(!strcmp(ParamDataType,"boolean"))
        *pParamType = hostIf_BooleanType;
    else if(!strcmp(ParamDataType,"hexBinary"))
        *pParamType = hostIf_StringType;
    else
        *pParamType = hostIf_StringType;
}

static int convertRbus2hostIfDataType (rbusValueType_t type, HostIf_ParamType_t* pParamType)
{
    int error = 0;
    switch(type)
    {
        case RBUS_BOOLEAN:
            *pParamType = hostIf_BooleanType;
            break;
        case RBUS_INT8:
        case RBUS_INT16:
        case RBUS_INT32:
            *pParamType = hostIf_IntegerType;
            break;
        case RBUS_UINT8:
        case RBUS_UINT16:
        case RBUS_UINT32:
            *pParamType = hostIf_UnsignedIntType;
            break;
        case RBUS_INT64:
        case RBUS_UINT64:
            *pParamType = hostIf_UnsignedLongType;
            break;
        case RBUS_STRING:
            *pParamType = hostIf_StringType;
            break;
        case RBUS_DATETIME:
            *pParamType = hostIf_DateTimeType;
            break;
        default:
            error = -1;
            break;
    }
    return error;
}

rbusError_t TR_Dml_EventSubHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char* eventName, rbusFilter_t filter, int32_t interval, bool* autoPublish)
{
    LOG_ENTRY_EXIT;
    (void)handle;
    (void)filter;
    (void)autoPublish;
    (void)interval;
    rbusError_t rc = RBUS_ERROR_SUCCESS;

    *autoPublish = true;

    if(!strcmp("Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.DownloadStatus", eventName))
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s][rbusdml] Disable Autopublish for action=%s eventName=%s", action == RBUS_EVENT_ACTION_SUBSCRIBE ? "subscribe" : "unsubscribe", __FUNCTION__, eventName);
        *autoPublish = false;
    }
    if(!strcmp("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType", eventName))
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s][rbusdml] Disable Autopublish for action=%s eventName=%s", action == RBUS_EVENT_ACTION_SUBSCRIBE ? "subscribe" : "unsubscribe", __FUNCTION__, eventName);
        *autoPublish = false;
    }
    else if(!strcmp("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.WebCfgData", eventName))
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s][rbusdml] Disable Autopublish for action=%s eventName=%s", action == RBUS_EVENT_ACTION_SUBSCRIBE ? "subscribe" : "unsubscribe", __FUNCTION__, eventName);
        *autoPublish = false;
    }
    else
    {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s][rbusdml]: Autopublish enabled by default for all DM!\n", __FUNCTION__);
    }

    return rc;
}

rbusError_t TR_Dml_GetHandler(rbusHandle_t handle, rbusProperty_t inProperty, rbusGetHandlerOptions_t* opts)
{
    LOG_ENTRY_EXIT;

    rbusError_t rc = RBUS_ERROR_SUCCESS;

    const char *name = NULL;
    rbusValue_t value;

    int status = NOK;
    DataModelParam dmParam = {0};
    void *dataBaseHandle = NULL;

    rbusValue_Init(&value);

    name = rbusProperty_GetName(inProperty);

    if(!name)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] The parameter requested through rbus interface is invalid..!!! \n ", __FUNCTION__);
        rc = RBUS_ERROR_INVALID_INPUT;
    }
    else
    {
        dataBaseHandle = getDataModelHandle();

        if (!dataBaseHandle)
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] Failed to initialized data base handle..!!! \n ", __FUNCTION__);
            rc = RBUS_ERROR_DESTINATION_RESPONSE_FAILURE;
        }
        else
        {
            if (getParamInfoFromDataModel(dataBaseHandle, name, &dmParam) == 0 || dmParam.dataType == NULL )
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] Invalid Parameter! \n ", __FUNCTION__);
                rc = RBUS_ERROR_INVALID_INPUT;
            }
            else
            {
                RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s][rbusdml] Valid Parameter..! \n ", __FUNCTION__);

                HOSTIF_MsgData_t param = { 0 };
                char *defaultValue = NULL;
                param.reqType = HOSTIF_GET;
                strncpy (param.paramName, name, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
                convert2hostIfDataType(dmParam.dataType, &(param.paramtype));
                param.bsUpdate = getBSUpdateEnum(dmParam.bsUpdate);
                param.requestor = HOSTIF_SRC_WEBPA;
                if(dmParam.defaultValue)
                {
                    defaultValue = strdup(dmParam.defaultValue);
                }

                freeDataModelParam(dmParam);

                status = hostIf_GetMsgHandler(&param);


                if(status == OK && param.faultCode == fcNoFault)
                {
                    switch (param.paramtype)
                    {
                    case hostIf_IntegerType:
                        rbusValue_SetInt32(value, *((int *)param.paramValue));
                        break;
                    case hostIf_BooleanType:
                        rbusValue_SetBoolean(value, *((int *)param.paramValue));
                        break;
                    case hostIf_UnsignedIntType:
                        rbusValue_SetUInt32(value, *((unsigned int *)param.paramValue));
                        break;
                    case hostIf_UnsignedLongType:
                        rbusValue_SetUInt64(value, *((unsigned long long *)param.paramValue));
                        break;
                    case hostIf_StringType:
                        rbusValue_SetString(value, param.paramValue);
                        break;
                    default: // handle as string
                        rbusValue_SetString(value, param.paramValue);
                        break;
                    }
                    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s][rbusdml] Successfully return the get value for the Parameter [%s].\n", __FUNCTION__, name);
                    rbusProperty_SetValue(inProperty, value);
                    rc = RBUS_ERROR_SUCCESS;
                }
                else
                {
                    if(defaultValue)
                    {
		        int ival=0;
			unsigned int uval=0;
			unsigned long i64=0;
			bool bval = false;
                        switch (param.paramtype)
                        {
                        case hostIf_IntegerType:
			    ival = string_to_int(defaultValue);
                            rbusValue_SetInt32(value, ival);
                            break;
                        case hostIf_BooleanType:
			    bval = string_to_bool(defaultValue);
                            rbusValue_SetBoolean(value, bval);
                            break;
                        case hostIf_UnsignedIntType:
			    uval = string_to_uint(defaultValue);
                            rbusValue_SetUInt32(value, uval);
                            break;
                        case hostIf_UnsignedLongType:
			    i64 = string_to_ulong(defaultValue);
                            rbusValue_SetUInt64(value, i64);
                            break;
                        case hostIf_StringType:
                            rbusValue_SetString(value, defaultValue);
                            break;
                        default: // handle as string
                            rbusValue_SetString(value, defaultValue);
                            break;
                        }
                        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s][rbusdml] Successfully return the get the default value for the Parameter [%s].\n", __FUNCTION__, name);
                        rbusProperty_SetValue(inProperty, value);
                        rc = RBUS_ERROR_SUCCESS;
                    }
                    else {
                        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] Get Parameter [%s] Invalid format to send across.\n", __FUNCTION__, name);
                        rc = RBUS_ERROR_BUS_ERROR;
                    }
                }

                if(defaultValue)
                {
                    free(defaultValue);
                }
            }
        }
    }

    rbusValue_Release(value);
    return rc;
}

rbusError_t TR_Dml_SetHandler(rbusHandle_t handle, rbusProperty_t inProperty, rbusSetHandlerOptions_t* opts)
{
    LOG_ENTRY_EXIT;

    rbusError_t rc = RBUS_ERROR_SUCCESS;
    DataModelParam dmParam = {0};
    void *dataBaseHandle = NULL;

    const char *rbusPropName = rbusProperty_GetName(inProperty);
    rbusValue_t rbusPropValue = rbusProperty_GetValue(inProperty);

    if(!rbusPropName || !rbusPropValue)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] Invalid input parameters..!!! \n ", __FUNCTION__);
        rc = RBUS_ERROR_INVALID_INPUT;
    }
    else
    {
        dataBaseHandle = getDataModelHandle();

        if (!dataBaseHandle)
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] Failed to initialized data base handle..!!! \n ", __FUNCTION__);
            rc = RBUS_ERROR_DESTINATION_RESPONSE_FAILURE;
        }
        else
        {
            if (getParamInfoFromDataModel(dataBaseHandle, rbusPropName, &dmParam) == 0)
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] Invalid Parameter! \n ", __FUNCTION__);
                rc = RBUS_ERROR_INVALID_INPUT;
            }
            else
            {
                if(dmParam.access != NULL && !strcasecmp(dmParam.access, "readOnly"))
                {
                    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s][rbusdml] Parameter is readOnly\n", __FUNCTION__);
                    rc = RBUS_ERROR_INVALID_OPERATION;
                }
                else
                {
                    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s][rbusdml] Valid Parameter..! \n ", __FUNCTION__);

                    rbusValueType_t rbusPropType = rbusValue_GetType(rbusPropValue);
                    HostIf_ParamType_t inputParamType = hostIf_StringType;

                    HOSTIF_MsgData_t param = { 0 };
                    memset(&param,0,sizeof(HOSTIF_MsgData_t));
                    param.reqType = HOSTIF_SET;
                    strncpy (param.paramName, rbusPropName, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
                    if(dmParam.dataType != NULL){
                    	convert2hostIfDataType(dmParam.dataType, &(param.paramtype));
                    }
                    param.bsUpdate = getBSUpdateEnum(dmParam.bsUpdate);
                    param.requestor = HOSTIF_SRC_WEBPA;

                    if (convertRbus2hostIfDataType(rbusPropType, &inputParamType))
                    {
                        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s][rbusdml] Unsupported rbus property type: %d\n",__FUNCTION__, rbusPropType);
                        rc = RBUS_ERROR_INVALID_INPUT;
                    }
                    else if (inputParamType != param.paramtype)
                    {
                        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] Parameter type mismatch! Given = %d vs DataModel = %d \n ", __FUNCTION__, inputParamType, param.paramtype);
                        rc = RBUS_ERROR_INVALID_INPUT;
                    }
                    else
                    {
                        char *strValue = rbusValue_ToString(rbusPropValue, 0, 0);
                        switch (param.paramtype)
                        {
                            case hostIf_StringType:
                            case hostIf_DateTimeType:
                                snprintf(param.paramValue,TR69HOSTIFMGR_MAX_PARAM_LEN, "%s", strValue);
                                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> Type: String/Date, Value = %s\n", param.paramValue);
                                break;
                            case hostIf_IntegerType:
                            case hostIf_UnsignedIntType:
                            {
                                int ivalue = atoi(strValue);
                                int *iptr = (int *)param.paramValue;
                                *iptr = ivalue;
                                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> Type: Integer/UnsignedInt, Value = %s\n", param.paramValue);
                            }
                            break;
                            case hostIf_UnsignedLongType:
                            {
                                long lvalue = atol(strValue);
                                long *lptr = (long *)param.paramValue;
                                *lptr = lvalue;
                                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> Type: UnsignedLong, Value = %s\n", param.paramValue);
                            }
                            break;
                            case hostIf_BooleanType:
                            {
                                bool *bptr = (bool *)param.paramValue;
                                *bptr = (0 == strncasecmp(strValue, "TRUE", 4)|| (isdigit(strValue[0]) && strValue[0] != '0' ));
                                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> Type: Boolean, Value = %s\n", param.paramValue);
                            }
                            break;
                            default:
                                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> This path should never be reached. param type is %d\n", param.paramtype);
                                rc = RBUS_ERROR_INVALID_INPUT;
                        }
                        free(strValue);

                        if (RBUS_ERROR_SUCCESS == rc)
                        {
                            int status = hostIf_SetMsgHandler(&param);
                            if(status == OK)
                                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s][rbusdml] rbus set handler success..! \n ", __FUNCTION__);
                            else
                            {
                                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] rbus set handler failed with %d! \n ", __FUNCTION__, status);
                                rc = RBUS_ERROR_BUS_ERROR;
                            }
                        }
                    }
                }
                freeDataModelParam(dmParam);
            }
        }
    }
    return rc;
}

void init_rbus_dml_provider()
{

    LOG_ENTRY_EXIT;

    /* Check for rbus status */
    rbusStatus_t rs;

    int max_retry = 6;
    int retry_count = 0;

    while(true) {
        rs = rbus_checkStatus();
        if(rs == RBUS_ENABLED)
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] [rbusdml] The rbus_checkStatus() is Enabled (%d). \n", __FUNCTION__, rs);
            break;
        }
        else {
            if(retry_count >= max_retry) {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] [rbusdml] The rbus_checkStatus() is Not Enabled, exiting rbus support after max retry (%d). \n",__FUNCTION__, max_retry);
                break;
            }
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] The rbus_checkStatus() is Not Enabled, returned with status (%d), so retrying... \n", __FUNCTION__, rs);
            sleep(5);
            retry_count++;
        }
    }

    if(rs != RBUS_ENABLED) {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] Failed to initialized, rbus_checkStatus() returned with status (%d). \n", __FUNCTION__, rs);
        return;
    }

    rbusError_t rc = RBUS_ERROR_SUCCESS;
    rbusDataElement_t* dataElements = NULL;
    int i = 0;
    rc = rbus_open(&rbusHandle, "tr69hostif");
    if(rc != RBUS_ERROR_SUCCESS)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "consumer: rbus_open failed: %d\n", rc);
        return ;
    }  //CID:161936 - checked return

    char **pParamNameList = (char**) calloc (MAX_NUM_PARAMETERS, sizeof(char*));
    int num_of_params = 0;
    int num_of_rbus_params = 0;
    int rbus_param_counter = 0;

    try {

        /* Get the list of properties that are supported by Data Model Xml */
        if(DB_SUCCESS != get_complete_param_list (pParamNameList, &num_of_params))
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s][rbusdml] Failed to get complete parameter list. The number of parameter is :%d\n", __FUNCTION__, num_of_params);
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d][rbusdml]Successfully get the complete parameter list, the Parameter counts is = %d\n", __FUNCTION__, __LINE__, num_of_params);

            num_of_rbus_params = num_of_params - EXCLUDE_LIST_SIZE ;
            dataElements = (rbusDataElement_t*)calloc(num_of_rbus_params, sizeof(rbusDataElement_t));

            if(dataElements != NULL)
            {
                for(i=0; i< num_of_params; i++)
                {
                    if(pParamNameList[i] != NULL) {
                        if(isInExclusionList(pParamNameList[i])) {
                            // Temporary solution to ignore externally managed TR181 parameters from rBus registration.
                            // This is to avoid recursive call of handlers.
                            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s] [rbusdml] Parameter %s is externally rbus registered, ignoring from datamodel list.\n", __FUNCTION__,pParamNameList[i]);
                            continue;
                        } else {
                            dataElements[rbus_param_counter].name = strdup(pParamNameList[i]);

                            void *dataBaseHandle = getDataModelHandle();
                            bool setAllowed = false;
                            if (!dataBaseHandle)
                            {
                                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] Failed to initialized data base handle..!!! \n ", __FUNCTION__);
                            }
                            else
                            {
                                DataModelParam dmParam = {0};
                                if (getParamInfoFromDataModel(dataBaseHandle, dataElements[rbus_param_counter].name, &dmParam) == 0)
                                {
                                    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] Parameter not found. \n ", __FUNCTION__);
                                }
                                else
                                {
                                    if(dmParam.access != NULL && !strcasecmp(dmParam.access, "readOnly"))
                                    {
                                        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s][rbusdml] Parameter is readOnly..\n ", __FUNCTION__);
                                    }
                                    else
                                    {
                                        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s][rbusdml] Valid for Sethandler..! \n ", __FUNCTION__);
                                        setAllowed = true;
                                    }
                                }
                                freeDataModelParam(dmParam);
                            }

                            if(strstr(pParamNameList[i],".{i}.")){
                                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s] [rbusdml] Parameter [%d] : [ %s ] registered as [RBUS_ELEMENT_TYPE_TABLE]\n", __FUNCTION__, i + 1,pParamNameList[i]);
                                dataElements[rbus_param_counter].type = RBUS_ELEMENT_TYPE_TABLE;
                                dataElements[rbus_param_counter].cbTable.getHandler = TR_Dml_GetHandler;
                                if (setAllowed)
                                    dataElements[rbus_param_counter].cbTable.setHandler = TR_Dml_SetHandler;
                                else
                                    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s][rbusdml] set not allowed by DM..! \n ", __FUNCTION__);
                            }
                            else {
                                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s] [rbusdml]Parameter [%d] : [ %s ] registered as [RBUS_ELEMENT_TYPE_PROPERTY]\n", __FUNCTION__, i + 1,pParamNameList[i]);
                                dataElements[rbus_param_counter].type = RBUS_ELEMENT_TYPE_PROPERTY;
                                dataElements[rbus_param_counter].cbTable.getHandler = TR_Dml_GetHandler;
                                if (setAllowed)
                                    dataElements[rbus_param_counter].cbTable.setHandler = TR_Dml_SetHandler;
                                else
                                    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s][rbusdml] set not allowed by DM..! \n ", __FUNCTION__);
                                RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s][rbusdml] using event sub Handler for DM..! \n ", __FUNCTION__);
                                dataElements[rbus_param_counter].cbTable.eventSubHandler = TR_Dml_EventSubHandler;
                            }
                            rbus_param_counter++ ;
                        }
                    }
                }

                rc = rbus_regDataElements(rbusHandle, rbus_param_counter, dataElements);

                if(rc != RBUS_ERROR_SUCCESS)
                {
                    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF, "[%s][rbusdml] rbusPropertyProvider_Register failed: %d\n", __FUNCTION__, rc);
                    free(dataElements);
                    rbus_close(rbusHandle);
                }
                else
                {

                    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s][rbusdml] rbus_regDataElements registered successfully.\n", __FUNCTION__);
                    free(dataElements);
                }

                for (i=0; i< num_of_params; i++) {
                    if(pParamNameList[i]) free(pParamNameList[i]);
                }

            }
        }
    }
    catch (const std::exception &e)
    {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s][rbusdml] Exception: %s\r\n",__FUNCTION__, e.what());
    }
    if(pParamNameList) free(pParamNameList);  //CID:161937 - Resource leak
}


int setRbusStringParam(const char *rbusParamName, char* rbusParamValue) {

    rbusValue_t value;
    rbusSetOptions_t opts = {true, 0};
    int ret = RBUS_ERROR_SUCCESS;
    if(NULL == rbusHandle) {
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s][rbusdml] rbusHandle is NULL.\n", __FUNCTION__);
        return NOK;
    }
    rbusValue_Init(&value);
    rbusValue_SetString(value, rbusParamValue);
    //ret = rbus_set(rbusHandle, "Device.X_RDKCENTRAL-COM_T2.ReportProfiles", value, &opts);
    ret = rbus_set(rbusHandle, rbusParamName, value, &opts);
    if (ret != RBUS_ERROR_SUCCESS) {
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s][rbusdml] rbus_set Failed for [%s] with error [%d].\n", __FUNCTION__, rbusParamName, ret);
        return NOT_HANDLED;
    }
    rbusValue_Release(value);

    return OK ;

}

int getRbusStringParam(const char *rbusParamName, char **rbusParamValue) {

    rbusValue_t paramValue_t;
    int ret = RBUS_ERROR_SUCCESS ;
    char *stringValue = NULL;

    if(NULL == rbusHandle) {
        printf("rbusHandle is NULL\n");
        return NOK;
    }

    ret = rbus_get(rbusHandle, rbusParamName, &paramValue_t);
    if(ret != RBUS_ERROR_SUCCESS) {
        printf("Unable to get %s\n", rbusParamName);
        return NOK;
    }
    stringValue = rbusValue_ToString(paramValue_t, NULL, 0);
    if (stringValue != NULL)
        *rbusParamValue = strdup(stringValue);

    rbusValue_Release(paramValue_t);
    return OK ;

}
