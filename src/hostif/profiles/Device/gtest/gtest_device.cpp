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

