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

# Source: ../tests/test_bootup_sequence.py
# Feature: tr69hostif_bootup_sequence.feature

Feature: tr69hostif Daemon Bootup Sequence

  The tr69hostif daemon must initialize all subsystems in the correct order
  during startup. These tests verify each initialization stage by scraping
  the daemon log file for expected success and error messages.

  Background:
    Given the tr69hostif binary has been invoked
    And the tr69hostif process is running as a daemon
    And the process has been active for at least 10 seconds

  # ---------- Server Thread Initialization ----------

  @order-1
  Scenario: JSON handler thread initialization
    When the daemon completes initialization
    Then the log should contain "SERVER: Started server successfully."

  @order-2
  Scenario: HTTP server thread initialization
    When the daemon completes initialization
    Then the log should contain "SERVER: Started server successfully."

  # ---------- Parodus / WebPA ----------

  @order-3
  Scenario: Parodus connection initialization
    When the daemon completes initialization
    Then the log should contain "Initiating Connection with PARODUS success.."

  # ---------- Thread Creation ----------

  @order-4
  Scenario: No thread creation failures
    When the daemon completes initialization
    Then the log should NOT contain "pthread_create() failed"

  # ---------- rbus DML Registration ----------

  @order-5
  Scenario: rbus DML provider initialization
    When the daemon completes initialization
    Then the log should NOT contain "[rbusdml] Failed to initialized, rbus_checkStatus() returned with status"
    And the log should NOT contain "consumer: rbus_open failed"
    And the log should contain "[rbusdml]Successfully get the complete parameter list"
    And the log should contain "rbus_regDataElements registered successfully"

  # ---------- Config Manager ----------

  @order-6
  Scenario: Config manager initialization succeeds
    When the daemon completes initialization
    Then the log should NOT contain "Failed to hostIf_initalize_ConfigManger()"

  # ---------- IARM Bus ----------

  @order-7
  Scenario: IARM bus initialization and PwrContInterface thread
    When the daemon completes initialization
    Then the log should contain "Success 'IARM_Bus_Init(tr69HostIfMgr)'"
    And the log should contain "created getPwrContInterface thread.."

  # ---------- Power Controller Thread ----------

  @order-8
  Scenario: PwrContInterface thread creation
    When the daemon completes initialization
    Then the log should contain "created getPwrContInterface thread.."

  # ---------- Data Model ----------

  @order-9
  Scenario: Data model XML merge pipeline
    When the daemon completes initialization
    Then the log should contain "Entering data model merge process"
    And the log should contain "Merged XML files successfully into /tmp/data-model.xml"
    And the log should contain "Successfully merged Data Model"
    And the log should contain "Merging XML files for profile:"

  @order-10
  Scenario: Data model initialization
    When the daemon completes initialization
    Then the log should contain "Successfully initialize Data Model"

  # ---------- Ethernet Client ----------

  @order-11
  Scenario: Ethernet client thread start
    When the daemon completes initialization
    Then the log should contain "checkForUpdates] Got lock.."

  # ---------- Bootstrap ----------

  @order-12
  Scenario: Bootstrap configuration loaded
    When the daemon completes initialization
    Then the log should contain "Bootstrap Properties File"
    And the log should contain "/opt/secure/RFC/bootstrap.ini"

  # ---------- Device Manager ----------

  @order-13
  Scenario: Device manager (dsClient) initialization
    When the daemon completes initialization
    Then the log should contain "Device manager Initialized success"
    And the log should contain "break loop"

  # ---------- WebPA ----------

  @order-14
  Scenario: WebPA ready to process requests
    When the daemon completes initialization
    Then the log should contain "Starting WEBPA Parodus Connections"

  # ---------- PowerController ----------

  @order-15
  Scenario: PowerController initialization completes
    When the daemon completes initialization
    Then the log should contain "start PowerController_Init()"
    And the log should contain "completed PowerController_Init()"
    And the log should contain "Got the powercontroller interface"

  @order-16
  Scenario: Power mode callback registration
    When the daemon completes initialization
    Then the log should contain "Registering power mode change callback"
    And the log should contain "Registered power mode change callback"

  # ---------- Critical Error Sweep ----------

  @order-17
  Scenario: No critical errors during bootup
    When the daemon completes initialization
    Then the log should NOT contain "pthread_create() failed"
    And the log should NOT contain "Failed to hostIf_initalize_ConfigManger()"
    And the log should NOT contain "[rbusdml] Failed to initialized"
    And the log should NOT contain "consumer: rbus_open failed"
    And the log should NOT contain "FATAL"
    And the log should NOT contain "CRITICAL"

  # ---------- RFC Defaults ----------

  @order-18
  Scenario: RFC defaults file created and readable via rbus
    When the daemon completes initialization
    Then the file "/tmp/rfcdefaults.ini" should exist
    And the file should contain "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable=false"
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable" via rbus
    Then the rbus response should not contain an error
    And the log should contain "Calling getValue in New RFC Store I/O"
