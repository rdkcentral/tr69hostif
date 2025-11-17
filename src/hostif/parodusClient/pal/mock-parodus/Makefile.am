##########################################################################
# If not stated otherwise in this file or this component's LICENSE
# file the following copyright and licenses apply:
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
##########################################################################

bin_PROGRAMS = parodus
PARODUS_INSTALL_DIR = /usr/local
LOCAL_DIR = /usr/local


parodus_SOURCES =  \
        parodus.cpp \
        ../webpa_notification.cpp \
        ../libpd.cpp \
        ../webpa_parameter.cpp \
        ../webpa_adapter.cpp ../webpa_attribute.cpp \
        ../../../handlers/src/hostIf_msgHandler.cpp \
        ../../../src/hostIf_utils.cpp  \
        ../../../handlers/src/hostIf_NotificationHandler.cpp \
        ../../../handlers/src/hostIf_TimeClient_ReqHandler.cpp \
        ../../../profiles/Time/Device_Time.cpp \
        ../../../profiles/DeviceInfo/XrdkCentralComBSStore.cpp  \
        ../../../profiles/DeviceInfo/XrdkCentralComBSStoreJournal.cpp \
        ../../../profiles/DeviceInfo/Device_DeviceInfo.cpp \
        ../../../handlers/src/hostIf_DeviceClient_ReqHandler.cpp \
        ../../../profiles/DeviceInfo/Device_DeviceInfo_ProcessStatus_Process.cpp \
        ../../../handlers/src/hostIf_EthernetClient_ReqHandler.cpp \
        ../../../profiles/Ethernet/Device_Ethernet_Interface_Stats.cpp \
        ../../../profiles/Ethernet/Device_Ethernet_Interface.cpp \
        ../../../profiles/DeviceInfo/XrdkCentralComRFC.cpp \
        ../../../profiles/DeviceInfo/XrdkCentralComRFCStore.cpp \
        ../../../profiles/DeviceInfo/Device_DeviceInfo_Processor.cpp \
        ../../../profiles/DeviceInfo/Device_DeviceInfo_ProcessStatus.cpp \
        ../../../src/IniFile.cpp \
	../../../../unittest/stubs/powerctrl_stubs.cpp \
        ../../../../unittest/stubs/paramMgr.c \
        ../../../handlers/src/hostIf_IPClient_ReqHandler.cpp \
        ../../../profiles/IP/Device_IP_Interface.cpp \
        ../../../profiles/IP/Device_IP.cpp \
        ../../../profiles/IP/Device_IP_Interface_IPv4Address.cpp \
        ../../../profiles/IP/Device_IP_Diagnostics_IPPing.cpp \
        ../../../profiles/IP/Device_IP_Interface_Stats.cpp \
        ../../../profiles/IP/Device_IP_ActivePort.cpp \
        ../../../handlers/src/hostIf_updateHandler.cpp \
        ../../../handlers/src/hostIf_dsClient_ReqHandler.cpp  \
        ../../../profiles/STBService/Components_VideoOutput.cpp \
        ../../../profiles/STBService/Components_HDMI.cpp \
        ../../../profiles/STBService/Components_AudioOutput.cpp \
        ../../../profiles/STBService/Components_VideoDecoder.cpp \
        ../../../profiles/STBService/Components_SPDIF.cpp \
        ../../../profiles/STBService/Capabilities.cpp \
        ../../../profiles/STBService/Components_DisplayDevice.cpp \
        ../../../handlers/src/x_rdk_req_handler.cpp \
        ../../../profiles/Device/x_rdk_profile.cpp \
        ../../../handlers/src/hostIf_XrdkCentralT2_ReqHandler.cpp \
        ../../../handlers/src/hostIf_rbus_Dml_Provider.cpp \
        ../../../handlers/tr69hostif_otlp_instrumentation.cpp

parodus_CPPFLAGS = -I../ -I../../../../unittest/stubs/ -I/usr/local/include/wrp-c/ -I../../waldb/ -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/local/include/wdmp-c/ -I/usr/local/include/rbus -I../../../include/ -I../../../../unittest/stubs/ -I../../../handlers/src/ -I/usr/local/include/cjson/ -I/usr/local/include/libparodus/ -I./ -I../../waldb/ -I../../../handlers/include/ -I../../../profiles/Time/ -I../../../profiles/DeviceInfo/ -I../../../profiles/Device -I../../../handlers/include/ -I../../../../unittest/stubs/ds -I../../../profiles/Ethernet/ -I../../../include/ -I../../../profiles/IP/ -I../../../profiles/STBService/ -I/usr/rdk-halif-device_settings/include/ -I/usr/rdkvhal-devicesettings-raspberrypi4/ -DUSE_REMOTE_DEBUGGER -I/usr/remote_debugger/src/

parodus_LDADD = -llibparodus -lrdkloggers -lwdmp-c -lwrp-c -lmsgpackc -lprocps -ltrower-base64 -lcimplog -lsoup-3.0 -lcjson -lpthread -lglib-2.0 -lnanomsg -lrbus -lIARMBus -lyajl -lds -ldshalcli -ldbus-1 -lsecure_wrapper -lcurl -L../../waldb/.libs/ -l:libwaldb.a -L../../../profiles/Time/.libs/ -l:libhostIfTime.a -ltinyxml2 -lIARMBus

parodus_LDADD += -L/usr/local/lib -L/usr/lib \
		           -lopentelemetry_metrics \
                           -lopentelemetry_trace \
                           -lopentelemetry_resources \
                           -lopentelemetry_common \
			   -lopentelemetry_exporter_otlp_http_metric \
                           -lopentelemetry_exporter_otlp_http \
                           -lopentelemetry_exporter_otlp_http_client \
                           -lopentelemetry_http_client_curl \
			   -lopentelemetry_version \
                           -lcurl \
                           -lpthread \
			   -ldl
