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

@pytest.mark.run(order=81)
def test_RFC_ClearDB_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDB"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=82)
def test_RFC_ClearDBEnd_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDBEnd"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING not in rstdout


@pytest.mark.run(order=83)
def test_RFC_RetrieveNow_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.RetrieveNow"
    VALUE = "100"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "uint", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=84)
def test_RFC_RoamTrigger_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RoamTrigger"
    VALUE = "triggered"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=85)
def test_RFC_DAPv2_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MS12.DAPv2_Enable"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=86)
def test_RFC_MS12_DE_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MS12.DE_Enable"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=87)
def test_RFC_LoudnessEquivalence_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LoudnessEquivalence.Enable"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=88)
def test_RFC_DAB_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DAB.Enable"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=89)
def test_RFC_AutoReboot_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AutoReboot.Enable"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=90)
def test_RFC_RebootStop_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Enable"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=91)
def test_RFC_wakeUpStart_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpStart"
    VALUE = "100"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "int", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=92)
def test_RFC_wakeUpEnd_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpEnd"
    VALUE = "100"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "int", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


