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

Feature: Processor and ProcessStatus Parameter GET via rbus

  These scenarios document the processor architecture and process status
  parameter handlers for Device.DeviceInfo.Processor and Device.DeviceInfo.ProcessStatus.

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: GET Device.DeviceInfo.Processor.1.Architecture
    When I GET "Device.DeviceInfo.Processor.1.Architecture" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "x86_64"

  Scenario: GET Device.DeviceInfo.ProcessStatus.Process.1.PID
    When I GET "Device.DeviceInfo.ProcessStatus.Process.1.PID" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.ProcessStatus.Process.1.Command
    When I GET "Device.DeviceInfo.ProcessStatus.Process.1.Command" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.ProcessStatus.Process.1.Size
    When I GET "Device.DeviceInfo.ProcessStatus.Process.1.Size" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.ProcessStatus.Process.1.Priority
    When I GET "Device.DeviceInfo.ProcessStatus.Process.1.Priority" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.ProcessStatus.Process.1.CPUTime
    When I GET "Device.DeviceInfo.ProcessStatus.Process.1.CPUTime" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.ProcessStatus.Process.1.State
    When I GET "Device.DeviceInfo.ProcessStatus.Process.1.State" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.ProcessStatus.ProcessNumberOfEntries
    When I GET "Device.DeviceInfo.ProcessStatus.ProcessNumberOfEntries" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.DeviceInfo.ProcessStatus.CPUUsage
    # TR-181: System CPU usage as a percentage (0-100%)
    When I GET "Device.DeviceInfo.ProcessStatus.CPUUsage" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a numeric value
    And the CPU usage value should be between 0 and 100  
