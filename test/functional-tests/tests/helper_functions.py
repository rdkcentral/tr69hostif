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
#import requests
import pip._vendor.requests as requests
from basic_constants import *
import os
import time
import re
import signal
import pytest

#/usr/local/bin/tr69hostif
def run_module(module_path: str):
    return subprocess.run("{module_path}", shell=True)


#tr69hostif
def kill_module(module: str, signal: int=9):
    print(f"Recived Signal to kill {module} {signal} with pid {get_pid({module})}")
    resp = subprocess.run(f"kill -{signal} {get_pid({module})}", shell=True, capture_output=True)
    print(resp.stdout.decode('utf-8'))
    print(resp.stderr.decode('utf-8'))
    return ""

def sigterm_module(pid):
    try:
        # Print the command being executed
        print(f"Sending SIGTERM to process {pid}")
        
        # Send the SIGTERM signal to the process
        os.kill(int(pid), signal.SIGTERM)
        
        print(f"Process {pid} has been terminated.")
    except ProcessLookupError:
        print(f"No process with PID {pid} found.")
    except PermissionError:
        print(f"Permission denied to kill process {pid}.")
    except Exception as e:
        print(f"An error occurred: {e}")

def adminSupport_cache(save: bool = True):
    if(requests.get(ADMIN_SUPPORT_SET, verify=False,params={ADMIN_CACHE_ARG: str(save).lower()})):
        return True
    return False

def adminSupport_requestData():
    return_data = requests.get(ADMIN_SUPPORT_GET, verify=False,params={ADMIN_RQUEST_DATA: "true"})
    try:
        return_json = return_data.json()
    except:
        return_json = []
    return return_json

def clear_persistant_files():
    subprocess.run("rm -rf "+XCONF_PERSISTANT_REGX, shell=True)

def rbus_get_data(param: str):
    return subprocess.run(RBUSCLI_GET_CMD + param, shell=True, capture_output=True).stdout.decode('utf-8')

def rbus_set_data(param: str, type:str, value: str):
    return subprocess.run(f"{RBUSCLI_SET_CMD} {param} {type} {value}", shell=True, capture_output=True).stdout.decode('utf-8')

def get_process_uptime(pid):
    return time.time() - os.path.getctime(f"/proc/{pid}")

def get_pid(name: str):
    return subprocess.run(f"pidof {name}", shell=True, capture_output=True).stdout.decode('utf-8').strip()

def clear_tr69hostiflogs():
    subprocess.run(f"echo '' > {LOG_FILE}", shell=True)

def grep_log_file(search: str, log_file: str):
    return subprocess.run(f"grep -i {search} {log_file}", shell=True, capture_output=True).stdout.decode('utf-8')

def remove_T2bootup_flag():
    FILE_LIST = BOOT_FLAGS_LIST
    for file in FILE_LIST:
        try:
            os.remove(file)
        except:
            pass

def grep_tr69hostiflogs(search: str):
    search_result = ""
    search_pattern = re.compile(re.escape(search), re.IGNORECASE)
    try:
        with open(LOG_FILE, 'r', encoding='utf-8', errors='ignore') as file:
            for line_number, line in enumerate(file, start=1):
                if search_pattern.search(line):
                    search_result = search_result + " \n" + line
    except Exception as e:
        print(f"Could not read file {LOG_FILE}: {e}")
    return search_result

def check_Rbus_data():
    result = subprocess.run(RBUSCLI_GET_CMD+T2_REPORT_PROFILE_PARAM, shell=True, capture_output=True, text=True)
    assert RBUS_EXCEPTION_STRING not in result.stdout
    result = subprocess.run(RBUSCLI_GET_CMD+T2_REPORT_PROFILE_PARAM_MSG_PCK, shell=True, capture_output=True, text=True)
    assert RBUS_EXCEPTION_STRING not in result.stdout
    result = subprocess.run(RBUSCLI_GET_CMD+T2_TEMP_REPORT_PROFILE_PARAM, shell=True, capture_output=True, text=True)
    assert RBUS_EXCEPTION_STRING not in result.stdout

def adminSupport_DLXconf_requestData():
    return_data = requests.get(ADMIN_SUPPORT_URL, verify=False,params={ADMIN_RQUEST_DATA: "true"})
    try:
        return_json = return_data.json()
    except:
        return_json = []
    return return_json

def adminSupport_DLReport_requestData():
    return_data = requests.get(ADMIN_SUPPORT_URL, verify=False,params={ADMIN_RQUEST_DATA: "true"})
    try:
        return_json = return_data.json()
    except:
        return_json = []
    return return_json

def adminSupport_DLXconf_cache(save:bool = True):
    if(requests.get(DL_ADMIN_URL +"Xconf", verify=False,params={ADMIN_CACHE_ARG: str(save).lower()})):
        return True
    return False

def adminSupport_DLReport_cache(save:bool = True):
    if(requests.get(DL_ADMIN_URL +"Report", verify=False,params={ADMIN_CACHE_ARG: str(save).lower()})):
        return True
    return False

