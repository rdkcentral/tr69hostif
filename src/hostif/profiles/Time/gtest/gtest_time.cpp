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
#include "Device_Time.h"
#include "rdk_debug.h"


#include <curl/curl.h>
#include "cJSON.h"

#include <mutex>
#include <condition_variable>

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "hostif_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 128

using namespace std;

TEST(TimeTest, get_Device_Time_LocalTimeZone) {
    int instanceNumber = 0;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_Time *hostIfTime= hostIf_Time::getInstance(instanceNumber);
    if(hostIfTime)
    {
       int ret = hostIfTime->get_Device_Time_LocalTimeZone(&param);
       EXPECT_EQ(ret, OK);
       EXPECT_EQ(param.paramtype, hostIf_StringType);
    }
}

TEST(TimeTest, get_Device_Time_CurrentLocalTime) {
    int instanceNumber = 0;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_Time *hostIfTime= hostIf_Time::getInstance(instanceNumber);
    if(hostIfTime)
    {
       int ret = hostIfTime->get_Device_Time_CurrentLocalTime(&param, &pChanged);
       EXPECT_EQ(ret, OK);
       EXPECT_EQ(param.paramtype, hostIf_StringType);
    }
}

TEST(TimeTest, get_xRDKCentralComBootstrap) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XDial.AppList", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    hostIf_Time *hostIfTime= hostIf_Time::getInstance(instanceNumber);
    if(hostIfTime)
    {
       int ret = hostIfTime->get_xRDKCentralComBootstrap(&param);
       std::string value = getStringValue(&param);
       EXPECT_EQ(ret, OK);
       EXPECT_EQ(value, "youtube:spotify:netflix:system");
    }
}

TEST(TimeTest, set_xRDKCentralComBootstrap) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.XconfUrl", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "https://xconf.xdp.eu-1.xcal.tv", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_Time *hostIfTime= hostIf_Time::getInstance(instanceNumber);
    if(hostIfTime)
    {
       int ret = hostIfTime->set_xRDKCentralComBootstrap(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(TimeTest, get_Device_Time_CurrentUTCTime) {
    int instanceNumber = 0;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_Time *hostIfTime= hostIf_Time::getInstance(instanceNumber);
    if(hostIfTime)
    {
       int ret = hostIfTime->get_Device_Time_CurrentUTCTime(&param, &pChanged);
       EXPECT_EQ(ret, OK);
       EXPECT_EQ(param.paramtype, hostIf_StringType);
    }
}

TEST(TimeTest, releaseLock) {
    int instanceNumber = 0;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_Time *hostIfTime= hostIf_Time::getInstance(instanceNumber);
    if(hostIfTime)
    {
       hostIfTime->getLock();
       gboolean locked = g_mutex_trylock(&hostIfTime->m_mutex);
       EXPECT_EQ(locked, false);
       hostIfTime->releaseLock();
    }
}

TEST(TimeTest, closeInstance) {
    int instanceNumber = 0;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_Time *hostIfTime= hostIf_Time::getInstance(instanceNumber);
    if(hostIfTime)
    {
       hostIf_Time::closeInstance(hostIfTime);
       hostIfTime->closeAllInstances();
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
