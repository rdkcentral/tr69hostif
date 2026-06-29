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

Feature: Standard TR-181 DeviceInfo Parameter GET via rbus

  These scenarios document the standard DeviceInfo parameter handlers
  for basic device identification and status parameters.

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: GET Device.DeviceInfo.ModelName
    When I GET "Device.DeviceInfo.ModelName" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "DOCKER"

  Scenario: GET Device.DeviceInfo.Description
    When I GET "Device.DeviceInfo.Description" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.ProductClass
    When I GET "Device.DeviceInfo.ProductClass" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.SoftwareVersion
    When I GET "Device.DeviceInfo.SoftwareVersion" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "99.99.15.07"

  Scenario: GET Device.DeviceInfo.ProvisioningCode
    When I GET "Device.DeviceInfo.ProvisioningCode" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.UpTime
    When I GET "Device.DeviceInfo.UpTime" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.ProcessorNumberOfEntries
    When I GET "Device.DeviceInfo.ProcessorNumberOfEntries" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.MemoryStatus.Total
    When I GET "Device.DeviceInfo.MemoryStatus.Total" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.MemoryStatus.Free
    When I GET "Device.DeviceInfo.MemoryStatus.Free" via rbus
    Then the rbus response should not contain an error
