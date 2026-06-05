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

# Source: ../tests/tr69hostif_webpa_negative_edge_cases.py
# Feature: tr69hostif_webpa_negative_edge_cases.feature

Feature: Negative Edge Cases for WebPA/Parodus Integration

  These scenarios document error handling for WebPA command parsing and
  parameter validation through the parodus interface.

  Background:
    Given the tr69hostif daemon is running and initialized
    And the parodus mock process is available

  @order-97
  Scenario: NEGATIVE - WebPA Malformed JSON Missing Closing Brace
    When I send a WebPA command with malformed JSON missing closing brace
    Then the parodus logs should contain statusCode 520
    And the parodus logs should contain "Invalid Input Command"

  @order-98
  Scenario: NEGATIVE - WebPA Malformed JSON Unquoted Key
    When I send a WebPA command with malformed JSON unquoted key
    Then the parodus logs should contain statusCode 520
    And the parodus logs should contain "Invalid Input Command"

  @order-99
  Scenario: NEGATIVE - WebPA SET with Wrong Data Type
    When I send a WebPA SET command with wrong data type (int instead of string)
    Then the parodus logs should contain statusCode 520
    And the parodus logs should contain "Invalid parameter value"

  @order-100
  Scenario: NEGATIVE - WebPA Malformed JSON Random Text Payload
    When I send a WebPA command with random non-JSON text payload
    Then the parodus logs should contain statusCode 520
    And the parodus logs should contain "Invalid Input Command"

  @order-101
  Scenario: NEGATIVE - WebPA GET_ATTRIBUTES with Wildcard Rejected
    When I send a WebPA GET_ATTRIBUTES command with wildcard in parameter name
    Then the parodus logs should contain "Wildcard is not supported"
    And the parodus logs should contain statusCode 552

  @order-102
  Scenario: WebPA GET_ATTRIBUTES Notify Handler
    When I send a WebPA GET_ATTRIBUTES command for X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.Enable
    Then the parodus logs should contain statusCode 200
    And the parodus logs should contain "Success"
    And the parodus logs should contain the parameter name in response
