################################################################################
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
################################################################################

"""
L2 functional tests for STBService in libds mode (Thunder disabled).

These tests validate that key STBService TR-069 parameters are reachable
in non-Thunder builds and return non-exception responses via rbus.
"""

import pytest
from helper_functions import *

STBSVC_BASE = "Device.Services.STBService.1"
AUDIO_BASE = STBSVC_BASE + ".Components.AudioOutput.1"
CAPS_BASE = STBSVC_BASE + ".Capabilities"


@pytest.mark.run(order=350)
def test_STBService_libds_AudioOutput_Get_Status():
    param = AUDIO_BASE + ".Status"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    valid = {"Enabled", "Muted", "Disabled", "Error"}
    assert any(v in rstdout for v in valid), \
        f"Unexpected Status value: {rstdout}"


@pytest.mark.run(order=351)
def test_STBService_libds_AudioOutput_Get_Enable():
    param = AUDIO_BASE + ".Enable"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert "true" in rstdout.lower() or "1" in rstdout


@pytest.mark.run(order=352)
def test_STBService_libds_AudioOutput_Get_Name():
    param = AUDIO_BASE + ".Name"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert "AudioOutputPort" in rstdout


@pytest.mark.run(order=353)
def test_STBService_libds_Capabilities_Get_VideoStandards():
    param = CAPS_BASE + ".VideoDecoder.VideoStandards"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
