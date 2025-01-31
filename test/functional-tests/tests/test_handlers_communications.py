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

@pytest.mark.run(order=10)
def test_DeviceInfo_Set_Get_Handler():
    #clear_T2logs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Telemetry.Version"
    VERSION_MSG = "2.2.1"
    rbus_set_data(DATA_ELEMENT_NAME, "string", VERSION_MSG)
    # Force reload config fetch from xconf
    sleep(10)
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VERSION_MSG in rstdout

@pytest.mark.run(order=11)
def test_DeviceTime_Set_Get_Handler():
    #clear_T2logs()
    
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.newNTP.Enable"
    SERVER_ELEMENT_NAME = "Device.Time.NTPServer1"
    
    STATE_MSG = "true"
    SERVER_IP = "3.236.252.118"
    
    rbus_set_data(DATA_ELEMENT_NAME, "string", STATE_MSG)
    sleep(10)
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATE_MSG in rstdout
    
    #rbus_set_data(SERVER_ELEMENT_NAME, "string", SERVER_IP)
    #sleep(10)
    #rstdout = rbus_get_data(SERVER_ELEMENT_NAME)
    #assert RBUS_EXCEPTION_STRING not in rstdout
    #assert SERVER_IP in rstdout
