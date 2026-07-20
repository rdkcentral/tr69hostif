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


import subprocess
import os
import pytest
from time import sleep

from helper_functions import *


@pytest.mark.run(order=344)
def test_StorageService_ClientNumberOfEntries_Get_Handler():
    DATA_ELEMENT_NAME = "Device.Services.StorageServiceNumberOfEntries"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=345)
def test_StorageService_PhysicalMedium_Alias_Get_Handler():
    DATA_ELEMENT_NAME = "Device.Services.StorageService.1.PhysicalMedium.1.Alias"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=346)
def test_StorageService_PhysicalMedium_Name_Get_Handler():
    DATA_ELEMENT_NAME = "Device.Services.StorageService.1.PhysicalMedium.1.Name"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=347)
def test_StorageService_PhysicalMedium_Vendor_Get_Handler():
    DATA_ELEMENT_NAME = "Device.Services.StorageService.1.PhysicalMedium.1.Vendor"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=348)
def test_StorageService_PhysicalMedium_Model_Get_Handler():
    DATA_ELEMENT_NAME = "Device.Services.StorageService.1.PhysicalMedium.1.Model"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=349)
def test_StorageService_PhysicalMedium_SerialNumber_Get_Handler():
    DATA_ELEMENT_NAME = "Device.Services.StorageService.1.PhysicalMedium.1.SerialNumber"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=350)
def test_StorageService_PhysicalMedium_FirmwareVersion_Get_Handler():
    DATA_ELEMENT_NAME = "Device.Services.StorageService.1.PhysicalMedium.1.FirmwareVersion"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=351)
def test_StorageService_PhysicalMedium_ConnectionType_Get_Handler():
    DATA_ELEMENT_NAME = "Device.Services.StorageService.1.PhysicalMedium.1.ConnectionType"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=352)
def test_StorageService_PhysicalMedium_Removable_Get_Handler():
    DATA_ELEMENT_NAME = "Device.Services.StorageService.1.PhysicalMedium.1.Removable"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=353)
def test_StorageService_PhysicalMedium_Status_Get_Handler():
    DATA_ELEMENT_NAME = "Device.Services.StorageService.1.PhysicalMedium.1.Status"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=354)
def test_StorageService_PhysicalMedium_Uptime_Get_Handler():
    DATA_ELEMENT_NAME = "Device.Services.StorageService.1.PhysicalMedium.1.Uptime"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=355)
def test_StorageService_PhysicalMedium_SMARTCapable_Get_Handler():
    DATA_ELEMENT_NAME = "Device.Services.StorageService.1.PhysicalMedium.1.SmartCapable"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=356)
def test_StorageService_PhysicalMedium_Health_Get_Handler():
    DATA_ELEMENT_NAME = "Device.Services.StorageService.1.PhysicalMedium.1.Health"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=357)
def test_StorageService_PhysicalMedium_HotSwappable_Get_Handler():
    DATA_ELEMENT_NAME = "Device.Services.StorageService.1.PhysicalMedium.1.HotSwappable"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=358)
def test_StorageService_PhysicalMedium_NumberOfEntries_Get_Handler():
    # TR-181 mapping: Device.StorageService.{i}.PhysicalMedium.NumberOfEntries
    DATA_ELEMENT_NAME = "Device.Services.StorageService.1.PhysicalMediumNumberOfEntries"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout
