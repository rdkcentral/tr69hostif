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

# Source: src/hostif/profiles/Ethernet/Device_Ethernet_Interface.cpp
# Source: src/hostif/profiles/Ethernet/Device_Ethernet_Interface_Stats.cpp
# Backing: sysfs (/sys/class/net/<if>/...), if_nameindex(), ifconfig
# Build: Always compiled (no conditional flag)

Feature: Device.Ethernet.Interface Parameter Handlers

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system
    And at least one Ethernet interface (eth*) exists on the system

  # --- Interface Count ---

  Scenario: GET Ethernet interface number of entries
    When I GET "Device.Ethernet.InterfaceNumberOfEntries" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a numeric value >= 1

  # --- Interface Properties ---

  Scenario: GET Ethernet interface Enable
    When I GET "Device.Ethernet.Interface.1.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a boolean value
    # Backing: /sys/class/net/<if>/carrier

  Scenario: GET Ethernet interface Status
    When I GET "Device.Ethernet.Interface.1.Status" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain one of "Up" or "Down"
    # Backing: /sys/class/net/<if>/carrier → "Up"/"Down"

  Scenario: GET Ethernet interface Name
    When I GET "Device.Ethernet.Interface.1.Name" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a string starting with "eth"
    # Backing: if_nameindex() resolves nth eth* interface

  Scenario: GET Ethernet interface MACAddress
    When I GET "Device.Ethernet.Interface.1.MACAddress" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a valid MAC address format (XX:XX:XX:XX:XX:XX)
    # Backing: /sys/class/net/<if>/address

  Scenario: GET Ethernet interface MaxBitRate
    When I GET "Device.Ethernet.Interface.1.MaxBitRate" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a numeric value
    # Backing: /sys/class/net/<if>/speed

  Scenario: GET Ethernet interface DuplexMode
    When I GET "Device.Ethernet.Interface.1.DuplexMode" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain one of "Half", "Full", or "Unknown"
    # Backing: /sys/class/net/<if>/duplex

  Scenario: GET Ethernet interface Upstream
    When I GET "Device.Ethernet.Interface.1.Upstream" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a boolean value
    # Backing: /sys/class/net/<if>/carrier → string_to_bool()

  # --- SET+GET Roundtrip ---

  Scenario: SET and GET Ethernet interface Enable
    When I SET "Device.Ethernet.Interface.1.Enable" to "false" as boolean via rbus
    And I GET "Device.Ethernet.Interface.1.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "false"
    # Backing: v_secure_system("ifconfig eth0 down")

  # --- Stub handlers (declared but return NOK) ---

  Scenario: GET Ethernet interface LastChange returns error
    When I GET "Device.Ethernet.Interface.1.LastChange" via rbus
    Then the rbus response should contain an error
    # Known stub: handler returns NOK

  Scenario: GET Ethernet interface LowerLayers returns error
    When I GET "Device.Ethernet.Interface.1.LowerLayers" via rbus
    Then the rbus response should contain an error
    # Known stub: handler returns NOK

  # --- Statistics (sysfs-backed) ---

  Scenario Outline: GET Ethernet interface statistics
    When I GET "<parameter>" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a numeric value >= 0

    Examples:
      | parameter                                                    |
      | Device.Ethernet.Interface.1.Stats.BytesSent                  |
      | Device.Ethernet.Interface.1.Stats.BytesReceived              |
      | Device.Ethernet.Interface.1.Stats.PacketsSent                |
      | Device.Ethernet.Interface.1.Stats.PacketsReceived            |
      | Device.Ethernet.Interface.1.Stats.ErrorsSent                 |
      | Device.Ethernet.Interface.1.Stats.ErrorsReceived             |
      | Device.Ethernet.Interface.1.Stats.DiscardPacketsSent         |
      | Device.Ethernet.Interface.1.Stats.DiscardPacketsReceived     |
      | Device.Ethernet.Interface.1.Stats.UnicastPacketsSent         |
      | Device.Ethernet.Interface.1.Stats.UnicastPacketsReceived     |
      | Device.Ethernet.Interface.1.Stats.MulticastPacketsSent       |
      | Device.Ethernet.Interface.1.Stats.MulticastPacketsReceived   |
      | Device.Ethernet.Interface.1.Stats.BroadcastPacketsSent       |
      | Device.Ethernet.Interface.1.Stats.BroadcastPacketsReceived   |
      | Device.Ethernet.Interface.1.Stats.UnknownProtoPacketsReceived|

  # --- Handler Coverage Table ---

  Scenario: Ethernet interface handler inventory
    Given the Ethernet profile source is analyzed
    Then the following GET handlers should exist
      | Parameter                                                        | Source File                              | Backing              |
      | Device.Ethernet.InterfaceNumberOfEntries                         | Device_Ethernet_Interface.cpp            | if_nameindex()       |
      | Device.Ethernet.Interface.{i}.Enable                             | Device_Ethernet_Interface.cpp            | sysfs carrier        |
      | Device.Ethernet.Interface.{i}.Status                             | Device_Ethernet_Interface.cpp            | sysfs carrier        |
      | Device.Ethernet.Interface.{i}.Name                               | Device_Ethernet_Interface.cpp            | if_nameindex()       |
      | Device.Ethernet.Interface.{i}.MACAddress                         | Device_Ethernet_Interface.cpp            | sysfs address        |
      | Device.Ethernet.Interface.{i}.MaxBitRate                         | Device_Ethernet_Interface.cpp            | sysfs speed          |
      | Device.Ethernet.Interface.{i}.DuplexMode                         | Device_Ethernet_Interface.cpp            | sysfs duplex         |
      | Device.Ethernet.Interface.{i}.Upstream                           | Device_Ethernet_Interface.cpp            | sysfs carrier        |
      | Device.Ethernet.Interface.{i}.LastChange                         | Device_Ethernet_Interface.cpp            | STUB (NOK)           |
      | Device.Ethernet.Interface.{i}.LowerLayers                        | Device_Ethernet_Interface.cpp            | STUB (NOK)           |
      | Device.Ethernet.Interface.{i}.Stats.BytesSent                    | Device_Ethernet_Interface_Stats.cpp      | sysfs tx_bytes       |
      | Device.Ethernet.Interface.{i}.Stats.BytesReceived                | Device_Ethernet_Interface_Stats.cpp      | sysfs rx_bytes       |
      | Device.Ethernet.Interface.{i}.Stats.PacketsSent                  | Device_Ethernet_Interface_Stats.cpp      | sysfs tx_packets     |
      | Device.Ethernet.Interface.{i}.Stats.PacketsReceived              | Device_Ethernet_Interface_Stats.cpp      | sysfs rx_packets     |
      | Device.Ethernet.Interface.{i}.Stats.ErrorsSent                   | Device_Ethernet_Interface_Stats.cpp      | sysfs tx_errors      |
      | Device.Ethernet.Interface.{i}.Stats.ErrorsReceived               | Device_Ethernet_Interface_Stats.cpp      | sysfs rx_errors      |
      | Device.Ethernet.Interface.{i}.Stats.DiscardPacketsSent           | Device_Ethernet_Interface_Stats.cpp      | sysfs tx_dropped     |
      | Device.Ethernet.Interface.{i}.Stats.DiscardPacketsReceived       | Device_Ethernet_Interface_Stats.cpp      | sysfs rx_dropped     |
      | Device.Ethernet.Interface.{i}.Stats.UnicastPacketsSent           | Device_Ethernet_Interface_Stats.cpp      | sysfs tx_packets     |
      | Device.Ethernet.Interface.{i}.Stats.UnicastPacketsReceived       | Device_Ethernet_Interface_Stats.cpp      | sysfs rx_packets     |
      | Device.Ethernet.Interface.{i}.Stats.MulticastPacketsSent         | Device_Ethernet_Interface_Stats.cpp      | sysfs tx_packets     |
      | Device.Ethernet.Interface.{i}.Stats.MulticastPacketsReceived     | Device_Ethernet_Interface_Stats.cpp      | sysfs rx_packets     |
      | Device.Ethernet.Interface.{i}.Stats.BroadcastPacketsSent         | Device_Ethernet_Interface_Stats.cpp      | sysfs tx_packets     |
      | Device.Ethernet.Interface.{i}.Stats.BroadcastPacketsReceived     | Device_Ethernet_Interface_Stats.cpp      | sysfs rx_packets     |
      | Device.Ethernet.Interface.{i}.Stats.UnknownProtoPacketsReceived  | Device_Ethernet_Interface_Stats.cpp      | sysfs rx_dropped     |
