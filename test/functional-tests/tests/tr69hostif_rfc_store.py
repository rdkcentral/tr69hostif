####################################################################################
# If not stated otherwise in this file or this component's Licenses file the
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

import os
import time
import subprocess
import pytest

import helper_functions as hf
from helper_functions import *


@pytest.mark.run(order=77)
def test_rfc_variable_ini_readback(restore_rfc_var_file):
    """
    rfcVariable.ini read-back using bare RFC_* key through HTTP server.
    """
    hf._write_rfc_var_file(f"{RFC_L2_READBACK_KEY}={RFC_L2_READBACK_VALUE}\n")
    hf._reload_rfc_var_cache()

    returned = hf._rfc_http_value(RFC_L2_READBACK_KEY)
    assert returned == RFC_L2_READBACK_VALUE, (
        f"Expected '{RFC_L2_READBACK_VALUE}', got '{returned}'"
    )

@pytest.mark.run(order=78)
def test_rfc_override_precedence_var_over_defaults(restore_rfc_store_files):
    """
    Runtime TR-181 RFC value should override rfcdefaults.ini for same key.
    """
    with open(RFC_DEFAULTS_FILE, "w") as f:
        f.write(f"{RFC_DEFAULTS_PARAM}={RFC_DEFAULTS_VALUE}\n")

    resp = hf._http_post([
        {
            "name": RFC_DEFAULTS_PARAM,
            "value": RFC_OVERRIDE_VALUE,
            "dataType": 3,
        }
    ], caller_id=RFC_CALLER)
    assert resp.status_code == 200, f"POST failed with HTTP {resp.status_code}"
    assert resp.json().get("statusCode", -1) == 0, "Override POST returned non-zero statusCode"

    returned = hf._http_value(RFC_DEFAULTS_PARAM)
    assert returned == RFC_OVERRIDE_VALUE, (
        f"Expected override value '{RFC_OVERRIDE_VALUE}', got '{returned}'"
    )


@pytest.mark.run(order=79)
def test_rfc_reloadcache_http_post_trigger(restore_rfc_var_file):
    """
    RFC_CONTROL_RELOADCACHE should be accepted through HTTP POST.
    """
    hf._write_rfc_var_file(f"{RFC_L2_NEWKEY}={RFC_L2_NEWVALUE}\n")

    resp = hf._http_post([
        {
            "name": RFC_RELOAD_CACHE_KEY,
            "value": "true",
            "dataType": 0,
        }
    ])

    assert resp.status_code == 200, f"POST failed with HTTP {resp.status_code}"
    assert resp.json().get("statusCode", -1) == 0, "RELOADCACHE returned non-zero statusCode"

    returned = hf._rfc_http_value(RFC_L2_NEWKEY)
    assert returned == RFC_L2_NEWVALUE, (
        f"Expected '{RFC_L2_NEWVALUE}' after reload, got '{returned}'"
    )


@pytest.mark.run(order=80)
def test_rfc_var_store_consistency_after_daemon_restart(restore_rfc_var_file):
    """
    RFC value from rfcVariable.ini should remain available after daemon restart.
    """
    hf._write_rfc_var_file(f"{RFC_L2_NEWKEY}={RFC_L2_NEWVALUE}\n")
    hf._reload_rfc_var_cache()

    before = hf._rfc_http_value(RFC_L2_NEWKEY)
    assert before == RFC_L2_NEWVALUE, (
        f"Expected '{RFC_L2_NEWVALUE}' before restart, got '{before}'"
    )

    assert hf._restart_daemon(), "Failed to restart tr69hostif daemon"

    after = hf._rfc_http_value(RFC_L2_NEWKEY)
    assert after == RFC_L2_NEWVALUE, (
        f"Expected '{RFC_L2_NEWVALUE}' after restart, got '{after}'"
    )


