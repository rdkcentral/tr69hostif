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
import os
import pytest
from time import sleep

from helper_functions import *


@pytest.mark.run(order=253)
def test_DHCPv4_ClientNumberOfEntries_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DHCPv4.ClientNumberOfEntries"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=254)
def test_DHCPv4_Client_InterfaceReference_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DHCPv4.Client.1.InterfaceReference"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=255)
def test_DHCPv4_Client_DnsServer_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DHCPv4.Client.1.DnsServer"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=256)
def test_DHCPv4_Client_IPRouters_Get_Handler():
    DATA_ELEMENT_NAME = "Device.DHCPv4.Client.1.IPRouters"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout

