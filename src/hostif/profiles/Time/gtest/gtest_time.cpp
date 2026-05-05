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

/* ------------------------------------------------------------------ */
/* Chrony RFC parameter tests                                           */
/* ------------------------------------------------------------------ */

static const char *kChronyDir    = "/opt/secure/RFC/chrony";
static const char *kChronyEnable = "/opt/secure/RFC/chrony/chronyd_enabled";
static const char *kNtpMaxstep   = "/opt/secure/RFC/chrony/ntp_maxstep";

/* Helper: create parent directory recursively (simple two-level) */
static void ensureChronyDir()
{
    mkdir("/opt", 0755);
    mkdir("/opt/secure", 0755);
    mkdir("/opt/secure/RFC", 0755);
    mkdir(kChronyDir, 0755);
}

/* Helper: remove a file silently */
static void removeFile(const char *path)
{
    std::remove(path);
}

/* ---- Device.Time.Chrony.Enable ------------------------------------ */

TEST(TimeTest, get_Device_Time_Chrony_Enable_FileAbsent)
{
    ensureChronyDir();
    removeFile(kChronyEnable);

    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->get_Device_Time_Chrony_Enable(&param);
        EXPECT_EQ(ret, OK);
        EXPECT_EQ(param.paramtype, hostIf_BooleanType);
        bool val = false;
        memcpy(&val, param.paramValue, sizeof(bool));
        EXPECT_EQ(val, false);
    }
}

TEST(TimeTest, set_get_Device_Time_Chrony_Enable_True)
{
    ensureChronyDir();
    removeFile(kChronyEnable);

    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "true", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_Chrony_Enable(&param);
        EXPECT_EQ(ret, OK);

        /* Now read back */
        HOSTIF_MsgData_t getParam = { 0 };
        memset(&getParam, 0, sizeof(HOSTIF_MsgData_t));
        ret = pIface->get_Device_Time_Chrony_Enable(&getParam);
        EXPECT_EQ(ret, OK);
        bool val = false;
        memcpy(&val, getParam.paramValue, sizeof(bool));
        EXPECT_EQ(val, true);

        removeFile(kChronyEnable);
    }
}

TEST(TimeTest, set_Device_Time_Chrony_Enable_InvalidValue)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "yes", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_Chrony_Enable(&param);
        EXPECT_EQ(ret, NOK);
    }
}

/* ---- Device.Time.Chrony.Makestep ---------------------------------- */

TEST(TimeTest, get_Device_Time_NTPMaxstep_DefaultValue)
{
    ensureChronyDir();
    removeFile(kNtpMaxstep);

    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->get_Device_Time_NTPMaxstep(&param);
        EXPECT_EQ(ret, OK);
        EXPECT_EQ(param.paramtype, hostIf_StringType);
        EXPECT_STREQ(param.paramValue, "1.0,3");
    }
}

TEST(TimeTest, set_get_Device_Time_NTPMaxstep_ValidValue)
{
    ensureChronyDir();
    removeFile(kNtpMaxstep);

    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.Makestep", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "0.5,5", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_NTPMaxstep(&param);
        EXPECT_EQ(ret, OK);

        HOSTIF_MsgData_t getParam = { 0 };
        memset(&getParam, 0, sizeof(HOSTIF_MsgData_t));
        ret = pIface->get_Device_Time_NTPMaxstep(&getParam);
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(getParam.paramValue, "0.5,5");

        removeFile(kNtpMaxstep);
    }
}

TEST(TimeTest, set_Device_Time_NTPMaxstep_MissingComma)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.Makestep", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "1.0", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_NTPMaxstep(&param);
        EXPECT_EQ(ret, NOK);
    }
}

/* ---- Device.Time.Chrony.NTPServer.{i}.Settings ------------------- */

static std::string ntpSettingsFile(int idx)
{
    char buf[128];
    snprintf(buf, sizeof(buf), "/opt/secure/RFC/chrony/ntp_server%d_settings", idx);
    return std::string(buf);
}

TEST(TimeTest, get_Device_Time_NTPServerSettings_DefaultValue)
{
    ensureChronyDir();
    std::string fp = ntpSettingsFile(1);
    removeFile(fp.c_str());

    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.NTPServer.1.Settings",
            TR69HOSTIFMGR_MAX_PARAM_LEN - 1);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->get_Device_Time_NTPServerSettings(&param);
        EXPECT_EQ(ret, OK);
        EXPECT_EQ(param.paramtype, hostIf_StringType);
        EXPECT_STREQ(param.paramValue, "server,0,false,10,12");
    }
}

