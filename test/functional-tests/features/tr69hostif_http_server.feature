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

# Source: src/hostif/httpserver/src/http_server.cpp
# Source: src/hostif/httpserver/src/request_handler.cpp
# Source: src/hostif/httpserver/src/XrdkCentralComRFCVar.cpp
# Backing: libsoup HTTP server, WDMP-C JSON protocol, cJSON
# Build: Conditional (!WITH_NEW_HTTP_SERVER_DISABLE)
# Port: configurable via hostIf_main.cpp argList.httpServerPort (default 11999)

Feature: HTTP Server WDMP-C Protocol

  Background:
    Given the tr69hostif daemon is running and initialized
    And the HTTP server is enabled (!WITH_NEW_HTTP_SERVER_DISABLE)
    And the HTTP server is listening on port 11999
    And the file /tmp/.tr69hostif_http_server_ready exists

  # =========================================================================
  # GET Requests
  # =========================================================================

  Scenario: GET single parameter via HTTP
    When I send HTTP GET to "http://127.0.0.1:11999" with body:
      """
      {"names":["Device.DeviceInfo.ModelName"]}
      """
    Then the HTTP response status should be 200
    And the response body should contain '"statusCode"'
    And the response body should contain '"parameters"'

  Scenario: GET multiple parameters via HTTP
    When I send HTTP GET to "http://127.0.0.1:11999" with body:
      """
      {"names":["Device.DeviceInfo.ModelName","Device.DeviceInfo.Manufacturer"]}
      """
    Then the HTTP response status should be 200
    And the response body should contain '"parameterCount":2'

  Scenario: GET wildcard parameter via HTTP
    When I send HTTP GET to "http://127.0.0.1:11999" with body:
      """
      {"names":["Device.DeviceInfo."]}
      """
    Then the HTTP response status should be 200
    And the response body should contain '"value":['
    # Wildcard GET returns array of {name,value,dataType} objects

  Scenario: GET with CallerID header
    When I send HTTP GET to "http://127.0.0.1:11999" with CallerID "TestAgent" and body:
      """
      {"names":["Device.DeviceInfo.ModelName"]}
      """
    Then the HTTP response status should be 200
    And the response body should contain '"statusCode"'
    # CallerID is optional for GET — treated as "Unknown" if missing

  Scenario: GET without CallerID header succeeds
    When I send HTTP GET to "http://127.0.0.1:11999" without CallerID header and body:
      """
      {"names":["Device.DeviceInfo.ModelName"]}
      """
    Then the HTTP response status should be 200
    # GET does not require CallerID

  Scenario: GET nonexistent parameter via HTTP
    When I send HTTP GET to "http://127.0.0.1:11999" with body:
      """
      {"names":["Device.Nonexistent.Parameter"]}
      """
    Then the HTTP response status should be 200
    And the response body should contain a non-zero statusCode indicating error
    # WDMP_ERR_INVALID_PARAMETER_NAME

  # =========================================================================
  # POST (SET) Requests
  # =========================================================================

  Scenario: SET parameter via HTTP with CallerID
    When I send HTTP POST to "http://127.0.0.1:11999" with CallerID "TestAgent" and body:
      """
      {"parameters":[{"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload","value":"test.bin","dataType":0}]}
      """
    Then the HTTP response status should be 200
    And the response body should contain '"statusCode"'
    And the response body should contain '"message"'

  Scenario: SET parameter via HTTP without CallerID is rejected
    When I send HTTP POST to "http://127.0.0.1:11999" without CallerID header and body:
      """
      {"parameters":[{"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload","value":"test.bin","dataType":0}]}
      """
    Then the HTTP response status should be 500
    And the response body should contain "POST Not Allowed without CallerID"

  Scenario: SET read-only parameter via HTTP
    When I send HTTP POST to "http://127.0.0.1:11999" with CallerID "TestAgent" and body:
      """
      {"parameters":[{"name":"Device.DeviceInfo.ModelName","value":"changed","dataType":0}]}
      """
    Then the HTTP response status should be 200
    And the response body should contain a non-zero statusCode
    # WDMP_ERR_NOT_WRITABLE

  # =========================================================================
  # RFC Variable Store (XRFCVarStore)
  # =========================================================================

  Scenario: GET RFC variable via HTTP
    When I send HTTP GET to "http://127.0.0.1:11999" with body:
      """
      {"names":["RFC_ENABLE_XDNS"]}
      """
    Then the HTTP response status should be 200
    And the response body should contain '"statusCode"'
    # RFC_* keys bypass data model and use XRFCVarStore file lookup

  # =========================================================================
  # Error Handling
  # =========================================================================

  Scenario: Unsupported HTTP method returns 501
    When I send HTTP PUT to "http://127.0.0.1:11999" with body:
      """
      {"names":["Device.DeviceInfo.ModelName"]}
      """
    Then the HTTP response status should be 501

  Scenario: Empty request body returns 400
    When I send HTTP GET to "http://127.0.0.1:11999" with empty body
    Then the HTTP response status should be 400
    And the response body should contain "No request data"

  Scenario: Malformed JSON body returns 400
    When I send HTTP GET to "http://127.0.0.1:11999" with body:
      """
      {this is not valid json
      """
    Then the HTTP response status should be 400

  # =========================================================================
  # WDMP Error Codes
  # =========================================================================

  Scenario Outline: HTTP server WDMP error code mapping
    When I send an HTTP request that triggers "<error_condition>"
    Then the response body statusCode should map to WDMP error "<wdmp_error>"

    Examples:
      | error_condition              | wdmp_error                        |
      | Invalid parameter name       | WDMP_ERR_INVALID_PARAMETER_NAME   |
      | Set read-only parameter      | WDMP_ERR_NOT_WRITABLE             |
      | Wrong data type on SET       | WDMP_ERR_INVALID_PARAMETER_TYPE   |
      | Invalid parameter value      | WDMP_ERR_INVALID_PARAMETER_VALUE  |
      | Wildcard not supported       | WDMP_ERR_WILDCARD_NOT_SUPPORTED   |
      | Null value on SET            | WDMP_ERR_VALUE_IS_NULL            |
      | Empty value on SET           | WDMP_ERR_VALUE_IS_EMPTY           |
      | Internal processing error    | WDMP_ERR_INTERNAL_ERROR           |
