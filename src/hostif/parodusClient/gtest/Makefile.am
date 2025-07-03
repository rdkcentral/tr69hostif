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
COMMON_CPPFLAGS = -std=c++11  -I$(TOP_DIR)/src/unittest/stubs -I/usr/include -I/usr/include/cjson -I$(TOP_DIR)/src/hostif/parodusClient/waldb/ -I$(TOP_DIR)/src/hostif/parodusClient/pal/ -I$(TOP_DIR)/src/hostif/include/ -I$(TOP_DIR)/src/hostif/profiles/DeviceInfo -I$(TOP_DIR)/src/unittest/stubs/rbus/include

# Define the libraries to link against
COMMON_LDADD =  -lgtest -lgtest_main -lgmock_main -lgmock -lgcov $(GLIB_LIBS) -ltinyxml2 -lcjson -lcurl

# Define the compiler flags
COMMON_CXXFLAGS = -frtti $(GLIB_CFLAGS) -fprofile-arcs -ftest-coverage

# Define the source files
dm_gtest_SOURCES = dm_test.cpp $(TOP_DIR)/src/hostif/parodusClient/waldb/waldb.cpp $(TOP_DIR)/src/hostif/src/hostIf_utils.cpp $(TOP_DIR)/src/unittest/stubs/secure_wrapper.c $(TOP_DIR)/src/unittest/stubs/dm_stubs.cpp $(TOP_DIR)/src/hostif/profiles/DeviceInfo/XrdkCentralComRFCStore.cpp

# Apply common properties to each program
dm_gtest_CPPFLAGS = $(COMMON_CPPFLAGS) -g -pg
dm_gtest_LDADD = $(COMMON_LDADD) -pg
dm_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)

