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

Feature: TR-69 Host Interface Bootstrap Initialization

  Background:
    Given the tr69hostif binary is available
    And the system has the required configuration files
    And no previous tr69hostif process is running

  Scenario: Complete Bootstrap Initialization Sequence
    When the tr69hostif process is started
    Then the process should be running within 5 seconds
    And the startup message should appear in logs
    And no fatal errors should be logged during startup

  Scenario: Command Line Processing and Signal Setup
    Given the tr69hostif process is started
    When command line arguments are processed
    Then no command line parsing errors should occur
    And signal handlers should be set up successfully
    And shutdown thread should be created without errors

  Scenario: Configuration Manager Initialization
    Given the tr69hostif process is running
    When the configuration manager initializes
    Then the manager configuration file should be loaded
    And parameter-to-manager mappings should be created
    And no configuration manager errors should be logged

  Scenario: RFC and Legacy Mode Detection
    Given the tr69hostif process is running
    When RFC initialization occurs
    Then RFC properties file should be accessible
    And legacy RFC mode should be detected correctly
    And appropriate server mode should be selected

  Scenario: IARM Bus Communication Setup
    Given the tr69hostif process is running
    When IARM interface initialization occurs
    Then IARM bus should be initialized successfully
    And IARM connection should be established
    And profile managers should be registered

  Scenario: Data Model Processing
    Given the tr69hostif process is running
    When data model processing occurs
    Then device profile should be determined from properties
    And appropriate XML data models should be merged
    And merged data model should be loaded successfully
    And no data model errors should be logged

  Scenario: Server Thread Initialization
    Given the tr69hostif process is running
    When server threads are created
    Then JSON handler thread should start successfully
    And HTTP server thread should start if not in legacy mode
    And update handler should be initialized
    And no thread creation errors should occur

  Scenario: WebPA/Parodus Integration
    Given the tr69hostif process is running
    And Parodus support is enabled
    When WebPA initialization occurs
    Then Parodus configuration should be loaded
    And Parodus connection thread should be created
    And WebPA notification configuration should be set

  Scenario: RBUS Interface Setup
    Given the tr69hostif process is running
    When RBUS DML provider initializes
    Then RBUS interface should be opened successfully
    And data elements should be registered
    And parameter list should be loaded
    And no RBUS initialization errors should occur

  Scenario: Bootstrap Store and RFC Variable Initialization
    Given the tr69hostif process is running
    When bootstrap and RFC stores initialize
    Then bootstrap properties file should be loaded
    And RFC variable file should be accessible
    And partner defaults should be loaded if available
    And default values should be initialized

  Scenario: Service Ready Notification
    Given all bootstrap components are initialized
    When the service completes initialization
    Then SystemD ready notification should be sent
    And device manageability notification should be triggered
    And main event loop should be started
    And process should remain stable

  Scenario: Bootstrap Health Verification
    Given the tr69hostif process completed bootstrap
    When bootstrap health is assessed
    Then at least 70% of success indicators should be present
    And critical error messages should be absent
    And process should be stable for at least 10 seconds
    And basic functionality should be operational

  Scenario: Configuration File Validation
    Given the tr69hostif process is running
    When configuration files are validated
    Then manager configuration file should exist and be readable
    And essential manager mappings should be present
    And RFC configuration should be accessible
    And data model files should be available

  Scenario: Process Stability and Error Handling
    Given the tr69hostif process completed bootstrap
    When the process runs for an extended period
    Then the process should remain stable
    And no crash indicators should appear in logs
    And the process ID should remain constant
    And memory usage should be stable

  Scenario: Functional Verification After Bootstrap
    Given the tr69hostif process completed bootstrap successfully
    When basic functionality is tested
    Then RBUS interface should be responsive
    And HTTP interface should be accessible if enabled
    And parameter queries should work correctly
    And no functional errors should occur

  Scenario Outline: Bootstrap Phase Verification
    Given the tr69hostif process is starting
    When <phase_name> phase executes
    Then <phase_name> should complete within <timeout> seconds
    And success indicators for <phase_name> should be logged
    And error indicators for <phase_name> should not appear

    Examples:
      | phase_name              | timeout |
      | Command Line Processing | 2       |
      | Signal Handler Setup    | 3       |
      | Configuration Manager   | 5       |
      | IARM Initialization     | 8       |
      | Data Model Processing   | 10      |
      | Server Threads         | 8       |
      | RBUS Interface         | 10      |
      | Service Ready          | 5       |

  Scenario: Bootstrap Rollback on Critical Failure
    Given the tr69hostif process encounters a critical bootstrap failure
    When the failure is detected
    Then the process should exit gracefully
    And appropriate error messages should be logged
    And no zombie processes should remain
    And system resources should be cleaned up

  Scenario: Bootstrap with Missing Optional Components
    Given some optional bootstrap components are not available
    When the tr69hostif process starts
    Then bootstrap should continue with available components
    And warnings should be logged for missing optional components
    And core functionality should remain operational
    And the process should complete initialization successfully