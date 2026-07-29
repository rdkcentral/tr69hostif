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

# Source: ../tests/tr69hostif_opsdevicemgmt_rpc.py
# Feature: tr69hostif_opsdevicemgmt_rpc.feature

Feature: xOpsDeviceMgmt RPC Parameter Handler Validation

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootNow to true
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootNow" to "true" as boolean via rbus
    Then the rbus response should not contain an error
    And the log should contain "Successfully executed reboot script"

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootNow to false
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootNow" to "false" as boolean via rbus
    Then the rbus response should not contain an error
    And the log should contain "Not rebooting. Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootNow = false"

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.DeviceManageableNotification
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.DeviceManageableNotification" to "manageable" as string via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.DeviceManageableNotification
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.DeviceManageableNotification" via rbus
    Then the rbus response should not contain an error

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadStartedNotification
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadStartedNotification" to "started" as string via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadStartedNotification
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadStartedNotification" via rbus
    Then the rbus response should not contain an error

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadCompletedNotification
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadCompletedNotification" to "false" as boolean via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadCompletedNotification
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadCompletedNotification" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "false"

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootPendingNotification
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootPendingNotification" to "0" as uint32 via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootPendingNotification
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootPendingNotification" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "0"
