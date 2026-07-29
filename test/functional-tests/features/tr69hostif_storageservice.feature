####################################################################################
# If not stated otherwise in this file or this component's Licenses file the
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

# Source: src/hostif/profiles/StorageService/

Feature: Device.StorageService Parameter Handlers

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: GET StorageServiceNumberOfEntries
    When I GET "Device.Services.StorageServiceNumberOfEntries" via rbus
    Then the rbus response should not contain an error

  Scenario: GET PhysicalMedium.1.Alias (read-only/not implemented)
    When I GET "Device.Services.StorageService.1.PhysicalMedium.1.Alias" via rbus
    Then the rbus response should contain an error

  Scenario: GET PhysicalMedium.1.Name (implemented)
    When I GET "Device.Services.StorageService.1.PhysicalMedium.1.Name" via rbus
    Then the rbus response should not contain an error

  Scenario: GET PhysicalMedium.1.Vendor (read-only/not implemented)
    When I GET "Device.Services.StorageService.1.PhysicalMedium.1.Vendor" via rbus
    Then the rbus response should contain an error

  Scenario: GET PhysicalMedium.1.Model (read-only/not implemented)
    When I GET "Device.Services.StorageService.1.PhysicalMedium.1.Model" via rbus
    Then the rbus response should contain an error

  Scenario: GET PhysicalMedium.1.SerialNumber (read-only/not implemented)
    When I GET "Device.Services.StorageService.1.PhysicalMedium.1.SerialNumber" via rbus
    Then the rbus response should contain an error

  Scenario: GET PhysicalMedium.1.FirmwareVersion (read-only/not implemented)
    When I GET "Device.Services.StorageService.1.PhysicalMedium.1.FirmwareVersion" via rbus
    Then the rbus response should contain an error

  Scenario: GET PhysicalMedium.1.ConnectionType (read-only/not implemented)
    When I GET "Device.Services.StorageService.1.PhysicalMedium.1.ConnectionType" via rbus
    Then the rbus response should contain an error

  Scenario: GET PhysicalMedium.1.Removable (read-only/not implemented)
    When I GET "Device.Services.StorageService.1.PhysicalMedium.1.Removable" via rbus
    Then the rbus response should contain an error

  Scenario: GET PhysicalMedium.1.Status (read-only/not implemented)
    When I GET "Device.Services.StorageService.1.PhysicalMedium.1.Status" via rbus
    Then the rbus response should contain an error

  Scenario: GET PhysicalMedium.1.Uptime (read-only/not implemented)
    When I GET "Device.Services.StorageService.1.PhysicalMedium.1.Uptime" via rbus
    Then the rbus response should contain an error

  Scenario: GET PhysicalMedium.1.SmartCapable (read-only/implemented)
    When I GET "Device.Services.StorageService.1.PhysicalMedium.1.SmartCapable" via rbus
    Then the rbus response should not contain an error

  Scenario: GET PhysicalMedium.1.Health (read-only/implemented)
    When I GET "Device.Services.StorageService.1.PhysicalMedium.1.Health" via rbus
    Then the rbus response should not contain an error

  Scenario: GET PhysicalMedium.1.HotSwappable (read-only/not implemented)
    When I GET "Device.Services.StorageService.1.PhysicalMedium.1.HotSwappable" via rbus
    Then the rbus response should contain an error

  Scenario: GET PhysicalMediumNumberOfEntries
    When I GET "Device.Services.StorageService.1.PhysicalMediumNumberOfEntries" via rbus
    Then the rbus response should not contain an error
