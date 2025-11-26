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


Feature: tr69hostif runs as daemon to collect data

  Scenario: tr69hostif bootup sequence
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    Then the tr69hostif validation is done for all instance

  Scenario: json handler thread initialization
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    And should log the "SERVER: Started server successfully." message

  Scenario: http server thread initialization
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    And should log the "SERVER: Started server successfully." message

  Scenario: thread creation
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    And should not log the "pthread_create() failed" message

  Scenario: parodus initialization
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    And should log the "Initiating Connection with PARODUS success.." message

  Scenario: rbus initialization
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    And should not log the "[rbusdml] Failed to initialized, rbus_checkStatus() returned with status" message
    And should not log the "consumer: rbus_open failed" message
    And should log the "[rbusdml]Successfully get the complete parameter list" message
    And should log the "rbus_regDataElements registered successfully" message
     		
  Scenario: hostif initialize config manager status
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    And should not log the "Failed to hostIf_initalize_ConfigManger()" message
	
  Scenario: Successful PwrContInterface thread initialization
    When a new thread is requested with valid configuration
    Then the system initializes the thread
    And should log the "created getPwrContInterface thread.." message
    And marks the thread as ready for execution

  Scenario: Successful ethernet thread initialization
    When a new thread is requested with valid configuration                                                                                                                     Then the system initializes the thread
    And should log the "checkForUpdates] Got lock.." message
    And marks the thread as ready for execution

  Scenario: Data model merge
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    And should log the "Entering data model merge process" message
    And should log the "Merged XML files successfully into /tmp/data-model.xml" message
    And should log the "Successfully merged Data Model" message

  Scenario: Data model initialization
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    And should log the "Successfully initialize Data Model" message

  Scenario: Bootstrap configuration
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    And should log the "Bootstrap Properties File" message
    And should log the "/opt/secure/RFC/bootstrap.ini" message

  Scenario: device manager initialization
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    And should log the "Device manager Initialized success" message
    And should log the "break loop" message
  
  Scenario: webpa process requests
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    And should log the "Starting WEBPA Parodus Connections" message

  Scenario: PowerController initialization
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    And should log the "start PowerController_Init()" message
    And should log the "completed PowerController_Init()" message
    And should log the "Got the powercontroller interface" message
   
  Scenario: power mode initialization
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    And should log the "Registering power mode change callback" message
    And should log the "Registered power mode change callback" message
 

  Scenario: rfc_defaults_ini file
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And  when the tr69hostif is initialized successfully
    And the /tmp/rfcdefaults.ini file should exist

    
