################################################################################
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
################################################################################

"""
L2 functional tests for Thunder-migrated STBService TR-069 parameters.

These tests exercise the tr69hostif daemon over rbus/TR-181 and verify that:
  1. Thunder-backed GET parameters return a valid response (no rbus exception).
  2. Thunder-backed SET parameters that are NOT_HANDLED return an rbus error.
  3. Key Thunder plugin log messages are emitted (curl response code 200).

Base TR-069 path:
  Device.Services.STBService.1.Components.<Component>.<Instance>.<Param>
"""

import pytest

from helper_functions import *

# ─────────────────────────────────────────────────────────────────────────────
# Constants
# ─────────────────────────────────────────────────────────────────────────────

STBSVC_BASE = "Device.Services.STBService.1"

AUDIO_BASE   = STBSVC_BASE + ".Components.AudioOutput.1"
SPDIF_BASE   = STBSVC_BASE + ".Components.SPDIF.1"
DISPDEV_BASE = STBSVC_BASE + ".Components.DisplayDevice.1"
VIDDEC_BASE  = STBSVC_BASE + ".Components.VideoDecoder.1"
VIDOUT_BASE  = STBSVC_BASE + ".Components.VideoOutput.1"
CAPS_BASE    = STBSVC_BASE + ".Capabilities"

CURL_OK_MSG = "curl response : 0 http response code: 200"

# ─────────────────────────────────────────────────────────────────────────────
# Per-test fixture: clear the tr69hostif log before every test so that
# grep_tr69hostiflogs() assertions only match entries produced by the
# current test and are not satisfied by log lines from previous tests.
# ─────────────────────────────────────────────────────────────────────────────

@pytest.fixture(autouse=True)
def clear_log_before_test():
    clear_tr69hostiflogs()
    yield

# ─────────────────────────────────────────────────────────────────────────────
# AudioOutput – GET tests
# ─────────────────────────────────────────────────────────────────────────────

@pytest.mark.run(order=310)
def test_STBService_AudioOutput_Get_Status():
    """
    GET AudioOutput.1.Status should return one of Enabled / Muted / Disabled
    and must produce a successful Thunder curl call in the tr69hostif log.
    """
    param = AUDIO_BASE + ".Status"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG), \
        "Expected Thunder curl log entry for getEnableAudioPort"
    # Status must be one of the valid TR-135 values
    valid = {"Enabled", "Muted", "Disabled"}
    assert any(v in rstdout for v in valid), \
        f"Unexpected Status value: {rstdout}"


@pytest.mark.run(order=311)
def test_STBService_AudioOutput_Get_Enable():
    """
    GET AudioOutput.1.Enable – always returns true (port is in the list).
    No Thunder call is made for this parameter.
    """
    param = AUDIO_BASE + ".Enable"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert "true" in rstdout.lower() or "1" in rstdout


@pytest.mark.run(order=312)
def test_STBService_AudioOutput_Get_CancelMute():
    """
    GET AudioOutput.1.CancelMute – boolean, backed by Thunder getMuted call.
    """
    param = AUDIO_BASE + ".CancelMute"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)


@pytest.mark.run(order=313)
def test_STBService_AudioOutput_Get_Name():
    """
    GET AudioOutput.1.Name – returns "AudioOutputPort<portName><devId>".
    No Thunder call required.
    """
    param = AUDIO_BASE + ".Name"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert "AudioOutputPort" in rstdout


@pytest.mark.run(order=314)
def test_STBService_AudioOutput_Get_AudioFormat():
    """
    GET AudioOutput.1.AudioFormat – backed by Thunder getAudioFormat.
    """
    param = AUDIO_BASE + ".AudioFormat"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)


@pytest.mark.run(order=315)
def test_STBService_AudioOutput_Get_AudioStereoMode():
    """
    GET AudioOutput.1.X_COMCAST-COM_AudioStereoMode – backed by getSoundMode.
    """
    param = AUDIO_BASE + ".X_COMCAST-COM_AudioStereoMode"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)


