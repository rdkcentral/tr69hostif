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

# Source: ../tests/tr69hostif_custom.py
# Feature: tr69hostif_custom.feature

Feature: Comcast/RDK Custom Parameter GET and SET via rbus

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario Outline: GET custom read-only parameter
    When I GET "<parameter>" via rbus
    Then the rbus response should not contain an error

    Examples:
      | parameter                                                                                               |
      | Device.DeviceInfo.X_COMCAST-COM_STB_IP                                                                  |
      | Device.DeviceInfo.X_COMCAST-COM_PowerStatus                                                             |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareFilename                                                     |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareUpdateState                                                  |
      | Device.DeviceInfo.X_RDKCENTRAL-COM.BootStatus                                                           |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_BootTime                                                             |
      | Device.DeviceInfo.X_RDKCENTRAL-COM.CPUTemp                                                              |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_Experience                                                           |
      | Device.DeviceInfo.X_RDK_FirmwareName                                                                    |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_MigrationPreparer.MigrationReady                                     |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_Migration.MigrationStatus                                            |

  Scenario Outline: SET then GET writable custom parameter
    When I SET "<parameter>" to "<value>" as <type> via rbus
    And I GET "<parameter>" via rbus
    Then the rbus response should not contain an error

    Examples:
      | parameter                                                | type    | value                  |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload   | string  | fw_image.bin           |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus| string  | IDLE                   |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol | string  | https                  |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL      | string  | https://example.com/fw.bin |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadUseCodebig | boolean | true               |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot | boolean | true              |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_PreferredGatewayType  | string  | DOCSIS                |
      | Device.DeviceInfo.X_RDKCENTRAL-COM.IUI.Version           | string  | 1.0.0                 |
      | Device.DeviceInfo.X_RDKCENTRAL-COM.IUI.AppsVersion       | string  | 1.0.0                 |

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_Reset
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_Reset" to "Factory" as string via rbus
    Then the rbus response should indicate success

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_Reset returns error
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_Reset" via rbus
    Then the rbus response should contain an error

  Scenario: SET FirmwareDownloadNow trigger
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadNow" to "true" as boolean via rbus
    Then the rbus response should indicate success
