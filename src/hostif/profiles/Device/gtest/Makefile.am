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
bin_PROGRAMS = device_gtest

# Define the include directories
COMMON_CPPFLAGS = -std=c++11 -DGTEST_ENABLE -DPARODUS -DUNIT_TEST -I$(TOP_DIR)/src/hostif/include -I$(TOP_DIR)/src/hostif/handlers/include -I/usr/include -I/usr/include/cjson -I$(TOP_DIR)/src/hostif/profiles/IP -I$(TOP_DIR)/src/hostif/profiles/Device -I$(TOP_DIR)/src/hostif/profiles/DeviceInfo -I$(TOP_DIR)/src/hostif/parodusClient/pal -I$(TOP_DIR)/src/hostif/parodusClient/waldb -I$(TOP_DIR)/src/unittest/stubs -I$(TOP_DIR)/src/unittest/stubs/ds -I$(TOP_DIR)/src/unittest/stubs/rbus/include  -I/usr/include/rbus -I/usr/local/include/rbus $(GLIB_CFLAGS) $(G_THREAD_CFLAGS) -I/usr/include/libsoup-3.0

# Define the libraries to link against
COMMON_LDADD =  -lgtest -lgtest_main -lgmock_main -lgmock -lgcov $(GLIB_LIBS) -ltinyxml2 -lcjson -lcurl -lsoup-3.0

# Define the compiler flags
COMMON_CXXFLAGS = -frtti $(GLIB_CFLAGS) $(SOUP_LIBS) -fprofile-arcs -ftest-coverage


# Define the source files
device_gtest_SOURCES = $(TOP_DIR)/src/hostif/src/hostIf_utils.cpp $(TOP_DIR)/src/hostif/src/IniFile.cpp $(TOP_DIR)/src/unittest/stubs/secure_wrapper.c $(TOP_DIR)/src/hostif/profiles/Device/x_rdk_profile.cpp $(TOP_DIR)/src/hostif/profiles/DeviceInfo/XrdkCentralComRFCStore.cpp $(TOP_DIR)/src/hostif/profiles/DeviceInfo/XrdkCentralComRFC.cpp $(TOP_DIR)/src/hostif/profiles/DeviceInfo/XrdkCentralComBSStore.cpp $(TOP_DIR)/src/hostif/profiles/DeviceInfo/XrdkCentralComBSStoreJournal.cpp $(TOP_DIR)/src/hostif/profiles/DeviceInfo/Device_DeviceInfo.cpp $(TOP_DIR)/src/hostif/handlers/src/hostIf_NotificationHandler.cpp $(TOP_DIR)/src/hostif/parodusClient/pal/webpa_notification.cpp $(TOP_DIR)/src/unittest/stubs/dm_stubs.cpp $(TOP_DIR)/src/hostif/parodusClient/waldb/waldb.cpp $(TOP_DIR)/src/hostif/profiles/Device/gtest/gtest_device.cpp

# Apply common properties to each program
device_gtest_CPPFLAGS = $(COMMON_CPPFLAGS)
device_gtest_LDADD = $(COMMON_LDADD)
device_gtest_CXXFLAGS = $(COMMON_CXXFLAGS)
