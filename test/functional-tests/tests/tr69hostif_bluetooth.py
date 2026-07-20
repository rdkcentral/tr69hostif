####################################################################################
# If not stated otherwise in this file or this component's Licenses file the
# following copyright and licenses apply:
#
# Copyright 2026 RDK Management
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
#
# Source: src/hostif/profiles/DeviceInfo/XrdkBlueTooth.cpp
# Guard:  USE_XRDK_BT_PROFILE (outer), BLE_TILE_PROFILE (Tile sub-section)
# Root:   Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.*
#
####################################################################################

import pytest

from helper_functions import *


@pytest.mark.run(order=437)
def test_xBlueTooth_Enable_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "enable"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=438)
def test_xBlueTooth_DiscoveryEnabled_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "DiscoveryEnabled"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=439)
def test_xBlueTooth_DiscoveredDeviceCnt_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "DiscoveredDeviceCnt"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=440)
def test_xBlueTooth_PairedDeviceCnt_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "PairedDeviceCnt"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=441)
def test_xBlueTooth_ConnectedDeviceCnt_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "ConnectedDeviceCnt"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout



@pytest.mark.run(order=442)
def test_xBlueTooth_GetDeviceInfo_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "GetDeviceInfo"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=443)
def test_xBlueTooth_DeviceInfo_DeviceID_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "DeviceInfo.DeviceID"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=444)
def test_xBlueTooth_DeviceInfo_Manufacturer_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "DeviceInfo.Manufacturer"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=445)
def test_xBlueTooth_DeviceInfo_Profile_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "DeviceInfo.Profile"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=446)
def test_xBlueTooth_DeviceInfo_MAC_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "DeviceInfo.MAC"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=447)
def test_xBlueTooth_DeviceInfo_SignalStrength_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "DeviceInfo.SignalStrength"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=448)
def test_xBlueTooth_DeviceInfo_RSSI_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "DeviceInfo.RSSI"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=449)
def test_xBlueTooth_SetDeviceInfo_Set_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "GetDeviceInfo"
    VALUE = "1"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "int", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout



@pytest.mark.run(order=450)
def test_xBlueTooth_DiscoveredDevice_Name_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "DiscoveredDevice.1.Name"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=451)
def test_xBlueTooth_DiscoveredDevice_DeviceID_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "DiscoveredDevice.1.DeviceID"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=452)
def test_xBlueTooth_DiscoveredDevice_DeviceType_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "DiscoveredDevice.1.DeviceType"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=453)
def test_xBlueTooth_DiscoveredDevice_Paired_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "DiscoveredDevice.1.Paired"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=454)
def test_xBlueTooth_DiscoveredDevice_Connected_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "DiscoveredDevice.1.Connected"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout



@pytest.mark.run(order=455)
def test_xBlueTooth_PairedDevice_Name_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "PairedDevice.1.Name"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=456)
def test_xBlueTooth_PairedDevice_DeviceID_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "PairedDevice.1.DeviceID"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=457)
def test_xBlueTooth_PairedDevice_Connected_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "PairedDevice.1.Connected"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=458)
def test_xBlueTooth_PairedDevice_DeviceType_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "PairedDevice.1.DeviceType"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout



@pytest.mark.run(order=459)
def test_xBlueTooth_ConnectedDevice_Name_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "ConnectedDevice.1.Name"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=460)
def test_xBlueTooth_ConnectedDevice_DeviceID_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "ConnectedDevice.1.DeviceID"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=461)
def test_xBlueTooth_ConnectedDevice_DeviceType_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "ConnectedDevice.1.DeviceType"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=462)
def test_xBlueTooth_ConnectedDevice_Active_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "ConnectedDevice.1.Active"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=463)
def test_xBlueTooth_LimitBeaconDetection_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "LimitBeaconDetection"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=464)
def test_xBlueTooth_LimitBeaconDetection_Set_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "LimitBeaconDetection"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", "true")
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=465)
def test_xBlueTooth_BLE_Tile_Ring_Id_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "BLE.Tile.Ring.Id"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=466)
def test_xBlueTooth_BLE_Tile_Ring_Id_Set_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "BLE.Tile.Ring.Id"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", "TILE-TEST-001")
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=467)
def test_xBlueTooth_BLE_Tile_Ring_SessionId_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "BLE.Tile.Ring.SessionId"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=468)
def test_xBlueTooth_BLE_Tile_Ring_SessionId_Set_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "BLE.Tile.Ring.SessionId"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", "SESSION-TEST-001")
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=469)
def test_xBlueTooth_BLE_Tile_Ring_Trigger_Get_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "BLE.Tile.Ring.Trigger"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=470)
def test_xBlueTooth_BLE_Tile_Ring_Trigger_Set_Handler():
    rbus_set_data(BT_ROOT + "BLE.Tile.Ring.Id", "string", "TILE-TEST-001")
    DATA_ELEMENT_NAME = BT_ROOT + "BLE.Tile.Ring.Trigger"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", "false")
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=471)
def test_xBlueTooth_BLE_Tile_Cmd_Request_Set_Handler():
    DATA_ELEMENT_NAME = BT_ROOT + "BLE.Tile.Cmd.Request"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", "test-cmd")
    assert RBUS_SET_EXCEPTION_STRING in rstdout
