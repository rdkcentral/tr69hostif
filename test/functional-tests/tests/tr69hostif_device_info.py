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

import base64
import shlex

import pytest

from helper_functions import *


@pytest.mark.run(order=360)
def test_DeviceInfo_AdditionalHardwareVersion_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.AdditionalHardwareVersion"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=361)
def test_DeviceInfo_AdditionalSoftwareVersion_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.AdditionalSoftwareVersion"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=362)
def test_DeviceInfo_FirstUseDate_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.FirstUseDate"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=363)
def test_DeviceInfo_HardwareVersion_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.HardwareVersion"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=364)
def test_DeviceInfo_Manufacturer_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.Manufacturer"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=365)
def test_DeviceInfo_ManufacturerOUI_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.ManufacturerOUI"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=366)
def test_DeviceInfo_Migration_MigrationStatus_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.Migration.MigrationStatus"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=367)
def test_DeviceInfo_MigrationPreparer_MigrationReady_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.MigrationPreparer.MigrationReady"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=368)
def test_DeviceInfo_SerialNumber_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.SerialNumber"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=369)
def test_DeviceInfo_SupportedDataModelNumberOfEntries_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.SupportedDataModelNumberOfEntries"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=370)
def test_DeviceInfo_VendorConfigFileNumberOfEntries_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.VendorConfigFileNumberOfEntries"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=371)
def test_DeviceInfo_VendorLogFileNumberOfEntries_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.VendorLogFileNumberOfEntries"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=372)
def test_DeviceInfo_COMCAST_FirmwareDownloadStatus_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_COMCAST-COM_FirmwareDownloadStatus"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=373)
def test_DeviceInfo_COMCAST_FirmwareToDownload_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_COMCAST-COM_FirmwareToDownload"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=374)
def test_DeviceInfo_COMCAST_Reset_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_COMCAST-COM_Reset"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=375)
def test_DeviceInfo_RDKVersion_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RDKVersion"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=376)
def test_DeviceInfo_ApparmorBlocklist_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.ApparmorBlocklist"
    profile = run_shell_command("awk -F: 'NF>0{print $1; exit}' /opt/secure/Apparmor_blocklist 2>/dev/null")
    if not profile:
        profile = run_shell_command("ls /etc/apparmor/service_profiles/*.service.sp 2>/dev/null | head -n 1 | xargs -r basename | sed 's/\\.service\\.sp$//'")
    if not profile:
        pytest.skip("No valid AppArmor profile found for ApparmorBlocklist set validation")
    VALUE = f"{profile}:enforce"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=377)
def test_DeviceInfo_ApparmorBlocklist_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.ApparmorBlocklist"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=378)
def test_DeviceInfo_Blocklist_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.Blocklist"
    VALUE = "sample-block-entry"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=379)
def test_DeviceInfo_Blocklist_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.Blocklist"
    VALUE = "sample-block-entry"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout


@pytest.mark.run(order=380)
def test_DeviceInfo_XRPollingAction_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_XRPolling.Action"
    VALUE = "XRPoll"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=381)
def test_DeviceInfo_XRPollingAction_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_XRPolling.Action"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=382)
def test_DeviceInfo_COMCAST_FirmwareDownloadStatus_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_COMCAST-COM_FirmwareDownloadStatus"
    VALUE = "IDLE"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=383)
def test_DeviceInfo_FirmwareToDownload_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload"
    VALUE = "fw_image.bin"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=384)
def test_DeviceInfo_COMCAST_Reset_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_COMCAST-COM_Reset"
    VALUE = "Factory"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=385)
def test_DeviceInfo_RebootStop_Detection_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Detection"
    VALUE = "true"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=386)
def test_DeviceInfo_RebootStop_Detection_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Detection"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=387)
def test_DeviceInfo_RebootStop_Duration_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Duration"
    VALUE = "10"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "int", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=388)
def test_DeviceInfo_RebootStop_Duration_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Duration"
    VALUE = "10"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout


@pytest.mark.run(order=389)
def test_DeviceInfo_MemInsight_Trigger_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Trigger"
    VALUE = "start"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=390)
def test_DeviceInfo_MemInsight_Trigger_Stop_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Trigger"
    VALUE = "stop"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=391)
def test_DeviceInfo_MemInsight_Enable_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Enable"
    VALUE = "true"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=392)
def test_DeviceInfo_MemInsight_Enable_False_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Enable"
    VALUE = "false"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=393)
