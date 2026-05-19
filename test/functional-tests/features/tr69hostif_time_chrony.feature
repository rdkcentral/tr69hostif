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

# Source: src/hostif/profiles/Time/Device_Time.cpp
# Source: src/hostif/handlers/src/hostIf_TimeClient_ReqHandler.cpp
# Backing: libc time functions + file-backed RFC chrony configuration
# Build: Always compiled (no conditional)
# File root: /opt/secure/RFC/chrony/

Feature: Time and Chrony NTP Configuration Parameters

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system
    And the directory /opt/secure/RFC/chrony/ exists

  # =========================================================================
  # Standard TR-181 Device.Time — libc-backed GET parameters
  # =========================================================================

  Scenario: GET CurrentLocalTime returns ISO8601 timestamp
    When I GET "Device.Time.CurrentLocalTime" via rbus
    Then the rbus response should not contain an error
    And the rbus response should match format "YYYY-MM-DDTHH:MM:SS+ZZZZ"
    # Backed by: libc time() + localtime(), formatted %Y-%m-%dT%H:%M:%S%z

  Scenario: GET CurrentUTCTime returns UTC timestamp
    When I GET "Device.Time.X_RDK_CurrentUTCTime" via rbus
    Then the rbus response should not contain an error
    And the rbus response should match format "YYYY-MM-DD HH:MM:SS"
    # Backed by: libc time() + gmtime(), formatted %Y-%m-%d %H:%M:%S

  Scenario: GET LocalTimeZone returns system timezone abbreviation
    When I GET "Device.Time.LocalTimeZone" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a non-empty string
    # Backed by: libc strftime(%Z) via gettimeofday() + localtime()

  Scenario: GET Enable reflects NTP enabled state
    When I GET "Device.Time.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a boolean value
    # Backed by: presence of /opt/.ntpEnabled (file exists = true)

  Scenario: GET Status returns NTP daemon status
    When I GET "Device.Time.Status" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a string value
    # Backed by: reads /tmp/ntp_status file (runtime NTP daemon writes)

  # =========================================================================
  # Standard TR-181 Device.Time — Stub SET handlers (return NOK)
  # =========================================================================

  Scenario: SET Enable returns error (stub handler)
    When I SET "Device.Time.Enable" to "true" as boolean via rbus
    Then the rbus response should contain an error
    # Implementation: set_Device_Time_Enable() returns NOK

  Scenario: SET LocalTimeZone returns error (stub handler)
    When I SET "Device.Time.LocalTimeZone" to "PST" as string via rbus
    Then the rbus response should contain an error
    # Implementation: set_Device_Time_LocalTimeZone() returns NOK

  # =========================================================================
  # Standard TR-181 NTPServer1-5 — Fully unimplemented (GET + SET = NOK)
  # =========================================================================

  Scenario Outline: GET NTPServer<index> returns error (unimplemented)
    When I GET "Device.Time.NTPServer<index>" via rbus
    Then the rbus response should contain an error

    Examples:
      | index |
      | 1     |
      | 2     |
      | 3     |
      | 4     |
      | 5     |

  Scenario Outline: SET NTPServer<index> returns error (unimplemented)
    When I SET "Device.Time.NTPServer<index>" to "pool.ntp.org" as string via rbus
    Then the rbus response should contain an error

    Examples:
      | index |
      | 1     |
      | 2     |
      | 3     |
      | 4     |
      | 5     |

  # =========================================================================
  # Chrony Enable (file-backed: /opt/secure/RFC/chrony/chronyd_enabled)
  # =========================================================================

  Scenario: GET Chrony Enable when file exists returns true
    Given the file /opt/secure/RFC/chrony/chronyd_enabled exists
    When I GET "Device.Time.Chrony.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "true"

  Scenario: GET Chrony Enable when file is absent returns false
    Given the file /opt/secure/RFC/chrony/chronyd_enabled does not exist
    When I GET "Device.Time.Chrony.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "false"

  Scenario: SET Chrony Enable to true creates file
    When I SET "Device.Time.Chrony.Enable" to "true" as boolean via rbus
    Then the rbus response should not contain an error
    And the file /opt/secure/RFC/chrony/chronyd_enabled should exist

  Scenario: SET Chrony Enable to false removes file
    When I SET "Device.Time.Chrony.Enable" to "false" as boolean via rbus
    Then the rbus response should not contain an error
    And the file /opt/secure/RFC/chrony/chronyd_enabled should not exist

  # =========================================================================
  # Chrony Makestep (file-backed: /opt/secure/RFC/chrony/ntp_maxstep)
  # =========================================================================

  Scenario: GET Chrony Makestep returns default when file missing
    Given the file /opt/secure/RFC/chrony/ntp_maxstep does not exist
    When I GET "Device.Time.Chrony.Makestep" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "1.0,3"
    # Default value: "1.0,3" (1.0 second threshold, 3 updates)

  Scenario: SET and GET Chrony Makestep roundtrip
    When I SET "Device.Time.Chrony.Makestep" to "0.5,5" as string via rbus
    And I GET "Device.Time.Chrony.Makestep" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "0.5,5"
    # File: /opt/secure/RFC/chrony/ntp_maxstep

  # =========================================================================
  # NTPMinpoll / NTPMaxpoll (file-backed: /opt/secure/RFC/chrony/)
  # =========================================================================

  Scenario: GET NTPMinpoll returns default 10 when file missing
    Given the file /opt/secure/RFC/chrony/ntp_minpoll does not exist
    When I GET "Device.Time.NTPMinpoll" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "10"

  Scenario: SET NTPMinpoll with valid value within range 4-24
    When I SET "Device.Time.NTPMinpoll" to "6" as integer via rbus
    Then the rbus response should not contain an error
    And the file /opt/secure/RFC/chrony/ntp_minpoll should contain "6"

  Scenario: SET NTPMinpoll with out-of-range value is rejected
    When I SET "Device.Time.NTPMinpoll" to "2" as integer via rbus
    Then the rbus response should contain an error
    # Valid range: 4–24

  Scenario: GET NTPMaxpoll returns default 12 when file missing
    Given the file /opt/secure/RFC/chrony/ntp_maxpoll does not exist
    When I GET "Device.Time.NTPMaxpoll" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "12"

  Scenario: SET NTPMaxpoll with valid value within range 4-24
    When I SET "Device.Time.NTPMaxpoll" to "14" as integer via rbus
    Then the rbus response should not contain an error
    And the file /opt/secure/RFC/chrony/ntp_maxpoll should contain "14"

  Scenario: SET NTPMaxpoll with out-of-range value is rejected
    When I SET "Device.Time.NTPMaxpoll" to "30" as integer via rbus
    Then the rbus response should contain an error
    # Valid range: 4–24

  # =========================================================================
  # NTPServer Directives (file-backed: /opt/secure/RFC/chrony/)
  # =========================================================================

  Scenario Outline: GET NTPServerDirective returns default "server" when file missing
    Given the file /opt/secure/RFC/chrony/ntp_server<index>_directive does not exist
    When I GET "Device.Time.NTPServer<index>Directive" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "server"

    Examples:
      | index |
      | 1     |
      | 2     |
      | 3     |
      | 4     |
      | 5     |

  Scenario: SET NTPServer1Directive and verify persistence
    When I SET "Device.Time.NTPServer1Directive" to "pool" as string via rbus
    And I GET "Device.Time.NTPServer1Directive" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "pool"
    # File: /opt/secure/RFC/chrony/ntp_server1_directive

  # =========================================================================
  # NTPServer Settings (file-backed: /opt/secure/RFC/chrony/)
  # =========================================================================

  Scenario: GET Chrony NTPServer.1.Settings returns default when missing
    Given the file /opt/secure/RFC/chrony/ntp_server1_settings does not exist
    When I GET "Device.Time.Chrony.NTPServer.1.Settings" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "server,0,true,10,12"
    # Default: "Type,Maxsources,Iburst,Minpoll,Maxpoll"

  Scenario: SET Chrony NTPServer.1.Settings with valid CSV format
    When I SET "Device.Time.Chrony.NTPServer.1.Settings" to "pool,4,true,6,12" as string via rbus
    Then the rbus response should not contain an error
    # Format: "Type,Maxsources,Iburst,Minpoll,Maxpoll"
    # Validates: Type={server|pool}, Maxsources=int, Iburst={true|false}, poll range [4-24]

  Scenario: SET Chrony NTPServer.1.Settings with invalid format rejected
    When I SET "Device.Time.Chrony.NTPServer.1.Settings" to "invalid_format" as string via rbus
    Then the rbus response should contain an error
    # Must be "Type,Maxsources,Iburst,Minpoll,Maxpoll" with valid values

  Scenario: SET Chrony NTPServer.1.Settings with poll out of range rejected
    When I SET "Device.Time.Chrony.NTPServer.1.Settings" to "server,0,true,2,30" as string via rbus
    Then the rbus response should contain an error
    # Minpoll=2 and Maxpoll=30 are outside valid range [4-24]

  Scenario Outline: GET Chrony NTPServer.<index>.Settings parameter exists
    When I GET "Device.Time.Chrony.NTPServer.<index>.Settings" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a string value

    Examples:
      | index |
      | 1     |
      | 2     |
      | 3     |
      | 4     |
      | 5     |

  # =========================================================================
  # Handler Inventory
  # =========================================================================

  Scenario: Time profile parameter handler inventory
    Given the Time source files are analyzed
    Then the following parameters should have handlers
      | TR-181 Parameter                           | GET | SET | Backing                                              |
      | Device.Time.Enable                         | Yes | NOK | /opt/.ntpEnabled (file existence)                   |
      | Device.Time.Status                         | Yes | No  | /tmp/ntp_status (runtime file)                       |
      | Device.Time.LocalTimeZone                  | Yes | NOK | libc strftime(%Z)                                   |
      | Device.Time.CurrentLocalTime               | Yes | No  | libc time() + localtime()                            |
      | Device.Time.X_RDK_CurrentUTCTime           | Yes | No  | libc time() + gmtime()                               |
      | Device.Time.NTPServer1                     | NOK | NOK | Unimplemented stub                                   |
      | Device.Time.NTPServer2                     | NOK | NOK | Unimplemented stub                                   |
      | Device.Time.NTPServer3                     | NOK | NOK | Unimplemented stub                                   |
      | Device.Time.NTPServer4                     | NOK | NOK | Unimplemented stub                                   |
      | Device.Time.NTPServer5                     | NOK | NOK | Unimplemented stub                                   |
      | Device.Time.Chrony.Enable                  | Yes | Yes | /opt/secure/RFC/chrony/chronyd_enabled               |
      | Device.Time.Chrony.Makestep                | Yes | Yes | /opt/secure/RFC/chrony/ntp_maxstep                   |
      | Device.Time.NTPMinpoll                     | Yes | Yes | /opt/secure/RFC/chrony/ntp_minpoll (range 4-24)      |
      | Device.Time.NTPMaxpoll                     | Yes | Yes | /opt/secure/RFC/chrony/ntp_maxpoll (range 4-24)      |
      | Device.Time.NTPServer1Directive            | Yes | Yes | /opt/secure/RFC/chrony/ntp_server1_directive          |
      | Device.Time.NTPServer2Directive            | Yes | Yes | /opt/secure/RFC/chrony/ntp_server2_directive          |
      | Device.Time.NTPServer3Directive            | Yes | Yes | /opt/secure/RFC/chrony/ntp_server3_directive          |
      | Device.Time.NTPServer4Directive            | Yes | Yes | /opt/secure/RFC/chrony/ntp_server4_directive          |
      | Device.Time.NTPServer5Directive            | Yes | Yes | /opt/secure/RFC/chrony/ntp_server5_directive          |
      | Device.Time.Chrony.NTPServer.1.Settings    | Yes | Yes | /opt/secure/RFC/chrony/ntp_server1_settings           |
      | Device.Time.Chrony.NTPServer.2.Settings    | Yes | Yes | /opt/secure/RFC/chrony/ntp_server2_settings           |
      | Device.Time.Chrony.NTPServer.3.Settings    | Yes | Yes | /opt/secure/RFC/chrony/ntp_server3_settings           |
      | Device.Time.Chrony.NTPServer.4.Settings    | Yes | Yes | /opt/secure/RFC/chrony/ntp_server4_settings           |
      | Device.Time.Chrony.NTPServer.5.Settings    | Yes | Yes | /opt/secure/RFC/chrony/ntp_server5_settings           |
