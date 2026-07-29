####################################################################################
# If not stated otherwise in this file or this component's Licenses.txt file the
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

# Source: ../tests/tr69hostif_moca.py
# Feature: tr69hostif_moca.feature

Feature: MoCA Interface Parameter Handler Validation

  Background:
    Given the tr69hostif daemon is running and initialized
    And rbuscli is available on the system

  Scenario: GET Device.MoCA.InterfaceNumberOfEntries
    When I GET "Device.MoCA.InterfaceNumberOfEntries" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Enable
    When I GET "Device.MoCA.Interface.1.Enable" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Status
    When I GET "Device.MoCA.Interface.1.Status" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Alias
    When I GET "Device.MoCA.Interface.1.Alias" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Name
    When I GET "Device.MoCA.Interface.1.Name" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.LastChange
    When I GET "Device.MoCA.Interface.1.LastChange" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.LowerLayers
    When I GET "Device.MoCA.Interface.1.LowerLayers" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Upstream
    When I GET "Device.MoCA.Interface.1.Upstream" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.MACAddress
    When I GET "Device.MoCA.Interface.1.MACAddress" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.FirmwareVersion
    When I GET "Device.MoCA.Interface.1.FirmwareVersion" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.MaxBitRate
    When I GET "Device.MoCA.Interface.1.MaxBitRate" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.MaxIngressBW
    When I GET "Device.MoCA.Interface.1.MaxIngressBW" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.MaxEgressBW
    When I GET "Device.MoCA.Interface.1.MaxEgressBW" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.HighestVersion
    When I GET "Device.MoCA.Interface.1.HighestVersion" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.CurrentVersion
    When I GET "Device.MoCA.Interface.1.CurrentVersion" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.NetworkCoordinator
    When I GET "Device.MoCA.Interface.1.NetworkCoordinator" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.NodeID
    When I GET "Device.MoCA.Interface.1.NodeID" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.MaxNodes
    When I GET "Device.MoCA.Interface.1.MaxNodes" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.PreferredNC
    When I GET "Device.MoCA.Interface.1.PreferredNC" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.BackupNC
    When I GET "Device.MoCA.Interface.1.BackupNC" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.PrivacyEnabledSetting
    When I GET "Device.MoCA.Interface.1.PrivacyEnabledSetting" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.FreqCapabilityMask
    When I GET "Device.MoCA.Interface.1.FreqCapabilityMask" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.FreqCurrentMaskSetting
    When I GET "Device.MoCA.Interface.1.FreqCurrentMaskSetting" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.FreqCurrentMask
    When I GET "Device.MoCA.Interface.1.FreqCurrentMask" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.TxBcastRate
    When I GET "Device.MoCA.Interface.1.TxBcastRate" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.PowerCntlPhyTarget
    When I GET "Device.MoCA.Interface.1.PowerCntlPhyTarget" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.TxBcastPowerReduction
    When I GET "Device.MoCA.Interface.1.TxBcastPowerReduction" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.QAM256Capable
    When I GET "Device.MoCA.Interface.1.QAM256Capable" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.PacketAggregationCapability
    When I GET "Device.MoCA.Interface.1.PacketAggregationCapability" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.AssociatedDeviceNumberOfEntries
    When I GET "Device.MoCA.Interface.1.AssociatedDeviceNumberOfEntries" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Stats.BytesSent
    When I GET "Device.MoCA.Interface.1.Stats.BytesSent" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Stats.BytesReceived
    When I GET "Device.MoCA.Interface.1.Stats.BytesReceived" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Stats.PacketsSent
    When I GET "Device.MoCA.Interface.1.Stats.PacketsSent" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Stats.PacketsReceived
    When I GET "Device.MoCA.Interface.1.Stats.PacketsReceived" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Stats.ErrorsSent
    When I GET "Device.MoCA.Interface.1.Stats.ErrorsSent" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Stats.ErrorsReceived
    When I GET "Device.MoCA.Interface.1.Stats.ErrorsReceived" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Stats.UnicastPacketsSent
    When I GET "Device.MoCA.Interface.1.Stats.UnicastPacketsSent" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Stats.UnicastPacketsReceived
    When I GET "Device.MoCA.Interface.1.Stats.UnicastPacketsReceived" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Stats.DiscardPacketsSent
    When I GET "Device.MoCA.Interface.1.Stats.DiscardPacketsSent" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Stats.DiscardPacketsReceived
    When I GET "Device.MoCA.Interface.1.Stats.DiscardPacketsReceived" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Stats.MulticastPacketsSent
    When I GET "Device.MoCA.Interface.1.Stats.MulticastPacketsSent" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.Stats.X_RDKCENTRAL-COM_RxMapPhyRate
    When I GET "Device.MoCA.Interface.1.Stats.X_RDKCENTRAL-COM_RxMapPhyRate" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.QoS.EgressNumFlows
    When I GET "Device.MoCA.Interface.1.QoS.EgressNumFlows" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.QoS.IngressNumFlows
    When I GET "Device.MoCA.Interface.1.QoS.IngressNumFlows" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.QoS.FlowStats.1.FlowID
    When I GET "Device.MoCA.Interface.1.QoS.FlowStats.1.FlowID" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.QoS.FlowStats.1.PacketDA
    When I GET "Device.MoCA.Interface.1.QoS.FlowStats.1.PacketDA" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.QoS.FlowStats.1.MaxRate
    When I GET "Device.MoCA.Interface.1.QoS.FlowStats.1.MaxRate" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.X_RDKCENTRAL-COM.MeshTable.1.MeshTxNodeId
    When I GET "Device.MoCA.Interface.1.X_RDKCENTRAL-COM.MeshTable.1.MeshTxNodeId" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.X_RDKCENTRAL-COM.MeshTable.1.MeshRxNodeId
    When I GET "Device.MoCA.Interface.1.X_RDKCENTRAL-COM.MeshTable.1.MeshRxNodeId" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.X_RDKCENTRAL-COM.MeshTable.1.MeshPHYTxRate
    When I GET "Device.MoCA.Interface.1.X_RDKCENTRAL-COM.MeshTable.1.MeshPHYTxRate" via rbus
    Then the rbus response should contain an error

  Scenario: SET Device.MoCA.Interface.1.Enable
    When I SET "Device.MoCA.Interface.1.Enable" to "true" as boolean via rbus
    Then the rbus set response should contain "setvalues failed"

  Scenario: SET Device.MoCA.Interface.1.Alias
    When I SET "Device.MoCA.Interface.1.Alias" to "TestAlias" as string via rbus
    Then the rbus set response should contain "setvalues failed"

  Scenario: SET Device.MoCA.Interface.1.LowerLayers
    When I SET "Device.MoCA.Interface.1.LowerLayers" to "" as string via rbus
    Then the rbus set response should contain "setvalues failed"

  Scenario: GET Device.MoCA.Interface.1.KeyPassphrase
    When I GET "Device.MoCA.Interface.1.KeyPassphrase" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.TxPowerLimit
    When I GET "Device.MoCA.Interface.1.TxPowerLimit" via rbus
    Then the rbus response should contain an error

  Scenario: GET Device.MoCA.Interface.1.BeaconPowerLimit
    When I GET "Device.MoCA.Interface.1.BeaconPowerLimit" via rbus
    Then the rbus response should contain an error

  Scenario: SET Device.MoCA.Interface.1.KeyPassphrase
    When I SET "Device.MoCA.Interface.1.KeyPassphrase" to "123456" as string via rbus
    Then the rbus set response should contain "setvalues failed"

  Scenario: SET Device.MoCA.Interface.1.TxPowerLimit
    When I SET "Device.MoCA.Interface.1.TxPowerLimit" to "0" as int via rbus
    Then the rbus set response should contain "setvalues failed"

  Scenario: SET Device.MoCA.Interface.1.BeaconPowerLimit
    When I SET "Device.MoCA.Interface.1.BeaconPowerLimit" to "0" as int via rbus
    Then the rbus set response should contain "setvalues failed"

