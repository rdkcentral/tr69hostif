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


@pytest.mark.run(order=334)
def test_xOpsDeviceMgmt_RPC_RebootNow_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootNow"
    VALUE = "true"
    SUCCESS_MSG = "Successfully executed reboot script"

    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout
    assert SUCCESS_MSG in grep_tr69hostiflogs(SUCCESS_MSG)

@pytest.mark.run(order=335)
def test_xOpsDeviceMgmt_RPC_RebootNow_Set_False_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootNow"
    VALUE = "false"
    SUCCESS_MSG = "Not rebooting. Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootNow = false"

    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout
    assert SUCCESS_MSG in grep_tr69hostiflogs(SUCCESS_MSG)


@pytest.mark.run(order=336)
def test_xOpsDeviceMgmt_RPC_DeviceManageableNotification_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.DeviceManageableNotification"
    VALUE = "manageable"

    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=337)
def test_xOpsDeviceMgmt_RPC_DeviceManageableNotification_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.DeviceManageableNotification"
    VALUE = "manageable"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=338)
def test_xOpsDeviceMgmt_RPC_FirmwareDownloadStartedNotification_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadStartedNotification"
    VALUE = "started"

    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=339)
def test_xOpsDeviceMgmt_RPC_FirmwareDownloadStartedNotification_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadStartedNotification"
    VALUE = "started"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=340)
def test_xOpsDeviceMgmt_RPC_FirmwareDownloadCompletedNotification_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadCompletedNotification"
    VALUE = "false"

    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=341)
def test_xOpsDeviceMgmt_RPC_FirmwareDownloadCompletedNotification_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadCompletedNotification"
    VALUE = "false"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout


@pytest.mark.run(order=342)
def test_xOpsDeviceMgmt_RPC_RebootPendingNotification_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootPendingNotification"
    VALUE = "0"

    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "uint32", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=343)
def test_xOpsDeviceMgmt_RPC_RebootPendingNotification_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootPendingNotification"
    VALUE = "0"

    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout
