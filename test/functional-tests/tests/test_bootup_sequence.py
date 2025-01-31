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
    JSON_ENTER_MSG = "[jsonIfHandlerThread:src/hostIf_jsonReqHandlerThread.cpp] Entering.."
    JSON_SUCCESS_MSG = "SERVER: Started server successfully." 
    assert JSON_ENTER_MSG in grep_T2logs(JSON_ENTER_MSG)
    assert JSON_SUCCESS_MSG in grep_T2logs(JSON_SUCCESS_MSG)

@pytest.mark.run(order=2)
def test_check_http_server_status():
    start_time = time.time();
    is_process_active()
    HTTP_ENTER_MSG = "[HTTPServerStartThread:src/http_server.cpp] Entering.."
    HTTP_SUCCESS_MSG = "SERVER: Started server successfully." 
    assert HTTP_ENTER_MSG in grep_T2logs(HTTP_ENTER_MSG)
    assert HTTP_SUCCESS_MSG in grep_T2logs(HTTP_SUCCESS_MSG)
    print("--- %s seconds ---" % (time.time() - start_time))

@pytest.mark.run(order=3)
def test_check_paradous_status():
    is_process_active()
    PD_SUCCESS_MSG = "Initiating Connection with PARODUS success.. \n"
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
    assert RBUS1_ERROR_MSG not in grep_T2logs(RBUS1_ERROR_MSG)
    assert RBUS2_ERROR_MSG not in grep_T2logs(RBUS2_ERROR_MSG)
    assert RBUS3_SUCCESS_MSG in grep_T2logs(RBUS3_SUCCESS_MSG)
    assert RBUS4_SUCCESS_MSG in grep_T2logs(RBUS4_SUCCESS_MSG)

@pytest.mark.run(order=6)
def test_hostIf_initalize_ConfigManger_status():
    is_process_active()
    CONFIG_ERROR_MSG = "Failed to hostIf_initalize_ConfigManger()" 
    assert CONFIG_ERROR_MSG not in grep_T2logs(CONFIG_ERROR_MSG)

@pytest.mark.run(order=7)
def test_loadDataModel_status():
    is_process_active()
    DM_SUCCESS_MSG = "Successfully initialize Data Model" 
    assert DM_SUCCESS_MSG in grep_T2logs(DM_SUCCESS_MSG)

@pytest.mark.run(order=8)
def test_check_UpdateHandler_init_status():
    is_process_active()
    UPDATE_HANDLER_ENTER_MSG = "[src/hostIf_updateHandler.cpp:Init] Entering.."
    UPDATE_HANDLER_EXIT_MSG = "[src/hostIf_updateHandler.cpp:Init] Exiting.."
    assert UPDATE_HANDLER_ENTER_MSG in grep_T2logs(UPDATE_HANDLER_ENTER_MSG)
    assert UPDATE_HANDLER_EXIT_MSG in grep_T2logs(UPDATE_HANDLER_EXIT_MSG)
    
@pytest.mark.run(order=9)
def test_profile_initialization():
    is_process_active()
    # Step 1: Run the rbuscli curl command
    response = profile_init_run_command()

    # Step 2: Sleep for 10 seconds.Changable
    time.sleep(10)

    # Step 3: Check for the RUN_MSG in logs. Rework to avoid line number
    RUN_MSG = "[src/hostIf_updateHandler.cpp:run:147] Entering.."
    assert RUN_MSG in  grep_T2logs(RUN_MSG)

