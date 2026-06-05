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

@pytest.mark.run(order=103)
def test_Ethernet_NumberofEntries_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.InterfaceNumberOfEntries"
    VERSION_MSG = "1"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VERSION_MSG in rstdout

@pytest.mark.run(order=104)
def test_Ethernet_InterfaceName_Get_Handler():
    #clear_tr69hostiflogs()

    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Name"
    IF_NAME = "eth0"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert IF_NAME in rstdout

@pytest.mark.run(order=105)
def test_Ethernet_Enable_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Enable"
    ENABLE_MSG = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert ENABLE_MSG in rstdout

@pytest.mark.run(order=106)
def test_Ethernet_Status_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Status"
    STATUS_MSG = "Up"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATUS_MSG in rstdout

@pytest.mark.run(order=107)
def test_Ethernet_LastChange_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.LastChange"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=108)
def test_Ethernet_LowerLayers_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.LowerLayers"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=109)
def test_Ethernet_Upstream_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Upstream"
    STATE_MSG = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert STATE_MSG in rstdout

@pytest.mark.run(order=110)
def test_Ethernet_MACAddr_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME ="Device.Ethernet.Interface.1.MACAddress"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=111)
def test_Ethernet_MAXBitRate_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME ="Device.Ethernet.Interface.1.MaxBitRate"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=112)
def test_Ethernet_DuplexMode_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME ="Device.Ethernet.Interface.1.DuplexMode"
    DUPLEX_MODE_MSG = "full"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert DUPLEX_MODE_MSG in rstdout

@pytest.mark.run(order=113)
def test_Ethernet_BytesSent_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME ="Device.Ethernet.Interface.1.Stats.BytesSent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=114)
def test_Ethernet_BytesReceived_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME ="Device.Ethernet.Interface.1.Stats.BytesReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=115)
def test_Ethernet_PacketsSent_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME ="Device.Ethernet.Interface.1.Stats.PacketsSent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=116)
def test_Ethernet_PacketsReceived_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Stats.PacketsReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=117)
def test_Ethernet_ErrorsSent_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Stats.ErrorsSent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=118)
def test_Ethernet_ErrorsReceived_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Stats.ErrorsReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=119)
def test_Ethernet_UnicastPacketsSent_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Stats.UnicastPacketsSent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=120)
def test_Ethernet_UnicastPacketsReceived_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Stats.UnicastPacketsReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=121)
def test_Ethernet_DiscardPacketsSent_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Stats.DiscardPacketsSent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=122)
def test_Ethernet_DiscardPacketsReceived_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Stats.DiscardPacketsReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=123)
def test_Etherne_MulticastPacketsSent_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Stats.MulticastPacketsSent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=124)
def test_Ethernet_MulticastPacketsReceived_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Stats.MulticastPacketsReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=125)
def test_Ethernet_BroadcastPacketsSent_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Stats.BroadcastPacketsSent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=126)
def test_Ethernet_BroadcastPacketsReceived_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.Ethernet.Interface.1.Stats.BroadcastPacketsReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