TEST(TimeTest, set_get_Device_Time_NTPServerSettings_ValidServer)
{
    ensureChronyDir();
    std::string fp = ntpSettingsFile(1);
    removeFile(fp.c_str());

    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.NTPServer.1.Settings",
            TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "server,0,true,6,12", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_NTPServerSettings(&param);
        EXPECT_EQ(ret, OK);

        HOSTIF_MsgData_t getParam = { 0 };
        memset(&getParam, 0, sizeof(HOSTIF_MsgData_t));
        strncpy(getParam.paramName, "Device.Time.Chrony.NTPServer.1.Settings",
                TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
        ret = pIface->get_Device_Time_NTPServerSettings(&getParam);
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(getParam.paramValue, "server,0,true,6,12");

        removeFile(fp.c_str());
    }
}

TEST(TimeTest, set_get_Device_Time_NTPServerSettings_ValidPool)
{
    ensureChronyDir();
    std::string fp = ntpSettingsFile(3);
    removeFile(fp.c_str());

    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.NTPServer.3.Settings",
            TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "pool,4,false,4,24", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_NTPServerSettings(&param);
        EXPECT_EQ(ret, OK);

        HOSTIF_MsgData_t getParam = { 0 };
        memset(&getParam, 0, sizeof(HOSTIF_MsgData_t));
        strncpy(getParam.paramName, "Device.Time.Chrony.NTPServer.3.Settings",
                TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
        ret = pIface->get_Device_Time_NTPServerSettings(&getParam);
        EXPECT_EQ(ret, OK);
        EXPECT_STREQ(getParam.paramValue, "pool,4,false,4,24");

        removeFile(fp.c_str());
    }
}

TEST(TimeTest, set_Device_Time_NTPServerSettings_InvalidInstance_Zero)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.NTPServer.0.Settings",
            TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "server,0,false,6,12", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_NTPServerSettings(&param);
        EXPECT_EQ(ret, NOK);
    }
}

TEST(TimeTest, set_Device_Time_NTPServerSettings_InvalidInstance_Six)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.NTPServer.6.Settings",
            TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "server,0,false,6,12", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_NTPServerSettings(&param);
        EXPECT_EQ(ret, NOK);
    }
}

TEST(TimeTest, set_Device_Time_NTPServerSettings_TrailingGarbage)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.NTPServer.1.Settings",
            TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "server,0,false,10,12,unexpected",
            sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_NTPServerSettings(&param);
        EXPECT_EQ(ret, NOK);
    }
}

TEST(TimeTest, set_Device_Time_NTPServerSettings_InvalidType)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.NTPServer.1.Settings",
            TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "peer,0,false,6,12", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_NTPServerSettings(&param);
        EXPECT_EQ(ret, NOK);
    }
}

TEST(TimeTest, set_Device_Time_NTPServerSettings_ServerNonZeroMaxsources)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.NTPServer.1.Settings",
            TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "server,2,false,6,12", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_NTPServerSettings(&param);
        EXPECT_EQ(ret, NOK);
    }
}

TEST(TimeTest, set_Device_Time_NTPServerSettings_InvalidIburst)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.NTPServer.1.Settings",
            TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    strncpy(param.paramValue, "server,0,yes,6,12", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_NTPServerSettings(&param);
        EXPECT_EQ(ret, NOK);
    }
}

TEST(TimeTest, set_Device_Time_NTPServerSettings_MinpollOutOfRange)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.NTPServer.1.Settings",
            TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    /* minpoll = 3 is below the allowed minimum of 4 */
    strncpy(param.paramValue, "server,0,false,3,12", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_NTPServerSettings(&param);
        EXPECT_EQ(ret, NOK);
    }
}

TEST(TimeTest, set_Device_Time_NTPServerSettings_MaxpollOutOfRange)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.NTPServer.1.Settings",
            TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    /* maxpoll = 25 is above the allowed maximum of 24 */
    strncpy(param.paramValue, "server,0,false,6,25", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_NTPServerSettings(&param);
        EXPECT_EQ(ret, NOK);
    }
}

TEST(TimeTest, set_Device_Time_NTPServerSettings_MaxpollLessThanMinpoll)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.NTPServer.1.Settings",
            TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    /* maxpoll(6) < minpoll(10) */
    strncpy(param.paramValue, "server,0,false,10,6", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_NTPServerSettings(&param);
        EXPECT_EQ(ret, NOK);
    }
}

TEST(TimeTest, set_Device_Time_NTPServerSettings_MissingFields)
{
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramName, "Device.Time.Chrony.NTPServer.1.Settings",
            TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    /* only 3 fields instead of 5 */
    strncpy(param.paramValue, "server,0,false", sizeof(param.paramValue) - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen  = strlen(param.paramValue);

    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if (pIface)
    {
        int ret = pIface->set_Device_Time_NTPServerSettings(&param);
        EXPECT_EQ(ret, NOK);
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
