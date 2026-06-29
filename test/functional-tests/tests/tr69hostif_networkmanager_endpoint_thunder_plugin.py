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


import pytest
from time import sleep

from helper_functions import *


@pytest.mark.run(order=57)
def test_ThunderPlugin_WiFi_EndPoint_SignalStrength_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.WiFi.EndPoint.1.Stats.SignalStrength"
    WIFI_SIGNAL_STRENGTH_MSG = "67"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"
    SIGNAL_STRENGTH_MSG = "Stats.SignalStrength = [67]"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert SIGNAL_STRENGTH_MSG in grep_tr69hostiflogs(SIGNAL_STRENGTH_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert WIFI_SIGNAL_STRENGTH_MSG in rstdout

@pytest.mark.run(order=58)
def test_ThunderPlugin_WiFi_EndPoint_Security_ModesEnabled_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.WiFi.EndPoint.1.Security.ModesEnabled"
    WIFI_MODES_ENABLED_MSG = "1"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"
    SECURITY_MODE_MSG = "WiFi Security Mode : 1"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert SECURITY_MODE_MSG in grep_tr69hostiflogs(SECURITY_MODE_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert WIFI_MODES_ENABLED_MSG in rstdout

@pytest.mark.run(order=59)
def test_ThunderPlugin_EndPoint_Status_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.WiFi.EndPoint.1.Status"
    STATUS_MSG = "Enabled"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATUS_MSG in rstdout

@pytest.mark.run(order=60)
def test_ThunderPlugin_EndPoint_Enable_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.WiFi.EndPoint.1.Enable"
    WIFI_ENDPOINT_ENABLE_MSG = "true"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert WIFI_ENDPOINT_ENABLE_MSG in rstdout

@pytest.mark.run(order=61)
def test_ThunderPlugin_WiFiEnable_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.WiFi.X_RDKCENTRAL-COM_WiFiEnable"
    WIFI_ENABLE_MSG = "false"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"

    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", WIFI_ENABLE_MSG)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=62)
def test_ThunderPlugin_EndPoint_Disable_Status_Get_Handler():
    #clear_tr69hostiflogs()
    sleep(2)
    DATA_ELEMENT_NAME = "Device.WiFi.EndPoint.1.Status"
    STATUS_MSG = "Disabled"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"
    ENDPOINT_DISABLED_MSG = "EndPoint is disabled"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert ENDPOINT_DISABLED_MSG in grep_tr69hostiflogs(ENDPOINT_DISABLED_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATUS_MSG in rstdout

@pytest.mark.run(order=63)
def test_ThunderPlugin_WiFiEnable_Restore_Set_Handler():
    # Cleanup: restore WiFi state for any subsequent tests
    DATA_ELEMENT_NAME = "Device.WiFi.X_RDKCENTRAL-COM_WiFiEnable"
    WIFI_ENABLE_MSG = "true"

    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", WIFI_ENABLE_MSG)
    assert RBUS_SUCCESS_STRING in rstdout

