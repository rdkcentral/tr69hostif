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

# Source: ../tests/tr69hostif_datamodel_export.py

Feature: DeviceInfo Data Model Export

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario Outline: Disabling GetDataModelList does not generate an export
    Given "/opt/secure/RFC/supported_datamodel_list" is <initial_state>
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.GetDataModelList" to "false" as boolean via rbus
    Then the rbus SET should succeed
    And GET of the parameter should return "false"
    And "/opt/secure/RFC/supported_datamodel_list" should remain <initial_state> and unchanged for 1 second

    Examples:
      | initial_state |
      | absent        |
      | present       |

  Scenario: Repeated GetDataModelList triggers export the loaded data model
    Given "/opt/secure/RFC/supported_datamodel_list" is absent
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.GetDataModelList" to "true" as boolean via rbus
    Then the rbus SET should succeed
    And GET of the parameter should return "true"
    And "/opt/secure/RFC/supported_datamodel_list" should be generated within 10 seconds
    And the export header should be "name|dataType|defaultValue|access|description"
    And every parameter row should contain five pipe-delimited fields with backslash escaping
    And parameter names should be sorted
    And the export should include table templates containing "{i}"
    And the export should include GetDataModelList with type "boolean", default "false", access "readWrite" and its description
    And the export permissions should be "0644"
    When I remove the export and SET the parameter to "true" again without an intervening SET to "false"
    Then a new export should satisfy the same checks
