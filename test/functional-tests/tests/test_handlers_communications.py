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
import os
import pytest
from time import sleep

from helper_functions import *

@pytest.mark.run(order=19)
def test_DeviceInfo_Set_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Telemetry.Version"
    VERSION_MSG = "2.2.1"
    rbus_set_data(DATA_ELEMENT_NAME, "string", VERSION_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VERSION_MSG in rstdout

@pytest.mark.run(order=20)
def test_DeviceTime_Set_Get_Handler():
    #clear_tr69hostiflogs()

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
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.HdmiCecSink.CECVersion"
    VERSION_MSG = "1.4"
    rbus_set_data(DATA_ELEMENT_NAME, "string", VERSION_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VERSION_MSG in rstdout

    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.Enable"
    STATE_MSG = "true"
    rbus_set_data(DATA_ELEMENT_NAME, "boolean", STATE_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATE_MSG in rstdout

     #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.TopSpeed"
    SPEED_LIMIT_MSG = "1280000"
    rbus_set_data(DATA_ELEMENT_NAME, "int", SPEED_LIMIT_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert SPEED_LIMIT_MSG in rstdout
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.eMMCFirmware.Version"
    VERSION_MSG = "08140310"
    rbus_set_data(DATA_ELEMENT_NAME, "string", VERSION_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VERSION_MSG in rstdout

    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IncrementalCDL.Enable"
    CDL_STATE_MSG = "true"
    rbus_set_data(DATA_ELEMENT_NAME, "boolean", CDL_STATE_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert CDL_STATE_MSG in rstdout

@pytest.mark.run(order=22)
def test_NonRFC_Set_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable"
    IPRS_STATE_MSG = "false"
    rbus_set_data(DATA_ELEMENT_NAME, "boolean", IPRS_STATE_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert IPRS_STATE_MSG in rstdout

    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable"
    SSH_STATE_MSG = "false"
    rbus_set_data(DATA_ELEMENT_NAME, "boolean", SSH_STATE_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert SSH_STATE_MSG in rstdout

    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot"
    FWDLRBT_STATE_MSG = "false"
    rbus_set_data(DATA_ELEMENT_NAME, "boolean", FWDLRBT_STATE_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert FWDLRBT_STATE_MSG in rstdout

    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadCompletedNotification"
    FWDLNOTIF_STATE_MSG = "false"
    rbus_set_data(DATA_ELEMENT_NAME, "boolean", FWDLNOTIF_STATE_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert FWDLNOTIF_STATE_MSG in rstdout

@pytest.mark.run(order=23)
def test_Bootstrap_Set_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerProductName"
    PRODUCT_NAME_MSG = "TestProduct123"
    rbus_set_data(DATA_ELEMENT_NAME, "string", PRODUCT_NAME_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert PRODUCT_NAME_MSG in rstdout

    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.NetflixESNprefix"
    ESN_PREFIX_MSG = "TESTESN"
    rbus_set_data(DATA_ELEMENT_NAME, "string", ESN_PREFIX_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert ESN_PREFIX_MSG in rstdout

     #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerName"
    PARTNER_NAME_MSG = "Test"
    rbus_set_data(DATA_ELEMENT_NAME, "string", PARTNER_NAME_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert PARTNER_NAME_MSG in rstdout

    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME ="Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.SsrUrl"
    SSR_URL_MSG = "https://ssr.test.tv"
    rbus_set_data(DATA_ELEMENT_NAME, "string", SSR_URL_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert SSR_URL_MSG in rstdout

@pytest.mark.run(order=24)
def test_Bootstrap_File_Creation():

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


                                               


@pytest.mark.run(order=25)
def test_Chrony_Enable_Set_Get_Handler():

    ENABLE_PARAM  = "Device.Time.Chrony.Enable"
    ENABLE_VALUE  = "true"
    CHRONY_FILE   = "/opt/secure/RFC/chrony/chronyd_enabled"

    try:
        os.remove(CHRONY_FILE)
    except FileNotFoundError:
        pass

    rbus_set_data(ENABLE_PARAM, "boolean", ENABLE_VALUE)
    rstdout = rbus_get_data(ENABLE_PARAM)
    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"RBUS error on GET after SET: {rstdout}"
    assert ENABLE_VALUE in rstdout, \
        f"Expected '{ENABLE_VALUE}' in GET result, got: {rstdout}"

    assert os.path.exists(CHRONY_FILE), \
        "chronyd_enabled file was not created after setting Chrony.Enable=true"

    try:
        os.remove(CHRONY_FILE)
    except FileNotFoundError:
        pass


@pytest.mark.run(order=26)
def test_Chrony_Makestep_Set_Get_Handler():

    MAKESTEP_PARAM = "Device.Time.Chrony.Makestep"
    MAKESTEP_VALUE = "1.0,3"
    MAKESTEP_FILE  = "/opt/secure/RFC/chrony/ntp_maxstep"

    # Remove any stale file so we know the SET actually created it
    try:
        os.remove(MAKESTEP_FILE)
    except FileNotFoundError:
        pass

    rbus_set_data(MAKESTEP_PARAM, "string", MAKESTEP_VALUE)
    rstdout = rbus_get_data(MAKESTEP_PARAM)
    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"RBUS error on GET after SET: {rstdout}"
    assert MAKESTEP_VALUE in rstdout, \
        f"Expected '{MAKESTEP_VALUE}' in GET result, got: {rstdout}"

    assert os.path.exists(MAKESTEP_FILE), \
        "ntp_maxstep file was not created after setting Chrony.Makestep"

    with open(MAKESTEP_FILE, "r") as f:
        content = f.read().strip()
        assert content == MAKESTEP_VALUE, \
            f"File content '{content}' does not match expected '{MAKESTEP_VALUE}'"

    try:
        os.remove(MAKESTEP_FILE)
    except FileNotFoundError:
        pass


@pytest.mark.run(order=27)
def test_Chrony_NTPServerSettings_Set_Get_Handler():

    SETTINGS_PARAM = "Device.Time.Chrony.NTPServer.1.Settings"
    SETTINGS_VALUE = "server,0,true,6,12"
    SETTINGS_FILE  = "/opt/secure/RFC/chrony/ntp_server1_settings"

    # Remove any stale file so we know the SET actually wrote a new value
    try:
        os.remove(SETTINGS_FILE)
    except FileNotFoundError:
        pass

    rbus_set_data(SETTINGS_PARAM, "string", SETTINGS_VALUE)
    rstdout = rbus_get_data(SETTINGS_PARAM)
    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"RBUS error on GET after SET: {rstdout}"
    assert SETTINGS_VALUE in rstdout, \
        f"Expected '{SETTINGS_VALUE}' in GET result, got: {rstdout}"

    assert os.path.exists(SETTINGS_FILE), \
        "ntp_server1_settings file was not created"

    with open(SETTINGS_FILE, "r") as f:
        content = f.read().strip()
        assert content == SETTINGS_VALUE, \
            f"File content '{content}' does not exactly match expected '{SETTINGS_VALUE}'"

    try:
        os.remove(SETTINGS_FILE)
    except FileNotFoundError:
        pass


@pytest.mark.run(order=28)
def test_Chrony_NTPServerSettings_Default_On_Missing_File():

    SETTINGS_PARAM = "Device.Time.Chrony.NTPServer.2.Settings"
    DEFAULT_VALUE  = "pool,4,true,10,12"
    SETTINGS_FILE  = "/opt/secure/RFC/chrony/ntp_server2_settings"

    try:
        os.remove(SETTINGS_FILE)
    except FileNotFoundError:
        pass

    rstdout = rbus_get_data(SETTINGS_PARAM)
    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"RBUS error: {rstdout}"
    assert DEFAULT_VALUE in rstdout, \
        f"Expected factory default '{DEFAULT_VALUE}' when file absent, got: {rstdout}"
