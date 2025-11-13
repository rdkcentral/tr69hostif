####################################################################################
# If not stated otherwise in this file or this component's Licenses file the
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


import subprocess
import pytest
import time
from time import sleep

from helper_functions import *

MODULE_NAME = "tr69hostif"



def profile_init_run_command():
    """Run the rbuscli curl command and return the result."""
    command = [
        "rbuscli", "-s", "curl", "-i", "-d", '{"names": ["Device.DeviceInfo."]}',
        "-X", "GET", "http://127.0.0.1:11999"
    ]

    # Run the command and capture the output
    result = subprocess.run(command, capture_output=True, text=True)

    if result.returncode != 0:
        raise RuntimeError(f"Command failed with error: {result.stderr}")

    return result.stdout
 
#Start the binary and verify the below threads have been started successfully:
def is_process_active():
    print("check process status")
    
    uptime_process = get_process_uptime(get_pid(MODULE_NAME))
    if uptime_process < 10:
        sleep(10 - uptime_process)

    command_to_get_pid = "pidof tr69hostif"
    pid = run_shell_command(command_to_get_pid)
    assert pid != ""

@pytest.mark.run(order=1)
def test_check_tr69_json_handler():
    is_process_active()
    JSON_SUCCESS_MSG = "SERVER: Started server successfully." 
    assert JSON_SUCCESS_MSG in grep_T2logs(JSON_SUCCESS_MSG)

@pytest.mark.run(order=2)
def test_check_http_server_status():
    start_time = time.time();
    is_process_active()
    HTTP_SUCCESS_MSG = "SERVER: Started server successfully." 
    assert HTTP_SUCCESS_MSG in grep_T2logs(HTTP_SUCCESS_MSG)
    print("--- %s seconds ---" % (time.time() - start_time))

@pytest.mark.run(order=3)
def test_check_paradous_status():
    is_process_active()
    PD_SUCCESS_MSG = "Initiating Connection with PARODUS success.."
    assert PD_SUCCESS_MSG in grep_T2logs(PD_SUCCESS_MSG)

@pytest.mark.run(order=4)
def test_check_shutdown_thread_status():
    is_process_active()
    SHUTDOWN_ERROR_MSG = "pthread_create() failed" 
    assert SHUTDOWN_ERROR_MSG not in grep_T2logs(SHUTDOWN_ERROR_MSG)

#Verify Initialization are success 
@pytest.mark.run(order=5)
def test_init_rbus_dml_provider_status():
    is_process_active()
    RBUS1_ERROR_MSG = "[rbusdml] Failed to initialized, rbus_checkStatus() returned with status"
    RBUS2_ERROR_MSG = "consumer: rbus_open failed"
    RBUS3_SUCCESS_MSG = "[rbusdml]Successfully get the complete parameter list"
    RBUS4_SUCCESS_MSG = "rbus_regDataElements registered successfully"
    RBUS5_PARTIAL_MSG = "rbusPropertyProvider_Register failed: 14"  # Duplicate registration (acceptable)
    RBUS6_INDIVIDUAL_MSG = "Attempting individual parameter registration"  # Individual registration fallback
    
    assert RBUS1_ERROR_MSG not in grep_T2logs(RBUS1_ERROR_MSG)
    assert RBUS2_ERROR_MSG not in grep_T2logs(RBUS2_ERROR_MSG)
    assert RBUS3_SUCCESS_MSG in grep_T2logs(RBUS3_SUCCESS_MSG)
    
    # Check for successful registration OR acceptable partial failure (duplicates)
    logs = grep_T2logs("rbus")
    assert (RBUS4_SUCCESS_MSG in logs or RBUS5_PARTIAL_MSG in logs), \
        "RBUS registration did not complete (neither success nor duplicate error found)"
    
    # If duplicates were found, report them
    if RBUS5_PARTIAL_MSG in logs and RBUS6_INDIVIDUAL_MSG in logs:
        print("\nDuplicate parameters detected - checking individual registration logs...")
        duplicate_logs = grep_T2logs("Duplicate parameter (already registered)")
        if duplicate_logs:
            print("Found duplicate parameters:")
            print(duplicate_logs)

