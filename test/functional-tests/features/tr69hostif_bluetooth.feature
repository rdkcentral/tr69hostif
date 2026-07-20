####################################################################################
# If not stated otherwise in this file or this component's Licenses.txt file the
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

# Source: ../tests/tr69hostif_bluetooth.py
# Source implementation: src/hostif/profiles/DeviceInfo/XrdkBlueTooth.cpp
# Feature: tr69hostif_bluetooth.feature

Feature: Bluetooth xBlueTooth Parameter Handler Validation

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario Outline: GET xBlueTooth top-level parameters returns error
    When I GET "<parameter>" via rbus
    Then the rbus response should contain an error

    Examples:
      | parameter                                                         |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.enable             |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveryEnabled   |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDeviceCnt|
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDeviceCnt    |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDeviceCnt |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.GetDeviceInfo      |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.LimitBeaconDetection |

  Scenario Outline: GET xBlueTooth DeviceInfo parameters returns error
    When I GET "<parameter>" via rbus
    Then the rbus response should contain an error

    Examples:
      | parameter                                                                |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.DeviceID       |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.Manufacturer   |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.Profile        |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.MAC            |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.SignalStrength |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.RSSI           |

  Scenario Outline: GET xBlueTooth discovered and paired list parameters returns error
    When I GET "<parameter>" via rbus
    Then the rbus response should contain an error

    Examples:
      | parameter                                                                    |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.1.Name       |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.1.DeviceID   |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.1.DeviceType |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.1.Paired     |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.1.Connected  |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.1.Name           |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.1.DeviceID       |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.1.Connected      |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.1.DeviceType     |

  Scenario Outline: GET xBlueTooth connected device list parameters returns error
    When I GET "<parameter>" via rbus
    Then the rbus response should contain an error

    Examples:
      | parameter                                                                       |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.1.Name           |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.1.DeviceID       |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.1.DeviceType     |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.1.Active         |

  Scenario Outline: SET xBlueTooth writable parameters returns set failure
    When I SET "<parameter>" to "<value>" as <type> via rbus
    Then the rbus set response should contain "setvalues failed"

    Examples:
      | parameter                                                            | type    | value            |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.GetDeviceInfo         | int     | 1                |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.LimitBeaconDetection  | boolean | true             |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.BLE.Tile.Ring.Id      | string  | TILE-TEST-001    |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.BLE.Tile.Ring.SessionId | string  | SESSION-TEST-001 |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.BLE.Tile.Cmd.Request  | string  | test-cmd         |

  Scenario: GET xBlueTooth BLE Tile Ring parameters returns error
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.BLE.Tile.Ring.Id" via rbus
    Then the rbus response should contain an error
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.BLE.Tile.Ring.SessionId" via rbus
    Then the rbus response should contain an error
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.BLE.Tile.Ring.Trigger" via rbus
    Then the rbus response should contain an error

  Scenario: SET xBlueTooth BLE Tile Ring Trigger returns set failure
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.BLE.Tile.Ring.Id" to "TILE-TEST-001" as string via rbus
    And I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.BLE.Tile.Ring.Trigger" to "false" as boolean via rbus
    Then the rbus set response should contain "setvalues failed"
