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

@pytest.mark.run(order=127)
def test_DeviceTime_Enable_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.Enable"
    STATUS_MSG = "false"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATUS_MSG in rstdout

@pytest.mark.run(order=128)
def test_DeviceTime_Status_Get_Handler():
    #clear_tr69hostiflogs()

    DATA_ELEMENT_NAME = "Device.Time.Status"
    IF_NAME = "eth0"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=129)
def test_DeviceTime_NTPServer1_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.NTPServer1"
    VALUE = "test.com"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=130)
def test_DeviceTime_NTPServer1_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.NTPServer1"
    VALUE = "test.com"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout


@pytest.mark.run(order=131)
def test_DeviceTime_NTPServer2_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.NTPServer2"
    VALUE = "test1.com"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=132)
def test_DeviceTime_NTPServer2_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.NTPServer2"
    VALUE = "test1.com"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=133)
def test_DeviceTime_NTPServer3_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.NTPServer3"
    VALUE = "test2.com"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=134)
def test_DeviceTime_NTPServer3_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.NTPServer3"
    VALUE = "test2.com"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=135)
def test_DeviceTime_NTPServer4_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.NTPServer4"
    VALUE = "test3.com"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=136)
def test_DeviceTime_NTPServer4_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.NTPServer4"
    VALUE = "test3.com"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout


@pytest.mark.run(order=137)
def test_DeviceTime_NTPServer5_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.NTPServer5"
    VALUE = "test4.com"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=138)
def test_DeviceTime_NTPServer5_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.NTPServer5"
    VALUE = "test4.com"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout


@pytest.mark.run(order=139)
def test_DeviceTime_LocalTime_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.CurrentLocalTime"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=140)
def test_DeviceTime_TimeZone_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.LocalTimeZone"
    TIMEZONE = "UTC"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert TIMEZONE in rstdout

@pytest.mark.run(order=141)
def test_DeviceTime_TimeZone_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.LocalTimeZone"
    VALUE = "UTC"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=142)
def test_DeviceTime_UTCTIME_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.X_RDK_CurrentUTCTime"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=143)
def test_DeviceTime_Enable_SetTrue_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.Enable"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", "true")
    assert RBUS_SET_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=144)
def test_DeviceTime_Enable_GetFalse_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.Enable"
    STATUS_MSG = "false"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATUS_MSG in rstdout

@pytest.mark.run(order=145)
def test_DeviceTime_Enable_SetFalse_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Time.Enable"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", "false")
    assert RBUS_SET_EXCEPTION_STRING in rstdout
