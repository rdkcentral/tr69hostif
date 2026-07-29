/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2026 RDK Management
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
 */

/**
 * @file gtest_stbservice_libds.cpp
 * @brief L1 unit tests for libds-backed STBService build.
 */

#include <gtest/gtest.h>
#include <cstring>

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "Capabilities.h"
#include "Components_AudioOutput.h"
#include "Components_SPDIF.h"
#include "Components_DisplayDevice.h"
#include "Components_HDMI.h"
#include "Components_VideoDecoder.h"
#include "Components_VideoOutput.h"
#include "videoOutputPort.hpp"

static HOSTIF_MsgData_t makeMsg()
{
    HOSTIF_MsgData_t msg;
    memset(&msg, 0, sizeof(msg));
    return msg;
}

static bool readBool(const HOSTIF_MsgData_t& msg)
{
    return get_boolean(msg.paramValue);
}

static int readInt(const HOSTIF_MsgData_t& msg)
{
    return get_int(msg.paramValue);
}

TEST(STBServiceLibdsSmokeTest, CapabilitiesInstanceLifecycle)
{
    hostIf_STBServiceCapabilities* iface = hostIf_STBServiceCapabilities::getInstance();
    ASSERT_NE(iface, nullptr);
    hostIf_STBServiceCapabilities::closeInstance(iface);
}

TEST(STBServiceLibdsSmokeTest, ComponentCloseAllInstancesIsSafe)
{
    EXPECT_NO_FATAL_FAILURE(hostIf_STBServiceAudioInterface::closeAllInstances());
    EXPECT_NO_FATAL_FAILURE(hostIf_STBServiceSPDIF::closeAllInstances());
    EXPECT_NO_FATAL_FAILURE(hostIf_STBServiceVideoOutput::closeAllInstances());
    EXPECT_NO_FATAL_FAILURE(hostIf_STBServiceVideoDecoder::closeAllInstances());
    EXPECT_NO_FATAL_FAILURE(hostIf_STBServiceHDMI::closeAllInstances());
}

class DisplayDeviceLibdsTest : public ::testing::Test
{
protected:
    device::VideoOutputPort& m_port = device::VideoOutputPort::getInstance(1);
    hostIf_STBServiceDisplayDevice m_iface{1, m_port};
};

TEST_F(DisplayDeviceLibdsTest, GetStatus_ReturnsPresent)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface.handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    EXPECT_STREQ(msg.paramValue, "Present");
}

TEST_F(DisplayDeviceLibdsTest, GetEEDID_ReturnsStructuredString)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface.handleGetMsg("EEDID", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    EXPECT_NE(strstr(msg.paramValue, "pcode=0x"), nullptr);
    EXPECT_NE(strstr(msg.paramValue, "pserial=0x"), nullptr);
    EXPECT_NE(strstr(msg.paramValue, "year="), nullptr);
    EXPECT_NE(strstr(msg.paramValue, "week="), nullptr);
}

TEST_F(DisplayDeviceLibdsTest, GetSupportedResolutions_ReturnsNonEmptyHzList)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface.handleGetMsg("SupportedResolutions", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    EXPECT_GT(msg.paramLen, 0);
    EXPECT_NE(strstr(msg.paramValue, "Hz"), nullptr);
}

TEST_F(DisplayDeviceLibdsTest, GetPreferredResolution_ReturnsSingleResolution)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface.handleGetMsg("PreferredResolution", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    EXPECT_GT(msg.paramLen, 0);
    EXPECT_NE(strstr(msg.paramValue, "Hz"), nullptr);
}

class AudioOutputLibdsTest : public ::testing::Test
{
protected:
    hostIf_STBServiceAudioInterface* m_iface = nullptr;

    void SetUp() override
    {
        m_iface = hostIf_STBServiceAudioInterface::getInstance(1);
        ASSERT_NE(m_iface, nullptr);
    }

    void TearDown() override
    {
        hostIf_STBServiceAudioInterface::closeAllInstances();
    }
};

TEST_F(AudioOutputLibdsTest, GetStatus_ReturnsMutedFromStub)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    EXPECT_STREQ(msg.paramValue, "Muted");
}

