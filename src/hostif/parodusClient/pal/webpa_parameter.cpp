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
 * @file webpa_parameter.cpp
 *
 * @description This file describes the Webpa Parameter Request Handling
 *
 */

#include <string.h>
#include <math.h>
#include <stdio.h>

#include "webpa_parameter.h"
#include "rdk_debug.h"
#include "waldb.h"
#include "hostIf_msgHandler.h"
#include "hostIf_utils.h"
#include "rbus.h"

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static WDMP_STATUS GetParamInfo (const char *pParameterName, param_t ***parametervalPtrPtr, int *paramCountPtr,int paramIndex);
static WDMP_STATUS get_ParamValues_tr69hostIf(HOSTIF_MsgData_t *ptrParam);
int isWildCardParam(const char *paramName);
static void converttohostIfType(char *ParamDataType,HostIf_ParamType_t* pParamType);
static void converttoWalType(HostIf_ParamType_t paramType,WAL_DATA_TYPE* pwalType);
static WAL_STATUS SetParamInfo(ParamVal paramVal , char * transaction_id);
static WAL_STATUS set_ParamValues_tr69hostIf (HOSTIF_MsgData_t *param);
static WAL_STATUS convertFaultCodeToWalStatus(faultCode_t faultCode);

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/



/**
 * @brief getValues interface returns the parameter values.
 *
 * getValues supports an option to pass wildward parameters. This can be achieved by passing an object name followed by '.'
 * instead of parameter name.
 *
 * @param[in] paramName List of Parameters.
 * @param[in] paramCount Number of parameters.
 * @param[out] paramValArr Two dimentional array of parameter name/value pairs.
 * @param[out] retValCount List of "number of parameters" for each input paramName. Usually retValCount will be 1 except
 * for wildcards request where it represents the number of param/value pairs retrieved for the particular wildcard parameter.
 * @param[out] retStatus List of Return status.
 */
void getValues (const char *paramName[], const unsigned int paramCount, param_t ***paramValArr,size_t **retValCount, WDMP_STATUS **retStatus)
{
    // Generic code mallocs paramValArr to hold paramCount items but only paramValArr[0] is ever accessed.
    // Generic code uses "paramValArr[0][cnt2]" (iterating over the 2nd dimension instead of the 1st). This means
    // paramValArr[0] (which is of type "ParamVal**") is expected to point to an array of "ParamVal*" objects
    unsigned int cnt = 0;
    int numParams = 0;
    for (cnt = 0; cnt < paramCount; cnt++)
    {
        // GetParamInfo is responsible for generating the output response including wild card. Allocate the memory for
        if(strlen(paramName[cnt]) < MAX_PARAMETERNAME_LEN) {
            (*retStatus)[cnt] =  GetParamInfo (paramName[cnt], paramValArr, &numParams,cnt);
            (*retValCount)[cnt] = numParams;
            RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "Parameter Name: %s return: %d\n", paramName[cnt], (*retStatus)[cnt]);
        } else {
            (*retStatus)[cnt] = WDMP_ERR_INVALID_PARAMETER_NAME;
            (*retValCount)[cnt] = 0;
        }
    }
}
/**
 * @brief setValues interface sets the parameter value.
 *
 * @param[in] paramVal List of Parameter name/value pairs.
 * @param[in] paramCount Number of parameters.
 * @param[out] retStatus List of Return status.
 */
void setValues(const ParamVal paramVal[], const unsigned int paramCount, const WEBPA_SET_TYPE setType, money_trace_spans *timeSpan, WDMP_STATUS **retStatus,char * transaction_id)
{
    unsigned int cnt=0;
    for(cnt = 0; cnt < paramCount; cnt++)
    {
        (*retStatus)[cnt] = (WDMP_STATUS) SetParamInfo(paramVal[cnt], transaction_id);
    }
}

