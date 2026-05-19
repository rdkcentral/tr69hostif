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

# Source: Various handler dispatch and HTTP server source files
# Purpose: Validates error handling, edge cases, and boundary conditions
# Scope: Cross-cutting negative scenarios not specific to a single profile

Feature: Negative and Edge Case Tests

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  # =========================================================================
  # Invalid Parameter Names
  # =========================================================================

  Scenario: GET nonexistent top-level object
    When I GET "Device.Nonexistent.Parameter" via rbus
    Then the rbus response should contain an error
    And the error code should indicate invalid parameter name

  Scenario: GET parameter with trailing dot (wildcard on non-table)
    When I GET "Device.DeviceInfo.ModelName." via rbus
    Then the rbus response should contain an error or empty result
    # Trailing dot implies wildcard enumeration; non-table params have no children

  Scenario: GET empty parameter name
    When I GET "" via rbus
    Then the rbus response should contain an error

  Scenario: GET parameter with invalid instance number
    When I GET "Device.Ethernet.Interface.999.Enable" via rbus
    Then the rbus response should contain an error
    # Instance number exceeds actual interface count

  Scenario: GET parameter with zero instance number
    When I GET "Device.Ethernet.Interface.0.Enable" via rbus
    Then the rbus response should contain an error
    # TR-181 instance numbers are 1-based

  Scenario: GET parameter with negative instance number
    When I GET "Device.Ethernet.Interface.-1.Enable" via rbus
    Then the rbus response should contain an error

  # =========================================================================
  # SET on Read-Only Parameters
  # =========================================================================

  Scenario: SET read-only ModelName via rbus
    When I SET "Device.DeviceInfo.ModelName" to "hacked" as string via rbus
    Then the rbus response should contain an error
    # ModelName is read-only (populated from /etc/device.properties)

  Scenario: SET read-only SerialNumber via rbus
    When I SET "Device.DeviceInfo.SerialNumber" to "FAKE123" as string via rbus
    Then the rbus response should contain an error

  Scenario: SET read-only Manufacturer via rbus
    When I SET "Device.DeviceInfo.Manufacturer" to "Evil Corp" as string via rbus
    Then the rbus response should contain an error

  Scenario: SET read-only Ethernet MACAddress via rbus
    When I SET "Device.Ethernet.Interface.1.MACAddress" to "00:11:22:33:44:55" as string via rbus
    Then the rbus response should contain an error
    # MACAddress is read from sysfs, not writable

  # =========================================================================
  # Data Type Mismatches on SET
  # =========================================================================

  Scenario: SET boolean parameter with non-boolean string value
    When I SET "Device.WiFi.Enable" to "notaboolean" as string via rbus
    Then the rbus response should contain an error
    # Expected: boolean type; provided string is not "true"/"false"/"0"/"1"

  Scenario: SET integer parameter with alpha string
    When I SET "Device.Time.NTPMinpoll" to "abc" as integer via rbus
    Then the rbus response should contain an error

  # =========================================================================
  # HTTP Server Negative Cases
  # =========================================================================

  Scenario: HTTP server not ready before initialization
    Given the tr69hostif daemon is starting but not yet initialized
    When I check for /tmp/.tr69hostif_http_server_ready
    Then the file should not exist until initialization completes

  Scenario: HTTP DELETE method not supported
    When I send HTTP DELETE to "http://127.0.0.1:11999" with body:
      """
      {"names":["Device.DeviceInfo.ModelName"]}
      """
    Then the HTTP response status should be 501

  Scenario: HTTP PATCH method not supported
    When I send HTTP PATCH to "http://127.0.0.1:11999" with body:
      """
      {"names":["Device.DeviceInfo.ModelName"]}
      """
    Then the HTTP response status should be 501

  Scenario: HTTP GET with oversized body
    When I send HTTP GET to "http://127.0.0.1:11999" with a body exceeding 10000 parameter names
    Then the HTTP response should indicate error or timeout
    # Stress test: excessive parameter count

  Scenario: HTTP POST with empty parameters array
    When I send HTTP POST to "http://127.0.0.1:11999" with CallerID "Test" and body:
      """
      {"parameters":[]}
      """
    Then the HTTP response status should be 400 or 200 with error statusCode
    # Empty parameters array is degenerate input

  Scenario: HTTP POST with missing name field
    When I send HTTP POST to "http://127.0.0.1:11999" with CallerID "Test" and body:
      """
      {"parameters":[{"value":"test","dataType":0}]}
      """
    Then the HTTP response should indicate an error
    # "name" field is required in each parameter entry

  Scenario: HTTP POST with missing value field
    When I send HTTP POST to "http://127.0.0.1:11999" with CallerID "Test" and body:
      """
      {"parameters":[{"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload","dataType":0}]}
      """
    Then the HTTP response should indicate WDMP_ERR_VALUE_IS_NULL or similar error

  Scenario: HTTP POST with invalid dataType enum
    When I send HTTP POST to "http://127.0.0.1:11999" with CallerID "Test" and body:
      """
      {"parameters":[{"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload","value":"v1","dataType":999}]}
      """
    Then the HTTP response should indicate WDMP_ERR_INVALID_PARAMETER_TYPE

  # =========================================================================
  # Thunder Plugin Unavailability
  # =========================================================================

  Scenario: GET Thunder-backed parameter when plugin is deactivated
    Given the Thunder plugin "org.rdk.NetworkManager" is not activated
    When I GET "Device.WiFi.SSID.1.BSSID" via rbus
    Then the rbus response should contain an error or empty value
    # Thunder invocation fails; handler should return error gracefully

  Scenario: GET Thunder-backed parameter when Thunder is unreachable
    Given the Thunder service is not running on localhost:9998
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_Experience" via rbus
    Then the rbus response should contain an error
    # Thunder JSON-RPC connection failure should propagate as parameter error

  # =========================================================================
  # Chrony File Permission Edge Cases
  # =========================================================================

  Scenario: SET Chrony parameter when /opt/secure/RFC/chrony/ is read-only
    Given the directory /opt/secure/RFC/chrony/ is mounted read-only
    When I SET "Device.Time.Chrony.Enable" to "true" as boolean via rbus
    Then the rbus response should contain an error
    # File creation will fail; handler should return error

  Scenario: GET Chrony parameter when file has no read permission
    Given the file /opt/secure/RFC/chrony/ntp_minpoll exists but is not readable
    When I GET "Device.Time.NTPMinpoll" via rbus
    Then the rbus response should return the default value "10"
    # Handler falls back to default when file read fails

  # =========================================================================
  # Ethernet Interface Edge Cases
  # =========================================================================

  Scenario: GET Ethernet Stats on interface that is down
    Given Ethernet interface 1 link status is "Down"
    When I GET "Device.Ethernet.Interface.1.Stats.BytesSent" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "0" or last known value
    # sysfs still provides stats even for down interfaces

  Scenario: GET Ethernet InterfaceNumberOfEntries
    When I GET "Device.Ethernet.InterfaceNumberOfEntries" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a positive integer
    # Derived from if_nameindex() count

  # =========================================================================
  # Concurrent Access / Stress
  # =========================================================================

  Scenario: Concurrent GET requests do not cause data corruption
    When I send 10 concurrent rbus GET requests for "Device.DeviceInfo.ModelName"
    Then all responses should contain the same value
    And no response should contain an error
    # Validates thread-safety of GET handlers

  Scenario: Rapid SET+GET does not return stale data
    When I SET "Device.Time.NTPMinpoll" to "8" as integer via rbus
    And I immediately GET "Device.Time.NTPMinpoll" via rbus
    Then the rbus response should contain "8"
    # File-backed parameters should be consistent after SET returns
