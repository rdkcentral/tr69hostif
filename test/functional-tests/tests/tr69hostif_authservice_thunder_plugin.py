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

@pytest.mark.run(order=46)
def test_ThunderPlugin_EXPERIENCE_Get_Handler():
    #clear_tr69hostiflogs()

    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_Experience"
    EXP_MSG = "TESTOS"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert EXP_MSG in rstdout

@pytest.mark.run(order=47)
def test_ThunderPlugin_AccountID_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID"
    ACCOUNT_ID_MSG = "123456789"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert ACCOUNT_ID_MSG in rstdout

@pytest.mark.run(order=48)
def test_ThunderPlugin_SyndicationPartnerId_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId"
    PARTNER_ID_MSG = "testpartner"
    SUCCESS_MSG = "PartnerID uploaded using AuthService plugin call success"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"

    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", PARTNER_ID_MSG)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert RBUS_SUCCESS_STRING in rstdout
    assert SUCCESS_MSG in grep_tr69hostiflogs(SUCCESS_MSG)

