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

Feature: WebPA DNSText and RDK Download Manager/Remote Debugger Parameters SET/GET via rbus

  These scenarios document the WebPA DNSText and RDK management parameter handlers
  for download manager and remote debugger configuration.

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: SET Device.X_RDK_WebPA_DNSText.URL
    When I SET "Device.X_RDK_WebPA_DNSText.URL" to "testurl.com" as string via rbus
    Then the rbus response should indicate success

  Scenario: GET Device.X_RDK_WebPA_DNSText.URL
    When I GET "Device.X_RDK_WebPA_DNSText.URL" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "testurl.com"

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.InstallPackage
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.InstallPackage" to "testpackage" as string via rbus
    Then the rbus response should indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.DownloadStatus
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.DownloadStatus" to "true" as boolean via rbus
    Then the rbus response should indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.Enable
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.Enable" to "true" as boolean via rbus
    Then the rbus response should indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType" to "test" as string via rbus
    Then the rbus response should indicate success

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.WebCfgData
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.WebCfgData" to "testcfgdata" as string via rbus
    Then the rbus response should indicate success

  Scenario: GET Device.X_RDK_WebPA_Server.URL
    When I GET "Device.X_RDK_WebPA_Server.URL" via rbus
    Then the rbus response should not contain an error

  Scenario: GET Device.X_RDK_WebPA_TokenServer.URL
    When I GET "Device.X_RDK_WebPA_TokenServer.URL" via rbus
    Then the rbus response should not contain an error
