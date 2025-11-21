####################################################################################
# If not stated otherwise in this file or this component's Licenses file the
# following copyright and licenses apply:
#
# Copyright 2024 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
####################################################################################


import subprocess
import pytest
from time import sleep

from helper_functions import *

@pytest.mark.run(order=19)
def test_DeviceInfo_Set_Get_Handler():
    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Telemetry.Version"
    VERSION_MSG = "2.2.1"
    rbus_set_data(DATA_ELEMENT_NAME, "string", VERSION_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VERSION_MSG in rstdout

@pytest.mark.run(order=20)
def test_DeviceTime_Set_Get_Handler():
    #clear_T2logs()

    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DHCPv6Client.Enable"
    SERVER_ELEMENT_NAME = "Device.Time.NTPServer1"

    STATE_MSG = "true"
    SERVER_IP = "3.236.252.118"

    rbus_set_data(DATA_ELEMENT_NAME, "boolean", STATE_MSG)
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATE_MSG in rstdout

    rbus_set_data(SERVER_ELEMENT_NAME, "string", SERVER_IP)
    rstdout = rbus_get_data(SERVER_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert SERVER_IP in rstdout


@pytest.mark.run(order=21)
def test_RFC_Set_Get_Handler():
    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.HdmiCecSink.CECVersion"
    VERSION_MSG = "1.4"
    rbus_set_data(DATA_ELEMENT_NAME, "string", VERSION_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VERSION_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.Enable"
    STATE_MSG = "true"
    rbus_set_data(DATA_ELEMENT_NAME, "boolean", STATE_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATE_MSG in rstdout

     #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.TopSpeed"
    SPEED_LIMIT_MSG = "1280000"
    rbus_set_data(DATA_ELEMENT_NAME, "int", SPEED_LIMIT_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert SPEED_LIMIT_MSG in rstdout
    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.eMMCFirmware.Version"
    VERSION_MSG = "08140310"
    rbus_set_data(DATA_ELEMENT_NAME, "string", VERSION_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VERSION_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IncrementalCDL.Enable"
    CDL_STATE_MSG = "true"
    rbus_set_data(DATA_ELEMENT_NAME, "boolean", CDL_STATE_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert CDL_STATE_MSG in rstdout

@pytest.mark.run(order=22)
def test_NonRFC_Set_Get_Handler():
    #clear_T2logs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable"
    IPRS_STATE_MSG = "false"
    rbus_set_data(DATA_ELEMENT_NAME, "boolean", IPRS_STATE_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert IPRS_STATE_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable"
    SSH_STATE_MSG = "false"
    rbus_set_data(DATA_ELEMENT_NAME, "boolean", SSH_STATE_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert SSH_STATE_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot"
    FWDLRBT_STATE_MSG = "false"
    rbus_set_data(DATA_ELEMENT_NAME, "boolean", FWDLRBT_STATE_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert FWDLRBT_STATE_MSG in rstdout

    #clear_T2logs()
       DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadCompletedNotification"
    FWDLNOTIF_STATE_MSG = "false"
    rbus_set_data(DATA_ELEMENT_NAME, "boolean", FWDLNOTIF_STATE_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert FWDLNOTIF_STATE_MSG in rstdout

@pytest.mark.run(order=23)
def test_Bootstrap_Set_Get_Handler():
    #clear_T2logs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerProductName"
    PRODUCT_NAME_MSG = "TestProduct123"
    rbus_set_data(DATA_ELEMENT_NAME, "string", PRODUCT_NAME_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert PRODUCT_NAME_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.NetflixESNprefix"
    ESN_PREFIX_MSG = "TESTESN"
    rbus_set_data(DATA_ELEMENT_NAME, "string", ESN_PREFIX_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert ESN_PREFIX_MSG in rstdout

     #clear_T2logs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerName"
    PARTNER_NAME_MSG = "Test"
    rbus_set_data(DATA_ELEMENT_NAME, "string", PARTNER_NAME_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert PARTNER_NAME_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.SsrUrl"
    SSR_URL_MSG = "https://ssr.test.tv"
    rbus_set_data(DATA_ELEMENT_NAME, "string", SSR_URL_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert SSR_URL_MSG in rstdout

@pytest.mark.run(order=24)
def test_Bootstrap_File_Creation():
    import os

    sleep(5)

    #  Verify bootstrap files exist
    assert os.path.exists("/opt/secure/RFC/bootstrap.ini"), \
        "bootstrap.ini file was not created"
    assert os.path.exists("/opt/secure/RFC/bootstrap.journal"), \
        "bootstrap.journal file was not created"

    # Test that bootstrap parameters can be set via RBUS
    TEST_PARAM = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerProductName"
    TEST_VALUE = "TestProduct123"

    # Set using standard L2 pattern
    rbus_set_data(TEST_PARAM, "string", TEST_VALUE)

    result = rbus_get_data(TEST_PARAM)
    assert RBUS_EXCEPTION_STRING not in result, \
        f"RBUS error occurred: {result}"
    assert TEST_VALUE in result, \
        f"Expected value '{TEST_VALUE}' not found in result: {result}"
    sleep(2)

    # Verify the parameter was written to bootstrap.ini
    with open("/opt/secure/RFC/bootstrap.ini", "r") as f:
        content = f.read()
         assert TEST_VALUE in content, \
            f"Bootstrap parameter not found in file. Content:\n{content}"


                                               