/******************************************************************************/
/*                      RBUS  Internal  functions                             */
/******************************************************************************/
rbusHandle_t   g_busHandle = 0;

static void ensureRbusHandle()
{
    if(!g_busHandle)
    {
        rbusError_t rc;
        char compName[50] = "";
        snprintf(compName, 50, "parodus-rbus-If");
        rc = rbus_open(&g_busHandle, compName);
        if(rc != RBUS_ERROR_SUCCESS)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"rbus_open failed err: %d\n", rc);
            return;
        }
    }
    return;
}

static rbusValueType_t getRbusDataTypefromWebPA(WAL_DATA_TYPE type)
{
    if (type == WAL_STRING)
        return RBUS_STRING;

    if (type == WAL_INT)
        return RBUS_INT32;

    if (type == WAL_UINT)
        return RBUS_UINT32;

    if (type == WAL_BOOLEAN)
        return RBUS_BOOLEAN;

    if (type == WAL_DATETIME)
        return RBUS_DATETIME;

    if (type == WAL_BASE64)
        return RBUS_BYTES;

    if (type == WAL_LONG)
        return RBUS_INT64;

    if (type == WAL_ULONG)
        return RBUS_UINT64;

    if (type == WAL_FLOAT)
        return RBUS_SINGLE;

    if (type == WAL_DOUBLE)
        return RBUS_DOUBLE;

    if (type == WAL_BYTE)
        return RBUS_BYTE;

    return RBUS_STRING;
}

static DATA_TYPE mapRbusDataTypeToWebPA (rbusValueType_t type)
{
    DATA_TYPE rc = WDMP_STRING;
    switch(type)
    {
    case RBUS_BOOLEAN:
        rc = WDMP_BOOLEAN;
        break;
    case RBUS_CHAR :
    case RBUS_BYTE:
    case RBUS_INT8:
    case RBUS_UINT8:
        rc = WDMP_BYTE;
        break;
    case RBUS_INT16:
    case RBUS_INT32:
        rc = WDMP_INT;
        break;
    case RBUS_UINT16:
    case RBUS_UINT32:
        rc = WDMP_UINT;
        break;
    case RBUS_INT64:
        rc = WDMP_LONG;
        break;
    case RBUS_UINT64:
        rc = WDMP_ULONG;
        break;
    case RBUS_STRING:
        rc = WDMP_STRING;
        break;
    case RBUS_DATETIME:
        rc = WDMP_DATETIME;
        break;
    case RBUS_BYTES:
        rc = WDMP_BASE64;
        break;
    case RBUS_SINGLE:
        rc = WDMP_FLOAT;
        break;
    case RBUS_DOUBLE:
        rc = WDMP_DOUBLE;
        break;
    case RBUS_PROPERTY:
    case RBUS_OBJECT:
    case RBUS_NONE:
        rc = WDMP_NONE;
        break;
    }
    return rc;
}

static WDMP_STATUS rbusGetParamInfo (const char *pParameterName, param_t ***parametervalPtrPtr, int *paramCountPtr, int index)
{
    WDMP_STATUS ret = WDMP_FAILURE;
    rbusError_t rc;
    int numOfOutVals = 0;
    rbusProperty_t outputVals = NULL;

    ensureRbusHandle();

    *paramCountPtr = 0;
    if (isWildCardParam(pParameterName)) // It is a wildcard Param
        rc = rbus_getExt(g_busHandle, 1, &pParameterName, &numOfOutVals, &outputVals);
    else
    {
        rbusValue_t getVal;
        rc = rbus_get(g_busHandle, pParameterName, &getVal);
        if(RBUS_ERROR_SUCCESS == rc)
        {
            numOfOutVals = 1;
            rbusProperty_Init(&outputVals, pParameterName, getVal);
            rbusValue_Release(getVal);
        }
    }

    if(RBUS_ERROR_SUCCESS == rc)
    {
        rbusProperty_t next = outputVals;
        *paramCountPtr = numOfOutVals;

        (*parametervalPtrPtr)[index] = (param_t *) calloc(sizeof(param_t),*paramCountPtr);

        for (int i = 0; i < numOfOutVals; i++)
        {
            rbusValue_t val = rbusProperty_GetValue(next);
            rbusValueType_t type = rbusValue_GetType(val);

            (*parametervalPtrPtr)[index][i].name  = strdup(rbusProperty_GetName(next));
            (*parametervalPtrPtr)[index][i].value = rbusValue_ToString(val,NULL,0);
            (*parametervalPtrPtr)[index][i].type  = mapRbusDataTypeToWebPA(type);

            next = rbusProperty_GetNext(next);
        }
        /* Free the memory */
        rbusProperty_Release(outputVals);
        ret = WDMP_SUCCESS;
    }
    else
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Failed to get the data for %s with error : %d\n", pParameterName, rc);
    }

    return ret;
}

