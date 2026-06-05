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

@pytest.mark.run(order=211)
def test_WebPA_DNSText_URL_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.X_RDK_WebPA_DNSText.URL"
    VALUE = "testurl.com"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=212)
def test_WebPA_DNSText_URL_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.X_RDK_WebPA_DNSText.URL"
    VALUE = "testurl.com"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout


@pytest.mark.run(order=213)
def test_RDKDownloadManager_InstallPackage_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.InstallPackage"
    VALUE = "testpackage"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=214)
def test_RDKDownloadManager_DownloadStatus_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.DownloadStatus"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=215)
def test_RDKRemoteDebugger_Enable_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.Enable"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=216)
def test_RDKRemoteDebugger_IssueType_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType"
    VALUE = "test"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=217)
def test_RDKRemoteDebugger_WebCfgData_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.WebCfgData"
    VALUE = "testcfgdata"
    # Force reload config fetch from xconf
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

