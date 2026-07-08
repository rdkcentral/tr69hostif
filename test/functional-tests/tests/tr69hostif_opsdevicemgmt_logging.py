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


@pytest.mark.run(order=327)
def test_xOpsDeviceMgmt_Logging_xOpsDMUploadLogsNow_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMUploadLogsNow"
    VALUE = "true"
    UPLOAD_MSG = "Start executing script to upload logs... "

    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout
    assert UPLOAD_MSG in grep_tr69hostiflogs(UPLOAD_MSG)


@pytest.mark.run(order=328)
def test_xOpsDeviceMgmt_Logging_xOpsDMUploadLogsNow_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMUploadLogsNow"
    VALUE = "false"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout


@pytest.mark.run(order=329)
def test_xOpsDeviceMgmt_Logging_xOpsDMLogsUploadStatus_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMLogsUploadStatus"
    UPLOAD_STATUS_MSG = "uploaded"
    STATUS_MSG = "Successfully read from /opt/loguploadstatus.txt."
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert UPLOAD_STATUS_MSG in rstdout
    assert STATUS_MSG in grep_tr69hostiflogs(STATUS_MSG)


@pytest.mark.run(order=330)
def test_xOpsDeviceMgmt_Logging_xOpsDMMoCALogEnabled_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogEnabled"
    VALUE = "true"

    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=331)
def test_xOpsDeviceMgmt_Logging_xOpsDMMoCALogEnabled_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogEnabled"
    VALUE = "true"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=332)
def test_xOpsDeviceMgmt_Logging_xOpsDMMoCALogPeriod_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogPeriod"
    VALUE = "300"

    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "int", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=333)
def test_xOpsDeviceMgmt_Logging_xOpsDMMoCALogPeriod_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogPeriod"
    VALUE = "300"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout
