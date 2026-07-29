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


@pytest.mark.run(order=257)
def test_MoCA_InterfaceNumberOfEntries_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.InterfaceNumberOfEntries"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=258)
def test_MoCA_Interface_Enable_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Enable"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=259)
def test_MoCA_Interface_Status_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Status"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=260)
def test_MoCA_Interface_Alias_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Alias"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=261)
def test_MoCA_Interface_Name_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Name"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=262)
def test_MoCA_Interface_LastChange_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.LastChange"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=263)
def test_MoCA_Interface_LowerLayers_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.LowerLayers"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=264)
def test_MoCA_Interface_Upstream_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Upstream"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=265)
def test_MoCA_Interface_MACAddress_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.MACAddress"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=266)
def test_MoCA_Interface_FirmwareVersion_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.FirmwareVersion"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=267)
def test_MoCA_Interface_MaxBitRate_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.MaxBitRate"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=268)
def test_MoCA_Interface_MaxIngressBW_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.MaxIngressBW"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=269)
def test_MoCA_Interface_MaxEgressBW_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.MaxEgressBW"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=270)
def test_MoCA_Interface_HighestVersion_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.HighestVersion"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=271)
def test_MoCA_Interface_CurrentVersion_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.CurrentVersion"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=272)
def test_MoCA_Interface_NetworkCoordinator_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.NetworkCoordinator"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=273)
def test_MoCA_Interface_NodeID_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.NodeID"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=274)
def test_MoCA_Interface_MaxNodes_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.MaxNodes"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=275)
def test_MoCA_Interface_PreferredNC_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.PreferredNC"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=276)
def test_MoCA_Interface_BackupNC_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.BackupNC"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=277)
def test_MoCA_Interface_PrivacyEnabledSetting_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.PrivacyEnabledSetting"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=278)
def test_MoCA_Interface_FreqCapabilityMask_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.FreqCapabilityMask"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=279)
def test_MoCA_Interface_FreqCurrentMaskSetting_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.FreqCurrentMaskSetting"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=280)
def test_MoCA_Interface_FreqCurrentMask_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.FreqCurrentMask"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=281)
def test_MoCA_Interface_TxBcastRate_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.TxBcastRate"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=282)
def test_MoCA_Interface_PowerCntlPhyTarget_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.PowerCntlPhyTarget"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=283)
def test_MoCA_Interface_TxBcastPowerReduction_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.TxBcastPowerReduction"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=284)
def test_MoCA_Interface_QAM256Capable_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.QAM256Capable"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=285)
def test_MoCA_Interface_PacketAggregationCapability_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.PacketAggregationCapability"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=286)
def test_MoCA_Interface_AssociatedDeviceNumberOfEntries_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.AssociatedDeviceNumberOfEntries"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=287)
def test_MoCA_Interface_Stats_BytesSent_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Stats.BytesSent"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=288)
def test_MoCA_Interface_Stats_BytesReceived_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Stats.BytesReceived"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=289)
def test_MoCA_Interface_Stats_PacketsSent_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Stats.PacketsSent"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=290)
def test_MoCA_Interface_Stats_PacketsReceived_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Stats.PacketsReceived"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=291)
def test_MoCA_Interface_Stats_ErrorsSent_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Stats.ErrorsSent"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=292)
def test_MoCA_Interface_Stats_ErrorsReceived_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Stats.ErrorsReceived"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=293)
def test_MoCA_Interface_Stats_UnicastPacketsSent_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Stats.UnicastPacketsSent"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=294)
def test_MoCA_Interface_Stats_UnicastPacketsReceived_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Stats.UnicastPacketsReceived"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=295)
def test_MoCA_Interface_Stats_DiscardPacketsSent_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Stats.DiscardPacketsSent"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=296)
def test_MoCA_Interface_Stats_DiscardPacketsReceived_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Stats.DiscardPacketsReceived"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=297)
def test_MoCA_Interface_Stats_MulticastPacketsSent_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Stats.MulticastPacketsSent"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=298)
def test_MoCA_Interface_Stats_RxMapPhyRate_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Stats.X_RDKCENTRAL-COM_RxMapPhyRate"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=299)
def test_MoCA_Interface_QoS_EgressNumFlows_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.QoS.EgressNumFlows"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=300)
def test_MoCA_Interface_QoS_IngressNumFlows_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.QoS.IngressNumFlows"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=301)
def test_MoCA_Interface_QoS_FlowStats_FlowID_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.QoS.FlowStats.1.FlowID"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=302)
def test_MoCA_Interface_QoS_FlowStats_PacketDA_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.QoS.FlowStats.1.PacketDA"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=303)
def test_MoCA_Interface_QoS_FlowStats_MaxRate_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.QoS.FlowStats.1.MaxRate"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=304)
def test_MoCA_Interface_MeshTable_MeshTxNodeId_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.X_RDKCENTRAL-COM.MeshTable.1.MeshTxNodeId"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=305)
def test_MoCA_Interface_MeshTable_MeshRxNodeId_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.X_RDKCENTRAL-COM.MeshTable.1.MeshRxNodeId"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=306)
def test_MoCA_Interface_MeshTable_MeshPHYTxRate_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.X_RDKCENTRAL-COM.MeshTable.1.MeshPHYTxRate"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=307)
def test_MoCA_Interface_Enable_Set_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Enable"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", "true")
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=308)
def test_MoCA_Interface_Alias_Set_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.Alias"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", "TestAlias")
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=309)
def test_MoCA_Interface_LowerLayers_Set_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.LowerLayers"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", "lower")
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=513)
def test_MoCA_Interface_PreferredNC_Set_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.PreferredNC"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", "true")
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=514)
def test_MoCA_Interface_PrivacyEnabledSetting_Set_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.PrivacyEnabledSetting"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "boolean", "true")
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=515)
def test_MoCA_Interface_FreqCurrentMaskSetting_Set_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.FreqCurrentMaskSetting"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", "0")
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=516)
def test_MoCA_Interface_PowerCntlPhyTarget_Set_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.PowerCntlPhyTarget"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "int32", "0")
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=517)
def test_MoCA_Interface_KeyPassphrase_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.KeyPassphrase"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=518)
def test_MoCA_Interface_TxPowerLimit_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.TxPowerLimit"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=519)
def test_MoCA_Interface_BeaconPowerLimit_Get_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.BeaconPowerLimit"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=520)
def test_MoCA_Interface_KeyPassphrase_Set_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.KeyPassphrase"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "string", "123456")
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=521)
def test_MoCA_Interface_TxPowerLimit_Set_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.TxPowerLimit"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "int32", "0")
    assert RBUS_SET_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=522)
def test_MoCA_Interface_BeaconPowerLimit_Set_Handler():
    DATA_ELEMENT_NAME = "Device.MoCA.Interface.1.BeaconPowerLimit"
    rstdout = rbus_set_data(DATA_ELEMENT_NAME, "int32", "0")
    assert RBUS_SET_EXCEPTION_STRING in rstdout

