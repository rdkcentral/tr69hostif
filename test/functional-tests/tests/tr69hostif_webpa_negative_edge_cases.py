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
import pytest

from helper_functions import *

@pytest.mark.run(order=97)
def test_NegEdge_WebPA_Malformed_JSON_Missing_Brace():
    print("Starting parodus mock process - Malformed WebPA JSON missing brace")
    payload = '{"command":"GET","names":["Device.DeviceInfo.ModelName"]'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":520'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    INVALID_INPUT_STATUS_MSG = '"message":"Invalid Input Command"'
    assert INVALID_INPUT_STATUS_MSG in grep_paroduslogs(INVALID_INPUT_STATUS_MSG)


@pytest.mark.run(order=98)
def test_NegEdge_WebPA_Malformed_JSON_Unquoted_Key():
    print("Starting parodus mock process - Malformed WebPA JSON unquoted key")
    payload = '{command:"GET","names":["Device.DeviceInfo.ModelName"]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":520'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    INVALID_INPUT_STATUS_MSG = '"message":"Invalid Input Command"'
    assert INVALID_INPUT_STATUS_MSG in grep_paroduslogs(INVALID_INPUT_STATUS_MSG)



@pytest.mark.run(order=99)
def test_NegEdge_WebPA_SET_Wrong_DataType():
    print("Starting parodus mock process - WebPA SET wrong data type")
    payload = '{"command":"SET","parameters":[{"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.XconfUrl","dataType":1,"value":"123"}]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":520'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    INVALID_PARAMETER_VALUE_MSG = '"Invalid parameter value"'
    assert INVALID_PARAMETER_VALUE_MSG in grep_paroduslogs(INVALID_PARAMETER_VALUE_MSG)


@pytest.mark.run(order=100)
def test_Negative_WebPA_Malformed_JSON_Random_Text():
    print("Starting parodus mock process - WebPA SET wrong data type")
    payload = 'not a json payload'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":520'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    INVALID_PARAMETER_VALUE_MSG = '"Invalid Input Command"'
    assert INVALID_PARAMETER_VALUE_MSG in grep_paroduslogs(INVALID_PARAMETER_VALUE_MSG)


@pytest.mark.run(order=101)
def test_WebPA_GetAttributes_Wildcard_Rejected():
    print("Starting parodus mock process - GET_ATTRIBUTES wildcard")
    payload = '{"command":"GET_ATTRIBUTES","attributes":"notify","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit."]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    WILDCARD_MSG = "Wildcard is not supported"
    assert WILDCARD_MSG in grep_paroduslogs(WILDCARD_MSG)

    STATUS_CODE_MSG = '"statusCode":552'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

@pytest.mark.run(order=102)
def test_WebPA_GetAttributes_Notify_Handler():
    print("Starting parodus mock process - GET_ATTRIBUTES")
    payload = '{"command":"GET_ATTRIBUTES","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.Enable"]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":200'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    SUCCESS_STATUS_MSG = '"message":"Success"'
    assert SUCCESS_STATUS_MSG in grep_paroduslogs(SUCCESS_STATUS_MSG)

    NAME_MSG = '"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.Enable"'
    assert NAME_MSG in grep_paroduslogs(NAME_MSG)


