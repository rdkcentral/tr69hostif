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
from time import sleep

from helper_functions import *

@pytest.mark.run(order=19)
def test_WebPA_Set_XCONF_URL_Handler():
    print("Starting parodus mock process")
    payload = '{"command":"SET","parameters":[{"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.XconfUrl","dataType":0,"value":"https://rdkautotool.ccp.xcal.tv/featureControl/getSettings"}]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":200'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    SUCCESS_STATUS_MSG = '"message":"Success"'
    assert SUCCESS_STATUS_MSG in grep_paroduslogs(SUCCESS_STATUS_MSG)

@pytest.mark.run(order=20)
def test_WebPA_Get_XCONF_URL_Handler():
    print("Starting parodus mock process")
    payload ='{"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.XconfUrl"]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":200'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    SUCCESS_STATUS_MSG = '"message":"Success"'
    assert SUCCESS_STATUS_MSG in grep_paroduslogs(SUCCESS_STATUS_MSG)

    XCONF_URL_STATUS_MSG = '"value":"https://rdkautotool.ccp.xcal.tv/featureControl/getSettings"'
    assert XCONF_URL_STATUS_MSG in grep_paroduslogs(XCONF_URL_STATUS_MSG)

@pytest.mark.run(order=20)
def test_WebPA_Set_FWUPDATE_Handler():
    print("Starting parodus mock process")
    payload = '{"command":"SET","parameters":[{"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.AutoExcluded.Enable","dataType":3,"value":"false"}]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":200'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    SUCCESS_STATUS_MSG = '"message":"Success"'
    assert SUCCESS_STATUS_MSG in grep_paroduslogs(SUCCESS_STATUS_MSG)

@pytest.mark.run(order=21)
def test_WebPA_Get_FWUPDATE_Handler():
    print("Starting parodus mock process")
    payload ='{"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.AutoExcluded.Enable"]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":200'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    SUCCESS_STATUS_MSG = '"message":"Success"'
    assert SUCCESS_STATUS_MSG in grep_paroduslogs(SUCCESS_STATUS_MSG)

    FWUPDATE_STATUS_MSG = '"value":"false"'
    assert FWUPDATE_STATUS_MSG in grep_paroduslogs(FWUPDATE_STATUS_MSG)

@pytest.mark.run(order=22)
def test_WebPA_Set_LOGURL_Handler():
    print("Starting parodus mock process")
    payload = '{"command":"SET","parameters":[{"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.LogServerUrl","dataType":0,"value":"logs.xcal.tv"}]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":200'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    SUCCESS_STATUS_MSG = '"message":"Success"'
    assert SUCCESS_STATUS_MSG in grep_paroduslogs(SUCCESS_STATUS_MSG)

@pytest.mark.run(order=23)
def test_WebPA_Get_LOGURL_Handler():
    print("Starting parodus mock process")
    payload ='{"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.LogServerUrl"]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":200'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    SUCCESS_STATUS_MSG = '"message":"Success"'
    assert SUCCESS_STATUS_MSG in grep_paroduslogs(SUCCESS_STATUS_MSG)

    LOGURL_MSG = '"value":"logs.xcal.tv"'
    assert LOGURL_MSG in grep_paroduslogs(LOGURL_MSG)

@pytest.mark.run(order=24)
def test_WebPA_Get_SPEED_Handler():
    print("Starting parodus mock process")
    payload ='{"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.LowSpeed"]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":200'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    SUCCESS_STATUS_MSG = '"message":"Success"'
    assert SUCCESS_STATUS_MSG in grep_paroduslogs(SUCCESS_STATUS_MSG)

    SPEED_STATUS_MSG = '"value":"12800"'
    assert SPEED_STATUS_MSG in grep_paroduslogs(SPEED_STATUS_MSG)

@pytest.mark.run(order=25)
def test_WebPA_Get_FW_PROTOCOL_Handler():
    print("Starting parodus mock process")
    payload ='{"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol"]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":200'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    SUCCESS_STATUS_MSG = '"message":"Success"'
    assert SUCCESS_STATUS_MSG in grep_paroduslogs(SUCCESS_STATUS_MSG)

    FWDL_PROTOCOL_MSG = '"value":"http"'
    assert FWDL_PROTOCOL_MSG in grep_paroduslogs(FWDL_PROTOCOL_MSG)

@pytest.mark.run(order=26)
def test_WebPA_Get_FWDL_STATUS_Handler():
    print("Starting parodus mock process")
    payload ='{"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus"]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":200'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    SUCCESS_STATUS_MSG = '"message":"Success"'
    assert SUCCESS_STATUS_MSG in grep_paroduslogs(SUCCESS_STATUS_MSG)

    FWDL_STATUS_MSG = '"value":"Download In Progress"'
    assert FWDL_STATUS_MSG in grep_paroduslogs(FWDL_STATUS_MSG)

@pytest.mark.run(order=27)
def test_WebPA_Get_FWDL_URL_Handler():
    print("Starting parodus mock process")
    payload ='{"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL"]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":200'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    SUCCESS_STATUS_MSG = '"message":"Success"'
    assert SUCCESS_STATUS_MSG in grep_paroduslogs(SUCCESS_STATUS_MSG)

    FWDL_URL_MSG = '"value":"https://dac15cdlserver.ae.ccp.xcal.tv/Images"'
    assert FWDL_URL_MSG in grep_paroduslogs(FWDL_URL_MSG)

@pytest.mark.run(order=28)
def test_WebPA_Get_FWDL_Handler():
    print("Starting parodus mock process")
    payload ='{"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload"]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":200'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    SUCCESS_STATUS_MSG = '"message":"Success"'
    assert SUCCESS_STATUS_MSG in grep_paroduslogs(SUCCESS_STATUS_MSG)

    FWDL_MSG = '"value":"ELTE11MWR_E037.000.00.8.1s22_DEV.bin"'
    assert FWDL_MSG in grep_paroduslogs(FWDL_MSG)

@pytest.mark.run(order=29)
def test_WebPA_Get_FWUPDATE_STATUS_Handler():
    print("Starting parodus mock process")
    payload ='{"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareUpdateState"]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":200'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    SUCCESS_STATUS_MSG = '"message":"Success"'
    assert SUCCESS_STATUS_MSG in grep_paroduslogs(SUCCESS_STATUS_MSG)

    FWUPDATE_STATUS_MSG = '"value":"Download complete"'
    assert FWUPDATE_STATUS_MSG in grep_paroduslogs(FWUPDATE_STATUS_MSG)

@pytest.mark.run(order=30)
def test_WebPA_Get_WILDCARD_STATUS_Handler():
    print("Starting parodus mock process")
    payload ='{"command":"GET","names":["Device.DeviceInfo."]}'
    command = ["/usr/local/bin/parodus", payload]

    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f"Command failed with error: {result.stderr}"

    STATUS_CODE_MSG = '"statusCode":200'
    assert STATUS_CODE_MSG in grep_paroduslogs(STATUS_CODE_MSG)

    SUCCESS_STATUS_MSG = '"message":"Success"'
    assert SUCCESS_STATUS_MSG in grep_paroduslogs(SUCCESS_STATUS_MSG)