def test_DeviceInfo_SecureUnlock_SSHPassword_Set_Get_Handler():
    parameter = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SecureUnlock.SSHPassword"
    shadow_path = "/opt/secure/signedlab-shadow"
    crypt_chars = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

    def set_password(value, data_type="string"):
        return rbus_set_data(parameter, data_type, shlex.quote(value))

    def get_password():
        response = rbus_get_data(parameter)
        assert RBUS_EXCEPTION_STRING not in response, "SSHPassword GET failed"
        values = []
        for line in response.splitlines():
            key, separator, value = line.partition(":")
            if separator and key.strip() == "Value":
                values.append(value.strip())
        assert len(values) == 1, "SSHPassword GET did not return exactly one Value field"
        return values[0]

    def password_span(contents):
        offset = 0
        spans = []
        for line in contents.splitlines(keepends=True):
            account, separator, fields = line.partition(b":")
            if account == b"rdk_user":
                password, field_separator, _ = fields.partition(b":")
                assert separator and field_separator, "Malformed rdk_user entry"
                start = offset + len(account) + 1
                spans.append((start, start + len(password)))
            offset += len(line)
        assert len(spans) == 1, "Expected exactly one rdk_user entry"
        return spans[0]

    def replace_password(contents, value):
        start, end = password_span(contents)
        return contents[:start] + value.encode("ascii") + contents[end:]

    def read_shadow(skip_if_missing=True):
        encoded = run_shell_command(
            "if [ -L {0} ]; then printf '__SIGNEDLAB_SHADOW_SYMLINK__'; "
            "elif [ ! -f {0} ]; then printf '__SIGNEDLAB_SHADOW_MISSING__'; "
            "else base64 {0} | tr -d '\\n'; fi".format(shadow_path)
        )
        if encoded == "__SIGNEDLAB_SHADOW_MISSING__":
            if skip_if_missing:
                pytest.skip("SecureUnlock shadow file is not present on this target")
            pytest.fail("SecureUnlock shadow file disappeared during the test")
        assert encoded != "__SIGNEDLAB_SHADOW_SYMLINK__", (
            "SecureUnlock shadow path is a symlink"
        )
        assert encoded, "Could not read SecureUnlock shadow file from target"
        try:
            return base64.b64decode(encoded, validate=True)
        except (ValueError, base64.binascii.Error):
            pytest.fail("Target returned invalid encoded shadow-file data")

    def is_restorable_value(value):
        if value == "*":
            return True
        parts = value.split("$")
        if len(parts) != 4 or parts[0] != "" or parts[1] != "6":
            return False
        salt, digest = parts[2], parts[3]
        return (
            1 <= len(salt) <= 16
            and all(char in crypt_chars for char in salt)
            and len(digest) == 86
            and all(char in crypt_chars for char in digest)
        )

    original_rfc = get_password()
    original_shadow = read_shadow()
    start, end = password_span(original_shadow)
    original_shadow_password = original_shadow[start:end].decode("ascii")
    if not is_restorable_value(original_rfc):
        pytest.skip(
            "Original SSHPassword RFC value cannot be restored through SET; "
            "skipping before device state changes"
        )
    assert original_rfc == original_shadow_password, (
        "Initial RFC value and rdk_user shadow password do not match"
    )

    test_hash = "$6$l2test$" + ("b" * 86)
    expected_hash_shadow = replace_password(original_shadow, test_hash)
    expected_reset_shadow = replace_password(original_shadow, "*")
    state_may_have_changed = False
    try:
        state_may_have_changed = True
        result = set_password(test_hash)
        assert RBUS_SUCCESS_STRING in result, "Valid SSHPassword SET failed"
        assert read_shadow() == expected_hash_shadow, (
            "Valid SET did not change only the rdk_user password field"
        )
        assert get_password() == test_hash, "GET did not return the hash set by the handler"

        invalid_requests = (
            ("plain-text", "string"),
            ("$5$salt$" + ("a" * 86), "string"),
            ("$6$salt$malformed", "string"),
            ("", "string"),
            ("   ", "string"),
            ("true", "boolean"),
        )
        for value, data_type in invalid_requests:
            result = set_password(value, data_type)
            assert RBUS_SET_EXCEPTION_STRING in result, (
                "Invalid SSHPassword SET was not rejected"
            )
            assert read_shadow() == expected_hash_shadow, (
                "Rejected SET changed the shadow file"
            )
            assert get_password() == test_hash, (
                "Rejected SET changed the previously configured RFC value"
            )

        result = set_password("*")
        assert RBUS_SUCCESS_STRING in result, "SSHPassword reset SET failed"
        assert read_shadow() == expected_reset_shadow, (
            "Reset did not set only the rdk_user password field to '*'"
        )
        assert get_password() == "*", "GET did not return the reset value"
    finally:
        if state_may_have_changed:
            result = set_password(original_rfc)
            if RBUS_SUCCESS_STRING not in result:
                pytest.fail("Could not restore original SSHPassword RFC value through SET")
            if get_password() != original_rfc:
                pytest.fail("SSHPassword GET did not return its original RFC value after restoration")
            if read_shadow(skip_if_missing=False) != original_shadow:
                pytest.fail("Original shadow-file state was not restored")
