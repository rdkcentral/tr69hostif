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
from tr69hostif_http_server import http_request

@pytest.mark.run(order=93)
def test_Negative_HTTP_SET_Wrong_DataType_StringParam_With_IntegerType():
    #clear_tr69hostiflogs()

    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.XconfUrl"
    PARAMETER_MISMATCH_MSG = "Parameter type mismatch! Given = 1 vs DataModel = 0"

    rbus_set_data(DATA_ELEMENT_NAME, "int", 123)
    assert PARAMETER_MISMATCH_MSG in grep_tr69hostiflogs(PARAMETER_MISMATCH_MSG)


@pytest.mark.run(order=94)
def test_Negative_HTTP_SET_Wrong_DataType_Boolean_As_String():
    #clear_tr69hostiflogs()

    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.AutoExcluded.Enable"
    PARAMETER_VALUE = "not_bool"    
    PARAMETER_MISMATCH_MSG = "Parameter type mismatch! Given = 0 vs DataModel = 3"

    rbus_set_data(DATA_ELEMENT_NAME, "string", PARAMETER_VALUE)
    assert PARAMETER_MISMATCH_MSG in grep_tr69hostiflogs(PARAMETER_MISMATCH_MSG)

@pytest.mark.run(order=95)
def test_Negative_HTTP_SET_OutOfRange_Integer_High_Value():
    #clear_tr69hostiflogs()

    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.LowSpeed"
    INVALID_VALUE_MSG = "Invalid data value passed to set. Please pass proper value with respect to the data type"
    
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "int", 9999999999)    
    assert INVALID_VALUE_MSG in rstdout


@pytest.mark.run(order=96)
def test_Negative_HTTP_SET_OutOfRange_Integer_Negative_Value():
    #clear_tr69hostiflogs()

    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.LowSpeed"
    INVALID_VALUE_MSG = "Invalid data value passed to set. Please pass proper value with respect to the data type"

    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "int", -2147483649)
    assert INVALID_VALUE_MSG in rstdout