static WAL_STATUS rbusSetParamInfo(ParamVal paramVal, char * transactionID)
{
    rbusError_t rc;
    WAL_STATUS ret = WAL_SUCCESS;
    rbusValue_t setVal;

    ensureRbusHandle();

    /* Get Param Type */
    rbusValueType_t type = getRbusDataTypefromWebPA(paramVal.type);
    rbusValue_Init(&setVal);
    if (rbusValue_SetFromString(setVal, type, paramVal.value))
    {
        rbusSetOptions_t opts = {true, strtoul (transactionID, NULL, 0)};
        rc = rbus_set(g_busHandle, paramVal.name, setVal, &opts);
        if(rc != RBUS_ERROR_SUCCESS)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF," Invalid Parameter name %s\n",paramVal.name);
            ret = WAL_ERR_INVALID_PARAMETER_VALUE;
        }
        /* Free the data pointer that was allocated */
        rbusValue_Release(setVal);
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"set_ParamValues_tr69hostIf %d\n",ret);
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF," Invalid Parameter name %s\n",paramVal.name);
        ret = WAL_ERR_INVALID_PARAMETER_NAME;
    }
    return ret;
}


/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
static WDMP_STATUS GetParamInfo (const char *pParameterName, param_t ***parametervalPtrPtr, int *paramCountPtr,int index)
{
    void *dataBaseHandle = NULL;
    DB_STATUS dbRet = DB_FAILURE;
    HOSTIF_MsgData_t Param = { 0 };
    WDMP_STATUS ret = WDMP_SUCCESS;

    // Memset to 0
    memset(&Param, '\0', sizeof(HOSTIF_MsgData_t));

    // Get DB handle
    dataBaseHandle = getDataModelHandle();
    if (dataBaseHandle)
    {
        if (isWildCardParam(pParameterName)) // It is a wildcard Param
        {
            /* Translate wildcard to list of parameters */
            char **getParamList = (char**) calloc (MAX_NUM_PARAMETERS, sizeof(char*));
            if (NULL == getParamList)
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Error allocating memory\n");
                return WDMP_FAILURE;
            }
            char **ParamDataTypeList = (char**) calloc (MAX_NUM_PARAMETERS, sizeof(char*));
            if(NULL == ParamDataTypeList)
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Error allocating memory\n");
                free(getParamList);
                return WDMP_FAILURE;
            }
            *paramCountPtr = 0;
            int wc_cnt = 0;
            WDMP_STATUS getRet = WDMP_FAILURE;
            int successWcCnt = 0;

            dbRet = getChildParamNamesFromDataModel((void *) dataBaseHandle,const_cast<char*> (pParameterName), getParamList, ParamDataTypeList, paramCountPtr);
            if(*paramCountPtr != 0 && dbRet == DB_SUCCESS)
            {
                (*parametervalPtrPtr)[index] = (param_t *) calloc(sizeof(param_t),*paramCountPtr);
                for(wc_cnt = 0; wc_cnt < *paramCountPtr; wc_cnt++)
                {
                    strncpy (Param.paramName, getParamList[wc_cnt], MAX_PARAM_LENGTH - 1);
                    free(getParamList[wc_cnt]);
                    Param.paramName[MAX_PARAM_LENGTH - 1] = '\0';

                    // Convert ParamDataType to hostIf datatype
                    converttohostIfType (ParamDataTypeList[wc_cnt], &(Param.paramtype));
                    Param.instanceNum = 0;
                    free(ParamDataTypeList[wc_cnt]);

                    // Initialize Name and Value to NULL
                    (*parametervalPtrPtr)[index][wc_cnt].name = NULL;
                    (*parametervalPtrPtr)[index][wc_cnt].value = NULL;

                    // Convert Param.paramtype to ParamVal.type
                    getRet = get_ParamValues_tr69hostIf (&Param);
                    // Fill Only if we can able to get Proper value
                    if(WDMP_SUCCESS == getRet)
                    {
                        int iParamValSize = MAX_PARAM_LENGTH;
                        if (Param.isLengthyParam && (NULL != Param.paramValueLong)) {
                            iParamValSize = strlen(Param.paramValueLong)+1;
                        }

                        (*parametervalPtrPtr)[index][successWcCnt].name = (char*) calloc (MAX_PARAM_LENGTH, sizeof(char));
                        (*parametervalPtrPtr)[index][successWcCnt].value = (char*) calloc (iParamValSize, sizeof(char));
                        if ((*parametervalPtrPtr)[index][successWcCnt].name == NULL || (*parametervalPtrPtr)[index][successWcCnt].value == NULL)
                        {
                            RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Error allocating memory\n");
                            ret = WDMP_FAILURE;
                            break;
                        }
                        // Copy Param Name
                        strncat((*parametervalPtrPtr)[index][successWcCnt].name, Param.paramName, MAX_PARAM_LENGTH - 1);
                        // Copy Type
                        converttoWalType (Param.paramtype, (WAL_DATA_TYPE *) &(*parametervalPtrPtr)[index][successWcCnt].type);
                        // Copy param value
                        switch (Param.paramtype)
                        {
                        case hostIf_IntegerType:
                        case hostIf_BooleanType:
                            snprintf ((*parametervalPtrPtr)[index][successWcCnt].value, MAX_PARAM_LENGTH, "%d", *((int *) Param.paramValue));
                            break;
                        case hostIf_UnsignedIntType:
                            snprintf ((*parametervalPtrPtr)[index][successWcCnt].value, MAX_PARAM_LENGTH, "%u", *((unsigned int *) Param.paramValue));
                            break;
                        case hostIf_UnsignedLongType:
                            snprintf ((*parametervalPtrPtr)[index][successWcCnt].value, MAX_PARAM_LENGTH, "%lu", *((unsigned long *) Param.paramValue));
                            break;
                        case hostIf_StringType:
                        default: // handle as string
                            if (Param.isLengthyParam && (NULL != Param.paramValueLong)) {
                                strncat ((*parametervalPtrPtr)[index][successWcCnt].value, Param.paramValueLong, iParamValSize);
                                free (Param.paramValueLong);
                                Param.paramValueLong = NULL;
                            }
                            else {
                                strncat ((*parametervalPtrPtr)[index][successWcCnt].value, Param.paramValue, MAX_PARAM_LENGTH - 1);
                            }
                            break;
                        }
                        memset(&Param, '\0', sizeof(HOSTIF_MsgData_t));
                        successWcCnt++;
                    }
                } // End of Wild card for loop
                // Lets Free GetParameter List
                free(getParamList);
                free(ParamDataTypeList);
                *paramCountPtr = successWcCnt;
            }
            else
            {
                free(getParamList);
                free(ParamDataTypeList);
                RDK_LOG (RDK_LOG_INFO, LOG_PARODUS_IF, " %s :: Wild card Param list is empty; Not owned by tr69HostIf; Post it to RBUS\n", pParameterName);
                ret = rbusGetParamInfo(pParameterName, parametervalPtrPtr, paramCountPtr, index);
            }
        }
        else // Not a wildcard Parameter Lets fill it
        {
            RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "Get Request for a Non-WildCard Parameter \n");
            *paramCountPtr = 0;
            DataModelParam dmParam = {0};

            if (getParamInfoFromDataModel(dataBaseHandle, const_cast<char*> (pParameterName), &dmParam))
            {
                RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "Valid Parameter..! \n ");
                strncpy (Param.paramName, pParameterName, MAX_PARAM_LENGTH - 1);
                Param.paramName[MAX_PARAM_LENGTH - 1] = '\0';
                Param.requestor = HOSTIF_SRC_WEBPA;
                Param.bsUpdate = getBSUpdateEnum(dmParam.bsUpdate);

                if(dmParam.dataType != NULL)
                {
                    converttohostIfType (dmParam.dataType, &(Param.paramtype));  //CID:18170 - FORWARD NULL
                }
                freeDataModelParam(dmParam);
                Param.instanceNum = 0;
                ret = get_ParamValues_tr69hostIf (&Param);
                if (ret == WDMP_SUCCESS)
                {
                    int iParamValSize = MAX_PARAM_LENGTH;
                    if (Param.isLengthyParam && (NULL != Param.paramValueLong)) {
                        iParamValSize = strlen(Param.paramValueLong)+1;
                    }

                    // allocate parametervalPtr as a single param_t and initialize variables
                    (*parametervalPtrPtr)[index] = (param_t*) calloc (1, sizeof(param_t));
                    (*parametervalPtrPtr)[index][0].name = (char*) calloc (strlen(pParameterName)+1, sizeof(char));
                    (*parametervalPtrPtr)[index][0].value = (char*) calloc (iParamValSize, sizeof(char));
                    (*parametervalPtrPtr)[index][0].type = WDMP_STRING;

                    if (NULL == (*parametervalPtrPtr)[index] || NULL == (*parametervalPtrPtr)[index][0].name || NULL == (*parametervalPtrPtr)[index][0].value)
                    {
                        RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Error allocating memory\n");
                        //de-allocating if not NULL and not required
                        if((*parametervalPtrPtr)[index][0].value != NULL) free((*parametervalPtrPtr)[index][0].value);
                        if((*parametervalPtrPtr)[index][0].name != NULL) free((*parametervalPtrPtr)[index][0].name);
                        if((*parametervalPtrPtr)[index] != NULL) free((*parametervalPtrPtr)[index]);
                        ret = WDMP_FAILURE;
                    }
                    else {
                        RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "CMCSA:: GetParamInfo Param.paramtype is %d\n", Param.paramtype);
                        // Convert Param.paramtype to ParamVal.type
                        converttoWalType (Param.paramtype, (WAL_DATA_TYPE *) &(*parametervalPtrPtr)[index][0].type);
                        RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "CMCSA:: GetParamInfo parametervalPtr->type is %d\n",((*parametervalPtrPtr)[index][0].type));
                        RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "CMCSA:: GetParamInfo\n");

                        strncpy((*parametervalPtrPtr)[index][0].name, Param.paramName, strlen(pParameterName)+1);
                        switch (Param.paramtype)
                        {
                        case hostIf_IntegerType:
                        case hostIf_BooleanType:
                            snprintf ((*parametervalPtrPtr)[index][0].value, MAX_PARAM_LENGTH, "%d", *((int *) Param.paramValue));
                            break;
                        case hostIf_UnsignedIntType:
                            snprintf ((*parametervalPtrPtr)[index][0].value, MAX_PARAM_LENGTH, "%u", *((unsigned int *) Param.paramValue));
                            break;
                        case hostIf_UnsignedLongType:
                            snprintf ((*parametervalPtrPtr)[index][0].value, MAX_PARAM_LENGTH, "%lu", *((unsigned long *) Param.paramValue));
                            break;
                        case hostIf_StringType:
                        default: // handle as string
                            if (Param.isLengthyParam && (NULL != Param.paramValueLong)) {
                                strncat ((*parametervalPtrPtr)[index][0].value, Param.paramValueLong, iParamValSize);
                                free (Param.paramValueLong);
                                Param.paramValueLong = NULL;
                            }
                            else {
                                strncat ((*parametervalPtrPtr)[index][0].value, Param.paramValue, MAX_PARAM_LENGTH - 1);
                            }
                            break;
                        }
                        RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "CMCSA:: GetParamInfo value is %s\n", (*parametervalPtrPtr)[index][0].value);
                        *paramCountPtr = 1;
                        ret = WDMP_SUCCESS;
                    }
                }
                else
                {
                    RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Failed get_ParamValues_tr69hostIf() Param Name :-  %s \n",pParameterName);
                }
            }
            else
            {
                RDK_LOG (RDK_LOG_INFO, LOG_PARODUS_IF, "%s :: Not owned by tr69HostIf; Post it to RBUS.\n",pParameterName);
                ret = rbusGetParamInfo(pParameterName, parametervalPtrPtr, paramCountPtr, index);
            }
        }
    }
    else
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Data base Handle is not Initialized %s\n", pParameterName);
        ret = rbusGetParamInfo(pParameterName, parametervalPtrPtr, paramCountPtr, index);
    }
    return ret;
}