@pytest.mark.run(order=316)
def test_STBService_AudioOutput_Get_AudioCompression():
    """
    GET AudioOutput.1.X_COMCAST-COM_AudioCompression – backed by getMS12AudioCompression.
    """
    param = AUDIO_BASE + ".X_COMCAST-COM_AudioCompression"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)


@pytest.mark.run(order=317)
def test_STBService_AudioOutput_Get_AudioEncoding():
    """
    GET AudioOutput.1.X_COMCAST-COM_AudioEncoding – backed by getAudioEncoding.
    """
    param = AUDIO_BASE + ".X_COMCAST-COM_AudioEncoding"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)


# ─────────────────────────────────────────────────────────────────────────────
# AudioOutput – SET tests (NOT_HANDLED in Thunder build)
# ─────────────────────────────────────────────────────────────────────────────

@pytest.mark.run(order=318)
def test_STBService_AudioOutput_Set_CancelMute_NotHandled():
    """
    SET AudioOutput.1.CancelMute must fail – setter is NOT_HANDLED in
    the Thunder build.
    """
    param = AUDIO_BASE + ".CancelMute"
    rstdout = rbus_set_data(param, "boolean", "true")

    assert RBUS_SUCCESS_STRING not in rstdout, \
        f"Expected SET to fail for NOT_HANDLED param {param}"


@pytest.mark.run(order=319)
def test_STBService_AudioOutput_Set_AudioLevel_NotHandled():
    """
    SET AudioOutput.1.AudioLevel must fail.
    """
    param = AUDIO_BASE + ".AudioLevel"
    rstdout = rbus_set_data(param, "uint", "50")

    assert RBUS_SUCCESS_STRING not in rstdout, \
        f"Expected SET to fail for NOT_HANDLED param {param}"


# ─────────────────────────────────────────────────────────────────────────────
# SPDIF – GET tests
# ─────────────────────────────────────────────────────────────────────────────

@pytest.mark.run(order=321)
def test_STBService_SPDIF_Get_Status_NotHandled():
    """
    GET SPDIF.1.Status is NOT_HANDLED in Thunder build – rbus exception expected.
    """
    param = SPDIF_BASE + ".Status"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING in rstdout, \
        f"Expected rbus exception for NOT_HANDLED param {param}"


@pytest.mark.run(order=322)
def test_STBService_SPDIF_Get_Enable_NotHandled():
    """
    GET SPDIF.1.Enable is NOT_HANDLED in Thunder build – rbus exception expected.
    """
    param = SPDIF_BASE + ".Enable"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING in rstdout, \
        f"Expected rbus exception for NOT_HANDLED param {param}"


# ─────────────────────────────────────────────────────────────────────────────
# DisplayDevice – GET tests
# ─────────────────────────────────────────────────────────────────────────────

@pytest.mark.run(order=323)
def test_STBService_DisplayDevice_Get_Status():
    """
    GET DisplayDevice.1.Status – backed by Thunder DisplayInfo.1.connected.
    Returns "Present" or "Absent".
    """
    param = DISPDEV_BASE + ".Status"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)
    assert rstdout.strip() in ("Present", "Absent"), \
        f"Unexpected DisplayDevice Status: {rstdout}"


@pytest.mark.run(order=324)
def test_STBService_DisplayDevice_Get_SupportedResolutions():
    """
    GET DisplayDevice.1.SupportedResolutions – backed by getSupportedResolutions.
    """
    param = DISPDEV_BASE + ".SupportedResolutions"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)


@pytest.mark.run(order=325)
def test_STBService_DisplayDevice_Get_PreferredResolution():
    """
    GET DisplayDevice.1.PreferredResolution – backed by getDefaultResolution.
    """
    param = DISPDEV_BASE + ".PreferredResolution"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)


