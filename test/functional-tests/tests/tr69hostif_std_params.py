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

@pytest.mark.run(order=202)
def test_DeviceInfo_ModelName_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.ModelName"
    VALUE = "DOCKER"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=203)
def test_DeviceInfo_Description_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.Description"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=204)
def test_DeviceInfo_ProductClass_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.ProductClass"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=205)
def test_DeviceInfo_SoftwareVersion_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.SoftwareVersion"
    VALUE  = "99.99.15.07"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout
    
@pytest.mark.run(order=206)
def test_Device_DeviceInfo_ProvisioningCode_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.ProvisioningCode"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=207)
def test_DeviceInfo_UpTime_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.UpTime"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=208)
def test_DeviceInfo_NumberOfEntries_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.ProcessorNumberOfEntries"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    
@pytest.mark.run(order=209)
def test_DeviceInfo_MemoryStatus_Total_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.MemoryStatus.Total"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=210)
def test_DeviceInfo_MemoryStatus_Free_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.MemoryStatus.Free"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


