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

from helper_functions import *

@pytest.mark.run(order=49)
def test_ThunderPlugin_WiFi_SSID_SSID_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.WiFi.SSID.1.SSID"
    WIFI_SSID_MSG = "WiFi_2.4G"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert WIFI_SSID_MSG in rstdout


@pytest.mark.run(order=50)
def test_ThunderPlugin_WiFi_SSID_BSSID_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.WiFi.SSID.1.BSSID"
    WIFI_BSSID_MSG = "AA:BB:CC:DD:EE:FF"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert WIFI_BSSID_MSG in rstdout

@pytest.mark.run(order=51)
def test_ThunderPlugin_WiFi_SSID_Name_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.WiFi.SSID.1.Name"
    WIFI_NAME_MSG = "WiFi_2.4G"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert WIFI_NAME_MSG in rstdout

@pytest.mark.run(order=52)
def test_ThunderPlugin_WiFi_SSID_Enable_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.WiFi.SSID.1.Enable"
    WIFI_ENABLE_MSG = "true"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"
    ENABLE_MSG = "ENABLE = 1"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert ENABLE_MSG in grep_tr69hostiflogs(ENABLE_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert WIFI_ENABLE_MSG in rstdout

@pytest.mark.run(order=53)
def test_ThunderPlugin_WiFi_SSID_MACAddress_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.WiFi.SSID.1.MACAddress"
    WIFI_MAC_MSG = "AA:BB:CC:DD:EE:01"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"


    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert WIFI_MAC_MSG in rstdout

@pytest.mark.run(order=54)
def test_ThunderPlugin_WiFi_SSID_Status_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.WiFi.SSID.1.Status"
    WIFI_STATUS_MSG = "CONNECTED"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"
    STATUS_MSG = "STATUS = CONNECTED"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert STATUS_MSG in grep_tr69hostiflogs(STATUS_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert WIFI_STATUS_MSG in rstdout


@pytest.mark.run(order=55)
def test_ThunderPlugin_WiFiEnable_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.WiFi.X_RDKCENTRAL-COM_WiFiEnable"
    WIFI_ENABLE_MSG = "true"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert WIFI_ENABLE_MSG in rstdout