@pytest.mark.run(order=326)
def test_STBService_DisplayDevice_Set_Status_NotHandled():
    """
    SET DisplayDevice.1.Status must fail – no setter in Thunder build.
    """
    param = DISPDEV_BASE + ".Status"
    rstdout = rbus_set_data(param, "string", "Present")

    assert RBUS_SUCCESS_STRING not in rstdout, \
        f"Expected SET to fail for NOT_HANDLED param {param}"


# ─────────────────────────────────────────────────────────────────────────────
# VideoDecoder – GET tests
# ─────────────────────────────────────────────────────────────────────────────

@pytest.mark.run(order=327)
def test_STBService_VideoDecoder_Get_Status():
    """
    GET VideoDecoder.1.Status – backed by org.rdk.PowerManager.GetPowerState.
    Returns one of Enabled / Disabled / X_COMCAST-COM_Standby.
    """
    param = VIDDEC_BASE + ".Status"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)
    valid = {"Enabled", "Disabled", "X_COMCAST-COM_Standby", "Error"}
    assert any(v in rstdout for v in valid), \
        f"Unexpected VideoDecoder Status: {rstdout}"


@pytest.mark.run(order=328)
def test_STBService_VideoDecoder_Get_Enable():
    """
    GET VideoDecoder.1.Enable – always true (decoder present).
    """
    param = VIDDEC_BASE + ".Enable"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert "true" in rstdout.lower() or "1" in rstdout


@pytest.mark.run(order=329)
def test_STBService_VideoDecoder_Get_Name():
    """
    GET VideoDecoder.1.Name – returns "VideoDecoder<portName>".
    """
    param = VIDDEC_BASE + ".Name"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert "VideoDecoder" in rstdout


@pytest.mark.run(order=330)
def test_STBService_VideoDecoder_Set_Status_NotHandled():
    """
    SET VideoDecoder.1.Status must fail – setter is NOT_HANDLED.
    """
    param = VIDDEC_BASE + ".Status"
    rstdout = rbus_set_data(param, "string", "Enabled")

    assert RBUS_SUCCESS_STRING not in rstdout, \
        f"Expected SET to fail for NOT_HANDLED param {param}"


# ─────────────────────────────────────────────────────────────────────────────
# AudioOutput – additional GET tests (missing from first pass)
# ─────────────────────────────────────────────────────────────────────────────

@pytest.mark.run(order=331)
def test_STBService_AudioOutput_Get_AudioLevel():
    """
    GET AudioOutput.1.AudioLevel – backed by getEnableAudioPort + getMuted,
    returns one of Enabled / Muted / Disabled.
    """
    param = AUDIO_BASE + ".AudioLevel"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)
    # AudioLevel is a numeric volume level (0-100)
    assert rstdout.strip().isdigit() or rstdout.strip().lstrip('-').isdigit(), \
        f"Expected numeric AudioLevel, got: {rstdout}"


@pytest.mark.run(order=332)
def test_STBService_AudioOutput_Get_AudioOptimalLevel():
    """
    GET AudioOutput.1.X_COMCAST-COM_AudioOptimalLevel – hardcoded "0.000000",
    no Thunder call required.
    """
    param = AUDIO_BASE + ".X_COMCAST-COM_AudioOptimalLevel"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert "0.000000" in rstdout


# ─────────────────────────────────────────────────────────────────────────────
# DisplayDevice – additional GET tests (missing from first pass)
# ─────────────────────────────────────────────────────────────────────────────

@pytest.mark.run(order=333)
def test_STBService_DisplayDevice_Get_EEDID():
    """
    GET DisplayDevice.1.EEDID – backed by readEDID; empty when no display connected.
    """
    param = DISPDEV_BASE + ".EEDID"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)


@pytest.mark.run(order=334)
def test_STBService_DisplayDevice_Get_X_COMCAST_EDID():
    """
    GET DisplayDevice.1.X_COMCAST-COM_EDID – same backend as EEDID.
    """
    param = DISPDEV_BASE + ".X_COMCAST-COM_EDID"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)