static WAL_STATUS SetParamInfo(ParamVal paramVal, char * transactionID)
{
    WAL_STATUS ret = WAL_SUCCESS;
    void *dataBaseHandle = NULL;
    HOSTIF_MsgData_t Param = {0};
    DataModelParam dmParam = {0};
    memset(&Param, '\0', sizeof(HOSTIF_MsgData_t));

    dataBaseHandle = getDataModelHandle();
    if(!dataBaseHandle)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Data Model Not Initialized... Unable to process the SET operation\n");
        ret = rbusSetParamInfo(paramVal, transactionID);
    }
    else if(getParamInfoFromDataModel((void *)dataBaseHandle,paramVal.name,&dmParam))
    {
        WAL_DATA_TYPE walType;
        if(dmParam.dataType != NULL)
        {
            converttohostIfType(dmParam.dataType,&(Param.paramtype));   //CID:18545 - FORWARD NULL
        }
        Param.bsUpdate = getBSUpdateEnum(dmParam.bsUpdate);
        Param.requestor = HOSTIF_SRC_WEBPA;
        freeDataModelParam(dmParam);
        // Convert Param.paramtype to ParamVal.type
        converttoWalType(Param.paramtype,&walType);

        if(walType != paramVal.type)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF," Invalid Parameter type for %s\n",paramVal.name);
            return WAL_ERR_INVALID_PARAMETER_TYPE;
        }

        strncpy(Param.paramName, paramVal.name,MAX_PARAM_LENGTH-1);
        Param.paramName[MAX_PARAM_LENGTH-1]='\0';

        Param.isLengthyParam = false;
        Param.paramValueLong = NULL;
        if (strcasecmp(Param.paramName, "Device.X_RDKCENTRAL-COM_T2.ReportProfiles") == 0) {

            RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"[%s:%s:%d] Device.X_RDKCENTRAL-COM_T2.ReportProfiles allocating long buff \n", __FILE__, __FUNCTION__, __LINE__);
            Param.isLengthyParam = true;
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"[%s:%s:%d] Not Device.X_RDKCENTRAL-COM_T2.ReportProfiles. Param.paramName: %s \n", __FILE__, __FUNCTION__, __LINE__, Param.paramName);
        }

        if (Param.paramtype == hostIf_BooleanType)
        {
            bool* boolPtr = (bool*) Param.paramValue;
            if (strcmp (paramVal.value, "1") == 0 || strcasecmp (paramVal.value, "true") == 0)
            {
                *boolPtr = 1;
            }
            else if (strcmp (paramVal.value, "0") == 0 || strcasecmp (paramVal.value, "false") == 0)
            {
                *boolPtr = 0;
            }
            else
            {
                return WAL_ERR_INVALID_PARAMETER_VALUE;
            }
        }
        else if (Param.paramtype == hostIf_IntegerType)
        {
            char *tailPtr;
            long int value = (int) strtol (paramVal.value, &tailPtr, 10);
            if (strlen (tailPtr)) // "whole" string cannot be interpreted as integer
                return WAL_ERR_INVALID_PARAMETER_VALUE;
            *((int*) Param.paramValue) = (int) value;
        }
        else if (Param.paramtype == hostIf_UnsignedIntType)
        {
            char *tailPtr;
            long int value = (int) strtol (paramVal.value, &tailPtr, 10);
            if (strlen (tailPtr) || value < 0) // "whole" string cannot be interpreted as unsigned integer
                return WAL_ERR_INVALID_PARAMETER_VALUE;
            *((int*) Param.paramValue) = (int) value;
        }
        else
        {

            if (Param.isLengthyParam) {
                int iParamLen = strlen(paramVal.value);
                if (TR69HOSTIFMGR_MAX_LONG_PARAM_LEN < iParamLen) {
                    RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"[%s:%s:%d] Longer than expected parameter. Max len:%d iParamLen: %d\n", 
                       __FILE__, __FUNCTION__, __LINE__, TR69HOSTIFMGR_MAX_LONG_PARAM_LEN, iParamLen);
                    ret = WAL_ERR_INVALID_PARAMETER_VALUE;
                    return ret;
                }
                Param.paramValueLong = (char*) malloc (iParamLen+1);
                if (NULL == Param.paramValueLong) {
                    RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF," Memory allocation failed.\n");
                    ret = WAL_STATUS_RESOURCES;                   
                    return ret; 
                }
                else {
                    memset (Param.paramValueLong, '\0', iParamLen+1);
                    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"[%s:%s:%d] Long iParamLen: %d\n", __FILE__, __FUNCTION__, __LINE__, iParamLen);
                    strncpy(Param.paramValueLong, paramVal.value, iParamLen);
                }
            }
            else {
                strncpy(Param.paramValue, paramVal.value,MAX_PARAM_LENGTH-1);
                Param.paramValue[MAX_PARAM_LENGTH-1]='\0';
            }
        }
