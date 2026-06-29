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

@pytest.mark.run(order=67)
def test_ThunderPlugin_ReverseSSH_Trigger_Set_Handler():
    #clear_tr69hostiflogs()

    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger"
    REVERSESSH_TRIGGER_MSG = "SHARE"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"
    PRIVACY_MODE_MSG = "PrivacyMode is SHARE"

    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", REVERSESSH_TRIGGER_MSG)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert PRIVACY_MODE_MSG in grep_tr69hostiflogs(PRIVACY_MODE_MSG)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=68)
def test_ThunderPlugin_STB_IP_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_COMCAST-COM_STB_IP"
    STB_IP_MSG = "192.168.1.100"
    CURL_RESPONSE_MSG = "curl response : 0 http response code: 200"
    IP_MSG = "successfully fetched ipaddress from NetworkManager"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert CURL_RESPONSE_MSG in grep_tr69hostiflogs(CURL_RESPONSE_MSG)
    assert IP_MSG in grep_tr69hostiflogs(IP_MSG)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STB_IP_MSG in rstdout