# ─────────────────────────────────────────────────────────────────────────────
# VideoDecoder – additional GET tests (missing from first pass)
# ─────────────────────────────────────────────────────────────────────────────

@pytest.mark.run(order=335)
def test_STBService_VideoDecoder_Get_ContentAspectRatio():
    """
    GET VideoDecoder.1.ContentAspectRatio – backed by getDisplayAspectRatio;
    falls back to "16:9" if Thunder fails.
    """
    param = VIDDEC_BASE + ".ContentAspectRatio"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    # Aspect ratio is always returned (fallback "16:9" on failure)
    assert rstdout.strip() != ""


# ─────────────────────────────────────────────────────────────────────────────
# VideoOutput – GET tests
# ─────────────────────────────────────────────────────────────────────────────

@pytest.mark.run(order=336)
def test_STBService_VideoOutput_Get_Status():
    """
    GET VideoOutput.1.Status – backed by Thunder DisplayInfo.1.connected.
    Returns "Enabled" when display is connected, "Disabled" otherwise.
    """
    param = VIDOUT_BASE + ".Status"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)
    assert rstdout.strip() in ("Enabled", "Disabled"), \
        f"Unexpected VideoOutput Status: {rstdout}"


@pytest.mark.run(order=337)
def test_STBService_VideoOutput_Get_Enable():
    """
    GET VideoOutput.1.Enable – always true (port is present in the list).
    No Thunder call required.
    """
    param = VIDOUT_BASE + ".Enable"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert "true" in rstdout.lower() or "1" in rstdout


@pytest.mark.run(order=338)
def test_STBService_VideoOutput_Get_DisplayFormat():
    """
    GET VideoOutput.1.DisplayFormat – backed by getCurrentResolution.
    Returns the current resolution string (e.g. "1080p60").
    """
    param = VIDOUT_BASE + ".DisplayFormat"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)
    assert rstdout.strip() != ""


@pytest.mark.run(order=339)
def test_STBService_VideoOutput_Get_VideoFormat():
    """
    GET VideoOutput.1.VideoFormat – backed by getDisplayAspectRatio;
    falls back to "Unknown" if Thunder fails.
    """
    param = VIDOUT_BASE + ".VideoFormat"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    # Always returns something (fallback "Unknown")
    assert rstdout.strip() != ""


@pytest.mark.run(order=340)
def test_STBService_VideoOutput_Get_AspectRatioBehaviour():
    """
    GET VideoOutput.1.AspectRatioBehaviour – backed by AVOutput.getZoomMode;
    falls back to "None" if Thunder fails.
    """
    param = VIDOUT_BASE + ".AspectRatioBehaviour"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert rstdout.strip() != ""


@pytest.mark.run(order=341)
def test_STBService_VideoOutput_Get_HDCP():
    """
    GET VideoOutput.1.HDCP – backed by HdcpProfile.getHDCPStatus; boolean.
    """
    param = VIDOUT_BASE + ".HDCP"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)
    assert "true" in rstdout.lower() or "false" in rstdout.lower() \
        or "1" in rstdout or "0" in rstdout


@pytest.mark.run(order=342)
def test_STBService_VideoOutput_Get_Name():
    """
    GET VideoOutput.1.Name – returns the port name (e.g. "HDMI0").
    No Thunder call required.
    """
    param = VIDOUT_BASE + ".Name"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert rstdout.strip() != ""


# ─────────────────────────────────────────────────────────────────────────────
# VideoOutput – SET tests (NOT_HANDLED)
# ─────────────────────────────────────────────────────────────────────────────

@pytest.mark.run(order=343)
def test_STBService_VideoOutput_Set_DisplayFormat_NotHandled():
    """
    SET VideoOutput.1.DisplayFormat must fail – all setters are NOT_HANDLED.
    """
    param = VIDOUT_BASE + ".DisplayFormat"
    rstdout = rbus_set_data(param, "string", "1080p60")

    assert RBUS_SUCCESS_STRING not in rstdout, \
        f"Expected SET to fail for NOT_HANDLED param {param}"


