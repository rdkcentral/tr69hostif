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

# Source: ../tests/test_handlers_communications.py
# Feature: tr69hostif_handlers_communications.feature

Feature: TR-181 Parameter Handler SET/GET via rbus DML

  The tr69hostif daemon exposes TR-181 data model parameters via rbus.
  These tests verify SET and GET roundtrips for DeviceInfo, Time, RFC,
  Non-RFC, Bootstrap, and Chrony parameter handlers using rbuscli.

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  # =====================================================================
  # DeviceInfo Handler
  # =====================================================================

  @order-19
  Scenario: DeviceInfo RFC Telemetry Version set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Telemetry.Version" to "2.2.1" as string via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Telemetry.Version" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "2.2.1"

  # =====================================================================
  # Device.Time Handler
  # =====================================================================

  @order-20
  Scenario: DHCPv6Client enable set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DHCPv6Client.Enable" to "true" as boolean via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DHCPv6Client.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "true"

  @order-20
  Scenario: NTPServer1 set and get
    When I SET "Device.Time.NTPServer1" to "3.236.252.118" as string via rbus
    And I GET "Device.Time.NTPServer1" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "3.236.252.118"

  # =====================================================================
  # RFC Parameter Handlers
  # =====================================================================

  @order-21
  Scenario: RFC HdmiCecSink CECVersion set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.HdmiCecSink.CECVersion" to "1.4" as string via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.HdmiCecSink.CECVersion" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "1.4"

  @order-21
  Scenario: RFC SWDLSpLimit Enable set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.Enable" to "true" as boolean via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "true"

  @order-21
  Scenario: RFC SWDLSpLimit TopSpeed set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.TopSpeed" to "1280000" as int via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.TopSpeed" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "1280000"

  @order-21
  Scenario: RFC eMMCFirmware Version set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.eMMCFirmware.Version" to "08140310" as string via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.eMMCFirmware.Version" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "08140310"

  @order-21
  Scenario: RFC IncrementalCDL Enable set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IncrementalCDL.Enable" to "true" as boolean via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IncrementalCDL.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "true"

  # =====================================================================
  # Non-RFC Parameter Handlers
  # =====================================================================

  @order-22
  Scenario: IPRemoteSupport Enable set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable" to "false" as boolean via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "false"

  @order-22
  Scenario: ForwardSSH Enable set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable" to "false" as boolean via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "false"

  @order-22
  Scenario: FirmwareDownloadDeferReboot set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot" to "false" as boolean via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "false"

  @order-22
  Scenario: FirmwareDownloadCompletedNotification set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadCompletedNotification" to "false" as boolean via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadCompletedNotification" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "false"

  # =====================================================================
  # Bootstrap Parameter Handlers
  # =====================================================================

  @order-23
  Scenario: Bootstrap PartnerProductName set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerProductName" to "TestProduct123" as string via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerProductName" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "TestProduct123"

  @order-23
  Scenario: Bootstrap NetflixESNprefix set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.NetflixESNprefix" to "TESTESN" as string via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.NetflixESNprefix" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "TESTESN"

  @order-23
  Scenario: Bootstrap PartnerName set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerName" to "Test" as string via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerName" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "Test"

  @order-23
  Scenario: Bootstrap SsrUrl set and get
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.SsrUrl" to "https://ssr.test.tv" as string via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.SsrUrl" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "https://ssr.test.tv"

  # =====================================================================
  # Bootstrap File Persistence
  # =====================================================================

  @order-24
  Scenario: Bootstrap files created and parameter persisted to disk
    Then the file "/opt/secure/RFC/bootstrap.ini" should exist
    And the file "/opt/secure/RFC/bootstrap.journal" should exist
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerProductName" to "TestProduct123" as string via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerProductName" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "TestProduct123"
    And the file "/opt/secure/RFC/bootstrap.ini" should contain "TestProduct123"

  # =====================================================================
  # Chrony Time Handlers
  # =====================================================================

  @order-25
  Scenario: Chrony Enable set and get with file creation
    Given the file "/opt/secure/RFC/chrony/chronyd_enabled" does not exist
    When I SET "Device.Time.Chrony.Enable" to "true" as boolean via rbus
    And I GET "Device.Time.Chrony.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "true"
    And the file "/opt/secure/RFC/chrony/chronyd_enabled" should exist

  @order-26
  Scenario: Chrony Makestep set and get with file creation
    Given the file "/opt/secure/RFC/chrony/ntp_maxstep" does not exist
    When I SET "Device.Time.Chrony.Makestep" to "1.0,3" as string via rbus
    And I GET "Device.Time.Chrony.Makestep" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "1.0,3"
    And the file "/opt/secure/RFC/chrony/ntp_maxstep" should exist
    And the file "/opt/secure/RFC/chrony/ntp_maxstep" should contain "1.0,3"

  @order-27
  Scenario: Chrony NTPServer Settings set and get with file creation
    Given the file "/opt/secure/RFC/chrony/ntp_server1_settings" does not exist
    When I SET "Device.Time.Chrony.NTPServer.1.Settings" to "server,0,true,6,12" as string via rbus
    And I GET "Device.Time.Chrony.NTPServer.1.Settings" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "server,0,true,6,12"
    And the file "/opt/secure/RFC/chrony/ntp_server1_settings" should exist
