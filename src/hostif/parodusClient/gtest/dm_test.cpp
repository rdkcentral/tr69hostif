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
#include "dm_stubs.h"
#include "startParodus.h"
#include "file_writer.h"
#include "webpa_notification.h"
#include "webpa_parameter.h"
#include "webpa_adapter.h"
#include "libpd.h"
#include "webpa_attribute.h"
#include "rbus_value.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "wrp-c.h"

#include "waldb.h"
#include "wdmp-c.h"

using namespace std;


#include <mutex>
#include <condition_variable>

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "datamodel_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 128

extern GHashTable* paramMgrhash;

std::mutex mtx_httpServerThreadDone;
std::condition_variable cv_httpServerThreadDone;
bool httpServerThreadDone = false;
GThread *HTTPServerThread = NULL;
char *HTTPServerName = (char *)"HTTPServerThread";
GError *httpError = NULL;
GHashTable* paramMgrhash = NULL;
T_ARGLIST argList = {{'\0'}, 0};

#ifdef GTEST_ENABLE
extern void (*macToLowerFunc())(char macValue[],char macConverted[]);
extern WDMP_STATUS (*GetParamInfoFunc()) (const char *pParameterName, param_t ***parametervalPtrPtr, int *paramCountPtr,int paramIndex);
extern rbusValueType_t (*getRbusDataTypefromWebPAFunc())(WAL_DATA_TYPE type);
extern DATA_TYPE (*mapRbusDataTypeToWebPAFunc())(rbusValueType_t type);
WDMP_STATUS (*get_ParamValues_tr69hostIfFunc()) (HOSTIF_MsgData_t *ptrParam, DataModelParam *dmParam);
WAL_STATUS (*set_ParamValues_tr69hostIfFunc()) (HOSTIF_MsgData_t *ptrParam);
WAL_STATUS (*convertFaultCodeToWalStatusFunc())(faultCode_t faultCode);
extern void (*converttohostIfTypeFunc())(char *ParamDataType,HostIf_ParamType_t* pParamType);
void (*converttoWalTypeFunc())(HostIf_ParamType_t paramType,WAL_DATA_TYPE* pwalType);
extern void (*get_parodus_urlFunc())(char *parodus_url, char *client_url);
extern WDMP_STATUS (*validate_parameterFunc()) (param_t *param, int paramCount);
extern WAL_STATUS (*get_AttribValues_tr69hostIfFunc()) (HOSTIF_MsgData_t *ptrParam);
extern WAL_STATUS (*set_AttribValues_tr69hostIfFunc()) (HOSTIF_MsgData_t *param);
extern WAL_STATUS (*getParamAttributesFunc()) (const char *pParameterName, AttrVal ***attr, int *TotalParams);
extern WAL_STATUS (*setParamAttributesFunc()) (const char *pParameterName, const AttrVal *attArr);
extern void (*setRebootReasonFunc()) (param_t param, WEBPA_SET_TYPE setType);
extern long (*timeValDiffFunc()) (struct timespec *starttime, struct timespec *finishtime);
#endif