#if 0
        /* transactionID */
        if ((NULL != strstr (Param.paramName, "Device.X_RDK_WebConfig.")) && (transactionID))
        {
            strncpy(Param.transactionID, transactionID, _BUF_LEN_256-1);
            Param.transactionID[_BUF_LEN_256-1]='\0';
        }
#endif
        ret = set_ParamValues_tr69hostIf(&Param);
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"set_ParamValues_tr69hostIf %d\n",ret);
    }
    else
    {
        /* Its not owned by tr69HostIf. lets post it thro the rbus. */
        RDK_LOG(RDK_LOG_WARN, LOG_PARODUS_IF, " %s :: Not owned by TR69HostIF.. Forward to RBUS\n",paramVal.name);
        ret = rbusSetParamInfo(paramVal, transactionID);
    }

    return ret;
}

/**
 * generic Api for get HostIf parameters
 **/
static WDMP_STATUS get_ParamValues_tr69hostIf(HOSTIF_MsgData_t *ptrParam)
{
    int status = -1;
    ptrParam->reqType = HOSTIF_GET;
    WDMP_STATUS retStatus = WDMP_FAILURE;
    status = hostIf_GetMsgHandler(ptrParam);

    if(status != 0) {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"[%s:%s:%d] Error in Get Message Handler : %d\n", __FILE__, __FUNCTION__, __LINE__, status);
        retStatus =(WDMP_STATUS) convertFaultCodeToWalStatus(ptrParam->faultCode); // returning appropriate fault code for get
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"[%s:%d] return status of fault code: %d\n", __FUNCTION__, __LINE__, retStatus);
    }
    else
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"[%s:%s:%d] The value for param: %s is %s paramLen : %d\n", __FILE__, __FUNCTION__, __LINE__, ptrParam->paramName,ptrParam->paramValue, ptrParam->paramLen);
        retStatus = WDMP_SUCCESS;
    }
    return retStatus;
}

