####################################################################################
# If not stated otherwise in this file or this component's Licenses.txt file the
# following copyright and licenses apply:
#
# Copyright 2024 RDK Management
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

# Source: ../tests/tr69hostif_dhcpv4.py
# Feature: tr69hostif_dhcpv4.feature

Feature: DHCPv4 Parameter GET Handler Validation

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: GET Device.DHCPv4.ClientNumberOfEntries
    When I GET "Device.DHCPv4.ClientNumberOfEntries" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DHCPv4.Client.1.InterfaceReference
    When I GET "Device.DHCPv4.Client.1.InterfaceReference" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DHCPv4.Client.1.DnsServer
    When I GET "Device.DHCPv4.Client.1.DnsServer" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.DHCPv4.Client.1.IPRouters
    When I GET "Device.DHCPv4.Client.1.IPRouters" via rbus
    Then the rbus response should contain an error

