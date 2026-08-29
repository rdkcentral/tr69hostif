################################################################################
# Copyright 2024 Comcast Cable Communications Management, LLC
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
#
# SPDX-License-Identifier: Apache-2.0
################################################################################

AUTOMAKE_OPTIONS = subdir-objects

# Define the program name and the source files
bin_PROGRAMS = dm_gtest

# Define the include directories
COMMON_CPPFLAGS = -std=c++11 -DGTEST_ENABLE -DUSE_DEV_PROPERTIES_CONF -DUSE_REMOTE_DEBUGGER -I/usr/include -I/usr/include/cjson -I$(TOP_DIR)/src/hostif/include -I$(TOP_DIR)/src/hostif/profiles/DeviceInfo -I$(TOP_DIR)/src/unittest/stubs -I$(TOP_DIR)/src/unittest/stubs/ds -I$(TOP_DIR)/src/unittest/stubs/rbus/include -I$(TOP_DIR)/src/hostif/handlers/include -I$(TOP_DIR)/src/hostif/parodusClient/pal -I$(TOP_DIR)/src/hostif/parodusClient/waldb -I$(TOP_DIR)/src/hostif/httpserver/include -I$(TOP_DIR)/src/hostif/handlers/src -I/usr/include/rbus -I/usr/local/include/rbus -Isrc/unittest/stubs/rbus/include/ -I$(TOP_DIR)/src/hostif/profiles/Time -I$(TOP_DIR)/src/hostif/profiles/Device -I$(TOP_DIR)/src/hostif/profiles/IP -I$(TOP_DIR)/src/hostif/profiles/STBService -I/usr/rdk-halif-device_settings/include/ -I/usr/rdkvhal-devicesettings-raspberrypi4/ -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I$(TOP_DIR)/src/hostif/profiles/Ethernet -I/usr/remote_debugger/src/ -I/usr/local/include/libparodus/ -I/usr/local/include/wrp-c/ -I$(TOP_DIR)/src/hostif/parodusClient/startParodus/ -I/usr/include/libsoup-3.0

# Define the libraries to link against
COMMON_LDADD =  -lgtest -lgtest_main -lgmock_main -lgmock -lgcov $(GLIB_LIBS) -ltinyxml2 -lcjson -lcurl -lglib-2.0 -llibparodus -lwrp-c -lnanomsg -lmsgpackc -ltrower-base64 -lcimplog -lsoup-3.0

# Define the compiler flags
COMMON_CXXFLAGS = -frtti $(GLIB_CFLAGS) -fprofile-arcs -ftest-coverage


