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

# Source: ../tests/tr69hostif_deviceip.py
# Feature: tr69hostif_deviceip.feature

Feature: Device Profile Parameter GET/SET Handlers (DeviceInfo, IP, Services, ReverseSSH)

  These tests validate TR-181 parameter GET and SET operations for the
  DeviceInfo default parameters, IP interface parameters, STB Services,
  and ReverseSSH handlers using rbuscli.

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  # =====================================================================
  # DeviceInfo Default (GET-only) Parameters
  # =====================================================================

  @order-25
  Scenario: GET Device.DeviceInfo.SoftwareVersion
    When I GET "Device.DeviceInfo.SoftwareVersion" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "99.99.15.07"

  @order-25
  Scenario: GET Device.DeviceInfo.ModelName
    When I GET "Device.DeviceInfo.ModelName" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "DOCKER"

  @order-25
  Scenario: GET Device.DeviceInfo.X_COMCAST-COM_FirmwareFilename
    When I GET "Device.DeviceInfo.X_COMCAST-COM_FirmwareFilename" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "Platform_Cotainer_1.0.0"

  @order-25
  Scenario: SET and GET MEMSWAP Enable
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MEMSWAP.Enable" to "true" as string via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MEMSWAP.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "true"

  # =====================================================================
  # Device.IP Interface Parameters (GET-only)
  # =====================================================================

  @order-26
  Scenario Outline: GET Device.IP interface parameters
    When I GET "<parameter>" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "<expected_value>"

    Examples:
      | parameter                                              | expected_value          |
      | Device.IP.Interface.1.IPv6Prefix.1.Autonomous          | false                   |
      | Device.IP.Interface.1.IPv6Address.1.Anycast             | false                   |
      | Device.IP.Interface.1.IPv6Address.1.Enable              | true                    |
      | Device.IP.Interface.1.IPv6Prefix.1.StaticType           | Inapplicable            |
      | Device.IP.Interface.1.IPv6AddressNumberOfEntries         | 1                       |
      | Device.IP.Interface.1.IPv6Address.1.Origin              | WellKnown               |
      | Device.IP.Interface.1.IPv4Address.1.Enable              | true                    |
      | Device.IP.Interface.1.IPv6Prefix.1.PrefixStatus         | Preferred               |
      | Device.IP.Interface.1.IPv6Address.1.PreferredLifetime   | 0001-01-01T00:00:00Z    |
      | Device.IP.Interface.1.IPv6Enable                        | true                    |
      | Device.IP.Interface.1.IPv6Prefix.1.ValidLifetime        | 0001-01-01T00:00:00Z    |

  # =====================================================================
  # Device.Services Parameters (GET-only)
  # =====================================================================

  @order-27
  Scenario: GET STBServiceNumberOfEntries
    When I GET "Device.Services.STBServiceNumberOfEntries" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "1"

  # =====================================================================
  # ReverseSSH Handlers
  # =====================================================================

  @order-28
  Scenario: GET ReverseSSH status shows INACTIVE
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshStatus" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "INACTIVE"

  @order-28
  Scenario: SET ReverseSSH trigger
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger" to "start shorts" as string via rbus
    Then the rbus response should indicate success

  @order-28
  Scenario: SET ReverseSSH args
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs" to "host=mockserver.tv;hostIp=mockserver.xmock.tv;stunnelport=2009;idletimeout=300;revsshport=3008;sshport=2221;user=webpa_user01;" as string via rbus
    Then the rbus response should indicate success