@pytest.mark.run(order=344)
def test_STBService_VideoOutput_Set_HDCP_NotHandled():
    """
    SET VideoOutput.1.HDCP must fail – setter is NOT_HANDLED.
    """
    param = VIDOUT_BASE + ".HDCP"
    rstdout = rbus_set_data(param, "boolean", "true")

    assert RBUS_SUCCESS_STRING not in rstdout, \
        f"Expected SET to fail for NOT_HANDLED param {param}"


# ─────────────────────────────────────────────────────────────────────────────
# Capabilities – GET tests
# ─────────────────────────────────────────────────────────────────────────────

@pytest.mark.run(order=345)
def test_STBService_Capabilities_Get_VideoStandards():
    """
    GET Capabilities.VideoDecoder.VideoStandards – backed by
    getSupportedVideoCodingFormats; returns TR-135 formatted codec strings.
    """
    param = CAPS_BASE + ".VideoDecoder.VideoStandards"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)
    # Must contain at least one known codec format
    known = {"MPEGH-Part2", "MPEG4-Part10", "MPEG2-Part2"}
    assert any(k in rstdout for k in known), \
        f"Unexpected VideoStandards value: {rstdout}"


@pytest.mark.run(order=346)
def test_STBService_Capabilities_Get_HEVCProfileEntries():
    """
    GET Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevelNumberOfEntries
    – backed by getVideoCodecInfo; returns integer count >= 1.
    """
    param = (CAPS_BASE +
             ".VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevelNumberOfEntries")
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)
    # Value should be a positive integer
    try:
        assert int(rstdout.strip()) >= 1
    except ValueError:
        assert False, f"Expected integer, got: {rstdout}"


@pytest.mark.run(order=347)
def test_STBService_Capabilities_Get_HEVCProfileLevel_Profile():
    """
    GET Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1.Profile
    – reads the first HEVC profile name from getVideoCodecInfo.
    """
    param = (CAPS_BASE +
             ".VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1.Profile")
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)
    assert rstdout.strip() != ""


@pytest.mark.run(order=348)
def test_STBService_Capabilities_Get_HEVCProfileLevel_Level():
    """
    GET Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1.Level
    – reads the first HEVC level string from getVideoCodecInfo.
    """
    param = (CAPS_BASE +
             ".VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1.Level")
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)
    assert rstdout.strip() != ""


@pytest.mark.run(order=349)
def test_STBService_Capabilities_Get_HDMI_SupportedResolutions():
    """
    GET Capabilities.HDMI.SupportedResolutions – backed by
    getSupportedSettopResolutions; returns comma-separated resolution codes.
    """
    param = CAPS_BASE + ".HDMI.SupportedResolutions"
    rstdout = rbus_get_data(param)

    assert RBUS_EXCEPTION_STRING not in rstdout, \
        f"rbus exception getting {param}"
    assert CURL_OK_MSG in grep_tr69hostiflogs(CURL_OK_MSG)
    assert rstdout.strip() != ""


# ─────────────────────────────────────────────────────────────────────────────
# Capabilities – SET test (NOT_HANDLED)
# ─────────────────────────────────────────────────────────────────────────────

@pytest.mark.run(order=350)
def test_STBService_Capabilities_Set_VideoStandards_NotHandled():
    """
    SET Capabilities.VideoDecoder.VideoStandards must fail –
    handleSetMsg always returns NOT_HANDLED.
    """
    param = CAPS_BASE + ".VideoDecoder.VideoStandards"
    rstdout = rbus_set_data(param, "string", "MPEG4-Part10 ([ISO/IEC14496-10])")

    assert RBUS_SUCCESS_STRING not in rstdout, \
        f"Expected SET to fail for NOT_HANDLED param {param}"
