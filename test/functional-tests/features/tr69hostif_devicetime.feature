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

# Source: ../tests/tr69hostif_devicetime.py
# Feature: tr69hostif_devicetime.feature

Feature: Device.Time Parameter GET/SET via rbus

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: GET Device.Time.Enable returns false
    When I GET "Device.Time.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "false"

  Scenario: GET Device.Time.Status returns error
    When I GET "Device.Time.Status" via rbus
    Then the rbus response should contain an error

  Scenario: SET and GET Device.Time.NTPServer1
    When I SET "Device.Time.NTPServer1" to "test.com" as string via rbus
    And I GET "Device.Time.NTPServer1" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "test.com"

  Scenario: SET and GET Device.Time.NTPServer2
    When I SET "Device.Time.NTPServer2" to "test1.com" as string via rbus
    And I GET "Device.Time.NTPServer2" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "test1.com"

  Scenario: SET and GET Device.Time.NTPServer3
    When I SET "Device.Time.NTPServer3" to "test2.com" as string via rbus
    And I GET "Device.Time.NTPServer3" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "test2.com"

  Scenario: SET and GET Device.Time.NTPServer4
    When I SET "Device.Time.NTPServer4" to "test3.com" as string via rbus
    And I GET "Device.Time.NTPServer4" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "test3.com"

  Scenario: SET and GET Device.Time.NTPServer5
    When I SET "Device.Time.NTPServer5" to "test4.com" as string via rbus
    And I GET "Device.Time.NTPServer5" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "test4.com"

  Scenario: GET Device.Time.CurrentLocalTime
    When I GET "Device.Time.CurrentLocalTime" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Time.LocalTimeZone contains UTC
    When I GET "Device.Time.LocalTimeZone" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "UTC"

  Scenario: SET Device.Time.LocalTimeZone returns error
    When I SET "Device.Time.LocalTimeZone" to "UTC" as string via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.Time.X_RDK_CurrentUTCTime
    When I GET "Device.Time.X_RDK_CurrentUTCTime" via rbus
    Then the rbus response should not contain an error

  Scenario: SET Device.Time.Enable to true returns error
    When I SET "Device.Time.Enable" to "true" as boolean via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.Time.Enable remains false
    When I GET "Device.Time.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "false"

  Scenario: SET Device.Time.Enable to false returns error
    When I SET "Device.Time.Enable" to "false" as boolean via rbus
    Then the rbus response should contain an error