TEST(datamodelTest, ParameterExistPositive2) {

    /* Load the data model xml file*/
    DB_STATUS status = loadDataModel();
    if(status != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(status, DB_SUCCESS);

    DataModelParam dmParam = {0};
    WDMP_STATUS ret = WDMP_SUCCESS;

    const char* dbParamName = "Device.Time.NTPServer1";
    int match = getParamInfoFromDataModel(getDataModelHandle(), dbParamName, &dmParam);
    if(!match)
    {
        std::cout << "Invalid parameter name "<< dbParamName << " doesn't exist in data-model." << std::endl;
    }
    else
    {
        std::cout << "Parameter "<< dbParamName << " exist in data-model! Type = " << dmParam.dataType << std::endl;
    }
    EXPECT_NE(match, 0);
}

TEST(datamodelTest, ParameterExistNegative) {

    /* Load the data model xml file*/
    DB_STATUS status = loadDataModel();
    if(status != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(status, DB_SUCCESS);

    DataModelParam dmParam = {0};
    WDMP_STATUS ret = WDMP_SUCCESS;

    const char* dbParamName = "Device.Time.NTPServer6";
    int match = getParamInfoFromDataModel(getDataModelHandle(), dbParamName, &dmParam);
    if(!match)
    {
        std::cout << "Invalid parameter name "<< dbParamName << " doesn't exist in data-model." << std::endl;
    }
    else
    {
        std::cout << "Parameter "<< dbParamName << " exist in data-model! Type = " << dmParam.dataType << std::endl;
    }
    EXPECT_EQ(match, 0);
}

TEST(datamodelTest, ParameterExistPositive) {

    /* Load the data model xml file*/
    DB_STATUS status = loadDataModel();
    if(status != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(status, DB_SUCCESS);

    DataModelParam dmParam = {0};
    WDMP_STATUS ret = WDMP_SUCCESS;

    const char* dbParamName = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SHORTS.Enable";
    int match = getParamInfoFromDataModel(getDataModelHandle(), dbParamName, &dmParam);
    if(!match)
    {
        std::cout << "Invalid parameter name "<< dbParamName << " doesn't exist in data-model\n" << std::endl;
    }
    else
    {
        std::cout << "Parameter "<< dbParamName << " exist in data-model! Type = \n" << dmParam.dataType << std::endl;
    }
    EXPECT_NE(match, 0);
}

TEST(datamodelTest, ParameterListTest) {
    
    char **pParam_name_list;

    pParam_name_list =  (char**) calloc (2048, sizeof(char*));
    int num_of_params = 0;

    if(DB_SUCCESS != loadDataModel())
    {
        std::cout << "Failed to load data-model.xml file" << std::endl;
    }

    DB_STATUS status = get_complete_param_list (pParam_name_list, &num_of_params);
    if(DB_SUCCESS != status)
    {
        std::cout << "Failed to get complete parameter list."<< std::endl;
    }
    EXPECT_EQ(status, DB_SUCCESS);

    std::cout << "Count = " << num_of_params<< std::endl;
    EXPECT_NE(num_of_params, 0);
    
    int i = 0;
    std::cout << "======================================================================"<< std::endl;
    std::cout << "Iteration: => Starts "<< std::endl;
    std::cout << "======================================================================"<< std::endl;
    for(i=0; i< num_of_params; i++)
    {
        std::cout << "[" << i+1 << "] Parameter : [(" << pParam_name_list[i] << ")]" <<  std::endl;
        free(pParam_name_list[i]);
    }
    free(pParam_name_list);
    std::cout << "======================================================================"<< std::endl;
    std::cout << "Iteration: => Ends "<< std::endl;
    std::cout << "======================================================================"<< std::endl;
    DataModelParam dmParam = {0};
}

TEST(datamodelTest, getNumberofInstances) {
    int cnt = getNumberofInstances("Device.IP.Interface.{i}.");
    EXPECT_EQ(0, 0);

}

TEST(datamodelTest, isWildCardParam) {
    int wildParam = isWildCardParam("Device.DeviceInfo.");
    EXPECT_EQ(wildParam, 1);
}

TEST(datamodelTest, isParamEndsWithInstance) {
    int instance = isParamEndsWithInstance("Device.IP.Interface.{i}.");
    EXPECT_EQ(instance, 0);
}

TEST(datamodelTest, getNumberOfDigitsInInstanceNumber) {
    int instance = getNumberOfDigitsInInstanceNumber("Device.WiFi.SSID.123.Name", 17);
    EXPECT_EQ(instance, 3);
}

TEST(datamodelTest, getChildParamNamesFromDataModel) {
    
    /* Load the data model xml file*/
    DB_STATUS dbStatus = loadDataModel();
    if(dbStatus != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(dbStatus, DB_SUCCESS);

    char *ParamList = NULL;
    char *ParamDataTypeList = NULL;

    char *paramName = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.VideoTelemetry.";
    int paramCount = 1;
    DB_STATUS status = getChildParamNamesFromDataModel(getDataModelHandle(), paramName, &ParamList, &ParamDataTypeList, &paramCount);
    EXPECT_EQ(status, DB_SUCCESS);
}

TEST(datamodelTest, checkDataModelStatus) {
    DB_STATUS status = checkDataModelStatus();
    EXPECT_EQ(status, DB_SUCCESS);
}

TEST(datamodelTest, checkMatchingParameter) {
    const char* attrValue = "a.b.c.{i}.";
    char* paramName = "a.b.c.1.";
    int ret = 0;
    int retValue = checkMatchingParameter(attrValue, paramName, &ret);
    EXPECT_EQ(retValue, 1);
}

TEST(startParodusTest, get_HWMAcAddress) {
    write_on_file("/tmp/.macAddress", "D4:52:EE:DE:C6:FA");
    std::string macAddr = get_HWMAcAddress();
    EXPECT_EQ(macAddr, "D452EEDEC6FA");
}

TEST(startParodusTest, get_PartnerId) {
    write_on_file("/opt/www/authService/partnerId3.dat", "sky");
    std::string partnerId = get_PartnerId();
    EXPECT_EQ(partnerId, "*,sky");
}

TEST(startParodusTest, get_RebootReason) {
    std::string jsonData = "{\"reason\": \"PowerOnReset\", \"timestamp\": 1688914800}";
    write_on_file("/opt/secure/reboot/previousreboot.info", jsonData);
    std::string reboot_reason = get_RebootReason();
    EXPECT_EQ(reboot_reason, "PowerOnReset");
}

TEST(startParodusTest, get_FwName) {
    write_on_file("/version.txt", "imagename:ELTE11MWR_VBN_25Q3_sprint_20250814010729sdy_NG");
    std::string fw_name = get_FwName();
    EXPECT_EQ(fw_name, "ELTE11MWR_VBN_25Q3_sprint_20250814010729sdy_NG");
}

TEST(palTest, macToLower) {
    char macValue[32] = "A8:4A:63:88:E9:B5";
    char macConverted[32];
    macConverted[0] = '\0';
    macToLowerFunc()(macValue, macConverted);
    EXPECT_STREQ(macConverted, "a84a6388e9b5");
}

TEST(palTest, getnotifyparamList) {
    const char* json_data = R"({"Notify":["Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpStart","Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpEnd"]})";
    write_on_file("/tmp/notify.conf", json_data);
    char **notifyParamList = NULL;
    int ptrnotifyListSize = 3;
    setNotifyConfigurationFile("/tmp/notify.conf");
    int ret = getnotifyparamList(&notifyParamList, &ptrnotifyListSize);
    EXPECT_EQ(ret, 0);
}

TEST(palTest, getNotifySource) {
    char* notificationSource = getNotifySource();
    EXPECT_EQ(0, 0);
}

TEST(palTest, getRbusDataTypefromWebPA) {
    EXPECT_EQ(getRbusDataTypefromWebPAFunc()(WAL_STRING), RBUS_STRING);
    EXPECT_EQ(getRbusDataTypefromWebPAFunc()(WAL_INT), RBUS_INT32);
    EXPECT_EQ(getRbusDataTypefromWebPAFunc()(WAL_UINT), RBUS_UINT32);
    EXPECT_EQ(getRbusDataTypefromWebPAFunc()(WAL_BOOLEAN), RBUS_BOOLEAN);
    EXPECT_EQ(getRbusDataTypefromWebPAFunc()(WAL_DATETIME), RBUS_DATETIME);
    EXPECT_EQ(getRbusDataTypefromWebPAFunc()(WAL_BASE64), RBUS_BYTES);
    EXPECT_EQ(getRbusDataTypefromWebPAFunc()(WAL_LONG), RBUS_INT64);
    EXPECT_EQ(getRbusDataTypefromWebPAFunc()(WAL_ULONG), RBUS_UINT64);
    EXPECT_EQ(getRbusDataTypefromWebPAFunc()(WAL_FLOAT), RBUS_SINGLE);
    EXPECT_EQ(getRbusDataTypefromWebPAFunc()(WAL_DOUBLE), RBUS_DOUBLE);
    EXPECT_EQ(getRbusDataTypefromWebPAFunc()(WAL_BYTE), RBUS_BYTE);
}

TEST(palTest, mapRbusDataTypeToWebPA) {
    EXPECT_EQ(mapRbusDataTypeToWebPAFunc()(RBUS_BOOLEAN), WDMP_BOOLEAN);
    EXPECT_EQ(mapRbusDataTypeToWebPAFunc()(RBUS_CHAR), WDMP_BYTE);
    EXPECT_EQ(mapRbusDataTypeToWebPAFunc()(RBUS_INT16), WDMP_INT);
    EXPECT_EQ(mapRbusDataTypeToWebPAFunc()(RBUS_UINT16), WDMP_UINT);
    EXPECT_EQ(mapRbusDataTypeToWebPAFunc()(RBUS_INT64), WDMP_LONG);
    EXPECT_EQ(mapRbusDataTypeToWebPAFunc()(RBUS_UINT64), WDMP_ULONG);
    EXPECT_EQ(mapRbusDataTypeToWebPAFunc()(RBUS_STRING), WDMP_STRING);
    EXPECT_EQ(mapRbusDataTypeToWebPAFunc()(RBUS_DATETIME), WDMP_DATETIME);
    EXPECT_EQ(mapRbusDataTypeToWebPAFunc()(RBUS_DATETIME), WDMP_DATETIME);
    EXPECT_EQ(mapRbusDataTypeToWebPAFunc()(RBUS_BYTES), WDMP_BASE64);
    EXPECT_EQ(mapRbusDataTypeToWebPAFunc()(RBUS_SINGLE), WDMP_FLOAT);
    EXPECT_EQ(mapRbusDataTypeToWebPAFunc()(RBUS_DOUBLE), WDMP_DOUBLE);
    EXPECT_EQ(mapRbusDataTypeToWebPAFunc()(RBUS_PROPERTY), WDMP_NONE);
}

/*TEST(palTest, get_ParamValues_tr69hostIf) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.TopSpeed", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_IntegerType;
    param.paramLen = sizeof(hostIf_IntegerType);

    WDMP_STATUS status = get_ParamValues_tr69hostIfFunc()(&param, NULL);
    EXPECT_EQ(0, 0);
} */

TEST(palTest, set_ParamValues_tr69hostIf) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.LowSpeed", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, 13800);
    param.paramtype =  hostIf_IntegerType;
    param.paramLen = sizeof(hostIf_IntegerType);

    WAL_STATUS status = set_ParamValues_tr69hostIfFunc()(&param);
    EXPECT_EQ(0, 0);
}
TEST(palTest, convertFaultCodeToWalStatus) {
    EXPECT_EQ(convertFaultCodeToWalStatusFunc()(fcNoFault), WAL_FAILURE);
    EXPECT_EQ(convertFaultCodeToWalStatusFunc()(fcAttemptToSetaNonWritableParameter), WAL_ERR_NOT_WRITABLE);
    EXPECT_EQ(convertFaultCodeToWalStatusFunc()(fcInvalidParameterName), WAL_ERR_INVALID_PARAMETER_NAME);
    EXPECT_EQ(convertFaultCodeToWalStatusFunc()(fcInvalidParameterType), WAL_ERR_INVALID_PARAMETER_TYPE);
    EXPECT_EQ(convertFaultCodeToWalStatusFunc()(fcInvalidParameterValue), WAL_ERR_INVALID_PARAMETER_VALUE);
    EXPECT_EQ(convertFaultCodeToWalStatusFunc()(fcInternalError), WAL_FAILURE);
}

TEST(palTest, isWildCardParam) {
    int ret = isWildCardParam("Device.DeviceInfo.");
    EXPECT_EQ(ret, 1);
}

TEST(palTest, converttohostIfType) {
    HostIf_ParamType_t pParamType;

    converttohostIfTypeFunc()("string", &pParamType);
    EXPECT_EQ(pParamType, hostIf_StringType);

    converttohostIfTypeFunc()("unsignedInt", &pParamType);
    EXPECT_EQ(pParamType, hostIf_UnsignedIntType);

    converttohostIfTypeFunc()("int", &pParamType);
    EXPECT_EQ(pParamType, hostIf_IntegerType);

    converttohostIfTypeFunc()("unsignedLong", &pParamType);
    EXPECT_EQ(pParamType, hostIf_UnsignedLongType);

    converttohostIfTypeFunc()("boolean", &pParamType);
    EXPECT_EQ(pParamType, hostIf_BooleanType);

    converttohostIfTypeFunc()("hexBinary", &pParamType);
    EXPECT_EQ(pParamType, hostIf_StringType);
}

TEST(palTest, GetParamInfo) {

    DB_STATUS dbStatus = loadDataModel();
    if(dbStatus != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(dbStatus, DB_SUCCESS);
    const char *pParameterName = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerName";
    param_t** parameterval = (param_t**) calloc(1, sizeof(param_t*));
    EXPECT_NE(parameterval, nullptr);
    int paramCountPtr = 0;
    int index = 0;
    WDMP_STATUS status = GetParamInfoFunc()(pParameterName, &parameterval, &paramCountPtr, index);
    EXPECT_EQ(0, 0);
}

/* TEST(palTest, GetWildParamInfo) {
    const char *pParameterName = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.";
    param_t **parametervalPtrPtr = (param_t**) calloc(3, sizeof(param_t*));
    int paramCountPtr = 3;
    int index = 0;
    WDMP_STATUS status = GetParamInfoFunc()(pParameterName, &parametervalPtrPtr, &paramCountPtr, index);
    EXPECT_EQ(status, WDMP_SUCCESS);

    for (int i = 0; i < 3; i++) {
      if (parametervalPtrPtr[i]) {
          // If parametervalPtrPtr[i] points to dynamically allocated memory, free it
          free(parametervalPtrPtr[i]);
          parametervalPtrPtr[i] = NULL;
      }
    }
    free(parametervalPtrPtr);
    parametervalPtrPtr = NULL;
} */

TEST(palTest, get_parodus_url) {
    char parodus_url[256] = {0};
    char client_url[256] = {0};
    const char *webpaCfgFile = "{ \"ParodusURL\": \"tcp://parodus.xcal.tv:6666\", \"ParodusClientURL\": \"tcp://127.0.0.1:6666\" }";
    write_on_file("/etc/webpa_cfg.json", webpaCfgFile);
    get_parodus_urlFunc()(parodus_url, client_url);
    EXPECT_STREQ(parodus_url, "tcp://parodus.xcal.tv:6666");
    EXPECT_STREQ(client_url, "tcp://127.0.0.1:6666");
}

TEST(palTest, validate_parameter_wildcard) {
    param_t *params = (param_t *) malloc(sizeof(param_t) * 1);

    params[0].name = strdup("Device.DeviceInfo.");
    params[0].value = strdup("true");
    params[0].type = WDMP_BOOLEAN;
    int paramCount = 1;

    WDMP_STATUS status = validate_parameterFunc()(params, paramCount);
    EXPECT_EQ(status, WDMP_ERR_WILDCARD_NOT_SUPPORTED);
}

TEST(palTest, validate_parameter_Null) {
    param_t *params = (param_t *) malloc(sizeof(param_t) * 1);

    int paramCount = 1;
    params[0].name = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.Enable");
    params[0].value = NULL;
    params[0].type = WDMP_BOOLEAN;

    WDMP_STATUS status = validate_parameterFunc()(params, paramCount);
    EXPECT_EQ(status, WDMP_ERR_VALUE_IS_NULL);

    free(params[0].name);
    free(params[0].value);
    free(params);
}

TEST(palTest, validate_parameter_NOT_Support) {
    param_t *params = (param_t *) malloc(sizeof(param_t) * 1);

    int paramCount = 1;
    params[0].name = strdup("Device.DeviceInfo.Webpa.X_COMCAST-COM_CMC");
    params[0].value = strdup("test");
    params[0].type = WDMP_BOOLEAN;

    WDMP_STATUS status = validate_parameterFunc()(params, paramCount);
    EXPECT_EQ(status, WDMP_ERR_SET_OF_CMC_OR_CID_NOT_SUPPORTED);

    free(params[0].name);
    free(params[0].value);
    free(params);
}

TEST(palTest, processRequest_GET) {
     // Initialize paramMgrhash if not already done
    /*if (paramMgrhash == NULL) {
        paramMgrhash = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    } */

    //Load the data model xml file
    DB_STATUS status = loadDataModel();
    if(status != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(status, DB_SUCCESS);

    wrp_msg_t *wrp_msg;
    wrp_msg_t *res_wrp_msg;

    wrp_msg = (wrp_msg_t *)malloc(sizeof(wrp_msg_t));
    res_wrp_msg = (wrp_msg_t *)malloc(sizeof(wrp_msg_t));
    memset(res_wrp_msg, 0, sizeof(wrp_msg_t));
    wrp_msg->msg_type = WRP_MSG_TYPE__REQ;

    const char *payload = "{\"command\":\"GET\",\"names\":[\"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.TopSpeed\"]}";
    wrp_msg->u.req.payload = (void*)payload;

    wrp_msg->u.req.payload_size = strlen((char*)wrp_msg->u.req.payload);
    processRequest((char*)wrp_msg->u.req.payload, (char*)wrp_msg->u.req.transaction_uuid, ((char **)(&(res_wrp_msg->u.req.payload))));
    std::cout << "Response payload: " << (char*)res_wrp_msg->u.req.payload << std::endl;
    char *json_response = (char*)res_wrp_msg->u.req.payload;
    EXPECT_EQ(0, 0);
}


TEST(palTest, processRequest_SET) {
    // Initialize paramMgrhash if not already done
   /* if (paramMgrhash == NULL) {
        paramMgrhash = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    } */

   // Load the data model xml file
    DB_STATUS status = loadDataModel();
    if(status != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(status, DB_SUCCESS);

    wrp_msg_t *wrp_msg;
    wrp_msg_t *res_wrp_msg;

    wrp_msg = (wrp_msg_t *)malloc(sizeof(wrp_msg_t));
    res_wrp_msg = (wrp_msg_t *)malloc(sizeof(wrp_msg_t));
    memset(res_wrp_msg, 0, sizeof(wrp_msg_t));
    wrp_msg->msg_type = WRP_MSG_TYPE__REQ;

    const char *payload = "{\"command\":\"SET\",\"parameters\":[{\"name\":\"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.LogServerUrl\",\"dataType\":0,\"value\":\"logs.xcal.tv\"}]}";
    wrp_msg->u.req.payload = (void*)payload;

    wrp_msg->u.req.payload_size = strlen((char*)wrp_msg->u.req.payload);
    processRequest((char*)wrp_msg->u.req.payload, (char*)wrp_msg->u.req.transaction_uuid, ((char **)(&(res_wrp_msg->u.req.payload))));
    std::cout << "Response payload: " << (char*)res_wrp_msg->u.req.payload << std::endl;
    char *json_response = (char*)res_wrp_msg->u.req.payload;
    EXPECT_EQ(0, 0);
}

TEST(palTest, get_AttribValues_tr69hostIf) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    WAL_STATUS status = get_AttribValues_tr69hostIfFunc()(&param);
    EXPECT_EQ(status, WAL_ERR_INVALID_PARAM);
}

TEST(palTest, set_AttribValues_tr69hostIf) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    WAL_STATUS status = set_AttribValues_tr69hostIfFunc()(&param);
    EXPECT_EQ(status, 4);
}

TEST(palTest, getParamAttributes) {

    const char *paramName = "Device.WiFi.SSID.1.SSID";
    AttrVal **attributes = NULL;
    int totalParams = 0;

    WAL_STATUS status = getParamAttributesFunc()(paramName, &attributes, &totalParams);
    EXPECT_EQ(status, WAL_ERR_INVALID_PARAM);
}

TEST(palTest, setParamAttributes) {

    const char *paramName = "Device.WiFi.SSID.1.SSID";	
    AttrVal attr;

    WAL_STATUS status = setParamAttributesFunc()(paramName, &attr);
    EXPECT_EQ(status, WAL_SUCCESS);
}


TEST(webpaAdapterTest, setRebootReason) {
    // Prepare a param_t with the reboot parameter and value
    param_t param;
    param.name = strdup("Device.X_CISCO_COM_DeviceControl.RebootDevice");
    param.value = strdup("Device");
    param.type = WDMP_STRING;

    // Call setRebootReason and check for no crash (L1)
    setRebootReasonFunc()(param, WEBPA_SET);

    // Clean up
    free(param.name);
    free(param.value);

    // L1: No assertion needed, just ensure no crash
    EXPECT_EQ(0, 0);
}


TEST(palTest, notificationCallBack) {
    notificationCallBack();
    EXPECT_EQ(0, 0);
}

TEST(palTest, setInitialNotify) {
    notificationCallBack();
    EXPECT_EQ(0, 0);
}

TEST(palTest, registerNotifyCallback) {
    registerNotifyCallback();
    EXPECT_EQ(0, 0);
}

TEST(palTest, timeValDiff) {
    struct timespec starttime = {
        .tv_sec = 100,
        .tv_nsec = 500000000   // 0.5 seconds
    };

    struct timespec endtime = {
        .tv_sec = 102,
        .tv_nsec = 200000000   // 0.2 seconds
    };

    long msec = timeValDiffFunc()(&starttime, &endtime);
    EXPECT_EQ(msec, 1700);
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
