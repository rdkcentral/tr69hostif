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

Feature: RFC Variable Store Readback via HTTP interface

  Background:
    Given the tr69hostif daemon is running and initialized
    And rfcVariable.ini exists at "/opt/secure/RFC/rfcVariable.ini"

  Scenario: RFC readback after cache reload - iteration 1
    When I write "RFC_L2_READBACK=readback_value" to rfcVariable.ini
    And I trigger RFC cache reload using "RFC_CONTROL_RELOADCACHE"
    And I GET RFC key "RFC_L2_READBACK" through HTTP
    Then the returned value should be "readback_value"

  Scenario: RFC readback after cache reload - iteration 2
    When I write "RFC_L2_READBACK=readback_value" to rfcVariable.ini
    And I trigger RFC cache reload using "RFC_CONTROL_RELOADCACHE"
    And I GET RFC key "RFC_L2_READBACK" through HTTP
    Then the returned value should be "readback_value"

  Scenario: RFC readback after cache reload - iteration 3
    When I write "RFC_L2_READBACK=readback_value" to rfcVariable.ini
    And I trigger RFC cache reload using "RFC_CONTROL_RELOADCACHE"
    And I GET RFC key "RFC_L2_READBACK" through HTTP
    Then the returned value should be "readback_value"

  Scenario: RFC readback after cache reload - iteration 4
    When I write "RFC_L2_READBACK=readback_value" to rfcVariable.ini
    And I trigger RFC cache reload using "RFC_CONTROL_RELOADCACHE"
    And I GET RFC key "RFC_L2_READBACK" through HTTP
    Then the returned value should be "readback_value"