def adminSupport_DLXconf_requestData():
    return_data = requests.get(DL_ADMIN_URL+"Xconf", verify=False,params={ADMIN_RQUEST_DATA: "true"})
    try:
        return_json = return_data.json()
    except:
        return_json = []
    return return_json

def adminSupport_DLReport_requestData():
    return_data = requests.get(DL_ADMIN_URL+"Report", verify=False,params={ADMIN_RQUEST_DATA: "true"})
    try:
        return_json = return_data.json()
    except:
        return_json = []
    return return_json
    
def run_shell_silent(command):
    subprocess.run(command, shell=True, capture_output=False, text=False)
    return 

def run_shell_command(command):
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    return result.stdout.strip()

def _http_get(names, caller_id=TEST_CALLER):
    headers = {CALLER_ID_HEADER: caller_id} if caller_id else {}
    return requests.get(HTTP_URL, json={"names": names}, headers=headers, verify=False, timeout=10)


def _http_post(parameters, caller_id=RFC_CALLER):
    headers = {CALLER_ID_HEADER: caller_id} if caller_id else {}
    return requests.post(HTTP_URL, json={"parameters": parameters}, headers=headers, verify=False, timeout=10)

def _rfc_http_value(bare_key):
    resp = _http_get([bare_key])
    if resp.status_code != 200:
        return ""
    body = resp.json()
    params = body.get("parameters", [])
    if not params:
        return ""
    return params[0].get("value", "")


def _reload_rfc_var_cache():
    resp = _http_post([{"name": RFC_RELOAD_CACHE_KEY, "value": "true", "dataType": 0}])
    assert resp.status_code == 200, "RELOADCACHE POST failed"
    assert resp.json().get("statusCode", -1) == 0, "RELOADCACHE returned non-zero statusCode"

def _write_rfc_var_file(content):
    os.makedirs(os.path.dirname(RFC_VAR_FILE), exist_ok=True)
    with open(RFC_VAR_FILE, "w") as f:
        f.write(content)


def _append_rfc_var(key, value):
    os.makedirs(os.path.dirname(RFC_VAR_FILE), exist_ok=True)
    with open(RFC_VAR_FILE, "a") as f:
        f.write(f"{key}={value}\n")

def _wait_for_daemon(timeout=DAEMON_READY_TIMEOUT):
    deadline = time.time() + timeout
    while time.time() < deadline:
        if get_pid(MODULE_NAME).strip():
            time.sleep(5)
            return True
        time.sleep(1)
    return False

def _http_value(param_name):
    resp = _http_get([param_name])
    assert resp.status_code == 200, f"GET failed with HTTP {resp.status_code}"
    body = resp.json()
    params = body.get("parameters", [])
    assert params, f"No parameters returned for {param_name}"
    return params[0].get("value", "")

def _restart_daemon():
    pid = get_pid(MODULE_NAME).strip()
    if pid:
        sigterm_module(pid)
        time.sleep(2)
    subprocess.Popen(f"{DAEMON_CMD} >> {LOG_FILE} 2>&1", shell=True)
    return _wait_for_daemon()

@pytest.fixture()
def restore_rfc_var_file():
    if os.path.exists(RFC_VAR_FILE):
        with open(RFC_VAR_FILE, "r") as f:
            original = f.read()
    else:
        original = None

    yield

    if original is not None:
        with open(RFC_VAR_FILE, "w") as f:
            f.write(original)
    elif os.path.exists(RFC_VAR_FILE):
        os.remove(RFC_VAR_FILE)

@pytest.fixture()
def restore_rfc_store_files():
    original_var = None
    original_defaults = None

    if os.path.exists(RFC_VAR_FILE):
        with open(RFC_VAR_FILE, "r") as f:
            original_var = f.read()

    if os.path.exists(RFC_DEFAULTS_FILE):
        with open(RFC_DEFAULTS_FILE, "r") as f:
            original_defaults = f.read()

    yield

    if original_var is not None:
        with open(RFC_VAR_FILE, "w") as f:
            f.write(original_var)
    elif os.path.exists(RFC_VAR_FILE):
        os.remove(RFC_VAR_FILE)

    if original_defaults is not None:
        with open(RFC_DEFAULTS_FILE, "w") as f:
            f.write(original_defaults)
    elif os.path.exists(RFC_DEFAULTS_FILE):
        os.remove(RFC_DEFAULTS_FILE)

def grep_paroduslogs(search: str):
    search_result = ""
    search_pattern = re.compile(re.escape(search), re.IGNORECASE)
    try:
        with open(PARODUS_LOG_FILE, 'r', encoding='utf-8', errors='ignore') as file:
            for line_number, line in enumerate(file, start=1):
                if search_pattern.search(line):
                    search_result = search_result + " \n" + line
    except Exception as e:
        print(f"Could not read file {PARODUS_LOG_FILE}: {e}")
    return search_result