@pytest.mark.run(order=6)
def test_tr69hostif_service_startup():
    """Verify tr69HostIf Service starts successfully"""
    is_process_active()
    STARTUP_MSG = "Starting tr69HostIf Service"
    assert STARTUP_MSG in grep_T2logs(STARTUP_MSG), "tr69HostIf Service failed to start"

@pytest.mark.run(order=7)
def test_iarm_initialization():
    """Verify IARM bus initialization"""
    is_process_active()
    IARM_INIT_MSG = "IARM_Init group name = com.comcast.rdk.iarm.bus"
    IARM_REGISTER_MSG = "Registering tr69HostIfMgr"
    
    assert IARM_INIT_MSG in grep_T2logs(IARM_INIT_MSG), "IARM initialization failed"
    assert IARM_REGISTER_MSG in grep_T2logs(IARM_REGISTER_MSG), "tr69HostIfMgr registration failed"

@pytest.mark.run(order=8)
def test_power_controller_thread():
    """Verify getPwrContInterface thread is created"""
    is_process_active()
    THREAD_MSG = "created getPwrContInterface thread.."
    assert THREAD_MSG in grep_T2logs(THREAD_MSG), "getPwrContInterface thread creation failed"

@pytest.mark.run(order=9)
def test_data_model_merge_process():
    """Verify data model merge workflow"""
    is_process_active()
    MERGE_START_MSG = "Entering data model merge process"
    MERGE_SUCCESS_MSG = "Merged XML files successfully into /tmp/data-model.xml"
    MERGE_COMPLETE_MSG = "Successfully merged Data Model"
    
    logs = grep_T2logs("merge")
    
    assert MERGE_START_MSG in logs, "Data model merge process did not start"
    assert MERGE_SUCCESS_MSG in logs, "XML merge failed"
    assert MERGE_COMPLETE_MSG in logs, "Data model merge did not complete successfully"
    
    # Verify the merged XML file contains profile information
    profile_log = grep_T2logs("Merging XML files for profile:")
    assert profile_log, "Profile information not found in merge logs"
    print(f"\nProfile merge info: {profile_log}")

@pytest.mark.run(order=10)
def test_rbus_open_status():
    """Verify RBUS connection opens successfully"""
    is_process_active()
    RBUS_OPEN_MSG = "rbus open (tr69hostif) success"
    RBUS_COMPONENT_MSG = "rbus open for component: tr69hostif"
    
    assert RBUS_COMPONENT_MSG in grep_T2logs(RBUS_COMPONENT_MSG), "RBUS component registration failed"
    assert RBUS_OPEN_MSG in grep_T2logs(RBUS_OPEN_MSG), "RBUS open failed"

@pytest.mark.run(order=11)
def test_check_duplicate_rbus_registrations():
    """Identify and report which parameters are already registered (duplicates)"""
    is_process_active()
    
    duplicate_logs = grep_T2logs("Duplicate parameter (already registered)")
    individual_summary = grep_T2logs("Individual registration complete")
    
    if duplicate_logs:
        print("\n=== DUPLICATE RBUS PARAMETERS FOUND ===")
        print(duplicate_logs)
        print("\n=== REGISTRATION SUMMARY ===")
        print(individual_summary)
    else:
        print("No duplicate parameters found - all registered successfully on first attempt")

@pytest.mark.run(order=12)
def test_parameter_count():
    """Verify the expected number of parameters are discovered"""
    is_process_active()
    PARAM_COUNT_MSG = "Successfully get the complete parameter list, the Parameter counts is ="
    
    logs = grep_T2logs(PARAM_COUNT_MSG)
    assert logs, "Parameter count not found in logs"
    
    # Extract the parameter count
    import re
    match = re.search(r'Parameter counts is = (\d+)', logs)
    if match:
        param_count = int(match.group(1))
        print(f"\nTotal parameters discovered: {param_count}")
        assert param_count > 0, "Parameter count is zero or invalid"
    else:
        pytest.fail("Could not extract parameter count from logs")