dm_gtest_SOURCES = dm_test.cpp $(TOP_DIR)/src/hostif/parodusClient/startParodus/startParodus.cpp $(TOP_DIR)/src/hostif/src/hostIf_utils.cpp $(TOP_DIR)/src/hostif/src/IniFile.cpp $(TOP_DIR)/src/unittest/stubs/secure_wrapper.c $(TOP_DIR)/src/hostif/profiles/DeviceInfo/XrdkCentralComRFCStore.cpp $(TOP_DIR)/src/hostif/profiles/DeviceInfo/XrdkCentralComRFC.cpp $(TOP_DIR)/src/hostif/profiles/DeviceInfo/XrdkCentralComBSStore.cpp $(TOP_DIR)/src/hostif/profiles/DeviceInfo/XrdkCentralComBSStoreJournal.cpp $(TOP_DIR)/src/hostif/profiles/DeviceInfo/Device_DeviceInfo.cpp $(TOP_DIR)/src/hostif/handlers/src/hostIf_NotificationHandler.cpp $(TOP_DIR)/src/hostif/parodusClient/pal/webpa_notification.cpp $(TOP_DIR)/src/hostif/parodusClient/pal/webpa_attribute.cpp $(TOP_DIR)/src/hostif/parodusClient/pal/webpa_parameter.cpp $(TOP_DIR)/src/hostif/parodusClient/pal/libpd.cpp $(TOP_DIR)/src/hostif/parodusClient/pal/webpa_adapter.cpp $(TOP_DIR)/src/hostif/handlers/src/hostIf_msgHandler.cpp $(TOP_DIR)/src/hostif/handlers/src/hostIf_XrdkCentralT2_ReqHandler.cpp $(TOP_DIR)/src/hostif/handlers/src/hostIf_IPClient_ReqHandler.cpp $(TOP_DIR)/src/hostif/handlers/src/hostIf_DeviceClient_ReqHandler.cpp $(TOP_DIR)/src/hostif/profiles/DeviceInfo/Device_DeviceInfo_ProcessStatus_Process.cpp $(TOP_DIR)/src/hostif/profiles/Ethernet/Device_Ethernet_Interface.cpp $(TOP_DIR)/src/hostif/profiles/STBService/Components_DisplayDevice.cpp $(TOP_DIR)/src/hostif/handlers/src/hostIf_EthernetClient_ReqHandler.cpp $(TOP_DIR)/src/hostif/profiles/Ethernet/Device_Ethernet_Interface_Stats.cpp $(TOP_DIR)/src/hostif/handlers/src/hostIf_TimeClient_ReqHandler.cpp $(TOP_DIR)/src/hostif/profiles/IP/Device_IP.cpp $(TOP_DIR)/src/hostif/profiles/IP/Device_IP_Interface.cpp $(TOP_DIR)/src/hostif/profiles/Time/Device_Time.cpp $(TOP_DIR)/src/hostif/handlers/src/hostIf_updateHandler.cpp $(TOP_DIR)/src/hostif/profiles/Device/x_rdk_profile.cpp $(TOP_DIR)/src/hostif/handlers/src/hostIf_dsClient_ReqHandler.cpp $(TOP_DIR)/src/hostif/handlers/src/hostIf_rbus_Dml_Provider.cpp $(TOP_DIR)/src/hostif/profiles/STBService/Capabilities.cpp $(TOP_DIR)/src/hostif/profiles/STBService/Components_SPDIF.cpp $(TOP_DIR)/src/hostif/profiles/STBService/Components_VideoDecoder.cpp $(TOP_DIR)/src/hostif/profiles/STBService/Components_AudioOutput.cpp $(TOP_DIR)/src/hostif/profiles/STBService/Components_HDMI.cpp $(TOP_DIR)/src/hostif/profiles/STBService/Components_VideoOutput.cpp $(TOP_DIR)/src/hostif/profiles/IP/Device_IP_Interface_Stats.cpp  $(TOP_DIR)/src/hostif/profiles/IP/Device_IP_ActivePort.cpp $(TOP_DIR)/src/hostif/profiles/IP/Device_IP_Diagnostics_IPPing.cpp $(TOP_DIR)/src/hostif/profiles/IP/Device_IP_Interface_IPv4Address.cpp $(TOP_DIR)/src/hostif/handlers/src/x_rdk_req_handler.cpp $(TOP_DIR)/src/unittest/stubs/dm_stubs.cpp $(TOP_DIR)/src/hostif/parodusClient/waldb/waldb.cpp $(TOP_DIR)/src/unittest/stubs/wdmp-c.c $(TOP_DIR)/src/unittest/stubs/wdmp_internal.c $(TOP_DIR)/src/hostif/httpserver/src/http_server.cpp $(TOP_DIR)/src/hostif/httpserver/src/request_handler.cpp $(TOP_DIR)/src/hostif/httpserver/src/XrdkCentralComRFCVar.cpp $(TOP_DIR)/src/hostif/profiles/DeviceInfo/Device_DeviceInfo_Processor.cpp $(TOP_DIR)/src/hostif/profiles/DeviceInfo/Device_DeviceInfo_ProcessStatus.cpp $(TOP_DIR)/src/unittest/stubs/ds/libprocps.cpp $(TOP_DIR)/src/unittest/stubs/file_writer.cpp

# Apply common properties to each program
dm_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
dm_gtest_LDADD = $(COMMON_LDADD)
dm_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)