/**
 * generic Api for get HostIf parameters through IARM_TR69Bus
 **/
static WAL_STATUS set_ParamValues_tr69hostIf (HOSTIF_MsgData_t *ptrParam)
{
    int status = -1;
    WAL_STATUS retStatus = WAL_FAILURE;

    ptrParam->reqType = HOSTIF_SET;
    status = hostIf_SetMsgHandler(ptrParam);
    if(status != 0) {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"[%s:%s:%d] Error in Set Message Handler Status : %d\n", __FILE__, __FUNCTION__, __LINE__, status);
        retStatus = convertFaultCodeToWalStatus(ptrParam->faultCode);
    }
    else
    {
        retStatus = WAL_SUCCESS;
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"[%s:%s:%d] The value for param: %s is %s paramLen : %d\n", __FILE__, __FUNCTION__, __LINE__, ptrParam->paramName,ptrParam->paramValue, ptrParam->paramLen);
    }
    return retStatus;
}

static WAL_STATUS convertFaultCodeToWalStatus(faultCode_t faultCode)
{
    WAL_STATUS retWalStatus = WAL_FAILURE;
    switch(faultCode)
    {
    case fcNoFault:
    case fcRequestDenied:
        retWalStatus = WAL_FAILURE;
        break;
    case fcAttemptToSetaNonWritableParameter:
        retWalStatus = WAL_ERR_NOT_WRITABLE;
        break;
    case fcInvalidParameterName:
        retWalStatus = WAL_ERR_INVALID_PARAMETER_NAME;
        break;
    case fcInvalidParameterType:
        retWalStatus = WAL_ERR_INVALID_PARAMETER_TYPE;
        break;
    case fcInvalidParameterValue:
        retWalStatus = WAL_ERR_INVALID_PARAMETER_VALUE;
        break;
    default:
        retWalStatus = WAL_FAILURE;
        break;
    }
    return retWalStatus;
}

