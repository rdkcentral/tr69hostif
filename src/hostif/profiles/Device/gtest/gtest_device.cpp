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

#include "hostIf_utils.h"
#include "XrdkCentralComRFCStore.h"
#include "XrdkCentralComBSStore.h"
#include "rdk_debug.h"

#include "x_rdk_profile.h"

#include <curl/curl.h>
#include "cJSON.h"

#include <mutex>
#include <condition_variable>

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "hostif_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 128

using namespace std;

TEST(DeviceTest, handleSetMsg) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.X_RDK_WebPA_DNSText.URL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;

    strncpy(param.paramValue, "fabric.xmidt.comcast.net", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    X_rdk_profile* profile = X_rdk_profile::getInstance();
    if(profile)
    {
       int ret = profile->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(DeviceTest, handleGetMsg) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.X_RDK_WebPA_DNSText.URL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    X_rdk_profile* profile = X_rdk_profile::getInstance();
    if(profile)
    {
       int ret = profile->handleGetMsg(&param);
       std::string value = getStringValue(&param);
       EXPECT_EQ(ret, OK);
       EXPECT_EQ(value, "fabric.xmidt.comcast.net");
    }
}

TEST(DeviceTest, get_WebPA_Server_URL) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.X_RDK_WebPA_Server.URL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    X_rdk_profile* profile = X_rdk_profile::getInstance();
    if(profile)
    {
       int ret = profile->get_WebPA_Server_URL(&param);
       std::string value = getStringValue(&param);
       EXPECT_EQ(ret, OK);
       EXPECT_EQ(value, "");
    }
}

TEST(DeviceTest, get_WebPA_TokenServer_URL) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.X_RDK_WebPA_TokenServer.URL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    X_rdk_profile* profile = X_rdk_profile::getInstance();
    if(profile)
    {
       int ret = profile->get_WebPA_TokenServer_URL(&param);
       std::string value = getStringValue(&param);
       EXPECT_EQ(ret, OK);
       EXPECT_EQ(value, "");
    }
}

TEST(DeviceTest, get_WebPA_DNSText_URL) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.X_RDK_WebPA_DNSText.URL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    X_rdk_profile* profile = X_rdk_profile::getInstance();
    if(profile)
    {
       int ret = profile->get_WebPA_DNSText_URL(&param);
       std::string value = getStringValue(&param);
       EXPECT_EQ(ret, OK);
       EXPECT_EQ(value, "fabric.xmidt.comcast.net");
    }
}

