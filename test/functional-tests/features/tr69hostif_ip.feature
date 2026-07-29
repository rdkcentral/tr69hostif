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


Feature: Device.IP Interface, Address, Stats and ActivePort GET/SET Handlers

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario Outline: GET Device.IP interface summary parameters
    When I GET "<parameter>" via rbus
    Then the rbus response should not contain an error

    Examples:
      | parameter                             |
      | Device.IP.InterfaceNumberOfEntries    |
      | Device.IP.ActivePortNumberOfEntries   |

  Scenario: SET and GET Device.IP.Interface.1.Enable
    When I SET "Device.IP.Interface.1.Enable" to "true" as boolean via rbus
    And I GET "Device.IP.Interface.1.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "true"

  Scenario: SET and GET Device.IP.Interface.1.IPv4Enable
    When I SET "Device.IP.Interface.1.IPv4Enable" to "true" as boolean via rbus
    And I GET "Device.IP.Interface.1.IPv4Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "true"

  Scenario: SET Device.IP.Interface.1.IPv6Enable returns error
    When I SET "Device.IP.Interface.1.IPv6Enable" to "true" as boolean via rbus
    Then the rbus response should contain an error

  Scenario: SET Device.IP.Interface.1.ULAEnable returns error
    When I SET "Device.IP.Interface.1.ULAEnable" to "true" as boolean via rbus
    Then the rbus response should contain an error

  Scenario Outline: GET Device.IP.Interface.1 core parameters with expected values
    When I GET "<parameter>" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "<value>"

    Examples:
      | parameter                         | value     |
      | Device.IP.Interface.1.Enable      | true      |
      | Device.IP.Interface.1.IPv4Enable  | true      |
      | Device.IP.Interface.1.IPv6Enable  | true      |
      | Device.IP.Interface.1.Status      | Up        |
      | Device.IP.Interface.1.Name        | lo        |
      | Device.IP.Interface.1.Type        | Loopback  |
      | Device.IP.Interface.1.Loopback    | true      |

  Scenario: GET Device.IP.Interface.1.ULAEnable returns error
    When I GET "Device.IP.Interface.1.ULAEnable" via rbus
    Then the rbus response should contain an error

  Scenario: SET Device.IP.Interface.1.Alias returns error
    When I SET "Device.IP.Interface.1.Alias" to "ip_interface_alias" as string via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.IP.Interface.1.Alias returns error
    When I GET "Device.IP.Interface.1.Alias" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.IP.Interface.1.LastChange returns error
    When I GET "Device.IP.Interface.1.LastChange" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.IP.Interface.1.LowerLayers
    When I GET "Device.IP.Interface.1.LowerLayers" via rbus
    Then the rbus response should not contain an error

  Scenario: SET Device.IP.Interface.1.LowerLayers returns error
    When I SET "Device.IP.Interface.1.LowerLayers" to "eth0" as string via rbus
    Then the rbus response should contain an error

  Scenario: SET Device.IP.Interface.1.Router returns error
    When I SET "Device.IP.Interface.1.Router" to "true" as boolean via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.IP.Interface.1.Router returns error
    When I GET "Device.IP.Interface.1.Router" via rbus
    Then the rbus response should contain an error

  Scenario: SET Device.IP.Interface.1.Loopback returns error
    When I SET "Device.IP.Interface.1.Loopback" to "true" as boolean via rbus
    Then the rbus response should contain an error

  Scenario Outline: GET Device.IP.Interface.1.IPv4Address.1 parameters with expected values
    When I GET "<parameter>" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "<value>"

    Examples:
      | parameter                                       | value      |
      | Device.IP.Interface.1.IPv4Address.1.Enable      | true       |
      | Device.IP.Interface.1.IPv4Address.1.Status      | Enabled    |
      | Device.IP.Interface.1.IPv4Address.1.IPAddress   | 127.0.0.1  |
      | Device.IP.Interface.1.IPv4Address.1.SubnetMask  | 255.0.0.0  |
      | Device.IP.Interface.1.IPv4Address.1.AddressingType | Static   |

  @order-153
  Scenario: GET Device.IP.Interface.1.IPv4AddressNumberOfEntries
    When I GET "Device.IP.Interface.1.IPv4AddressNumberOfEntries" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.IP.Interface.1.IPv6AddressNumberOfEntries
    When I GET "Device.IP.Interface.1.IPv6AddressNumberOfEntries" via rbus
    Then the rbus response should not contain an error

  Scenario: SET and GET Device.IP.Interface.1.IPv4Address.1.Enable
    When I SET "Device.IP.Interface.1.IPv4Address.1.Enable" to "true" as boolean via rbus
    And I GET "Device.IP.Interface.1.IPv4Address.1.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "true"

  Scenario: SET Device.IP.Interface.1.IPv4Address.1.Alias returns error
    When I SET "Device.IP.Interface.1.IPv4Address.1.Alias" to "ipv4_alias" as string via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.IP.Interface.1.IPv4Address.1.Alias returns error
    When I GET "Device.IP.Interface.1.IPv4Address.1.Alias" via rbus
    Then the rbus response should contain an error

  Scenario: SET and GET Device.IP.Interface.1.IPv4Address.1.IPAddress
    When I SET "Device.IP.Interface.1.IPv4Address.1.IPAddress" to "127.0.0.1" as string via rbus
    And I GET "Device.IP.Interface.1.IPv4Address.1.IPAddress" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "127.0.0.1"

  Scenario: SET and GET Device.IP.Interface.1.IPv4Address.1.SubnetMask
    When I SET "Device.IP.Interface.1.IPv4Address.1.SubnetMask" to "255.0.0.0" as string via rbus
    And I GET "Device.IP.Interface.1.IPv4Address.1.SubnetMask" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "255.0.0.0"

  Scenario Outline: GET Device.IP.Interface.1.IPv6Address.1 key parameters
    When I GET "<parameter>" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "<value>"

    Examples:
      | parameter                                      | value      |
      | Device.IP.Interface.1.IPv6Address.1.Enable     | true       |
      | Device.IP.Interface.1.IPv6Address.1.Status     | Enabled    |
      | Device.IP.Interface.1.IPv6Address.1.IPAddress  | ::1        |
      | Device.IP.Interface.1.IPv6Address.1.Origin     | WellKnown  |
      | Device.IP.Interface.1.IPv6Address.1.Anycast    | false      |

  Scenario: SET Device.IP.Interface.1.IPv6Address.1.Enable returns error
    When I SET "Device.IP.Interface.1.IPv6Address.1.Enable" to "true" as boolean via rbus
    Then the rbus response should contain an error

  Scenario: SET Device.IP.Interface.1.IPv6Address.1.IPAddress returns error
    When I SET "Device.IP.Interface.1.IPv6Address.1.IPAddress" to "::1" as string via rbus
    Then the rbus response should contain an error

  Scenario Outline: GET Device.IP.Interface.1.IPv6Address.1 additional parameters
    When I GET "<parameter>" via rbus
    Then the rbus response should not contain an error

    Examples:
      | parameter                                         |
      | Device.IP.Interface.1.IPv6Address.1.Prefix        |
      | Device.IP.Interface.1.IPv6Address.1.PreferredLifetime |
      | Device.IP.Interface.1.IPv6Address.1.ValidLifetime |

  Scenario Outline: GET Device.IP.Interface.1.IPv6Prefix.1 parameters
    When I GET "<parameter>" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "<value>"

    Examples:
      | parameter                                       | value         |
      | Device.IP.Interface.1.IPv6Prefix.1.Autonomous  | false         |
      | Device.IP.Interface.1.IPv6Prefix.1.StaticType  | Inapplicable  |
      | Device.IP.Interface.1.IPv6Prefix.1.PrefixStatus| Preferred     |

  Scenario: GET Device.IP.Interface.1.IPv6Prefix.1.ValidLifetime
    When I GET "Device.IP.Interface.1.IPv6Prefix.1.ValidLifetime" via rbus
    Then the rbus response should not contain an error

  Scenario Outline: GET Device.IP.Interface.1.Stats counters
    When I GET "<parameter>" via rbus
    Then the rbus response should not contain an error

    Examples:
      | parameter                                                   |
      | Device.IP.Interface.1.Stats.BytesSent                       |
      | Device.IP.Interface.1.Stats.BytesReceived                   |
      | Device.IP.Interface.1.Stats.PacketsSent                     |
      | Device.IP.Interface.1.Stats.PacketsReceived                 |
      | Device.IP.Interface.1.Stats.ErrorsSent                      |
      | Device.IP.Interface.1.Stats.ErrorsReceived                  |
      | Device.IP.Interface.1.Stats.UnicastPacketsSent              |
      | Device.IP.Interface.1.Stats.UnicastPacketsReceived          |
      | Device.IP.Interface.1.Stats.DiscardPacketsSent              |
      | Device.IP.Interface.1.Stats.DiscardPacketsReceived          |
      | Device.IP.Interface.1.Stats.MulticastPacketsSent            |
      | Device.IP.Interface.1.Stats.MulticastPacketsReceived        |
      | Device.IP.Interface.1.Stats.BroadcastPacketsSent            |
      | Device.IP.Interface.1.Stats.BroadcastPacketsReceived        |
      | Device.IP.Interface.1.Stats.UnknownProtoPacketsReceived     |

  Scenario Outline: GET Device.IP.ActivePort.1 parameters with expected values
    When I GET "<parameter>" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "<value>"

    Examples:
      | parameter                            | value        |
      | Device.IP.ActivePort.1.LocalIPAddress| 127.0.0.1    |
      | Device.IP.ActivePort.1.RemoteIPAddress| 0.0.0.0     |
      | Device.IP.ActivePort.1.RemotePort    | 0            |

  Scenario: GET Device.IP.ActivePort.1.LocalPort
    When I GET "Device.IP.ActivePort.1.LocalPort" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.IP.ActivePort.1.Status
    When I GET "Device.IP.ActivePort.1.Status" via rbus
    Then the rbus response should not contain an error
