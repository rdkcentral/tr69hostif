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

@pytest.mark.run(order=65)
def test_ThunderPlugin_HotelCheckout_LastResetTime_Get_Handler():
    #clear_tr69hostiflogs()

    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xAccount.HotelCheckout.LastResetTime"
    RESET_TIME_MSG = "1717000000"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert RESET_TIME_MSG in rstdout


@pytest.mark.run(order=66)
def test_ThunderPlugin_HotelCheckout_Status_Get_Handler():
    #clear_tr69hostiflogs()

    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xAccount.HotelCheckout.Status"
    STATUS_MSG = "success"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATUS_MSG in rstdout

