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

Feature: IPRemoteSupport and Syndication Parameter GET/SET via rbus

  These scenarios document the IPRemoteSupport and Syndication parameter
  handlers covered in tr69hostif_ipremotesupport.py.

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: SET Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable" to "true" as boolean via rbus
    Then the rbus response should indicate success

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "true"

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.IPAddr
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.IPAddr" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "unknown"

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.MACAddr
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.MACAddr" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "unknown"

  Scenario: GET Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "testpartner"
