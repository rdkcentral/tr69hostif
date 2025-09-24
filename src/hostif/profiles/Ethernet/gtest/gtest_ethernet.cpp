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

#include "hostIf_utils.h"
#include "Device_Ethernet_Interface.h"
#include "Device_Ethernet_Interface_Stats.h"
#include "rdk_debug.h"


#include <curl/curl.h>
#include "cJSON.h"

#include <mutex>
#include <condition_variable>

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "hostif_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 128

using namespace std;

#ifdef GTEST_ENABLE
int (*EthernetInterfaceName(void))(unsigned int, char*);
#endif

TEST(EthernetTest, getEthernetInterfaceName) {
    unsigned int ethInterfaceNum = 1;
    char name[64];
    int ret = EthernetInterfaceName()(1, name);
    EXPECT_EQ(ret, OK);
}


TEST(EthernetTest, get_Device_Ethernet_InterfaceNumberOfEntries) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterface *ethernetIf= hostIf_EthernetInterface::getInstance(instanceNumber);
    if(ethernetIf)
    {
       int ret = ethernetIf->get_Device_Ethernet_InterfaceNumberOfEntries(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}


TEST(EthernetTest, get_Device_Ethernet_Interface_Enable) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterface *ethernetIf= hostIf_EthernetInterface::getInstance(instanceNumber);
    if(ethernetIf)
    {
       int ret = ethernetIf->get_Device_Ethernet_Interface_Enable(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}


TEST(EthernetTest, get_Device_Ethernet_Interface_Status) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterface *ethernetIf= hostIf_EthernetInterface::getInstance(instanceNumber);
    if(ethernetIf)
    {
       int ret = ethernetIf->get_Device_Ethernet_Interface_Status(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}

TEST(EthernetTest, get_Device_Ethernet_Interface_Name) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterface *ethernetIf= hostIf_EthernetInterface::getInstance(instanceNumber);
    if(ethernetIf)
    {
       int ret = ethernetIf->get_Device_Ethernet_Interface_Name(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}

TEST(EthernetTest, get_Device_Ethernet_Interface_Upstream) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterface *ethernetIf= hostIf_EthernetInterface::getInstance(instanceNumber);
    if(ethernetIf)
    {
       int ret = ethernetIf->get_Device_Ethernet_Interface_Upstream(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}

TEST(EthernetTest, get_Device_Ethernet_Interface_MACAddress) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterface *ethernetIf= hostIf_EthernetInterface::getInstance(instanceNumber);
    if(ethernetIf)
    {
       int ret = ethernetIf->get_Device_Ethernet_Interface_MACAddress(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}

TEST(EthernetTest, get_Device_Ethernet_Interface_MaxBitRate) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterface *ethernetIf= hostIf_EthernetInterface::getInstance(instanceNumber);
    if(ethernetIf)
    {
       int ret = ethernetIf->get_Device_Ethernet_Interface_MaxBitRate(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}
TEST(EthernetTest, get_Device_Ethernet_Interface_DuplexMode) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterface *ethernetIf= hostIf_EthernetInterface::getInstance(instanceNumber);
    if(ethernetIf)
    {
       int ret = ethernetIf->get_Device_Ethernet_Interface_DuplexMode(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}

TEST(EthernetTest, set_Device_Ethernet_Interface_Enable) {
    int instanceNumber = 1;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    strncpy(param.paramValue, "TRUE", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    hostIf_EthernetInterface *ethernetIf= hostIf_EthernetInterface::getInstance(instanceNumber);
    if(ethernetIf)
    {
       int ret = ethernetIf->set_Device_Ethernet_Interface_Enable(&param);
       EXPECT_EQ(ret, OK);
    }
}

TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_BytesSent) {
    int instanceNumber = 1;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_BytesSent(&param);
       EXPECT_EQ(ret, OK);
    }
}


TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_BytesReceived) {
    int instanceNumber = 1;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_BytesReceived(&param);
       EXPECT_EQ(ret, OK);
    }
}


TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_PacketsSent) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_PacketsSent(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}

TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_PacketsReceived) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_PacketsReceived(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}


TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_ErrorsSent) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_ErrorsSent(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}


TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_ErrorsReceived) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_ErrorsReceived(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}


TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_UnicastPacketsSent) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_UnicastPacketsSent(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}



TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_UnicastPacketsReceived) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_UnicastPacketsSent(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}

TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_DiscardPacketsSent) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_DiscardPacketsSent(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}

TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_DiscardPacketsReceived) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_DiscardPacketsReceived(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}

TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_MulticastPacketsSent) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_MulticastPacketsSent(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}


TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_MulticastPacketsReceived) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_MulticastPacketsReceived(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}


TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_BroadcastPacketsSent) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_BroadcastPacketsSent(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}

TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_BroadcastPacketsReceived) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_BroadcastPacketsReceived(&param, &pChanged);
       EXPECT_EQ(ret, OK);
    }
}

TEST(EthernetTest, get_Device_Ethernet_Interface_Stats_UnknownProtoPacketsReceived) {
    int instanceNumber = 1;
    bool pChanged = false;
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));

    hostIf_EthernetInterfaceStats *ethernetIfStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
    if(ethernetIfStats)
    {
       int ret = ethernetIfStats->get_Device_Ethernet_Interface_Stats_UnknownProtoPacketsReceived(&param, &pChanged);
       EXPECT_EQ(ret, OK);
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
