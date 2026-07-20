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

# Source: ../tests/tr69hostif_ethernet_handlers.py
# Feature: tr69hostif_ethernet_handlers.feature

Feature: Ethernet Interface and Stats GET/SET Handler Validation

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: GET Device.Ethernet.InterfaceNumberOfEntries
    When I GET "Device.Ethernet.InterfaceNumberOfEntries" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "1"

  Scenario: GET Device.Ethernet.Interface.1.Name
    When I GET "Device.Ethernet.Interface.1.Name" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "eth0"

  Scenario: SET Device.Ethernet.Interface.1.Enable
    When I SET "Device.Ethernet.Interface.1.Enable" to "true" as boolean via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.Enable
    When I GET "Device.Ethernet.Interface.1.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "true"

  Scenario: SET Device.Ethernet.Interface.1.Alias returns error
    When I SET "Device.Ethernet.Interface.1.Alias" to "eth0_alias" as string via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.Ethernet.Interface.1.Alias returns error
    When I GET "Device.Ethernet.Interface.1.Alias" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.Ethernet.Interface.1.Status
    When I GET "Device.Ethernet.Interface.1.Status" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "Up"

  Scenario: GET Device.Ethernet.Interface.1.LastChange returns error
    When I GET "Device.Ethernet.Interface.1.LastChange" via rbus
    Then the rbus response should contain an error

  Scenario: SET Device.Ethernet.Interface.1.LowerLayers returns error
    When I SET "Device.Ethernet.Interface.1.LowerLayers" to "eth0" as string via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.Ethernet.Interface.1.LowerLayers returns error
    When I GET "Device.Ethernet.Interface.1.LowerLayers" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.Ethernet.Interface.1.Upstream
    When I GET "Device.Ethernet.Interface.1.Upstream" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "true"

  Scenario: GET Device.Ethernet.Interface.1.MACAddress
    When I GET "Device.Ethernet.Interface.1.MACAddress" via rbus
    Then the rbus response should not contain an error

  Scenario: SET Device.Ethernet.Interface.1.MaxBitRate
    When I SET "Device.Ethernet.Interface.1.MaxBitRate" to "1000" as int via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.MaxBitRate
    When I GET "Device.Ethernet.Interface.1.MaxBitRate" via rbus
    Then the rbus response should not contain an error

  Scenario: SET Device.Ethernet.Interface.1.DuplexMode returns error
    When I SET "Device.Ethernet.Interface.1.DuplexMode" to "full" as string via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.Ethernet.Interface.1.DuplexMode
    When I GET "Device.Ethernet.Interface.1.DuplexMode" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "full"

  Scenario: GET Device.Ethernet.Interface.1.Stats.BytesSent
    When I GET "Device.Ethernet.Interface.1.Stats.BytesSent" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.Stats.BytesReceived
    When I GET "Device.Ethernet.Interface.1.Stats.BytesReceived" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.Stats.PacketsSent
    When I GET "Device.Ethernet.Interface.1.Stats.PacketsSent" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.Stats.PacketsReceived
    When I GET "Device.Ethernet.Interface.1.Stats.PacketsReceived" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.Stats.ErrorsSent
    When I GET "Device.Ethernet.Interface.1.Stats.ErrorsSent" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.Stats.ErrorsReceived
    When I GET "Device.Ethernet.Interface.1.Stats.ErrorsReceived" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.Stats.UnicastPacketsSent
    When I GET "Device.Ethernet.Interface.1.Stats.UnicastPacketsSent" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.Stats.UnicastPacketsReceived
    When I GET "Device.Ethernet.Interface.1.Stats.UnicastPacketsReceived" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.Stats.DiscardPacketsSent
    When I GET "Device.Ethernet.Interface.1.Stats.DiscardPacketsSent" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.Stats.DiscardPacketsReceived
    When I GET "Device.Ethernet.Interface.1.Stats.DiscardPacketsReceived" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.Stats.MulticastPacketsSent
    When I GET "Device.Ethernet.Interface.1.Stats.MulticastPacketsSent" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.Stats.MulticastPacketsReceived
    When I GET "Device.Ethernet.Interface.1.Stats.MulticastPacketsReceived" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.Stats.BroadcastPacketsSent
    When I GET "Device.Ethernet.Interface.1.Stats.BroadcastPacketsSent" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.Ethernet.Interface.1.Stats.BroadcastPacketsReceived
    When I GET "Device.Ethernet.Interface.1.Stats.BroadcastPacketsReceived" via rbus
    Then the rbus response should not contain an error
