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

import pytest

from helper_functions import *


@pytest.mark.run(order=218)
def test_DeviceInfo_STB_MAC_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_COMCAST-COM_STB_MAC"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=219)
def test_DeviceInfo_STB_IP_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_COMCAST-COM_STB_IP"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=220)
def test_DeviceInfo_PowerStatus_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_COMCAST-COM_PowerStatus"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    #assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=221)
def test_DeviceInfo_FirmwareFilename_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareFilename"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=223)
def test_DeviceInfo_FirmwareToDownload_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=222)
def test_DeviceInfo_FirmwareToDownload_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload"
    VALUE = "fw_image.bin"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=224)
def test_DeviceInfo_FirmwareDownloadStatus_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus"
    VALUE = "IDLE"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=225)
def test_DeviceInfo_FirmwareDownloadStatus_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=227)
def test_DeviceInfo_FirmwareDownloadProtocol_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=226)
def test_DeviceInfo_FirmwareDownloadProtocol_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol"
    VALUE = "https"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=229)
def test_DeviceInfo_FirmwareDownloadURL_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=228)
def test_DeviceInfo_FirmwareDownloadURL_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL"
    VALUE = "https://example.com/fw.bin"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=231)
def test_DeviceInfo_FirmwareDownloadUseCodebig_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadUseCodebig"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout



@pytest.mark.run(order=230)
def test_DeviceInfo_FirmwareDownloadUseCodebig_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadUseCodebig"
    VALUE = "true"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=233)
def test_DeviceInfo_FirmwareDownloadDeferReboot_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=232)
def test_DeviceInfo_FirmwareDownloadDeferReboot_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot"
    VALUE = "true"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=234)
def test_DeviceInfo_FirmwareDownloadPercent_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadPercent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=235)
def test_DeviceInfo_FirmwareUpdateState_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareUpdateState"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=236)
def test_DeviceInfo_FirmwareDownloadNow_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadNow"
    VALUE = "true"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=238)
def test_DeviceInfo_Reset_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_Reset"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=237)
def test_DeviceInfo_Reset_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_Reset"
    VALUE = "Factory"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=239)
def test_DeviceInfo_BootStatus_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM.BootStatus"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=240)
def test_DeviceInfo_BootTime_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_BootTime"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=241)
def test_DeviceInfo_CPUTemp_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM.CPUTemp"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=242)
def test_DeviceInfo_LastRebootReason_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_LastRebootReason"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=243)
def test_DeviceInfo_Experience_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_Experience"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=244)
def test_DeviceInfo_X_RDK_FirmwareName_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDK_FirmwareName"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=246)
def test_DeviceInfo_PreferredGatewayType_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_PreferredGatewayType"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=245)
def test_DeviceInfo_PreferredGatewayType_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_PreferredGatewayType"
    VALUE = "DOCSIS"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=247)
def test_DeviceInfo_MigrationPreparer_MigrationReady_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_MigrationPreparer.MigrationReady"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=248)
def test_DeviceInfo_Migration_MigrationStatus_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_Migration.MigrationStatus"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=250)
def test_DeviceInfo_IUI_Version_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM.IUI.Version"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout



@pytest.mark.run(order=249)
def test_DeviceInfo_IUI_Version_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM.IUI.Version"
    VALUE = "1.0.0"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=252)
def test_DeviceInfo_IUI_AppsVersion_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM.IUI.AppsVersion"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=251)
def test_DeviceInfo_IUI_AppsVersion_Set_Handler():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM.IUI.AppsVersion"
    VALUE = "1.0.0"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=431)
def test_DeviceInfo_FirmwareDownloadProtocol_Get_Handler_COMCAST_Alias():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol"
    VALUE  = "http"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout


@pytest.mark.run(order=430)
def test_DeviceInfo_FirmwareDownloadProtocol_Set_Handler_COMCAST_Alias():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol"
    VALUE = "http"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=432)
def test_DeviceInfo_FirmwareDownloadURL_Get_Handler_COMCAST_Alias():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=433)
def test_DeviceInfo_FirmwareDownloadURL_Set_Handler_COMCAST_Alias():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL"
    VALUE = "https://example.com/fw-comcast.bin"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout


@pytest.mark.run(order=434)
def test_DeviceInfo_FirmwareDownloadPercent_Get_Handler_COMCAST_Alias():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadPercent"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=435)
def test_DeviceInfo_BootStatus_Get_Handler_Underscore_Alias():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM.BootStatus"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=436)
def test_DeviceInfo_CPUTemp_Get_Handler_Underscore_Alias():
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM.CPUTemp"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

