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


Feature: RFC Store Control and Feature Parameters SET via rbus

  These scenarios document the RFC control and feature parameter handlers
  for Device.DeviceInfo.X_RDKCENTRAL-COM_RFC parameters stored in the RFC database.

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDB
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDB" to "true" as boolean via rbus
    Then the rbus response should indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDBEnd
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDBEnd" to "true" as boolean via rbus
    Then the rbus response should not indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.RetrieveNow
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.RetrieveNow" to "100" as uint via rbus
    Then the rbus response should indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RoamTrigger
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RoamTrigger" to "triggered" as string via rbus
    Then the rbus response should indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MS12.DAPv2_Enable
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MS12.DAPv2_Enable" to "true" as boolean via rbus
    Then the rbus response should indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MS12.DE_Enable
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MS12.DE_Enable" to "true" as boolean via rbus
    Then the rbus response should indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LoudnessEquivalence.Enable
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LoudnessEquivalence.Enable" to "true" as boolean via rbus
    Then the rbus response should indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DAB.Enable
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DAB.Enable" to "true" as boolean via rbus
    Then the rbus response should indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AutoReboot.Enable
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AutoReboot.Enable" to "true" as boolean via rbus
    Then the rbus response should indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Enable
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Enable" to "true" as boolean via rbus
    Then the rbus response should indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpStart
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpStart" to "100" as integer via rbus
    Then the rbus response should indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpEnd
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpEnd" to "100" as integer via rbus
    Then the rbus response should indicate success