TEST(DeviceTest, set_WebPA_DNSText_URL) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.X_RDK_WebPA_DNSText.URL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;

    strncpy(param.paramValue, "fabric.xmidt-eu.comcast.net", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    X_rdk_profile* profile = X_rdk_profile::getInstance();
    if(profile)
    {
       int ret = profile->set_WebPA_DNSText_URL(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(DeviceTest, DNSTextRejectsConfigInjectionWithoutChangingBootstrapValues) {
    X_rdk_profile *profile = X_rdk_profile::getInstance();
    ASSERT_NE(profile, nullptr);
    const char *names[] = {X_RDK_WebPA_DNSText_URL_STRING, X_RDK_WebPA_SERVER_URL_STPRING,
                           X_RDK_WebPA_TokenServer_URL_STRING};
    string originalValues[3];
    for (size_t index = 0; index < 3; ++index)
    {
        HOSTIF_MsgData_t param = {};
        snprintf(param.paramName, sizeof(param.paramName), "%s", names[index]);
        ASSERT_EQ(profile->handleGetMsg(&param), OK);
        originalValues[index] = param.paramValue;
    }
    const char *invalidValues[] = {
        "fabric.example\r\nDevice.X_RDK_WebPA_Server.URL=https://other.invalid",
        "fabric.example\nDevice.X_RDK_WebPA_TokenServer.URL=https://other.invalid",
        "fabric.example\rX_TEST_CANARY=INJECTED", "fabric.example\t", "fabric.example ",
        "fabric.example\x01", "fabric.example\x7f", "fabric.example\xc2\x85"
    };
    const HostIf_Source_Type_t sources[] = {HOSTIF_SRC_RFC, HOSTIF_SRC_WEBPA};
    for (const auto source : sources)
    {
        for (const char *value : invalidValues)
        {
            SCOPED_TRACE(::testing::Message() << "requestor=" << source << ", value=" << value);
            HOSTIF_MsgData_t param = {};
            snprintf(param.paramName, sizeof(param.paramName), "%s", X_RDK_WebPA_DNSText_URL_STRING);
            snprintf(param.paramValue, sizeof(param.paramValue), "%s", value);
            param.paramtype = hostIf_StringType;
            param.paramLen = strlen(param.paramValue);
            param.requestor = source;
            EXPECT_EQ(profile->handleSetMsg(&param), NOK);
            EXPECT_EQ(param.faultCode, fcInvalidParameterValue);
            for (size_t index = 0; index < 3; ++index)
            {
                HOSTIF_MsgData_t result = {};
                snprintf(result.paramName, sizeof(result.paramName), "%s", names[index]);
                ASSERT_EQ(profile->handleGetMsg(&result), OK);
                EXPECT_EQ(string(result.paramValue), originalValues[index]);
            }
        }
    }
}

TEST(DeviceTest, DNSTextRejectsInvalidTypeAndUnterminatedValues) {
    X_rdk_profile *profile = X_rdk_profile::getInstance();
    ASSERT_NE(profile, nullptr);
    HOSTIF_MsgData_t param = {};
    snprintf(param.paramName, sizeof(param.paramName), "%s", X_RDK_WebPA_DNSText_URL_STRING);
    param.paramtype = hostIf_BooleanType;
    EXPECT_EQ(profile->handleSetMsg(&param), NOK);
    EXPECT_EQ(param.faultCode, fcInvalidParameterType);

    param.paramtype = hostIf_StringType;
    EXPECT_EQ(profile->handleSetMsg(&param), NOK);
    EXPECT_EQ(param.faultCode, fcInvalidParameterValue);

    memset(param.paramValue, 'a', sizeof(param.paramValue));
    EXPECT_EQ(profile->handleSetMsg(&param), NOK);
    EXPECT_EQ(param.faultCode, fcInvalidParameterValue);
}

TEST(DeviceTest, DNSTextRejectsParameterNameSuffix) {
    X_rdk_profile *profile = X_rdk_profile::getInstance();
    ASSERT_NE(profile, nullptr);
    HOSTIF_MsgData_t param = {};
    snprintf(param.paramName, sizeof(param.paramName), "%s.extra", X_RDK_WebPA_DNSText_URL_STRING);
    snprintf(param.paramValue, sizeof(param.paramValue), "%s", "fabric.xmidt.comcast.net");
    param.paramtype = hostIf_StringType;
    EXPECT_EQ(profile->handleSetMsg(&param), NOK);
    EXPECT_EQ(param.faultCode, fcInvalidParameterName);
}

TEST(DeviceTest, handleGetMsg_WebPA_Server_URL) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.X_RDK_WebPA_Server.URL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    X_rdk_profile* profile = X_rdk_profile::getInstance();
    if(profile)
    {
       int ret = profile->handleGetMsg(&param);
       std::string value = getStringValue(&param);
       EXPECT_EQ(ret, OK);
       EXPECT_EQ(value, "");
    }
}

TEST(DeviceTest, handleGetMsg_WebPA_TokenServer_URL) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.X_RDK_WebPA_TokenServer.URL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    X_rdk_profile* profile = X_rdk_profile::getInstance();
    if(profile)
    {
       int ret = profile->handleGetMsg(&param);
       std::string value = getStringValue(&param);
       EXPECT_EQ(ret, OK);
       EXPECT_EQ(value, "");
    }
}

TEST(DeviceTest, handleSetMsg_EmptyParamName) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    param.paramName[0] = '\0';
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;

    X_rdk_profile* profile = X_rdk_profile::getInstance();
    ASSERT_NE(profile, nullptr);

    int ret = profile->handleSetMsg(&param);
    EXPECT_EQ(param.faultCode, fcInvalidParameterName);
    EXPECT_EQ(ret, NOK);
}

TEST(DeviceTest, handleGetMsg_EmptyParamName) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    param.paramName[0] = '\0';
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    X_rdk_profile* profile = X_rdk_profile::getInstance();
    ASSERT_NE(profile, nullptr);

    int ret = profile->handleGetMsg(&param);    
    EXPECT_EQ(ret, NOK);
    EXPECT_EQ(param.faultCode, fcInvalidParameterName);
}

TEST(DeviceTest, handleSetMsg_InvalidParam) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.FriendlyName.URL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;

    X_rdk_profile* profile = X_rdk_profile::getInstance();
    if(profile)
    {
       int ret = profile->handleSetMsg(&param);
       EXPECT_EQ(ret, NOK);
       EXPECT_EQ(param.faultCode, fcInvalidParameterName);
    }
}

TEST(DeviceTest, handleGetMsg_InvalidParam) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.FriendlyName.URL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    X_rdk_profile* profile = X_rdk_profile::getInstance();
    if(profile)
    {
       int ret = profile->handleGetMsg(&param);
       EXPECT_EQ(ret, NOK);
       EXPECT_EQ(param.faultCode, fcInvalidParameterName);
    }
    profile->closeInstance();
}

GTEST_API_ int main(int argc, char *argv[]){
    char testresults_fullfilepath[GTEST_REPORT_FILEPATH_SIZE];
    char buffer[GTEST_REPORT_FILEPATH_SIZE];

    memset( testresults_fullfilepath, 0, GTEST_REPORT_FILEPATH_SIZE );
    memset( buffer, 0, GTEST_REPORT_FILEPATH_SIZE );
    snprintf( testresults_fullfilepath, GTEST_REPORT_FILEPATH_SIZE, "json:%s%s" , GTEST_DEFAULT_RESULT_FILEPATH , GTEST_DEFAULT_RESULT_FILENAME);

    ::testing::GTEST_FLAG(output) = testresults_fullfilepath;
    ::testing::InitGoogleMock(&argc, argv);
    std::cout <<"running ut"<< std::endl;
    return RUN_ALL_TESTS();
}

