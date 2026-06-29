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

Feature: Negative Edge Cases for Thunder Plugin Integration

  These scenarios document error handling for Thunder plugin communication
  failures including timeouts, incomplete responses, and malformed JSON.

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system
    And a mock Thunder edge server is running on 127.0.0.1:9998

  Scenario: NEGATIVE - Thunder Plugin Timeout Returns NOK
    Given the Thunder mock server is configured for timeout mode with 12 second delay
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID" via rbus
    Then the rbus response should contain an error
    And the tr69hostif logs should contain "getJsonRPCData failed"
    And the tr69hostif logs should contain "failed to fetch serviceAccountId"

  Scenario: NEGATIVE - Thunder Plugin Empty Response Returns NOK
    Given the Thunder mock server is configured to return empty response mode
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_Experience" via rbus
    Then the rbus response should contain an error
    And the tr69hostif logs should contain "json parse error, no \"result\" in the output from Thunder plugin"
    And the tr69hostif logs should contain "failed to fetch experience from AuthService"

  Scenario: NEGATIVE - Thunder Plugin Server Killed Mid-Request Returns NOK
    Given the Thunder mock server is configured to terminate mid-request
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_Experience" via rbus
    Then the rbus response should contain an error
    And the tr69hostif logs should contain incomplete JSON response
    And the tr69hostif logs should contain "Failed to parse Thunder response JSON"