TEST_F(AudioOutputLibdsTest, GetEnable_ReturnsTrue)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface->handleGetMsg("Enable", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_BooleanType);
    EXPECT_TRUE(readBool(msg));
}

TEST_F(AudioOutputLibdsTest, GetAudioLevel_RoundsStubLevel)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface->handleGetMsg("AudioLevel", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_UnsignedIntType);
    EXPECT_EQ(readInt(msg), 1);
}

class SPDIFLibdsTest : public ::testing::Test
{
protected:
    hostIf_STBServiceSPDIF* m_iface = nullptr;

    void SetUp() override
    {
        m_iface = hostIf_STBServiceSPDIF::getInstance(1);
        ASSERT_NE(m_iface, nullptr);
    }

    void TearDown() override
    {
        hostIf_STBServiceSPDIF::closeAllInstances();
    }
};

TEST_F(SPDIFLibdsTest, GetStatus_ReturnsEnabled)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    EXPECT_STREQ(msg.paramValue, "Enabled");
}

TEST_F(SPDIFLibdsTest, GetForcePCM_ReturnsTrueForPCMStub)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface->handleGetMsg("ForcePCM", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_BooleanType);
    EXPECT_TRUE(readBool(msg));
}

class VideoOutputLibdsTest : public ::testing::Test
{
protected:
    hostIf_STBServiceVideoOutput* m_iface = nullptr;

    void SetUp() override
    {
        m_iface = hostIf_STBServiceVideoOutput::getInstance(1);
        ASSERT_NE(m_iface, nullptr);
    }

    void TearDown() override
    {
        hostIf_STBServiceVideoOutput::closeAllInstances();
    }
};

TEST_F(VideoOutputLibdsTest, GetStatus_ReturnsEnabled)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    EXPECT_STREQ(msg.paramValue, "Enabled");
}

TEST_F(VideoOutputLibdsTest, GetDisplayFormat_ReturnsNonEmpty)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface->handleGetMsg("DisplayFormat", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    EXPECT_GT(msg.paramLen, 0);
}

class HDMILibdsTest : public ::testing::Test
{
protected:
    hostIf_STBServiceHDMI* m_iface = nullptr;

    void SetUp() override
    {
        m_iface = hostIf_STBServiceHDMI::getInstance(1);
        ASSERT_NE(m_iface, nullptr);
    }

    void TearDown() override
    {
        hostIf_STBServiceHDMI::closeAllInstances();
    }
};

TEST_F(HDMILibdsTest, GetStatus_ReturnsEnabled)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    EXPECT_STREQ(msg.paramValue, "Enabled");
}

TEST_F(HDMILibdsTest, GetName_UsesInstanceNumber)
{
    HOSTIF_MsgData_t msg = makeMsg();
    msg.instanceNum = 1;
    const int rc = m_iface->handleGetMsg("Name", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    EXPECT_STREQ(msg.paramValue, "HDMI0");
}

class VideoDecoderLibdsTest : public ::testing::Test
{
protected:
    hostIf_STBServiceVideoDecoder* m_iface = nullptr;

    void SetUp() override
    {
        m_iface = hostIf_STBServiceVideoDecoder::getInstance(1);
        ASSERT_NE(m_iface, nullptr);
    }

    void TearDown() override
    {
        hostIf_STBServiceVideoDecoder::closeAllInstances();
    }
};

TEST_F(VideoDecoderLibdsTest, GetEnable_ReturnsTrue)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface->handleGetMsg("Enable", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_BooleanType);
    EXPECT_TRUE(readBool(msg));
}

TEST_F(VideoDecoderLibdsTest, GetName_ReturnsExpectedPortName)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface->handleGetMsg("Name", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    EXPECT_STREQ(msg.paramValue, "VideoDecoderHDMI0");
}

TEST_F(VideoDecoderLibdsTest, GetMpegHPart2_ReturnsProfilePath)
{
    HOSTIF_MsgData_t msg = makeMsg();
    const int rc = m_iface->handleGetMsg("X_RDKCENTRAL-COM_MPEGHPart2", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    EXPECT_NE(strstr(msg.paramValue, ".ProfileLevel.1"), nullptr);
}
