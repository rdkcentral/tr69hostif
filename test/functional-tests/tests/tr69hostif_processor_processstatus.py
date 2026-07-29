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

@pytest.mark.run(order=526)
def test_Processor_Architecture_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.Processor.1.Architecture"
    VALUE = "x86_64"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=527)
def test_ProcessStatus_PID_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.ProcessStatus.Process.1.PID"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=528)
def test_ProcessStatus_Command_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.ProcessStatus.Process.1.Command"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=529)
def test_ProcessStatus_Size_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.ProcessStatus.Process.1.Size"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    
@pytest.mark.run(order=530)
def test_ProcessStatus_Priority_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.ProcessStatus.Process.1.Priority"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=531)
def test_ProcessStatus_CPUTime_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.ProcessStatus.Process.1.CPUTime"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=532)
def test_ProcessStatus_State_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.ProcessStatus.Process.1.State"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    
@pytest.mark.run(order=533)
def test_ProcessStatus_NumberOfEntries_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.ProcessStatus.ProcessNumberOfEntries"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=534)
def test_ProcessStatus_CPUUsage_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.ProcessStatus.CPUUsage"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

