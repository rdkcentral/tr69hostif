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

# Source: test/functional-tests/tests/tr69hostif_webpa.py
# Mapped feature: test/functional-tests/features/tr69hostif_webpa.feature

Feature: WebPA Parameter SET/GET via Mock Parodus

  These tests validate WebPA/Parodus communication by executing the mock
  parodus binary with JSON payloads and verifying the responses in the
  parodus log file (/opt/logs/parodus.log).

  Background:
    Given the tr69hostif daemon is running and initialized
    And the mock parodus executable is available at "/usr/local/bin/parodus"

  # =====================================================================
  # XconfUrl SET / GET
  # =====================================================================

  @order-29
  Scenario: SET XconfUrl via WebPA
    When I send a WebPA SET payload:
      """
      {"command":"SET","parameters":[{"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.XconfUrl","dataType":0,"value":"https://mockurl/featurecontrol/getSettings"}]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'

  @order-30
  Scenario: GET XconfUrl via WebPA
    When I send a WebPA GET payload:
      """
      {"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.XconfUrl"]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'
    And the parodus log should contain '"value":"https://mockurl/featurecontrol/getSettings"'

  # =====================================================================
  # FWUpdate AutoExcluded SET / GET
  # =====================================================================

  @order-31
  Scenario: SET FWUpdate AutoExcluded Enable via WebPA
    When I send a WebPA SET payload:
      """
      {"command":"SET","parameters":[{"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.AutoExcluded.Enable","dataType":3,"value":"false"}]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'

  @order-32
  Scenario: GET FWUpdate AutoExcluded Enable via WebPA
    When I send a WebPA GET payload:
      """
      {"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.AutoExcluded.Enable"]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'
    And the parodus log should contain '"value":"false"'

  # =====================================================================
  # LogUpload LogServerUrl SET / GET
  # =====================================================================

  @order-33
  Scenario: SET LogServerUrl via WebPA
    When I send a WebPA SET payload:
      """
      {"command":"SET","parameters":[{"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.LogServerUrl","dataType":0,"value":"logs.mock.tv"}]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'

  @order-34
  Scenario: GET LogServerUrl via WebPA
    When I send a WebPA GET payload:
      """
      {"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.LogServerUrl"]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'
    And the parodus log should contain '"value":"logs.mock.tv"'

  # =====================================================================
  # GET-only WebPA Parameters
  # =====================================================================

  @order-35
  Scenario: GET SWDLSpLimit LowSpeed via WebPA
    When I send a WebPA GET payload:
      """
      {"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.LowSpeed"]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'
    And the parodus log should contain '"value":"12800"'

  @order-36
  Scenario: GET FirmwareDownloadProtocol via WebPA
    When I send a WebPA GET payload:
      """
      {"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol"]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'
    And the parodus log should contain '"value":"http"'

  @order-37
  Scenario: GET FirmwareDownloadStatus via WebPA
    When I send a WebPA GET payload:
      """
      {"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus"]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'
    And the parodus log should contain "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus"

  @order-38
  Scenario: GET FirmwareDownloadURL via WebPA
    When I send a WebPA GET payload:
      """
      {"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL"]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'
    And the parodus log should contain '"value":"https://mockserver.tv/Images"'

  @order-39
  Scenario: GET FirmwareToDownload via WebPA
    When I send a WebPA GET payload:
      """
      {"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload"]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'
    And the parodus log should contain '"value":"TESTIMAGE_DEV.bin"'

  @order-40
  Scenario: GET FirmwareUpdateState via WebPA
    When I send a WebPA GET payload:
      """
      {"command":"GET","names":["Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareUpdateState"]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'
    And the parodus log should contain "FirmwareUpdateState"

  # =====================================================================
  # Wildcard GET
  # =====================================================================

  @order-41
  Scenario: GET wildcard Device.DeviceInfo. via WebPA
    When I send a WebPA GET payload:
      """
      {"command":"GET","names":["Device.DeviceInfo."]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'

  # =====================================================================
  # Firmware Upgrade SET Sequence
  # =====================================================================

  @order-42
  Scenario: SET FirmwareDownloadProtocol for upgrade via WebPA
    When I send a WebPA SET payload:
      """
      {"command":"SET","parameters":[{"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol","dataType":0,"value":"http"}]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'

  @order-43
  Scenario: SET FirmwareDownloadURL for upgrade via WebPA
    When I send a WebPA SET payload:
      """
      {"command":"SET","parameters":[{"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL","dataType":0,"value":"https://mockserver.tv/Images"}]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'

  @order-44
  Scenario: SET FirmwareToDownload for upgrade via WebPA
    When I send a WebPA SET payload:
      """
      {"command":"SET","parameters":[{"name":"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload","dataType":0,"value":"TESTIMAGE_DEV.bin"}]}
      """
    Then the parodus mock should exit with code 0
    And the parodus log should contain '"statusCode":200'
    And the parodus log should contain '"message":"Success"'