/**
 * @brief Check if Parameter Name ends with . If yes it is a wild card param
 *
 * @param[in] paramName Name of the Parameter.
 * @param[out] retValue 0 if present and 1 if not
 */
int isWildCardParam(const char *paramName)
{
    int isWildCard = 0;
    if(NULL != paramName)
    {
        if(!strcmp(paramName+strlen(paramName)-1,"."))
        {
            isWildCard = 1;
        }
    }
    return isWildCard;
}

static void converttohostIfType(char *ParamDataType,HostIf_ParamType_t* pParamType)
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

static void converttoWalType(HostIf_ParamType_t paramType,WAL_DATA_TYPE* pwalType)
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Inside converttoWalType \n");
    switch(paramType)
    {
    case hostIf_StringType:
        *pwalType = WAL_STRING;
        break;
    case hostIf_UnsignedIntType:
        *pwalType = WAL_UINT;
        break;
    case hostIf_IntegerType:
        *pwalType = WAL_INT;
        break;
    case hostIf_BooleanType:
        *pwalType = WAL_BOOLEAN;
        break;
    case hostIf_UnsignedLongType:
        *pwalType = WAL_ULONG;
        break;
    case hostIf_DateTimeType:
        *pwalType = WAL_DATETIME;
        break;
    default:
        *pwalType = WAL_STRING;
        break;
    }
}

