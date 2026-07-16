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

# Source: ../tests/tr69hostif_opsdevicemgmt_logging.py
# Feature: tr69hostif_opsdevicemgmt_logging.feature

Feature: xOpsDeviceMgmt Logging Parameter Handler Validation

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMUploadLogsNow
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMUploadLogsNow" to "true" as boolean via rbus
    Then the rbus response should not contain an error
    And the log should contain "Start executing script to upload logs... "

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMUploadLogsNow
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMUploadLogsNow" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "false"

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMLogsUploadStatus
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMLogsUploadStatus" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "uploaded"
    And the log should contain "Successfully read from /opt/loguploadstatus.txt."

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogEnabled
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogEnabled" to "true" as boolean via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogEnabled
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogEnabled" via rbus
    Then the rbus response should contain an error

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogPeriod
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogPeriod" to "300" as int via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogPeriod
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogPeriod" via rbus
    Then the rbus response should contain an error
