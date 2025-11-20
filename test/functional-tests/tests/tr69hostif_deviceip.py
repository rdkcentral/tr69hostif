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

@pytest.mark.run(order=24)
def test_DeviceDefault_Set_Get_Handler():
    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.SoftwareVersion"
    VERSION_MSG = "99.99.15.07"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VERSION_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.ModelName"
    MODEL_NAME = "DOCKER"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert MODEL_NAME in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_COMCAST-COM_FirmwareFilename"
    FW_FILE_NAME = "Platform_Cotainer_1.0.0"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert FW_FILE_NAME in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MEMSWAP.Enable"
    STATUS_MSG = "true"
    rbus_set_data(DATA_ELEMENT_NAME, "string", STATUS_MSG)
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATUS_MSG in rstdout



@pytest.mark.run(order=25)
def test_DeviceIP_Set_Get_Handler():
    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Prefix.1.Autonomous"
    STATUS_MSG = "false"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATUS_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Address.1.Anycast"
    STATUS_MSG = "false"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATUS_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Address.1.Enable"
    STATUS_MSG = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATUS_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Prefix.1.StaticType"
    STATUS_MSG = "Inapplicable"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATUS_MSG in rstdout
   
    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6AddressNumberOfEntries"
    ENTRY_COUNT_MSG = "1"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert ENTRY_COUNT_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Address.1.Origin"
    ORIGIN_MSG = "WellKnown"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert ORIGIN_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv4Address.1.Enable"
    STATUS_MSG = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATUS_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Prefix.1.PrefixStatus"
    PREFIX_STATUS_MSG = "Preferred"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert PREFIX_STATUS_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Address.1.PreferredLifetime"
    PREF_LTF_STATUS_MSG = "0001-01-01T00:00:00Z"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert PREF_LTF_STATUS_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Enable"
    STATUS_MSG = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATUS_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Prefix.1.ValidLifetime"
    PREF_LTF_STATUS_MSG = "0001-01-01T00:00:00Z"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert PREF_LTF_STATUS_MSG in rstdout

@pytest.mark.run(order=26)
def test_DeviceServices_Set_Get_Handler():
    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.Services.STBServiceNumberOfEntries"
    STB_ENTRY_COUNT_MSG = "1"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STB_ENTRY_COUNT_MSG in rstdout

@pytest.mark.run(order=27)
def test_ReverseSSH_Set_Get_Handler():
    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshStatus"
    SSH_STATUS_MSG = "INACTIVE"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert SSH_STATUS_MSG in rstdout

    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger"
    SSH_TRG_MSG = "start shorts"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", SSH_TRG_MSG)
    # Force reload config fetch from xconf
    assert RBUS_SUCCESS_STRING in rstdout
    
    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs"
    SSH_ARGS_MSG = "host=skyfoxtel.xcal.tv;hostIp=skyfoxtel.xcal.tv;stunnelport=2009;idletimeout=300;revsshport=3008;sshport=2221;user=webpa_user01;"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", SSH_ARGS_MSG)
    # Force reload config fetch from xconf
    assert RBUS_SUCCESS_STRING in rstdout
    
