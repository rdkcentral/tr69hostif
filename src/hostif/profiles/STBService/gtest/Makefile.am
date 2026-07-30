# If not stated otherwise in this file or this component's LICENSE file the
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

AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = stbservice_thunder_gtest stbservice_libds_gtest

# -DUSE_THUNDER_CLIENT  : compile the Thunder-backed implementations
# -DUNIT_TEST           : suppress any production-only #ifdefs
# -DGTEST_ENABLE        : enable gtest-specific code paths
COMMON_CPPFLAGS = \
    -std=c++11 \
    -DGTEST_ENABLE \
    -DUNIT_TEST \
    -I/usr/include \
    -I/usr/include/cjson \
    -I/usr/include/glib-2.0 \
    -I/usr/lib/x86_64-linux-gnu/glib-2.0/include \
    -I$(TOP_DIR)/src/hostif/include \
    -I$(TOP_DIR)/src/hostif/profiles/STBService \
    -I$(TOP_DIR)/src/hostif/handlers/include \
    -I$(TOP_DIR)/src/unittest/stubs \
    -I$(TOP_DIR)/src/unittest/stubs/ds \
    -I/usr/local/include

LIBDS_ONLY_CPPFLAGS = \
    -I/usr/rdk-halif-device_settings/include \
    -I/usr/rdkvhal-devicesettings-raspberrypi4 \
    -I/usr/devicesettings/ds \
    -I/usr/devicesettings/ds/include \
    -I/usr/devicesettings/rpc/include \
    -I/usr/devicesettings/rpc/cli \
    -I/usr/devicesettings/rpc/srv

THUNDER_CPPFLAGS = $(COMMON_CPPFLAGS) -DUSE_THUNDER_CLIENT
LIBDS_CPPFLAGS = $(COMMON_CPPFLAGS) $(LIBDS_ONLY_CPPFLAGS)

COMMON_LDADD = \
    -lgtest \
    -lgmock \
    -lgcov \
    $(GLIB_LIBS) \
    -lcjson

# No additional libraries needed for libds mode - using header-only stubs
LIBDS_ONLY_LDADD =

COMMON_CXXFLAGS = \
    -frtti \
    $(GLIB_CFLAGS) \
    -fprofile-arcs \
    -ftest-coverage

stbservice_thunder_gtest_SOURCES = \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_AudioOutput_Thunder.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_SPDIF_Thunder.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_DisplayDevice_Thunder.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_HDMI_Thunder.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_VideoDecoder_Thunder.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_VideoOutput_Thunder.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Capabilities_Thunder.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/gtest/thunder_plugin_stub.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/gtest/gtest_stbservice_thunder.cpp

stbservice_libds_gtest_SOURCES = \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_AudioOutput.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_SPDIF.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_DisplayDevice.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_HDMI.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_VideoDecoder.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_VideoOutput.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Capabilities.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/gtest/thunder_plugin_stub.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/gtest/gtest_stbservice_libds.cpp

stbservice_thunder_gtest_CPPFLAGS = $(THUNDER_CPPFLAGS)
stbservice_thunder_gtest_LDADD    = $(COMMON_LDADD)
stbservice_thunder_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)

stbservice_libds_gtest_CPPFLAGS = $(LIBDS_CPPFLAGS)
stbservice_libds_gtest_LDADD    = $(COMMON_LDADD) $(LIBDS_ONLY_LDADD)
stbservice_libds_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
