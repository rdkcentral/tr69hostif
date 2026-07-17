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

bin_PROGRAMS = stbservice_thunder_gtest

TOP_DIR = $(top_srcdir)

# -DUSE_THUNDER_CLIENT  : compile the Thunder-backed implementations
# -DUNIT_TEST           : suppress any production-only #ifdefs
# -DGTEST_ENABLE        : enable gtest-specific code paths
COMMON_CPPFLAGS = \
    -std=c++11 \
    -DGTEST_ENABLE \
    -DUNIT_TEST \
    -DUSE_THUNDER_CLIENT \
    -I/usr/include \
    -I/usr/include/glib-2.0 \
    -I/usr/lib/x86_64-linux-gnu/glib-2.0/include \
    -I$(TOP_DIR)/src/hostif/include \
    -I$(TOP_DIR)/src/hostif/profiles/STBService \
    -I$(TOP_DIR)/src/hostif/handlers/include \
    -I$(TOP_DIR)/src/unittest/stubs \
    -I$(TOP_DIR)/src/unittest/stubs/ds

COMMON_LDADD = \
    -lgtest \
    -lgmock \
    -lgcov \
    $(GLIB_LIBS)

COMMON_CXXFLAGS = \
    -frtti \
    $(GLIB_CFLAGS) \
    -fprofile-arcs \
    -ftest-coverage

stbservice_thunder_gtest_SOURCES = \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_AudioOutput_Thunder.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_SPDIF_Thunder.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_DisplayDevice_Thunder.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/Components_VideoDecoder_Thunder.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/gtest/thunder_plugin_stub.cpp \
    $(TOP_DIR)/src/hostif/profiles/STBService/gtest/gtest_stbservice_thunder.cpp

stbservice_thunder_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
stbservice_thunder_gtest_LDADD    = $(COMMON_LDADD)
stbservice_thunder_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
