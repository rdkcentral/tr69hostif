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

@pytest.mark.run(order=487)
def test_IP_InterfaceNumberOfEntries_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.InterfaceNumberOfEntries"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=488)
def test_IP_ActivePortNumberOfEntries_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.ActivePortNumberOfEntries"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=489)
def test_IP_Enable_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Enable"
    VALUE = "true"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=490)
def test_IP_Enable_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Enable"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=146)
def test_IP_IPv4Enable_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv4Enable"
    VALUE = "true"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=147)
def test_IP_IPv4Enable_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv4Enable"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout


@pytest.mark.run(order=148)
def test_IP_IPv6Enable_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Enable"
    VALUE = "true"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=149)
def test_IP_IPv6Enable_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Enable"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=150)
def test_IP_ULAEnable_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.ULAEnable"
    VALUE = "true"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=151)
def test_IP_ULAEnable_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.ULAEnable"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout
  
@pytest.mark.run(order=152)
def test_IP_Alias_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Alias"
    VALUE = "ip_interface_alias"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=153)
def test_IP_Alias_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Alias"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=154)
def test_IP_Status_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Status"
    VALUE = "Up"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=155)
def test_IP_Name_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Name"
    VALUE = "lo"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=156)
def test_IP_LastChange_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.LastChange"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=157)
def test_IP_LowerLayers_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.LowerLayers"
    VALUE = "eth0"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=158)
def test_IP_LowerLayers_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.LowerLayers"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=159)
def test_IP_Type_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Type"
    VALUE = "Loopback"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout


@pytest.mark.run(order=160)
def test_IP_Router_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Router"
    VALUE = "true"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=161)
def test_IP_Router_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Router"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=162)
def test_IP_Loopback_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Loopback"
    VALUE = "true"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=163)
def test_IP_Loopback_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Loopback"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=164)
def test_IP_IPv4AddressNumberOfEntries_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv4AddressNumberOfEntries"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=165)
def test_IP_IPv6AddressNumberOfEntries_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6AddressNumberOfEntries"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=166)
def test_IP_IPv4Address_Enable_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv4Address.1.Enable"
    VALUE = "true"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=167)
def test_IP_IPv4Address_Enable_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv4Address.1.Enable"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=168)
def test_IP_IPv4Address_Status_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv4Address.1.Status"
    VALUE = "Enabled"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout
    
@pytest.mark.run(order=169)
def test_IP_IPv4Address_Alias_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv4Address.1.Alias"
    VALUE = "ipv4_alias"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=170)
def test_IP_IPv4Address_Alias_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv4Address.1.Alias"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=171)
def test_IP_IPv4Address_IPAddress_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv4Address.1.IPAddress"
    VALUE = "127.0.0.1"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=172)
def test_IP_IPv4Address_IPAddress_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv4Address.1.IPAddress"
    VALUE = "127.0.0.1"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout


@pytest.mark.run(order=173)
def test_IP_IPv4Address_SubnetMask_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv4Address.1.SubnetMask"
    VALUE = "255.0.0.0"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SUCCESS_STRING in rstdout

@pytest.mark.run(order=174)
def test_IP_IPAddress_SubnetMask_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv4Address.1.SubnetMask"
    VALUE = "255.0.0.0"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout


@pytest.mark.run(order=175)
def test_IP_IPAddress_AddressingType_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv4Address.1.AddressingType"
    VALUE = "Static"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=176)
def test_IP_IPv6Address_Enable_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Address.1.Enable"
    VALUE = "true"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=177)
def test_IP_IPv6Address_Enable_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Address.1.Enable"
    VALUE = "true"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=178)
def test_IP_IPv6Address_Status_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Address.1.Status"
    VALUE = "Enabled"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=179)
def test_IP_IPv6Address_IPv6Address_Set_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Address.1.IPAddress"
    VALUE = "::1"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", VALUE)
    assert RBUS_SET_EXCEPTION_STRING in rstdout

@pytest.mark.run(order=180)
def test_IP_IPv6Address_IPv6Address_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Address.1.IPAddress"
    VALUE = "::1"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=181)
def test_IP_IPv6Address_Prefix_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Address.1.Prefix"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=182)
def test_IP_IPv6Address_Origin_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Address.1.Origin"
    VALUE = "WellKnown"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=183)
def test_IP_IPv6Address_Anycast_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Address.1.Anycast"
    VALUE = "false"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout


@pytest.mark.run(order=184)
def test_IP_IPv6Address_PreferredLifetime_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Address.1.PreferredLifetime"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=185)
def test_IP_IPv6Address_ValidLifetime_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Address.1.ValidLifetime"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=186)
def test_IP_IPv6Prefix_Autonomous_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Prefix.1.Autonomous"
    VALUE =  "false"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=187)
def test_IP_IPv6Prefix_StaticType_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Prefix.1.StaticType"
    VALUE = "Inapplicable"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=188)
def test_IP_IPv6Prefix_PrefixStatus_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Prefix.1.PrefixStatus"
    VALUE = "Preferred"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout
 
@pytest.mark.run(order=189)
def test_IP_IPv6Prefix_ValidLifetime_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.IPv6Prefix.1.ValidLifetime"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=190)
def test_IP_Stats_BytesSent_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.BytesSent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=191)
def test_IP_Stats_BytesReceived_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.BytesReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=192)
def test_IP_Stats_PacketsSent_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.PacketsSent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=193)
def test_IP_Stats_PacketsReceived_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.PacketsReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=194)
def test_IP_Stats_ErrorsSent_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.ErrorsSent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    
@pytest.mark.run(order=195)
def test_IP_Stats_ErrorsReceived_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.ErrorsReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=196)
def test_IP_Stats_UnicastPacketsSent_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.UnicastPacketsSent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=197)
def test_IP_Stats_UnicastPacketsReceived_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.UnicastPacketsReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=198)
def test_IP_Stats_DiscardPacketsSent_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.DiscardPacketsSent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    

@pytest.mark.run(order=199)
def test_IP_Stats_DiscardPacketsReceived_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.DiscardPacketsReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=200)
def test_IP_Stats_MulticastPacketsSent_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.MulticastPacketsSent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=201)
def test_IP_Stats_MulticastPacketsReceived_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.MulticastPacketsReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=202)
def test_IP_Stats_BroadcastPacketsSent_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.BroadcastPacketsSent"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=203)
def test_IP_Stats_BroadcastPacketsReceived_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.BroadcastPacketsReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
 
@pytest.mark.run(order=204)
def test_IP_Stats_UnknownProtoPacketsReceived_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.Interface.1.Stats.UnknownProtoPacketsReceived"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout

@pytest.mark.run(order=205)
def test_IP_ActivePort_LocalIPAddress_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.ActivePort.1.LocalIPAddress"
    VALUE = "127.0.0.1"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=206)
def test_IP_ActivePort_LocalPort_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.ActivePort.1.LocalPort"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=207)
def test_IP_ActivePort_RemoteIPAddress_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.ActivePort.1.RemoteIPAddress"
    VALUE =  "0.0.0.0"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=208)
def test_IP_ActivePort_RemotePort_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.ActivePort.1.RemotePort"
    VALUE =  "0"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    assert VALUE in rstdout

@pytest.mark.run(order=209)
def test_IP_ActivePort_Status_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.IP.ActivePort.1.Status"
    # Force reload config fetch from xconf
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout
    

 
