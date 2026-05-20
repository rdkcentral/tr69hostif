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

# Source: src/hostif/profiles/wifi/Device_WiFi.cpp
# Source: src/hostif/profiles/wifi/Device_WiFi_SSID.cpp
# Source: src/hostif/profiles/wifi/Device_WiFi_EndPoint.cpp
# Source: src/hostif/profiles/wifi/Device_WiFi_EndPoint_Security.cpp
# Source: src/hostif/profiles/DeviceInfo/Device_DeviceInfo.cpp
# Backing: Thunder JSON-RPC plugins (org.rdk.NetworkManager, org.rdk.AuthService, org.rdk.Account, org.rdk.MigrationPreparer)
# Build: WiFi conditional (WITH_WIFI_PROFILE); DeviceInfo always compiled

Feature: Thunder Plugin-Backed TR-181 Parameter Handlers

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system
    And Thunder plugins are activated and responding

  # =========================================================================
  # org.rdk.NetworkManager — WiFi SSID parameters
  # =========================================================================

  Scenario: GET WiFi SSID BSSID via Thunder NetworkManager
    When I GET "Device.WiFi.SSID.1.BSSID" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a valid MAC address format
    # Thunder: org.rdk.NetworkManager.GetConnectedSSID → field "bssid"

  Scenario: GET WiFi SSID name via Thunder NetworkManager
    When I GET "Device.WiFi.SSID.1.SSID" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a non-empty string
    # Thunder: org.rdk.NetworkManager.GetConnectedSSID → field "ssid"

  Scenario: GET WiFi SSID.Name via Thunder NetworkManager
    When I GET "Device.WiFi.SSID.1.Name" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a non-empty string
    # Thunder: org.rdk.NetworkManager.GetConnectedSSID → field "ssid"

  Scenario: GET WiFi SSID MACAddress via Thunder NetworkManager
    When I GET "Device.WiFi.SSID.1.MACAddress" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a valid MAC address format
    # Thunder: org.rdk.NetworkManager.GetAvailableInterfaces → WIFI entry "mac"

  Scenario: GET WiFi SSID Enable via Thunder NetworkManager
    When I GET "Device.WiFi.SSID.1.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a boolean value
    # Thunder: org.rdk.NetworkManager.GetAvailableInterfaces → WIFI entry "enabled"

  Scenario: GET WiFi SSID Status via Thunder NetworkManager
    When I GET "Device.WiFi.SSID.1.Status" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain one of "Up", "Down", "Error", "Disabled"
    # Thunder: org.rdk.NetworkManager.GetWifiState → field "state"

  # =========================================================================
  # org.rdk.NetworkManager — WiFi EndPoint parameters
  # =========================================================================

  Scenario: GET WiFi EndPoint Enable via Thunder NetworkManager
    When I GET "Device.WiFi.EndPoint.1.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a boolean value
    # Thunder: org.rdk.NetworkManager.GetAvailableInterfaces → WIFI entry "enabled"

  Scenario: GET WiFi EndPoint Status via Thunder NetworkManager
    When I GET "Device.WiFi.EndPoint.1.Status" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain one of "Enabled" or "Disabled"
    # Thunder: org.rdk.NetworkManager.GetAvailableInterfaces → derived string

  Scenario: GET WiFi EndPoint SignalStrength via Thunder NetworkManager
    When I GET "Device.WiFi.EndPoint.1.Stats.SignalStrength" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a numeric value
    # Thunder: org.rdk.NetworkManager.GetConnectedSSID → field "strength"

  Scenario: GET WiFi EndPoint Security ModesEnabled via Thunder NetworkManager
    When I GET "Device.WiFi.EndPoint.1.Security.ModesEnabled" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a non-empty string
    # Thunder: org.rdk.NetworkManager.GetConnectedSSID → field "security"

  # =========================================================================
  # org.rdk.NetworkManager — WiFi top-level Enable
  # =========================================================================

  Scenario: GET WiFi Enable via Thunder NetworkManager
    When I GET "Device.WiFi.X_RDKCENTRAL-COM_WiFiEnable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a boolean value
    # Thunder: org.rdk.NetworkManager.GetAvailableInterfaces → WIFI entry "enabled"

  Scenario: SET and GET WiFi Enable via Thunder NetworkManager
    When I SET "Device.WiFi.Enable" to "false" as boolean via rbus
    And I GET "Device.WiFi.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "false"
    # Thunder: org.rdk.NetworkManager.SetInterfaceState

  # =========================================================================
  # org.rdk.NetworkManager — DeviceInfo STB IP
  # =========================================================================

  Scenario: GET STB IP via Thunder NetworkManager
    When I GET "Device.DeviceInfo.X_COMCAST-COM_STB_IP" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a valid IP address format
    # Thunder: org.rdk.NetworkManager.GetPrimaryInterface → GetIPSettings → "ipaddress"

  # =========================================================================
  # org.rdk.AuthService
  # =========================================================================

  Scenario: GET Experience via Thunder AuthService
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_Experience" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a non-empty string
    # Thunder: org.rdk.AuthService.getExperience

  Scenario: SET and GET Syndication PartnerId via Thunder AuthService
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId" to "comcast" as string via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "comcast"
    # Thunder: org.rdk.AuthService.setPartnerId

  Scenario: GET AccountID via Thunder AuthService
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a non-empty string
    # Thunder: org.rdk.AuthService.getServiceAccountId

  # =========================================================================
  # org.rdk.Account
  # =========================================================================

  Scenario: GET HotelCheckout LastResetTime via Thunder Account
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xAccount.HotelCheckout.LastResetTime" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a string value
    # Thunder: org.rdk.Account.getLastCheckoutResetTime

  Scenario: GET HotelCheckout Status via Thunder Account
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xAccount.HotelCheckout.Status" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a string value
    # Thunder: org.rdk.Account.getLastCheckoutResetTime

  # =========================================================================
  # org.rdk.MigrationPreparer
  # =========================================================================

  Scenario: GET MigrationReady via Thunder MigrationPreparer
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_Migration.MigrationReady" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a boolean value
    # Thunder: org.rdk.MigrationPreparer.getComponentReadiness

  # =========================================================================
  # org.rdk.UserSettings — Used as guard in ReverseSSH SET
  # =========================================================================

  Scenario: SET ReverseSSH Trigger exercises Thunder UserSettings guard
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger" to "start" as string via rbus
    Then the log should contain evidence of org.rdk.UserSettings.getPrivacyMode invocation
    # Thunder: org.rdk.UserSettings.getPrivacyMode (called as guard before SSH action)

  # =========================================================================
  # Thunder Plugin Coverage Table
  # =========================================================================

  Scenario: Thunder-backed parameter handler inventory
    Given all Thunder-backed source files are analyzed
    Then the following parameters should use Thunder plugins
      | TR-181 Parameter                                                              | Thunder Plugin.Method                              | Source File                    |
      | Device.DeviceInfo.X_COMCAST-COM_STB_IP                                        | org.rdk.NetworkManager.GetPrimaryInterface/GetIPSettings | Device_DeviceInfo.cpp    |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId                      | org.rdk.AuthService.setPartnerId                   | Device_DeviceInfo.cpp          |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_Experience                                 | org.rdk.AuthService.getExperience                  | Device_DeviceInfo.cpp          |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID           | org.rdk.AuthService.getServiceAccountId            | Device_DeviceInfo.cpp          |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xAccount.HotelCheckout.LastResetTime        | org.rdk.Account.getLastCheckoutResetTime           | Device_DeviceInfo.cpp          |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xAccount.HotelCheckout.Status               | org.rdk.Account.getLastCheckoutResetTime           | Device_DeviceInfo.cpp          |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_Migration.MigrationReady                    | org.rdk.MigrationPreparer.getComponentReadiness    | Device_DeviceInfo.cpp          |
      | Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger | org.rdk.UserSettings.getPrivacyMode           | Device_DeviceInfo.cpp          |
      | Device.WiFi.Enable                                                             | org.rdk.NetworkManager.SetInterfaceState           | Device_WiFi.cpp                |
      | Device.WiFi.X_RDKCENTRAL-COM_WiFiEnable                                       | org.rdk.NetworkManager.GetAvailableInterfaces      | Device_WiFi.cpp                |
      | Device.WiFi.SSID.{i}.BSSID                                                    | org.rdk.NetworkManager.GetConnectedSSID            | Device_WiFi_SSID.cpp           |
      | Device.WiFi.SSID.{i}.SSID                                                     | org.rdk.NetworkManager.GetConnectedSSID            | Device_WiFi_SSID.cpp           |
      | Device.WiFi.SSID.{i}.Name                                                     | org.rdk.NetworkManager.GetConnectedSSID            | Device_WiFi_SSID.cpp           |
      | Device.WiFi.SSID.{i}.MACAddress                                               | org.rdk.NetworkManager.GetAvailableInterfaces      | Device_WiFi_SSID.cpp           |
      | Device.WiFi.SSID.{i}.Enable                                                   | org.rdk.NetworkManager.GetAvailableInterfaces      | Device_WiFi_SSID.cpp           |
      | Device.WiFi.SSID.{i}.Status                                                   | org.rdk.NetworkManager.GetWifiState                | Device_WiFi_SSID.cpp           |
      | Device.WiFi.EndPoint.{i}.Enable                                               | org.rdk.NetworkManager.GetAvailableInterfaces      | Device_WiFi_EndPoint.cpp       |
      | Device.WiFi.EndPoint.{i}.Status                                               | org.rdk.NetworkManager.GetAvailableInterfaces      | Device_WiFi_EndPoint.cpp       |
      | Device.WiFi.EndPoint.{i}.Stats.SignalStrength                                  | org.rdk.NetworkManager.GetConnectedSSID            | Device_WiFi_EndPoint.cpp       |
      | Device.WiFi.EndPoint.{i}.Security.ModesEnabled                                | org.rdk.NetworkManager.GetConnectedSSID            | Device_WiFi_EndPoint_Security.cpp |
