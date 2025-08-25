/*
 * Copyright 2024 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>

#include "rdk_debug.h"

#include "Device_DHCPv4_Client.h"

#include <curl/curl.h>
#include "cJSON.h"

#include <mutex>
#include <condition_variable>

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "hostif_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 128

using namespace std;


TEST(dhcpv4Test, isValidIPAddr) {
    int instanceNumber = 0;
    char* addr = (char*)"192.168.1.1";
    hostIf_DHCPv4Client *dhcpClient= hostIf_DHCPv4Client::getInstance(instanceNumber);
    if(dhcpClient)
    {
      bool result = dhcpClient->isValidIPAddr(addr);
      EXPECT_EQ(result, true);
    }
}

TEST(dhcpv4Test, getInterfaceName) {
    int instanceNumber = 0;
    char *ifname = (char*)"eth0";
    hostIf_DHCPv4Client *dhcpClient= hostIf_DHCPv4Client::getInstance(instanceNumber);
    if(dhcpClient)
    {
      int result = dhcpClient->getInterfaceName(ifname);
      EXPECT_EQ(result, -1);
    }
}

TEST(dhcpv4Test, isIfnameInroutetoDNSServer) {
    int instanceNumber = 0;
    char* dnsServer = (char*)"8.8.8.8";
    char* ifname = (char*)"eth0";
    hostIf_DHCPv4Client *dhcpClient= hostIf_DHCPv4Client::getInstance(instanceNumber);
    if(dhcpClient)
    {
      bool result = dhcpClient->isIfnameInroutetoDNSServer(dnsServer, ifname);
      EXPECT_EQ(result, true);
    }
}

TEST(dhcpv4Test, get_Device_DHCPv4_ClientNumberOfEntries) {
    int instanceNumber = 0;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    hostIf_DHCPv4Client *dhcpClient= hostIf_DHCPv4Client::getInstance(instanceNumber);
    if(dhcpClient)
    {
      int result = dhcpClient->get_Device_DHCPv4_ClientNumberOfEntries(&param);
      EXPECT_EQ(result, OK);
      EXPECT_EQ(param.paramtype, hostIf_UnsignedIntType);
    }
}

GTEST_API_ int main(int argc, char *argv[]){
    char testresults_fullfilepath[GTEST_REPORT_FILEPATH_SIZE];
    char buffer[GTEST_REPORT_FILEPATH_SIZE];

    memset( testresults_fullfilepath, 0, GTEST_REPORT_FILEPATH_SIZE );
    memset( buffer, 0, GTEST_REPORT_FILEPATH_SIZE );
    snprintf( testresults_fullfilepath, GTEST_REPORT_FILEPATH_SIZE, "json:%s%s" , GTEST_DEFAULT_RESULT_FILEPATH , GTEST_DEFAULT_RESULT_FILENAME);

    ::testing::GTEST_FLAG(output) = testresults_fullfilepath;
    ::testing::InitGoogleMock(&argc, argv);
    std::cout <<"running ut"<< std::endl;
    return RUN_ALL_TESTS();
}

