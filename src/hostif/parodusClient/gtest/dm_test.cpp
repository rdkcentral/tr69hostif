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

#include "waldb.h"
#include "wdmp-c.h"

using namespace std;

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "datamodel_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 128

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
    EXPECT_EQ(cnt, 4);
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
    int instance = getNumberOfDigitsInInstanceNumber("Device.WiFi.SSID.123.Name", 16);
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
    std::string fw_name = get_FwName();
    EXPECT_EQ(fw_name, "T2_Container_0.0.0");
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
    EXPECT_STREQ(notificationSource, "mac:a84a6388e9b5");
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

TEST(palTest, get_ParamValues_tr69hostIf) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.TopSpeed", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_IntegerType;
    param.paramLen = sizeof(hostIf_IntegerType);

    WDMP_STATUS status = get_ParamValues_tr69hostIfFunc()(&param);
    EXPECT_EQ(status, WAL_SUCCESS);
}

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
    EXPECT_EQ(status, WAL_SUCCESS);
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
    EXPECT_EQ(status, WDMP_SUCCESS);
}

TEST(palTest, GetWildParamInfo) {
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
