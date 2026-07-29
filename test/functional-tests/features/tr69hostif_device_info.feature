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

# Source: ../tests/tr69hostif_device_info.py
# Feature: tr69hostif_device_info.feature

Feature: DeviceInfo Extended Parameter GET/SET Handler Validation

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: GET Device.DeviceInfo.AdditionalHardwareVersion returns error
    When I GET "Device.DeviceInfo.AdditionalHardwareVersion" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DeviceInfo.AdditionalSoftwareVersion returns error
    When I GET "Device.DeviceInfo.AdditionalSoftwareVersion" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DeviceInfo.FirstUseDate returns error
    When I GET "Device.DeviceInfo.FirstUseDate" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DeviceInfo.HardwareVersion returns error
    When I GET "Device.DeviceInfo.HardwareVersion" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DeviceInfo.Manufacturer returns error
    When I GET "Device.DeviceInfo.Manufacturer" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DeviceInfo.ManufacturerOUI returns error
    When I GET "Device.DeviceInfo.ManufacturerOUI" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DeviceInfo.Migration.MigrationStatus
    When I GET "Device.DeviceInfo.Migration.MigrationStatus" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.MigrationPreparer.MigrationReady returns error
    When I GET "Device.DeviceInfo.MigrationPreparer.MigrationReady" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DeviceInfo.SerialNumber returns error
    When I GET "Device.DeviceInfo.SerialNumber" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DeviceInfo.SupportedDataModelNumberOfEntries
    When I GET "Device.DeviceInfo.SupportedDataModelNumberOfEntries" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.VendorConfigFileNumberOfEntries returns error
    When I GET "Device.DeviceInfo.VendorConfigFileNumberOfEntries" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DeviceInfo.VendorLogFileNumberOfEntries returns error
    When I GET "Device.DeviceInfo.VendorLogFileNumberOfEntries" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DeviceInfo.X_COMCAST-COM_FirmwareDownloadStatus
    When I GET "Device.DeviceInfo.X_COMCAST-COM_FirmwareDownloadStatus" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.X_COMCAST-COM_FirmwareToDownload
    When I GET "Device.DeviceInfo.X_COMCAST-COM_FirmwareToDownload" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.X_COMCAST-COM_Reset returns error
    When I GET "Device.DeviceInfo.X_COMCAST-COM_Reset" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_RDKVersion
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RDKVersion" via rbus
    Then the rbus response should not contain an error

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.ApparmorBlocklist
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.ApparmorBlocklist" to "sample-profile:enforce" as string via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.ApparmorBlocklist returns error
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.ApparmorBlocklist" via rbus
    Then the rbus response should contain an error

  Scenario: SET and GET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.Blocklist
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.Blocklist" to "sample-block-entry" as string via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.Blocklist" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "sample-block-entry"

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_XRPolling.Action returns error
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_XRPolling.Action" to "XRPoll" as string via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_XRPolling.Action returns error
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_XRPolling.Action" via rbus
    Then the rbus response should contain an error

  Scenario: SET Device.DeviceInfo.X_COMCAST-COM_FirmwareDownloadStatus returns error
    When I SET "Device.DeviceInfo.X_COMCAST-COM_FirmwareDownloadStatus" to "IDLE" as string via rbus
    Then the rbus response should contain an error

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload" to "fw_image.bin" as string via rbus
    Then the rbus response should not contain an error

  Scenario: SET Device.DeviceInfo.X_COMCAST-COM_Reset
    When I SET "Device.DeviceInfo.X_COMCAST-COM_Reset" to "Factory" as string via rbus
    Then the rbus response should not contain an error

  Scenario: SET and GET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Detection
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Detection" to "true" as boolean via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Detection" via rbus
    Then the rbus response should not contain an error

  Scenario: SET and GET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Duration
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Duration" to "10" as int via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Duration" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "10"

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Trigger to start
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Trigger" to "start" as string via rbus
    Then the rbus response should not contain an error

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Trigger to stop
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Trigger" to "stop" as string via rbus
    Then the rbus response should not contain an error

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Enable to true
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Enable" to "true" as boolean via rbus
    Then the rbus response should not contain an error

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Enable to false
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Enable" to "false" as boolean via rbus
    Then the rbus response should not contain an error

