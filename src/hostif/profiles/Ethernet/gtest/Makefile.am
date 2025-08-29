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
bin_PROGRAMS = ethernet_gtest

# Define the include directories
COMMON_CPPFLAGS = -std=c++11 -DGTEST_ENABLE -I$(TOP_DIR)/src/hostif/include -I$(TOP_DIR)/src/hostif/handlers/include -I/usr/include -I/usr/include/cjson -I$(TOP_DIR)/src/hostif/profiles/DeviceInfo -I$(TOP_DIR)/src/hostif/profiles/Ethernet -I$(TOP_DIR)/src/hostif/profiles/DeviceInfo -I$(TOP_DIR)/src/hostif/parodusClient/pal -I$(TOP_DIR)/src/hostif/parodusClient/waldb -I$(TOP_DIR)/src/unittest/stubs -I$(TOP_DIR)/src/unittest/stubs/ds -I$(TOP_DIR)/src/unittest/stubs/rbus/include  -I/usr/include/rbus -I/usr/local/include/rbus $(GLIB_CFLAGS) $(G_THREAD_CFLAGS)

if LIBSOUP3_ENABLE
COMMON_CPPFLAGS +=  -I/usr/include/libsoup-3.0 -DLIBSOUP3_ENABLE
else
COMMON_CPPFLAGS +=  -I/usr/include/libsoup-2.4
endif

# Define the libraries to link against
COMMON_LDADD =  -lgtest -lgtest_main -lgmock_main -lgmock -lgcov $(GLIB_LIBS) -ltinyxml2 -lcjson -lcurl

if LIBSOUP3_ENABLE
COMMON_LDADD +=  -lsoup-3.0
endif

# Define the compiler flags
COMMON_CXXFLAGS = -frtti $(GLIB_CFLAGS) $(SOUP_LIBS) -fprofile-arcs -ftest-coverage

# Define the source files
ethernet_gtest_SOURCES = $(TOP_DIR)/src/hostif/src/hostIf_utils.cpp $(TOP_DIR)/src/hostif/src/IniFile.cpp $(TOP_DIR)/src/unittest/stubs/secure_wrapper.c $(TOP_DIR)/src/hostif/profiles/Ethernet/Device_Ethernet_Interface.cpp $(TOP_DIR)/src/hostif/profiles/Ethernet/Device_Ethernet_Interface_Stats.cpp $(TOP_DIR)/src/hostif/profiles/Ethernet/gtest/gtest_ethernet.cpp


# Apply common properties to each program
ethernet_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
ethernet_gtest_LDADD = $(COMMON_LDADD)
ethernet_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)

