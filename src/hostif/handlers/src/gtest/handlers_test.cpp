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
#include "x_rdk_req_handler.h"
#include "hostIf_rbus_Dml_Provider.h"
#include "rbus_value.h"
#include "rbus.h"
#include "hostIf_XrdkCentralT2_ReqHandler.h"
#include "hostIf_TimeClient_ReqHandler.h"
#include "hostIf_DeviceClient_ReqHandler.h"
#include "hostIf_EthernetClient_ReqHandler.h"
#include "hostIf_IPClient_ReqHandler.h"
#include "hostIf_DHCPv4Client_ReqHandler.h"


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

extern int (*convertRbus2hostIfDataTypeFunc()) (rbusValueType_t type, HostIf_ParamType_t* pParamType);


TEST(handlersTest, handleGetMsg) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.X_RDK_WebPA_DNSText.URL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    X_rdk_req_hdlr* reqHandler = static_cast<X_rdk_req_hdlr*>(X_rdk_req_hdlr::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, handleSetMsg) {
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

    X_rdk_req_hdlr* reqHandler = static_cast<X_rdk_req_hdlr*>(X_rdk_req_hdlr::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, convertRbus2hostIfDataType) {
    HostIf_ParamType_t paramType;

    EXPECT_EQ(convertRbus2hostIfDataTypeFunc()(RBUS_BOOLEAN, &paramType), 0);
    EXPECT_EQ(convertRbus2hostIfDataTypeFunc()(RBUS_INT8, &paramType), 0);
    EXPECT_EQ(convertRbus2hostIfDataTypeFunc()(RBUS_UINT8, &paramType), 0);
    EXPECT_EQ(convertRbus2hostIfDataTypeFunc()(RBUS_INT64, &paramType), 0);
    EXPECT_EQ(convertRbus2hostIfDataTypeFunc()(RBUS_STRING, &paramType), 0);
    EXPECT_EQ(convertRbus2hostIfDataTypeFunc()(RBUS_DATETIME, &paramType), 0);
}

TEST(handlersTest, TR_Dml_EventSubHandler) {
    HostIf_ParamType_t paramType;
    rbusHandle_t handle;
    rbusEventSubAction_t action;
    const char* eventName = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType";
    rbusFilter_t filter;
    int32_t interval = 60;
    bool autoPublish = false;

    rbusError_t rc = TR_Dml_EventSubHandler(handle, action, eventName, filter, interval, &autoPublish);
    EXPECT_EQ(rc, RBUS_ERROR_SUCCESS);
}

TEST(handlersTest, TR_Dml_GetHandler) {
    //MockRbusProperty mockProp = { "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.Enable" };
    //rbusProperty_t testProp = (rbusProperty_t)&mockProp;
    rbusProperty_t testProp;

    rbusHandle_t handle;
    rbusGetHandlerOptions_t opts = {};

    rbusError_t rc = TR_Dml_GetHandler(handle, testProp, &opts);
    EXPECT_EQ(rc, 2);
}

TEST(handlersTest, setRbusStringParam) {
    const char *paramName = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType";
    char* paramValue = "testdata";

    int rc = setRbusStringParam(paramName, paramValue);
    EXPECT_EQ(rc, -1);
}

TEST(handlersTest, getRbusStringParam) {
    const char *paramName = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType";
    char* paramValue = nullptr;

    int rc = getRbusStringParam(paramName, &paramValue);
    EXPECT_EQ(rc, -1);
}

TEST(handlersTest, XrdkCentralT2handleSetMsg) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.X_RDKCENTRAL-COM_T2.ReportProfiles", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;

    strncpy(param.paramValue, "testprofile", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    XRdkCentralT2* reqHandler = static_cast<XRdkCentralT2*>(XRdkCentralT2::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, XrdkCentralT2handleGetMsg) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.X_RDKCENTRAL-COM_T2.ReportProfiles", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    XRdkCentralT2* reqHandler = static_cast<XRdkCentralT2*>(XRdkCentralT2::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, TimeClientReqHandler_handleGetMsg_Enable) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Time.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    TimeClientReqHandler* reqHandler = static_cast<TimeClientReqHandler*>(TimeClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}


TEST(handlersTest, TimeClientReqHandler_handleGetMsg_Status) {
    write_on_file("/tmp/ntp_status", "ntp status");
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Time.Status", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    TimeClientReqHandler* reqHandler = static_cast<TimeClientReqHandler*>(TimeClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
       EXPECT_STREQ(param.paramValue, "ntp status");
    }
}

TEST(handlersTest, TimeClientReqHandler_handleGetMsg_LocalTimeZone) {
    write_on_file("/tmp/ntp_status", "ntp status");
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Time.LocalTimeZone", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    TimeClientReqHandler* reqHandler = static_cast<TimeClientReqHandler*>(TimeClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, TimeClientReqHandler_handleGetMsg_CurrentLocalTime) {
    write_on_file("/tmp/ntp_status", "ntp status");
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Time.CurrentLocalTime", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    TimeClientReqHandler* reqHandler = static_cast<TimeClientReqHandler*>(TimeClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, TimeClientReqHandler_handleGetMsg_X_RDK_CurrentUTCTime) {
    write_on_file("/tmp/ntp_status", "ntp status");
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Time.X_RDK_CurrentUTCTime", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    TimeClientReqHandler* reqHandler = static_cast<TimeClientReqHandler*>(TimeClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, TimeClientReqHandler_handleSetMsg) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.Time.NTPServer5", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_SRC_RFC;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "override_time4.com", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    TimeClientReqHandler* reqHandler = static_cast<TimeClientReqHandler*>(TimeClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}


TEST(handlersTest, TimeClientReqHandler_handleGetAttributesMsg) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Time.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    TimeClientReqHandler* reqHandler = static_cast<TimeClientReqHandler*>(TimeClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetAttributesMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, TimeClientReqHandler_handleSetAttributesMsg) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.Time.NTPServer4", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "override_time3.com", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = sizeof(hostIf_StringType);

    TimeClientReqHandler* reqHandler = static_cast<TimeClientReqHandler*>(TimeClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetAttributesMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleSetMsg) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.SsrUrl", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_SRC_RFC;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "https://ssr.ccp.xcal.tv", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleSetMsg_FirmwareDownloadStatus) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "Download Started", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleSetMsg_FirmwareDownloadProtocol) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "http", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleSetMsg_FirmwareDownloadURL) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "https://xconf.xdp.eu-1.xcal.tv", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleSetMsg_PreferredGatewayType) {
    write_on_file("/opt/prefered-gateway", "192.168.1.1");
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_PreferredGatewayType", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleSetMsg_set_xOpsDMUploadLogsNow) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMUploadLogsNow", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "true", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleSetMsg_ForwardSSHEnable) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleSetMsg_DownloadStatus) {	
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.DownloadStatus", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleSetMsg_IPRemoteSupportEnable) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleSetMsg_PartnerId) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "global", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleSetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.SsrUrl", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_SRC_RFC;
    param.requestor = HOSTIF_SRC_RFC;


    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_PreferredGatewayType) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_PreferredGatewayType", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;


    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_Manufacturer) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.Manufacturer", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;


    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_ManufacturerOUI) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.ManufacturerOUI", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;


    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, -2);
    }
}



TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_ModelName) {
     write_on_file("/tmp/.model", "SKY");
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.ModelName", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
       EXPECT_STREQ(param.paramValue, "SKY");
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_Description) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.Description", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_HardwareVersion) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.HardwareVersion", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, NOK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_SoftwareVersion) {
    writeToTr181storeFile("VERSION", "99.99.15.07", "/version.txt", Plain);	
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.SoftwareVersion", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
    std::remove("/version.txt");
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_MigrationStatus) {
    write_on_file("/opt/secure/persistent/MigrationStatus", "NEEDED");
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.Migration.MigrationStatus", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
       EXPECT_STREQ(param.paramValue, "NEEDED");
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_IUI_Version) {
    write_on_file("/tmp/.iuiVersion", "4.4");
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM.IUI.Version", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
       EXPECT_STREQ(param.paramValue, "4.4");
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_UpTime) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.UpTime", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());         
    if(reqHandler)
    {       
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_FirstUseDate) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.FirstUseDate", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_FirmwareFilename) {
     write_on_file("/tmp/currently_running_image_name", "ELTE11MWR_DEV_develop_20250808222826_NG");
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareFilename", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       EXPECT_EQ(ret, OK);
       EXPECT_STREQ(param.paramValue, "ELTE11MWR_DEV_develop_20250808222826_NG");
    }
    std::remove("/tmp/currently_running_image_name");
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_FirmwareDownloadStatus) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_FirmwareDownloadURL) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler) {
        int ret = reqHandler->handleGetMsg(&param);
         cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_FirmwareDownloadProtocol) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
        cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
        EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_FirmwareDownloadPercent) {
    write_on_file("/opt/curl_progress", "Download is completed");
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadPercent", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);

    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler) {
        int ret = reqHandler->handleGetMsg(&param);
        cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
        EXPECT_EQ(ret, OK);
     }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_FirmwareUpdateState) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareUpdateState", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
      param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler) {
        int ret = reqHandler->handleGetMsg(&param);
         cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
        EXPECT_EQ(ret, OK);
     }

}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_NumberOfEntries) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.ProcessorNumberOfEntries", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler) {
        int ret = reqHandler->handleGetMsg(&param);
        cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
        EXPECT_EQ(ret, OK);
     }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_MemoryStatusTotal) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.MemoryStatus.Total", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler) {
      int ret = reqHandler->handleGetMsg(&param);
      cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
      EXPECT_EQ(ret, OK);
    }
}


TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_MemoryStatusFree) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.MemoryStatus.Free", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler) {
    int ret = reqHandler->handleGetMsg(&param);
    cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
    EXPECT_EQ(ret, OK);}
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_IPRemoteSupportMACaddress) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.MACAddr", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler) {
         int ret = reqHandler->handleGetMsg(&param);
         cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
         EXPECT_EQ(ret, OK);
         EXPECT_STREQ(param.paramValue, "unknown");
    }
}


TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_Action) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_XRPolling.Action", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler)
    {
    int ret = reqHandler->handleGetMsg(&param);
    cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
    EXPECT_EQ(ret, OK);
    }
}


TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_IPRemoteSupport) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler) {
      int ret = reqHandler->handleGetMsg(&param);
      cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
      EXPECT_EQ(ret, OK);
      EXPECT_STREQ(param.paramValue, "true");
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleGetMsg_PartnerId) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler) {
    int ret = reqHandler->handleGetMsg(&param);
    cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
    EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DeviceClientReqHandler_handleSetAttributesMsg) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Telemetry.ConfigURL", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    strncpy(param.paramValue, "https://xconf.xdp.eu-1.xcal.tv", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    DeviceClientReqHandler* reqHandler = static_cast<DeviceClientReqHandler*>(DeviceClientReqHandler::getInstance());
    if(reqHandler) {
      int ret = reqHandler->handleSetAttributesMsg(&param);
      cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
      EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, EthernetClientReqHandler_handleSetMsg_Enable) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
            int ret = reqHandler->handleSetMsg(&param);
            cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
            EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_Enable) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
      int ret = reqHandler->handleGetMsg(&param);
      cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
      EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_Status) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Status", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
      int ret = reqHandler->handleGetMsg(&param);
      cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
      EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_Name) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Name", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
      int ret = reqHandler->handleGetMsg(&param);
      cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
      EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_LastChange) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.LastChange", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
      int ret = reqHandler->handleGetMsg(&param);
      cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
      EXPECT_EQ(ret, NOK);
    }
}


TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_Upstream) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Upstream", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
      int ret = reqHandler->handleGetMsg(&param);
      cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
      EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_MACAddress) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.MACAddress", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) { 
      int ret = reqHandler->handleGetMsg(&param);             
      cout << "msgData.paramValue = " <<  param.paramValue <<std::endl; 
      EXPECT_EQ(ret, OK);
    }
}


TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_MaxBitRate) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.MaxBitRate", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {     
	    int ret = reqHandler->handleGetMsg(&param); 
	    cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;      
	    EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_DuplexMode) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.DuplexMode", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {                     
	    int ret = reqHandler->handleGetMsg(&param);   
	    cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
	    EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_BytesSent) {                                                                  
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Stats.BytesSent", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;           
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
	    int ret = reqHandler->handleGetMsg(&param);
	    cout << "msgData.paramValue = " <<  param.paramValue <<std::endl; 
	    EXPECT_EQ(ret, OK);
    }
}


TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_BytesReceived) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Stats.BytesReceived", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_PacketsReceived) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Stats.PacketsReceived", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_UnicastPacketsSent) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Stats.UnicastPacketsSent", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}


TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_UnicastPacketsReceived) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Stats.UnicastPacketsReceived", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_DiscardPacketsSent) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Stats.DiscardPacketsSent", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}


TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_DiscardPacketsReceived) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Stats.DiscardPacketsReceived", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_MulticastPacketsSent) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Stats.MulticastPacketsSent", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}


TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_MulticastPacketsReceived) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Stats.MulticastPacketsReceived", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_BroadcastPacketsSent) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Stats.BroadcastPacketsSent", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}


TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_BroadcastPacketsReceived) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Stats.BroadcastPacketsReceived", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, EthernetClientReqHandler_handleGetMsg_UnknownProtoPacketsReceived) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Stats.UnknownProtoPacketsReceived", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DHCPv4ClientReqHandler_handleGetMsg) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    EthernetClientReqHandler* reqHandler = static_cast<EthernetClientReqHandler*>(EthernetClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleSetAttributesMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}


TEST(handlersTest, IPClientReqHandler_handleGetMsg) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.IP.ActivePort.1.LocalPort", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    IPClientReqHandler* reqHandler = static_cast<IPClientReqHandler*>(IPClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, IPClientReqHandler_handleSetMsg) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.IP.IPv4Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    IPClientReqHandler* reqHandler = static_cast<IPClientReqHandler*>(IPClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleSetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, -2);
    }
}

TEST(handlersTest, DHCPv4ClientReqHandler_handleSetMsg_Interface) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DHCPv4..Interface", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    DHCPv4ClientReqHandler* reqHandler = static_cast<DHCPv4ClientReqHandler*>(DHCPv4ClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, NOK);
    }
}

TEST(handlersTest, DHCPv4ClientReqHandler_handleGetMsg_Interface) {    
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DHCPv4.Client.Interface", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;        
    
    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    DHCPv4ClientReqHandler* reqHandler = static_cast<DHCPv4ClientReqHandler*>(DHCPv4ClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, NOK);
    }
}


TEST(handlersTest, DHCPv4ClientReqHandler_handleGetMsg_IPRouters) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DHCPv4.Client.IPRouters", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    DHCPv4ClientReqHandler* reqHandler = static_cast<DHCPv4ClientReqHandler*>(DHCPv4ClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, NOK);
    }
}

TEST(handlersTest, DHCPv4ClientReqHandler_handleGetMsg_DNSServers) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DHCPv4.Client.DNSServers", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    DHCPv4ClientReqHandler* reqHandler = static_cast<DHCPv4ClientReqHandler*>(DHCPv4ClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, NOK);
    }
}

TEST(handlersTest, DHCPv4ClientReqHandler_handleSetAttributesMsg) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DHCPv4.Client.DNSServers", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);
    DHCPv4ClientReqHandler* reqHandler = static_cast<DHCPv4ClientReqHandler*>(DHCPv4ClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleSetAttributesMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
       EXPECT_EQ(ret, OK);
    }
}

TEST(handlersTest, DHCPv4ClientReqHandler_handleGetAttributesMsg) {                                                                      
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DHCPv4.Client.DNSServers", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    DHCPv4ClientReqHandler* reqHandler = static_cast<DHCPv4ClientReqHandler*>(DHCPv4ClientReqHandler::getInstance());
    if(reqHandler) {
       int ret = reqHandler->handleGetAttributesMsg(&param);
       cout << "msgData.paramValue = " <<  param.paramValue <<std::endl;
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
    return RUN_ALL_TESTS();
}
