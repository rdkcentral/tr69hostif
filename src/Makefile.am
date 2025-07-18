##########################################################################
# If not stated otherwise in this file or this component's LICENSE
# file the following copyright and licenses apply:
#
# Copyright 2016 RDK Management
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
SUBDIRS = hostif/handlers hostif/profiles
DIST_SUBDIRS = hostif/handlers hostif/profiles
if WITH_SNMP_ADAPTER
SUBDIRS += hostif/snmpAdapter
DIST_SUBDIRS += hostif/snmpAdapter
endif

SUBDIRS +=hostif/parodusClient
DIST_SUBDIRS += hostif/parodusClient

SUBDIRS +=hostif/parodusClient/startParodus
DIST_SUBDIRS += hostif/parodusClient/startParodus

if !WITH_NEW_HTTP_SERVER_DISABLE
SUBDIRS += hostif/httpserver
DIST_SUBDIRS += hostif/httpserver
endif

AM_CXXFLAGS = $(GLIB_CFLAGS) $(GTHREAD_CFLAGS) \
	$(SOUP_CFLAGS) \
	-I$(top_srcdir)/src/hostif/include \
	-I$(top_srcdir)/src/hostif/handlers/include \
        -I$(top_srcdir)/src/hostif/handlers/src \
	-I$(top_srcdir)/src/hostif/profiles/STBService \
	-I$(top_srcdir)/src/hostif/profiles/Ethernet \
	-I$(top_srcdir)/src/hostif/profiles/IP \
	-I$(top_srcdir)/src/hostif/profiles/Time \
	-I$(top_srcdir)/src/hostif/profiles/DeviceInfo \
	-I$(top_srcdir)/src/hostif/profiles/Device

AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/parodusClient/pal \
               -I$(top_srcdir)/src/hostif/parodusClient/waldb 

if WITH_MOCA_PROFILE
NEXUS_LIB = -lnexus
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/profiles/moca
else
NEXUS_LIB =
endif



if WITH_WIFI_PROFILE
AM_CXXFLAGS += -DUSE_WIFI_PROFILE
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/profiles/wifi
endif
if WITH_DHCP_PROFILE
AM_CXXFLAGS += -DUSE_DHCPv4_PROFILE
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/profiles/DHCPv4
endif

if WITH_STORAGESERVICE_PROFILE
AM_CXXFLAGS += -DUSE_STORAGESERVICE_PROFILE
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/profiles/StorageService
endif

if WITH_INTFSTACK_PROFILE
AM_CXXFLAGS += -DUSE_INTFSTACK_PROFILE
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/profiles/InterfaceStack
endif

if WITH_WEBPA_RFC
AM_CXXFLAGS += -DWEBPA_RFC_ENABLED
endif

if WITH_SNMP_ADAPTER
AM_CXXFLAGS += -DSNMP_ADAPTER_ENABLED
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/snmpAdapter
endif

if XRELIB_FLAG
AM_LDFLAGS = $(GLIB_LDFLAGS) $(GLIB_LIBS) \
	$(G_THREAD_LIBS) -lyajl $(SOUP_LIBS) \
	$(NEXUS_LIB) -ldbus-1 \
	-lIARMBus -lrdkloggers\
	-lds -ldshalcli -ltr69ProfileXcaliber -lrbus
else
AM_LDFLAGS = $(GLIB_LDFLAGS) $(GLIB_LIBS) \
	$(G_THREAD_LIBS) -lyajl $(SOUP_LIBS) \
	$(NEXUS_LIB) -ldbus-1 \
	-lIARMBus -lrdkloggers\ -lrbus
	-lds -ldshalcli
endif

AM_LDFLAGS += -lWPEFrameworkPowerController

AM_CFLAGS =  $(GLIB_CFLAGS) $(GTHREAD_CFLAGS)
AM_CPPFLAGS =  $(GLIB_CFLAGS) $(GTHREAD_CFLAGS)

# Start : Support for DeviceSetting client.
#ENABLE_WITH_DS_CLIENT=@DEF_ENABLE_WITH_DS_CLIENT@
#ifeq ($(ENABLE_WITH_DS_CLIENT),yes)
#        WITH_DS_CLIENT=TRUE
#endif
# Enabled SD_NOTIFY
AM_CXXFLAGS += $(SYSTEMD_SDNOTIFY_CFLAGS) $(YOCTO_FLAG)
if IS_YOCTO_ENABLED
AM_LDFLAGS += $(SYSTEMD_SDNOTIFY_LDFLAGS) -lsecure_wrapper
else
AM_LDFLAGS += $(SYSTEMD_SDNOTIFY_LDFLAGS)
endif

if IS_TELEMETRY2_ENABLED
AM_CXXFLAGS += $(T2_EVENT_FLAG)
AM_LDFLAGS += -ltelemetry_msgsender -lt2utils
endif

bin_PROGRAMS = tr69hostif backgroundrun
tr69hostif_SOURCES = $(top_srcdir)/src/hostif/src/hostIf_main.cpp $(top_srcdir)/src/hostif/src/hostIf_utils.cpp
tr69hostif_SOURCES += $(top_srcdir)/src/hostif/src/IniFile.cpp
backgroundrun_SOURCES = $(top_srcdir)/src/backgroundrun.c

tr69hostif_LDADD = $(top_builddir)/src/hostif/handlers/libMsgHandlers.la
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/STBService/libstbservice.la
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/IP/libhostIfIP.la
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/Time/libhostIfTime.la
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/Ethernet/libhostIfEthernet.la
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/DeviceInfo/libdeviceinfo.la
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/Device/libdevice.la

if WITH_DHCP_PROFILE
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/DHCPv4/libhostIfDHCPv4Client.la
endif

if WITH_INTFSTACK_PROFILE
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/InterfaceStack/libhostIfInterfaceStack.la
endif

if WITH_STORAGESERVICE_PROFILE
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/StorageService/libstorageservice.la
endif

if WITH_HWSELFTEST_PROFILE
tr69hostif_LDADD += -ltr69ProfileHwSelfTest
endif

if WITH_MOCA_PROFILE
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/moca/libhostIfMoCA.la
endif

if XRELIB_FLAG
tr69hostif_LDADD += -ltr69ProfileXcaliber
endif

if WITH_WIFI_PROFILE
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/wifi/libhostIfWiFi.la
endif

if WITH_SNMP_ADAPTER
tr69hostif_LDADD += $(top_builddir)/src/hostif/snmpAdapter/libSNMPAdapter.la
endif

AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/parodusClient/pal 
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/parodusClient/waldb
tr69hostif_LDADD += $(top_builddir)/src/hostif/parodusClient/waldb/libwaldb.la
tr69hostif_LDADD += $(top_builddir)/src/hostif/parodusClient/pal/libparodusclient.la

if !WITH_NEW_HTTP_SERVER_DISABLE
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/httpserver/include
tr69hostif_LDADD += $(top_builddir)/src/hostif/httpserver/libhttpserver.la
else
AM_CXXFLAGS += $(NEW_HTTP_SERVER_DISABLE_FLAG)
endif

if WITH_PARODUS_SUPPORT
AM_CXXFLAGS += $(PARODUS_ENABLE_FLAG)
endif
