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

# Source: ../tests/tr69hostif_interfacestack.py
# Feature: tr69hostif_interfacestack.feature

Feature: InterfaceStack Parameter Handler Validation

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: GET Device.InterfaceStackNumberOfEntries
    When I GET "Device.InterfaceStackNumberOfEntries" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.InterfaceStack.1.HigherLayer
    When I GET "Device.InterfaceStack.1.HigherLayer" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.InterfaceStack.1.LowerLayer
    When I GET "Device.InterfaceStack.1.LowerLayer" via rbus
    Then the rbus response should contain an error
