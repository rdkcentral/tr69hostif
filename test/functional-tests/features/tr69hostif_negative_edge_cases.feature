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

Feature: Negative Edge Cases for Parameter Type Validation and Range Checking

  These scenarios document negative edge case handling for RFC and parameter
  handlers, including type mismatch errors and out-of-range value validation.

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: NEGATIVE - SET String Parameter with Integer Type
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.XconfUrl" to "123" as integer via rbus
    Then the tr69hostif logs should contain "Parameter type mismatch! Given = 1 vs DataModel = 0"

  Scenario: NEGATIVE - SET Boolean Parameter with String Type
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.AutoExcluded.Enable" to "not_bool" as string via rbus
    Then the tr69hostif logs should contain "Parameter type mismatch! Given = 0 vs DataModel = 3"

  Scenario: NEGATIVE - SET Integer Parameter with Out-of-Range High Value
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.LowSpeed" to "9999999999" as integer via rbus
    Then the rbus response should contain "Invalid data value passed to set"

  Scenario: NEGATIVE - SET Integer Parameter with Out-of-Range Negative Value
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.LowSpeed" to "-2147483649" as integer via rbus
    Then the rbus response should contain "Invalid data value passed to set"