@pytest.mark.run(order=13)
def test_ethernet_client_thread():
    """Verify Ethernet client thread starts"""
    is_process_active()
    ETH_THREAD_MSG = "checkForUpdates] Got lock.."
    assert ETH_THREAD_MSG in grep_T2logs(ETH_THREAD_MSG), "Ethernet client thread did not start"

@pytest.mark.run(order=14)
def test_bootstrap_configuration():
    """Verify bootstrap configuration is loaded"""
    is_process_active()
    BOOTSTRAP_MSG = "Bootstrap Properties File"
    BOOTSTRAP_PATH = "/opt/secure/RFC/bootstrap.ini"
    
    logs = grep_T2logs(BOOTSTRAP_MSG)
    assert logs, "Bootstrap configuration not loaded"
    assert BOOTSTRAP_PATH in logs, f"Bootstrap file path {BOOTSTRAP_PATH} not found"

@pytest.mark.run(order=15)
def test_device_manager_initialization():
    """Verify device manager (dsClient) initialization"""
    is_process_active()
    DS_INIT_MSG = "Device manager Initialized success"
    DS_BREAK_LOOP_MSG = "break loop"
    
    logs = grep_T2logs("Device manager")
    assert DS_INIT_MSG in logs, "Device manager initialization failed"
    assert DS_BREAK_LOOP_MSG in logs, "Device manager initialization loop did not break properly"

@pytest.mark.run(order=16)
def test_power_controller_initialization():
    """Verify PowerController initialization completes"""
    is_process_active()
    PC_START_MSG = "start PowerController_Init()"
    PC_COMPLETE_MSG = "completed PowerController_Init()"
    PC_INTERFACE_MSG = "Got the powercontroller interface"
    PC_CALLBACK_MSG = "Registered power mode change callback"
    
    logs = grep_T2logs("PowerController")
    
    assert PC_START_MSG in logs, "PowerController initialization did not start"
    assert PC_COMPLETE_MSG in logs, "PowerController initialization did not complete"
    assert PC_INTERFACE_MSG in logs, "PowerController interface not acquired"
    assert PC_CALLBACK_MSG in logs, "Power mode change callback not registered"

@pytest.mark.run(order=17)
def test_hostIf_initalize_ConfigManger_status():
    is_process_active()
    CONFIG_ERROR_MSG = "Failed to hostIf_initalize_ConfigManger()" 
    assert CONFIG_ERROR_MSG not in grep_T2logs(CONFIG_ERROR_MSG)

@pytest.mark.run(order=18)
def test_loadDataModel_status():
    is_process_active()
    DM_SUCCESS_MSG = "Successfully initialize Data Model" 
    assert DM_SUCCESS_MSG in grep_T2logs(DM_SUCCESS_MSG)

@pytest.mark.run(order=19)
def test_webpa_ready_status():
    """Verify WebPA is ready to process requests"""
    is_process_active()
    WEBPA_READY_MSG = "WebPA is now ready to process requests"
    PARODUS_SUCCESS_MSG = "Init for parodus Success..!!"
    
    logs = grep_T2logs("parodus")
    
    assert PARODUS_SUCCESS_MSG in logs, "Parodus initialization failed"
    assert WEBPA_READY_MSG in logs, "WebPA not ready to process requests"

@pytest.mark.run(order=20)
def test_sd_notify_event():
    """Verify systemd notification is sent"""
    is_process_active()
    SD_NOTIFY_MSG = "tr69hostif sd notify envent  is sent  Successfully"
    assert SD_NOTIFY_MSG in grep_T2logs(SD_NOTIFY_MSG), "Systemd notify event not sent"

@pytest.mark.run(order=21)
def test_no_critical_errors():
    """Verify no critical errors during bootup"""
    is_process_active()
    
    # Check for common critical errors
    CRITICAL_ERRORS = [
        "pthread_create() failed",
        "Failed to hostIf_initalize_ConfigManger()",
        "[rbusdml] Failed to initialized",
        "consumer: rbus_open failed",
        "FATAL",
        "CRITICAL"
    ]
    
    logs = grep_T2logs("tr69hostif")
    
    for error_msg in CRITICAL_ERRORS:
        assert error_msg not in logs, f"Critical error found: {error_msg}"
    
    print("\nNo critical errors found during bootup sequence")

