/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2024 RDK Management
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
 * @file gtest_stbservice_thunder.cpp
 *
 * L1 unit tests for the Thunder-backed STBService components:
 *   - hostIf_STBServiceAudioInterface  (Components_AudioOutput_Thunder.cpp)
 *   - hostIf_STBServiceSPDIF           (Components_SPDIF_Thunder.cpp)
 *   - hostIf_STBServiceDisplayDevice   (Components_DisplayDevice_Thunder.cpp)
 *   - hostIf_STBServiceVideoDecoder    (Components_VideoDecoder_Thunder.cpp)
 *
 * Thunder calls are intercepted by ThunderStub (thunder_plugin_stub.cpp).
 * Each test fixture sets up canned responses before calling the SUT via the
 * public handleGetMsg / handleSetMsg API.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstring>
#include <string>

#include "hostIf_tr69ReqHandler.h"
#include "hostIf_main.h"
#include "Components_AudioOutput.h"
#include "Components_SPDIF.h"
#include "Components_DisplayDevice.h"
#include "Components_VideoDecoder.h"
#include "Components_VideoOutput.h"
#include "Components_HDMI.h"
#include "Capabilities.h"

/* ThunderStub API (defined in thunder_plugin_stub.cpp) */
namespace ThunderStub {
    void setBool(const std::string& method, bool success, bool value);
    void setString(const std::string& method, bool success, const std::string& value);
    void setInt(const std::string& method, bool success, int value);
    void setRaw(const std::string& method, bool success, const std::string& response);
    void clear();
}

/* Thunder method name constants mirrored here to avoid including internal .cpp headers */
#define THUNDER_DS_GET_SUPPORTED_AUDIO_PORTS  "org.rdk.DisplaySettings.getSupportedAudioPorts"
#define THUNDER_DS_GET_ENABLE_AUDIO_PORT      "org.rdk.DisplaySettings.getEnableAudioPort"
#define THUNDER_DS_GET_MUTED                  "org.rdk.DisplaySettings.getMuted"
#define THUNDER_DS_GET_VOLUME_LEVEL           "org.rdk.DisplaySettings.getVolumeLevel"
#define THUNDER_DS_GET_AUDIO_ENCODING         "org.rdk.DisplaySettings.getAudioEncoding"
#define THUNDER_DS_GET_AUDIO_FORMAT           "org.rdk.DisplaySettings.getAudioFormat"
#define THUNDER_DS_GET_SOUND_MODE             "org.rdk.DisplaySettings.getSoundMode"
#define THUNDER_DS_GET_MS12_AUDIO_COMPRESSION "org.rdk.DisplaySettings.getMS12AudioCompression"

#define THUNDER_DI_CONNECTED                  "DisplayInfo.1.connected"
#define THUNDER_DS_GET_SUPPORTED_RESOLUTIONS  "org.rdk.DisplaySettings.getSupportedResolutions"
#define THUNDER_DS_GET_DEFAULT_RESOLUTION     "org.rdk.DisplaySettings.getDefaultResolution"
#define THUNDER_DS_READ_EDID                  "org.rdk.DisplaySettings.readEDID"

#define THUNDER_PM_GET_POWER_STATE            "org.rdk.PowerManager.GetPowerState"
#define THUNDER_DS_GET_DISPLAY_ASPECT_RATIO   "org.rdk.DisplaySettings.getDisplayAspectRatio"
#define THUNDER_DS_GET_SUPPORTED_VIDEO_DISPLAYS "org.rdk.DisplaySettings.getSupportedVideoDisplays"

#define THUNDER_DS_GET_SUPPORTED_VIDEO_CODING_FORMATS "org.rdk.DisplaySettings.getSupportedVideoCodingFormats"
#define THUNDER_DS_GET_VIDEO_CODEC_INFO               "org.rdk.DisplaySettings.getVideoCodecInfo"
#define THUNDER_DS_GET_SUPPORTED_SETTOP_RESOLUTIONS   "org.rdk.DisplaySettings.getSupportedSettopResolutions"

#define THUNDER_DS_GET_CURRENT_RESOLUTION   "org.rdk.DisplaySettings.getCurrentResolution"
#define THUNDER_DS_GET_ENABLE_VIDEO_PORT    "org.rdk.DisplaySettings.getEnableVideoPort"
#define THUNDER_DS_GET_ZOOM_SETTINGS        "org.rdk.DisplaySettings.getZoomSetting"
#define THUNDER_HDCP_GET_STATUS             "org.rdk.HdcpProfile.getHDCPStatus"
#define THUNDER_DI_FRAMERATE                "DisplayInfo.1.framerate"

/* Helpers */
static HOSTIF_MsgData_t makeMsg()
{
    HOSTIF_MsgData_t m;
    memset(&m, 0, sizeof(m));
    return m;
}

/* ====================================================================
 * AudioOutput Tests
 * ==================================================================== */

class AudioOutputThunderTest : public ::testing::Test
{
protected:
    hostIf_STBServiceAudioInterface *m_iface = nullptr;

    void SetUp() override
    {
        ThunderStub::clear();
        hostIf_STBServiceAudioInterface::closeAllInstances();

        /* buildPortNameHash() will be called from getInstance() and will
         * query getSupportedAudioPorts.  Return "HDMI0" so that one
         * instance (dev_id=1, portName="HDMI0") is created. */
        ThunderStub::setString(THUNDER_DS_GET_SUPPORTED_AUDIO_PORTS,
                               true, "HDMI0");

        m_iface = hostIf_STBServiceAudioInterface::getInstance(1);
        ASSERT_NE(m_iface, nullptr) << "getInstance returned nullptr";
    }

    void TearDown() override
    {
        hostIf_STBServiceAudioInterface::closeAllInstances();
        ThunderStub::clear();
    }
};

/* getStatus: port enabled and un-muted → "Enabled" */
TEST_F(AudioOutputThunderTest, GetStatus_Enabled)
{
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_AUDIO_PORT, true, true);
    ThunderStub::setBool(THUNDER_DS_GET_MUTED,             true, false);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Enabled");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* getStatus: port enabled but muted → "Muted" */
TEST_F(AudioOutputThunderTest, GetStatus_Muted)
{
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_AUDIO_PORT, true, true);
    ThunderStub::setBool(THUNDER_DS_GET_MUTED,             true, true);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Muted");
}

/* getStatus: port disabled → "Disabled" */
TEST_F(AudioOutputThunderTest, GetStatus_Disabled)
{
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_AUDIO_PORT, true, false);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Disabled");
}

/* getStatus: Thunder call fails → NOK */
TEST_F(AudioOutputThunderTest, GetStatus_ThunderFailure)
{
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_AUDIO_PORT, false, false);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, NOK);
}

/* getEnable: always returns OK with boolean true (port is present in the list) */
TEST_F(AudioOutputThunderTest, GetEnable_AlwaysTrue)
{
    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Enable", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_BooleanType);
    /* put_boolean stores '1' for true */
    EXPECT_EQ(msg.paramValue[0], '1');
}

/* getName: returns "AudioOutputPort<portName><devId>" */
TEST_F(AudioOutputThunderTest, GetName_Format)
{
    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Name", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    /* portName="HDMI0", dev_id=1 → "AudioOutputPortHDMI01" */
    EXPECT_STREQ(msg.paramValue, "AudioOutputPortHDMI01");
}

/* getCancelMute: muted=true → paramValue '1' (boolean true) */
TEST_F(AudioOutputThunderTest, GetCancelMute_Muted)
{
    ThunderStub::setBool(THUNDER_DS_GET_MUTED, true, true);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("CancelMute", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_BooleanType);
    EXPECT_EQ(msg.paramValue[0], '1');
}

/* getCancelMute: muted=false → paramValue '0' (boolean false) */
TEST_F(AudioOutputThunderTest, GetCancelMute_NotMuted)
{
    ThunderStub::setBool(THUNDER_DS_GET_MUTED, true, false);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("CancelMute", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramValue[0], '0');
}

/* getCancelMute: Thunder failure → NOK */
TEST_F(AudioOutputThunderTest, GetCancelMute_ThunderFailure)
{
    ThunderStub::setBool(THUNDER_DS_GET_MUTED, false, false);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("CancelMute", &msg);

    EXPECT_EQ(rc, NOK);
}

/* getX_COMCAST_COM_AudioStereoMode: maps soundMode string through */
TEST_F(AudioOutputThunderTest, GetAudioStereoMode_Stereo)
{
    ThunderStub::setString(THUNDER_DS_GET_SOUND_MODE, true, "STEREO");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("X_COMCAST-COM_AudioStereoMode", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "STEREO");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* getX_COMCAST_COM_AudioStereoMode: Thunder failure → NOK */
TEST_F(AudioOutputThunderTest, GetAudioStereoMode_ThunderFailure)
{
    ThunderStub::setString(THUNDER_DS_GET_SOUND_MODE, false, "");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("X_COMCAST-COM_AudioStereoMode", &msg);

    EXPECT_EQ(rc, NOK);
}

/* getX_COMCAST_COM_AudioCompression: maps compressionlevel int */
TEST_F(AudioOutputThunderTest, GetAudioCompression_Value)
{
    ThunderStub::setInt(THUNDER_DS_GET_MS12_AUDIO_COMPRESSION, true, 3);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("X_COMCAST-COM_AudioCompression", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_UnsignedIntType);
    EXPECT_EQ(*reinterpret_cast<unsigned int*>(msg.paramValue), 3u);
}

/* getX_COMCAST_COM_AudioEncoding: maps encoding string through */
TEST_F(AudioOutputThunderTest, GetAudioEncoding_AC3)
{
    ThunderStub::setString(THUNDER_DS_GET_AUDIO_ENCODING, true, "AC3");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("X_COMCAST-COM_AudioEncoding", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "AC3");
}

/* getX_COMCAST_COM_AudioFormat: maps audioFormat string through */
TEST_F(AudioOutputThunderTest, GetAudioFormat)
{
    /* getX_COMCAST_COM_AudioFormat calls THUNDER_DS_GET_AUDIO_ENCODING internally */
    ThunderStub::setString(THUNDER_DS_GET_AUDIO_ENCODING, true, "PCM");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("AudioFormat", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "PCM");
}

/* handleSetMsg: all setters are NOT_HANDLED in Thunder build */
TEST_F(AudioOutputThunderTest, HandleSetMsg_AllReturnsNotHandled)
{
    const char *setParams[] = {
        "CancelMute", "AudioLevel", "X_COMCAST-COM_AudioEncoding",
        "X_COMCAST-COM_AudioStereoMode", "X_COMCAST-COM_AudioCompression",
        "X_COMCAST-COM_DialogEnhancement", "X_COMCAST-COM_AudioDB",
        "X_COMCAST-COM_AudioLoopThru"
    };
    HOSTIF_MsgData_t msg = makeMsg();
    for (const char *p : setParams)
    {
        int rc = m_iface->handleSetMsg(p, &msg);
        EXPECT_EQ(rc, NOT_HANDLED) << "Expected NOT_HANDLED for setter: " << p;
    }
}

/* handleGetMsg: unknown parameter name → NOT_HANDLED */
TEST_F(AudioOutputThunderTest, HandleGetMsg_UnknownParam_NotHandled)
{
    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("X_COMCAST-COM_AudioDB", &msg);
    EXPECT_EQ(rc, NOT_HANDLED);
}

/* getStatus with pChanged: detects change */
TEST_F(AudioOutputThunderTest, GetStatus_WithChangeDetection)
{
    /* First call: Enabled */
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_AUDIO_PORT, true, true);
    ThunderStub::setBool(THUNDER_DS_GET_MUTED, true, false);
    HOSTIF_MsgData_t msg1 = makeMsg();
    m_iface->handleGetMsg("Status", &msg1);

    /* Second call: Muted (changed) */
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_AUDIO_PORT, true, true);
    ThunderStub::setBool(THUNDER_DS_GET_MUTED, true, true);
    HOSTIF_MsgData_t msg2 = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg2);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg2.paramValue, "Muted");
}

/* getName with pChanged: no change */
TEST_F(AudioOutputThunderTest, GetName_NoChange)
{
    /* First call */
    HOSTIF_MsgData_t msg1 = makeMsg();
    m_iface->handleGetMsg("Name", &msg1);

    /* Second call: name doesn't change */
    HOSTIF_MsgData_t msg2 = makeMsg();
    int rc = m_iface->handleGetMsg("Name", &msg2);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg2.paramValue, "AudioOutputPortHDMI01");
}

/* getAudioLevel with pChanged: detects change */
TEST_F(AudioOutputThunderTest, GetAudioLevel_WithChangeDetection)
{
    /* First call: level 75 */
    ThunderStub::setInt(THUNDER_DS_GET_VOLUME_LEVEL, true, 75);
    HOSTIF_MsgData_t msg1 = makeMsg();
    m_iface->handleGetMsg("AudioLevel", &msg1);

    /* Second call: level 50 (changed) */
    ThunderStub::setInt(THUNDER_DS_GET_VOLUME_LEVEL, true, 50);
    HOSTIF_MsgData_t msg2 = makeMsg();
    int rc = m_iface->handleGetMsg("AudioLevel", &msg2);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(get_uint(msg2.paramValue), 50u);
}

/* getX_COMCAST_COM_AudioEncoding with pChanged: detects change */
TEST_F(AudioOutputThunderTest, GetAudioEncoding_WithChangeDetection)
{
    /* First call: AC3 */
    ThunderStub::setString(THUNDER_DS_GET_AUDIO_ENCODING, true, "AC3");
    HOSTIF_MsgData_t msg1 = makeMsg();
    m_iface->handleGetMsg("X_COMCAST-COM_AudioEncoding", &msg1);

    /* Second call: PCM (changed) */
    ThunderStub::setString(THUNDER_DS_GET_AUDIO_ENCODING, true, "PCM");
    HOSTIF_MsgData_t msg2 = makeMsg();
    int rc = m_iface->handleGetMsg("X_COMCAST-COM_AudioEncoding", &msg2);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg2.paramValue, "PCM");
}

/* ====================================================================
 * SPDIF Tests
 * ==================================================================== */

class SPDIFThunderTest : public ::testing::Test
{
protected:
    hostIf_STBServiceSPDIF *m_iface = nullptr;

    void SetUp() override
    {
        ThunderStub::clear();
        hostIf_STBServiceSPDIF::closeAllInstances();

        /* buildPortNameHash filters for names containing "spdif" (case-insensitive).
         * Return "SPDIF0" so one instance (dev_id=1, portName="SPDIF0") is created. */
        ThunderStub::setString(THUNDER_DS_GET_SUPPORTED_AUDIO_PORTS,
                               true, "SPDIF0");

        m_iface = hostIf_STBServiceSPDIF::getInstance(1);
        ASSERT_NE(m_iface, nullptr) << "getInstance returned nullptr";
    }

    void TearDown() override
    {
        hostIf_STBServiceSPDIF::closeAllInstances();
        ThunderStub::clear();
    }
};

/* getAlias: returns the port name stored at construction */
TEST_F(SPDIFThunderTest, GetAlias_ReturnsPortName)
{
    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Alias", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "SPDIF0");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* getEnable / getStatus: NOT_HANDLED in Thunder build */
TEST_F(SPDIFThunderTest, HandleGetMsg_Enable_NotHandled)
{
    HOSTIF_MsgData_t msg = makeMsg();
    EXPECT_EQ(m_iface->handleGetMsg("Enable", &msg), NOT_HANDLED);
}

TEST_F(SPDIFThunderTest, HandleGetMsg_Status_NotHandled)
{
    HOSTIF_MsgData_t msg = makeMsg();
    EXPECT_EQ(m_iface->handleGetMsg("Status", &msg), NOT_HANDLED);
}

TEST_F(SPDIFThunderTest, HandleGetMsg_ForcePCM_NotHandled)
{
    HOSTIF_MsgData_t msg = makeMsg();
    EXPECT_EQ(m_iface->handleGetMsg("ForcePCM", &msg), NOT_HANDLED);
}

/* handleSetMsg: all setters are NOT_HANDLED */
TEST_F(SPDIFThunderTest, HandleSetMsg_AllReturnsNotHandled)
{
    HOSTIF_MsgData_t msg = makeMsg();
    EXPECT_EQ(m_iface->handleSetMsg("Enable", &msg),  NOT_HANDLED);
    EXPECT_EQ(m_iface->handleSetMsg("Alias",  &msg),  NOT_HANDLED);
    EXPECT_EQ(m_iface->handleSetMsg("ForcePCM",&msg), NOT_HANDLED);
}

/* handleGetMsg: unknown parameter → NOT_HANDLED */
TEST_F(SPDIFThunderTest, HandleGetMsg_UnknownParam_NotHandled)
{
    HOSTIF_MsgData_t msg = makeMsg();
    EXPECT_EQ(m_iface->handleGetMsg("X_UnknownParam", &msg), NOT_HANDLED);
}

/* getAlias with pChanged: detects no change */
TEST_F(SPDIFThunderTest, GetAlias_NoChange)
{
    /* First call */
    HOSTIF_MsgData_t msg1 = makeMsg();
    m_iface->handleGetMsg("Alias", &msg1);

    /* Second call: alias doesn't change */
    HOSTIF_MsgData_t msg2 = makeMsg();
    int rc = m_iface->handleGetMsg("Alias", &msg2);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg2.paramValue, "SPDIF0");
}

/* Test getInstance with multiple ports */
TEST_F(SPDIFThunderTest, GetInstance_MultipleDevices)
{
    /* Close existing instances first */
    hostIf_STBServiceSPDIF::closeAllInstances();

    /* Return two SPDIF ports */
    ThunderStub::setString(THUNDER_DS_GET_SUPPORTED_AUDIO_PORTS, true, "SPDIF0,SPDIF1");

    /* Get first instance */
    hostIf_STBServiceSPDIF *inst1 = hostIf_STBServiceSPDIF::getInstance(1);
    ASSERT_NE(inst1, nullptr);

    /* Get second instance */
    hostIf_STBServiceSPDIF *inst2 = hostIf_STBServiceSPDIF::getInstance(2);
    ASSERT_NE(inst2, nullptr);

    /* Verify they're different instances */
    EXPECT_NE(inst1, inst2);

    hostIf_STBServiceSPDIF::closeAllInstances();
}

/* Test getInstance with invalid dev_id */
TEST_F(SPDIFThunderTest, GetInstance_InvalidDevId_ReturnsNull)
{
    hostIf_STBServiceSPDIF *inst = hostIf_STBServiceSPDIF::getInstance(999);
    EXPECT_EQ(inst, nullptr);
}

/* Test getAllInstances */
TEST_F(SPDIFThunderTest, GetAllInstances_ReturnsKeys)
{
    GList *list = hostIf_STBServiceSPDIF::getAllInstances();
    ASSERT_NE(list, nullptr);
    
    /* Should have at least one entry (dev_id=1 from setup) */
    EXPECT_GE(g_list_length(list), 1u);
    
    g_list_free(list);
}

/* ====================================================================
 * DisplayDevice Tests
 * ==================================================================== */

class DisplayDeviceThunderTest : public ::testing::Test
{
protected:
    hostIf_STBServiceDisplayDevice *m_iface = nullptr;

    void SetUp() override
    {
        ThunderStub::clear();
        /* DisplayDevice creates its instance directly (not via port list).
         * Instantiate with dev_id=1, portName="HDMI0". */
        m_iface = new hostIf_STBServiceDisplayDevice(1, "HDMI0");
        ASSERT_NE(m_iface, nullptr);
    }

    void TearDown() override
    {
        delete m_iface;
        m_iface = nullptr;
        ThunderStub::clear();
    }
};

/* getStatus: display connected → "Present" */
TEST_F(DisplayDeviceThunderTest, GetStatus_Present)
{
    ThunderStub::setBool(THUNDER_DI_CONNECTED, true, true);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Present");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* getStatus: display not connected → "Absent" */
TEST_F(DisplayDeviceThunderTest, GetStatus_Absent)
{
    ThunderStub::setBool(THUNDER_DI_CONNECTED, true, false);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Absent");
}

/* getStatus: Thunder call fails → still OK but returns "Absent" (connected=false) */
TEST_F(DisplayDeviceThunderTest, GetStatus_ThunderFailure_ReturnsAbsent)
{
    ThunderStub::setBool(THUNDER_DI_CONNECTED, false, false);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Absent");
}

/* handleSetMsg: always NOT_HANDLED */
TEST_F(DisplayDeviceThunderTest, HandleSetMsg_NotHandled)
{
    HOSTIF_MsgData_t msg = makeMsg();
    EXPECT_EQ(m_iface->handleSetMsg("Status", &msg), NOT_HANDLED);
}

/* ====================================================================
 * VideoDecoder Tests
 * ==================================================================== */

class VideoDecoderThunderTest : public ::testing::Test
{
protected:
    hostIf_STBServiceVideoDecoder *m_iface = nullptr;

    void SetUp() override
    {
        ThunderStub::clear();
        hostIf_STBServiceVideoDecoder::closeAllInstances();

        /* buildPortNameHash queries getSupportedVideoDisplays then creates
         * instance for each display.  Return "HDMI0" → dev_id=2 (first
         * entry reserved for "VideoDecoderHDMI0" at dev_id=1 in the impl).
         * Use getInstance(1) which triggers buildPortNameHash. */
        ThunderStub::setString(THUNDER_DS_GET_SUPPORTED_VIDEO_DISPLAYS, true, "HDMI0");

        m_iface = hostIf_STBServiceVideoDecoder::getInstance(1);
        ASSERT_NE(m_iface, nullptr) << "getInstance returned nullptr";
    }

    void TearDown() override
    {
        hostIf_STBServiceVideoDecoder::closeAllInstances();
        ThunderStub::clear();
    }
};

/* getStatus: currentState="ON" → "Enabled" */
TEST_F(VideoDecoderThunderTest, GetStatus_ON_Enabled)
{
    ThunderStub::setString(THUNDER_PM_GET_POWER_STATE, true, "ON");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Enabled");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* getStatus: currentState="STANDBY" → "X_COMCAST-COM_Standby" */
TEST_F(VideoDecoderThunderTest, GetStatus_STANDBY)
{
    ThunderStub::setString(THUNDER_PM_GET_POWER_STATE, true, "STANDBY");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "X_COMCAST-COM_Standby");
}

/* getStatus: currentState="LIGHT_SLEEP" also maps to Standby */
TEST_F(VideoDecoderThunderTest, GetStatus_LIGHT_SLEEP_IsStandby)
{
    ThunderStub::setString(THUNDER_PM_GET_POWER_STATE, true, "LIGHT_SLEEP");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "X_COMCAST-COM_Standby");
}

/* getStatus: currentState="DEEP_SLEEP" also maps to Standby */
TEST_F(VideoDecoderThunderTest, GetStatus_DEEP_SLEEP_IsStandby)
{
    ThunderStub::setString(THUNDER_PM_GET_POWER_STATE, true, "DEEP_SLEEP");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "X_COMCAST-COM_Standby");
}

/* getStatus: currentState="OFF" → "Disabled" */
TEST_F(VideoDecoderThunderTest, GetStatus_OFF_Disabled)
{
    ThunderStub::setString(THUNDER_PM_GET_POWER_STATE, true, "OFF");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Disabled");
}

/* getStatus: Thunder failure → NOK, paramValue="Error" */
TEST_F(VideoDecoderThunderTest, GetStatus_ThunderFailure)
{
    ThunderStub::setString(THUNDER_PM_GET_POWER_STATE, false, "");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, NOK);
    EXPECT_STREQ(msg.paramValue, "Error");
}

/* getName: returns "VideoDecoder<portName>" */
TEST_F(VideoDecoderThunderTest, GetName_Format)
{
    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Name", &msg);

    EXPECT_EQ(rc, OK);
    /* dev_id=1 instance is "VideoDecoderHDMI0" in buildPortNameHash */
    EXPECT_THAT(std::string(msg.paramValue), ::testing::StartsWith("VideoDecoder"));
}

/* getEnable: always returns true */
TEST_F(VideoDecoderThunderTest, GetEnable_AlwaysTrue)
{
    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Enable", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_BooleanType);
    EXPECT_EQ(msg.paramValue[0], '1');
}

/* handleSetMsg: always NOT_HANDLED */
TEST_F(VideoDecoderThunderTest, HandleSetMsg_NotHandled)
{
    HOSTIF_MsgData_t msg = makeMsg();
    EXPECT_EQ(m_iface->handleSetMsg("Enable", &msg), NOT_HANDLED);
    EXPECT_EQ(m_iface->handleSetMsg("Status", &msg), NOT_HANDLED);
}

/* getContentAspectRatio: Thunder returns "16:9" */
TEST_F(VideoDecoderThunderTest, GetContentAspectRatio_Success)
{
    ThunderStub::setString(THUNDER_DS_GET_DISPLAY_ASPECT_RATIO, true, "16:9");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("ContentAspectRatio", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "16:9");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* getContentAspectRatio: Thunder failure → falls back to "16:9", still returns OK */
TEST_F(VideoDecoderThunderTest, GetContentAspectRatio_ThunderFailure_FallsBackTo16_9)
{
    ThunderStub::setString(THUNDER_DS_GET_DISPLAY_ASPECT_RATIO, false, "");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("ContentAspectRatio", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "16:9");
}

/* getX_RDKCENTRAL-COM_MPEGHPart2: returns the capabilities path string */
TEST_F(VideoDecoderThunderTest, GetHEVC_ReturnsProfilePath)
{
    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("X_RDKCENTRAL-COM_MPEGHPart2", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    /* Value should be the HEVC_PROFILE_PATH constant from the implementation */
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("MPEGHPart2"));
}

/* ====================================================================
 * Additional AudioOutput Tests (missing getters)
 * ==================================================================== */

/* getAudioLevel: calls getVolumeLevel → returns numeric level as UnsignedInt */
TEST_F(AudioOutputThunderTest, GetAudioLevel_Success)
{
    ThunderStub::setInt(THUNDER_DS_GET_VOLUME_LEVEL, true, 75);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("AudioLevel", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_UnsignedIntType);
    EXPECT_EQ(get_uint(msg.paramValue), 75u);
}

/* getAudioLevel: Thunder failure → NOK */
TEST_F(AudioOutputThunderTest, GetAudioLevel_ThunderFailure)
{
    ThunderStub::setInt(THUNDER_DS_GET_VOLUME_LEVEL, false, 0);

    HOSTIF_MsgData_t msg = makeMsg();
    EXPECT_EQ(m_iface->handleGetMsg("AudioLevel", &msg), NOK);
}

/* getX_COMCAST_COM_AudioOptimalLevel: hardcoded "0.000000", no Thunder call */
TEST_F(AudioOutputThunderTest, GetAudioOptimalLevel_HardcodedZero)
{
    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("X_COMCAST-COM_AudioOptimalLevel", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "0.000000");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* ====================================================================
 * Additional DisplayDevice Tests (missing getters)
 * ==================================================================== */

/* getSupportedResolutions: Thunder returns "720p,1080p60" → formatted CSV */
TEST_F(DisplayDeviceThunderTest, GetSupportedResolutions_Success)
{
    ThunderStub::setString(THUNDER_DS_GET_SUPPORTED_RESOLUTIONS, true, "720p,1080p60");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("SupportedResolutions", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    /* "720p" → "1280x720p/59.94Hz", "1080p60" → "1920x1080p/59.94Hz" */
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("1280x720p/59.94Hz"));
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("1920x1080p/59.94Hz"));
}

/* getSupportedResolutions: Thunder failure → NOK */
TEST_F(DisplayDeviceThunderTest, GetSupportedResolutions_ThunderFailure)
{
    ThunderStub::setString(THUNDER_DS_GET_SUPPORTED_RESOLUTIONS, false, "");

    HOSTIF_MsgData_t msg = makeMsg();
    EXPECT_EQ(m_iface->handleGetMsg("SupportedResolutions", &msg), NOK);
}

/* getPreferredResolution: Thunder returns "1080p60" → TR-181 format */
TEST_F(DisplayDeviceThunderTest, GetPreferredResolution_Success)
{
    ThunderStub::setString(THUNDER_DS_GET_DEFAULT_RESOLUTION, true, "1080p60");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("PreferredResolution", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "1920x1080p/59.94Hz");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* getPreferredResolution: Thunder failure → NOK */
TEST_F(DisplayDeviceThunderTest, GetPreferredResolution_ThunderFailure)
{
    ThunderStub::setString(THUNDER_DS_GET_DEFAULT_RESOLUTION, false, "");

    HOSTIF_MsgData_t msg = makeMsg();
    EXPECT_EQ(m_iface->handleGetMsg("PreferredResolution", &msg), NOK);
}

/* getX_COMCAST_COM_EDID (via "EEDID"): empty EDID → empty paramValue, returns OK */
TEST_F(DisplayDeviceThunderTest, GetEEDID_NotConnected_EmptyValue)
{
    /* Stub returns empty string for EDID — display not connected */
    ThunderStub::setString(THUNDER_DS_READ_EDID, true, "");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("EEDID", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramValue[0], '\0');
}

/* getX_COMCAST_COM_EDID (via "X_COMCAST-COM_EDID"): same as above */
TEST_F(DisplayDeviceThunderTest, GetComcastEDID_NotConnected_EmptyValue)
{
    ThunderStub::setString(THUNDER_DS_READ_EDID, true, "");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("X_COMCAST-COM_EDID", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramValue[0], '\0');
}

/* ====================================================================
 * Capabilities Tests
 * ==================================================================== */

class CapabilitiesThunderTest : public ::testing::Test
{
protected:
    hostIf_STBServiceCapabilities *m_iface = nullptr;

    void SetUp() override
    {
        ThunderStub::clear();
        m_iface = hostIf_STBServiceCapabilities::getInstance();
        ASSERT_NE(m_iface, nullptr);
    }

    void TearDown() override
    {
        hostIf_STBServiceCapabilities::closeInstance(m_iface);
        m_iface = nullptr;
        ThunderStub::clear();
    }

    /** Helper: build a HOSTIF_MsgData_t with paramName set to the given path. */
    static HOSTIF_MsgData_t makeMsgWithPath(const char *paramName)
    {
        HOSTIF_MsgData_t m;
        memset(&m, 0, sizeof(m));
        strncpy(m.paramName, paramName, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
        return m;
    }
};

/* getVideoStandards: Thunder returns JSON containing "HEVC" and "H264" */
TEST_F(CapabilitiesThunderTest, GetVideoStandards_HEVC_and_H264)
{
    ThunderStub::setRaw(THUNDER_DS_GET_SUPPORTED_VIDEO_CODING_FORMATS, true,
        "{\"jsonrpc\":\"2.0\",\"id\":\"3\",\"result\":"
        "{\"supportedFormats\":[\"HEVC\",\"H264\"],\"success\":true}}");

    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities.VideoDecoder.VideoStandards");
    int rc = m_iface->handleGetMsg(&msg);

    EXPECT_EQ(rc, OK);
    EXPECT_THAT(std::string(msg.paramValue),
                ::testing::HasSubstr("MPEGH-Part2"));
    EXPECT_THAT(std::string(msg.paramValue),
                ::testing::HasSubstr("MPEG4-Part10"));
}

/* getVideoStandards: Thunder returns MPEG2 only */
TEST_F(CapabilitiesThunderTest, GetVideoStandards_MPEG2Only)
{
    ThunderStub::setRaw(THUNDER_DS_GET_SUPPORTED_VIDEO_CODING_FORMATS, true,
        "{\"jsonrpc\":\"2.0\",\"id\":\"3\",\"result\":"
        "{\"supportedFormats\":[\"MPEG2\"],\"success\":true}}");

    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities.VideoDecoder.VideoStandards");
    int rc = m_iface->handleGetMsg(&msg);

    EXPECT_EQ(rc, OK);
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("MPEG2-Part2"));
}

/* getVideoStandards: Thunder failure → NOK */
TEST_F(CapabilitiesThunderTest, GetVideoStandards_ThunderFailure)
{
    ThunderStub::setRaw(THUNDER_DS_GET_SUPPORTED_VIDEO_CODING_FORMATS, false, "");

    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities.VideoDecoder.VideoStandards");
    int rc = m_iface->handleGetMsg(&msg);

    EXPECT_EQ(rc, NOK);
}

/* getNumHEVCProfileEntries: Thunder returns 1 entry */
TEST_F(CapabilitiesThunderTest, GetNumHEVCProfileEntries_Success)
{
    ThunderStub::setRaw(THUNDER_DS_GET_VIDEO_CODEC_INFO, true,
        "{\"jsonrpc\":\"2.0\",\"id\":\"3\",\"result\":"
        "{\"numberOfEntries\":1,"
         "\"entries\":[{\"profile\":\"MAIN 10\",\"level\":5.1}],"
         "\"success\":true}}");

    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities."
        "VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevelNumberOfEntries");
    int rc = m_iface->handleGetMsg(&msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_UnsignedIntType);
    EXPECT_EQ(*reinterpret_cast<unsigned int *>(msg.paramValue), 1u);
}

/* getNumHEVCProfileEntries: Thunder failure → NOK */
TEST_F(CapabilitiesThunderTest, GetNumHEVCProfileEntries_ThunderFailure)
{
    ThunderStub::setRaw(THUNDER_DS_GET_VIDEO_CODEC_INFO, false, "");

    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities."
        "VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevelNumberOfEntries");
    int rc = m_iface->handleGetMsg(&msg);

    EXPECT_EQ(rc, NOK);
}

/* getHEVCProfileDetails: reads Profile name for ProfileLevel.1.Profile */
TEST_F(CapabilitiesThunderTest, GetHEVCProfileDetails_ProfileName)
{
    ThunderStub::setRaw(THUNDER_DS_GET_VIDEO_CODEC_INFO, true,
        "{\"jsonrpc\":\"2.0\",\"id\":\"3\",\"result\":"
        "{\"numberOfEntries\":1,"
         "\"entries\":[{\"profile\":\"MAIN 10\",\"level\":5.1}],"
         "\"success\":true}}");

    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities."
        "VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1.Profile");
    int rc = m_iface->handleGetMsg(&msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "MAIN 10");
}

/* getHEVCProfileDetails: reads Level for ProfileLevel.1.Level */
TEST_F(CapabilitiesThunderTest, GetHEVCProfileDetails_Level)
{
    ThunderStub::setRaw(THUNDER_DS_GET_VIDEO_CODEC_INFO, true,
        "{\"jsonrpc\":\"2.0\",\"id\":\"3\",\"result\":"
        "{\"numberOfEntries\":1,"
         "\"entries\":[{\"profile\":\"MAIN 10\",\"level\":5.1}],"
         "\"success\":true}}");

    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities."
        "VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1.Level");
    int rc = m_iface->handleGetMsg(&msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    /* Level is formatted as "5.1" */
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("5.1"));
}

/* getSupportedResolutions (HDMI): Thunder returns comma-delimited codes */
TEST_F(CapabilitiesThunderTest, GetSupportedResolutions_Success)
{
    /* getSupportedResolutions uses invokeThunderPluginMethod (raw JSON path) */
    ThunderStub::setRaw(THUNDER_DS_GET_SUPPORTED_SETTOP_RESOLUTIONS, true,
        "{\"supportedSettopResolutions\":[\"720p\",\"1080p60\"],\"success\":true}");

    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities.HDMI.SupportedResolutions");
    int rc = m_iface->handleGetMsg(&msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    /* getTR181ResolutionString converts "720p"→"1280x720p/59.94Hz", "1080p60"→"1920x1080p/59.94Hz" */
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("1280x720p/59.94Hz"));
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("1920x1080p/59.94Hz"));
}

/* getSupportedResolutions (HDMI): Thunder failure → NOK */
TEST_F(CapabilitiesThunderTest, GetSupportedResolutions_ThunderFailure)
{
    ThunderStub::setRaw(THUNDER_DS_GET_SUPPORTED_SETTOP_RESOLUTIONS, false, "");

    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities.HDMI.SupportedResolutions");
    int rc = m_iface->handleGetMsg(&msg);

    EXPECT_EQ(rc, NOK);
}

/* handleSetMsg: always NOT_HANDLED */
TEST_F(CapabilitiesThunderTest, HandleSetMsg_AlwaysNotHandled)
{
    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities.VideoDecoder.VideoStandards");
    EXPECT_EQ(m_iface->handleSetMsg(&msg), NOT_HANDLED);
}

/* Unknown path under Capabilities → NOK (invalid parameter name) */
TEST_F(CapabilitiesThunderTest, HandleGetMsg_UnknownPath_NOK)
{
    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities.VideoDecoder.UnknownParam");
    int rc = m_iface->handleGetMsg(&msg);
    EXPECT_EQ(rc, NOK);
}

/* getVideoStandards: multiple formats */
TEST_F(CapabilitiesThunderTest, GetVideoStandards_MultipleFormats)
{
    ThunderStub::setRaw(THUNDER_DS_GET_SUPPORTED_VIDEO_CODING_FORMATS, true,
        "{\"jsonrpc\":\"2.0\",\"id\":\"3\",\"result\":"
        "{\"supportedFormats\":[\"HEVC\",\"H264\",\"MPEG2\",\"VP9\"],\"success\":true}}");

    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities.VideoDecoder.VideoStandards");
    int rc = m_iface->handleGetMsg(&msg);

    EXPECT_EQ(rc, OK);
    /* Should contain all mapped formats */
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("MPEGH-Part2"));
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("MPEG4-Part10"));
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("MPEG2-Part2"));
}

/* getNumHEVCProfileEntries: multiple entries */
TEST_F(CapabilitiesThunderTest, GetNumHEVCProfileEntries_MultipleEntries)
{
    ThunderStub::setRaw(THUNDER_DS_GET_VIDEO_CODEC_INFO, true,
        "{\"jsonrpc\":\"2.0\",\"id\":\"3\",\"result\":"
        "{\"numberOfEntries\":3,"
         "\"entries\":["
         "{\"profile\":\"MAIN\",\"level\":4.0},"
         "{\"profile\":\"MAIN 10\",\"level\":5.0},"
         "{\"profile\":\"MAIN 10\",\"level\":5.1}"
         "],\"success\":true}}");

    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities."
        "VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevelNumberOfEntries");
    int rc = m_iface->handleGetMsg(&msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(*reinterpret_cast<unsigned int *>(msg.paramValue), 3u);
}

/* getHEVCProfileDetails: access second entry */
TEST_F(CapabilitiesThunderTest, GetHEVCProfileDetails_SecondEntry)
{
    ThunderStub::setRaw(THUNDER_DS_GET_VIDEO_CODEC_INFO, true,
        "{\"jsonrpc\":\"2.0\",\"id\":\"3\",\"result\":"
        "{\"numberOfEntries\":2,"
         "\"entries\":["
         "{\"profile\":\"MAIN\",\"level\":4.0},"
         "{\"profile\":\"MAIN 10\",\"level\":5.1}"
         "],\"success\":true}}");

    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities."
        "VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.2.Profile");
    int rc = m_iface->handleGetMsg(&msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "MAIN 10");
}

/* getSupportedResolutions: empty list */
TEST_F(CapabilitiesThunderTest, GetSupportedResolutions_EmptyList)
{
    ThunderStub::setRaw(THUNDER_DS_GET_SUPPORTED_SETTOP_RESOLUTIONS, true,
        "{\"supportedSettopResolutions\":[],\"success\":true}");

    HOSTIF_MsgData_t msg = makeMsgWithPath(
        "Device.Services.STBService.1.Capabilities.HDMI.SupportedResolutions");
    int rc = m_iface->handleGetMsg(&msg);

    EXPECT_EQ(rc, OK);
    /* Should return empty or minimal string */
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* ====================================================================
 * VideoOutput Tests
 * ==================================================================== */

class VideoOutputThunderTest : public ::testing::Test
{
protected:
    hostIf_STBServiceVideoOutput *m_iface = nullptr;

    void SetUp() override
    {
        ThunderStub::clear();
        hostIf_STBServiceVideoOutput::closeAllInstances();

        /* buildPortNameHash() queries getSupportedVideoDisplays.
         * Return "HDMI0" so one instance (dev_id=1, portName="HDMI0") is created. */
        ThunderStub::setString(THUNDER_DS_GET_SUPPORTED_VIDEO_DISPLAYS, true, "HDMI0");

        m_iface = hostIf_STBServiceVideoOutput::getInstance(1);
        ASSERT_NE(m_iface, nullptr) << "getInstance returned nullptr";
    }

    void TearDown() override
    {
        hostIf_STBServiceVideoOutput::closeAllInstances();
        ThunderStub::clear();
    }
};

/* getStatus: DisplayInfo reports connected → "Enabled" */
TEST_F(VideoOutputThunderTest, GetStatus_Connected_Enabled)
{
    ThunderStub::setBool(THUNDER_DI_CONNECTED, true, true);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Enabled");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* getStatus: DisplayInfo reports not connected → "Disabled" */
TEST_F(VideoOutputThunderTest, GetStatus_NotConnected_Disabled)
{
    ThunderStub::setBool(THUNDER_DI_CONNECTED, true, false);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Disabled");
}

/* getStatus: Thunder failure → connected=false → "Disabled", still OK */
TEST_F(VideoOutputThunderTest, GetStatus_ThunderFailure_Disabled)
{
    ThunderStub::setBool(THUNDER_DI_CONNECTED, false, false);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Disabled");
}

/* getEnable: always returns OK with boolean true */
TEST_F(VideoOutputThunderTest, GetEnable_AlwaysTrue)
{
    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Enable", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_BooleanType);
    EXPECT_EQ(msg.paramValue[0], '1');
}

/* getDisplayFormat: Thunder returns current resolution string */
TEST_F(VideoOutputThunderTest, GetDisplayFormat_Success)
{
    ThunderStub::setString(THUNDER_DS_GET_CURRENT_RESOLUTION, true, "1080p60");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("DisplayFormat", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "1080p60");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* getDisplayFormat: Thunder failure → NOK */
TEST_F(VideoOutputThunderTest, GetDisplayFormat_ThunderFailure)
{
    ThunderStub::setString(THUNDER_DS_GET_CURRENT_RESOLUTION, false, "");

    HOSTIF_MsgData_t msg = makeMsg();
    EXPECT_EQ(m_iface->handleGetMsg("DisplayFormat", &msg), NOK);
}

/* getVideoFormat: Thunder returns aspect ratio string */
TEST_F(VideoOutputThunderTest, GetVideoFormat_Success)
{
    ThunderStub::setString(THUNDER_DS_GET_DISPLAY_ASPECT_RATIO, true, "16:9");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("VideoFormat", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "16:9");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* getVideoFormat: Thunder failure → falls back to "Unknown", still OK */
TEST_F(VideoOutputThunderTest, GetVideoFormat_ThunderFailure_FallsBackToUnknown)
{
    ThunderStub::setString(THUNDER_DS_GET_DISPLAY_ASPECT_RATIO, false, "");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("VideoFormat", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Unknown");
}

/* getAspectRatioBehaviour: Thunder returns zoom mode */
TEST_F(VideoOutputThunderTest, GetAspectRatioBehaviour_Success)
{
    ThunderStub::setString(THUNDER_DS_GET_ZOOM_SETTINGS, true, "FULL");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("AspectRatioBehaviour", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "FULL");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* getAspectRatioBehaviour: Thunder failure → falls back to "None", still OK */
TEST_F(VideoOutputThunderTest, GetAspectRatioBehaviour_ThunderFailure_FallsBackToNone)
{
    ThunderStub::setString(THUNDER_DS_GET_ZOOM_SETTINGS, false, "");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("AspectRatioBehaviour", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "None");
}

/* getHDCP: HDCP compliant → boolean true */
TEST_F(VideoOutputThunderTest, GetHDCP_Compliant)
{
    ThunderStub::setBool(THUNDER_HDCP_GET_STATUS, true, true);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("HDCP", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_BooleanType);
    EXPECT_EQ(msg.paramValue[0], '1');
}

/* getHDCP: HDCP not compliant → boolean false */
TEST_F(VideoOutputThunderTest, GetHDCP_NotCompliant)
{
    ThunderStub::setBool(THUNDER_HDCP_GET_STATUS, true, false);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("HDCP", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramValue[0], '0');
}

/* getHDCP: Thunder failure → defaults to not-compliant (false), still OK */
TEST_F(VideoOutputThunderTest, GetHDCP_ThunderFailure_DefaultsToFalse)
{
    ThunderStub::setBool(THUNDER_HDCP_GET_STATUS, false, false);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("HDCP", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramValue[0], '0');
}

/* getName: returns the port name set at construction */
TEST_F(VideoOutputThunderTest, GetName_ReturnsPortName)
{
    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Name", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "HDMI0");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* handleSetMsg: always NOT_HANDLED */
TEST_F(VideoOutputThunderTest, HandleSetMsg_AlwaysNotHandled)
{
    HOSTIF_MsgData_t msg = makeMsg();
    EXPECT_EQ(m_iface->handleSetMsg("Status",               &msg), NOT_HANDLED);
    EXPECT_EQ(m_iface->handleSetMsg("Enable",               &msg), NOT_HANDLED);
    EXPECT_EQ(m_iface->handleSetMsg("DisplayFormat",        &msg), NOT_HANDLED);
    EXPECT_EQ(m_iface->handleSetMsg("AspectRatioBehaviour", &msg), NOT_HANDLED);
    EXPECT_EQ(m_iface->handleSetMsg("HDCP",                 &msg), NOT_HANDLED);
}

/* handleGetMsg: unknown parameter → NOT_HANDLED */
TEST_F(VideoOutputThunderTest, HandleGetMsg_UnknownParam_NotHandled)
{
    HOSTIF_MsgData_t msg = makeMsg();
    EXPECT_EQ(m_iface->handleGetMsg("X_UnknownParam", &msg), NOT_HANDLED);
}

/* ====================================================================
 * HDMI Tests
 * ==================================================================== */

class HDMIThunderTest : public ::testing::Test
{
protected:
    hostIf_STBServiceHDMI *m_iface = nullptr;

    void SetUp() override
    {
        ThunderStub::clear();
        hostIf_STBServiceHDMI::closeAllInstances();

        /* buildPortNameHash() queries getSupportedVideoDisplays.
         * Return "HDMI0" so one instance (dev_id=1, portName="HDMI0") is created. */
        ThunderStub::setString(THUNDER_DS_GET_SUPPORTED_VIDEO_DISPLAYS, true, "HDMI0");

        m_iface = hostIf_STBServiceHDMI::getInstance(1);
        ASSERT_NE(m_iface, nullptr) << "getInstance returned nullptr";
    }

    void TearDown() override
    {
        hostIf_STBServiceHDMI::closeAllInstances();
        ThunderStub::clear();
    }
};

/* getEnable: port enabled → boolean true */
TEST_F(HDMIThunderTest, GetEnable_Enabled)
{
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_VIDEO_PORT, true, true);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Enable", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_BooleanType);
    EXPECT_EQ(msg.paramValue[0], '1');
}

/* getEnable: port disabled → boolean false */
TEST_F(HDMIThunderTest, GetEnable_Disabled)
{
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_VIDEO_PORT, true, false);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Enable", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_BooleanType);
    EXPECT_EQ(msg.paramValue[0], '0');
}

/* getEnable: Thunder failure → NOK */
TEST_F(HDMIThunderTest, GetEnable_ThunderFailure)
{
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_VIDEO_PORT, false, false);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Enable", &msg);

    EXPECT_EQ(rc, NOK);
}

/* getStatus: port enabled → "Enabled" */
TEST_F(HDMIThunderTest, GetStatus_Enabled)
{
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_VIDEO_PORT, true, true);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Enabled");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* getStatus: port disabled → "Disabled" */
TEST_F(HDMIThunderTest, GetStatus_Disabled)
{
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_VIDEO_PORT, true, false);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Disabled");
}

/* getName: returns the port name */
TEST_F(HDMIThunderTest, GetName_ReturnsPortName)
{
    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("Name", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "HDMI0");
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
}

/* getResolutionValue: Thunder returns resolution details */
TEST_F(HDMIThunderTest, GetResolutionValue_Success)
{
    /* Mock getCurrentResolution to return full JSON with w, h, progressive */
    ThunderStub::setRaw(THUNDER_DS_GET_CURRENT_RESOLUTION, true,
        "{\"resolution\":\"1080p60\",\"w\":1920,\"h\":1080,\"progressive\":true,\"success\":true}");
    
    /* Mock framerate query to return "Framerate6000" (60.00 Hz) */
    ThunderStub::setString(THUNDER_DI_FRAMERATE, true, "Framerate6000");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("ResolutionValue", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    /* Expected format: "1920x1080p/60Hz" */
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("1920x1080p"));
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("60Hz"));
}

/* getResolutionValue: non-integer framerate (59.94) */
TEST_F(HDMIThunderTest, GetResolutionValue_DecimalFramerate)
{
    ThunderStub::setRaw(THUNDER_DS_GET_CURRENT_RESOLUTION, true,
        "{\"resolution\":\"1080p\",\"w\":1920,\"h\":1080,\"progressive\":true,\"success\":true}");
    
    /* Mock framerate "Framerate5994" → 59.94 Hz */
    ThunderStub::setString(THUNDER_DI_FRAMERATE, true, "Framerate5994");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("ResolutionValue", &msg);

    EXPECT_EQ(rc, OK);
    /* Expected format: "1920x1080p/59.94Hz" */
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("59.94Hz"));
}

/* getResolutionValue: interlaced format */
TEST_F(HDMIThunderTest, GetResolutionValue_Interlaced)
{
    ThunderStub::setRaw(THUNDER_DS_GET_CURRENT_RESOLUTION, true,
        "{\"resolution\":\"1080i50\",\"w\":1920,\"h\":1080,\"progressive\":false,\"success\":true}");
    
    ThunderStub::setString(THUNDER_DI_FRAMERATE, true, "Framerate5000");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("ResolutionValue", &msg);

    EXPECT_EQ(rc, OK);
    /* Expected format: "1920x1080i/50Hz" */
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("1920x1080i"));
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("50Hz"));
}

/* getResolutionValue: Thunder failure → NOK */
TEST_F(HDMIThunderTest, GetResolutionValue_ThunderFailure)
{
    ThunderStub::setRaw(THUNDER_DS_GET_CURRENT_RESOLUTION, false, "");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("ResolutionValue", &msg);

    EXPECT_EQ(rc, NOK);
}

/* getHDMIResolutionMode: returns static member value */
TEST_F(HDMIThunderTest, GetResolutionMode_ReturnsStaticValue)
{
    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("ResolutionMode", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg.paramtype, hostIf_StringType);
    /* Default is "Manual" defined in Components_HDMI.h (HDMI_RESOLUTION_MODE_MANUAL) */
    EXPECT_STREQ(msg.paramValue, "Manual");
}

/* handleSetMsg: all setters return NOT_HANDLED in Thunder build */
TEST_F(HDMIThunderTest, HandleSetMsg_AllReturnsNotHandled)
{
    const char *setParams[] = {
        "ResolutionMode", "ResolutionValue", "Enable"
    };
    HOSTIF_MsgData_t msg = makeMsg();
    for (const char *p : setParams)
    {
        int rc = m_iface->handleSetMsg(p, &msg);
        EXPECT_EQ(rc, NOT_HANDLED) << "Expected NOT_HANDLED for setter: " << p;
    }
}

/* handleGetMsg: unknown parameter → NOT_HANDLED */
TEST_F(HDMIThunderTest, HandleGetMsg_UnknownParam_NotHandled)
{
    HOSTIF_MsgData_t msg = makeMsg();
    EXPECT_EQ(m_iface->handleGetMsg("X_UnknownParam", &msg), NOT_HANDLED);
}

/* DisplayDevice sub-object: forwards to DisplayDevice handler */
TEST_F(HDMIThunderTest, GetDisplayDevice_Status_ForwardedToSubObject)
{
    ThunderStub::setBool(THUNDER_DI_CONNECTED, true, true);

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("DisplayDevice.Status", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg.paramValue, "Present");
}

/* getResolutionValue: framerate fallback to parsing resolution string */
TEST_F(HDMIThunderTest, GetResolutionValue_FramerateFallback)
{
    /* DisplayInfo.framerate fails, fall back to parsing resolution string */
    ThunderStub::setRaw(THUNDER_DS_GET_CURRENT_RESOLUTION, true,
        "{\"resolution\":\"1080p60\",\"w\":1920,\"h\":1080,\"progressive\":true,\"success\":true}");
    
    /* DisplayInfo.framerate fails */
    ThunderStub::setString(THUNDER_DI_FRAMERATE, false, "");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("ResolutionValue", &msg);

    EXPECT_EQ(rc, OK);
    /* Should parse "60" from "1080p60" → "1920x1080p/60Hz" */
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("60Hz"));
}

/* getResolutionValue: no framerate data available */
TEST_F(HDMIThunderTest, GetResolutionValue_NoFramerateData)
{
    /* Resolution without framerate indicator */
    ThunderStub::setRaw(THUNDER_DS_GET_CURRENT_RESOLUTION, true,
        "{\"resolution\":\"720p\",\"w\":1280,\"h\":720,\"progressive\":true,\"success\":true}");
    
    ThunderStub::setString(THUNDER_DI_FRAMERATE, false, "");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("ResolutionValue", &msg);

    EXPECT_EQ(rc, OK);
    /* Should return resolution without framerate: "1280x720p" */
    EXPECT_THAT(std::string(msg.paramValue), ::testing::StartsWith("1280x720p"));
}

/* getResolutionValue: 4K resolution */
TEST_F(HDMIThunderTest, GetResolutionValue_4K)
{
    ThunderStub::setRaw(THUNDER_DS_GET_CURRENT_RESOLUTION, true,
        "{\"resolution\":\"2160p60\",\"w\":3840,\"h\":2160,\"progressive\":true,\"success\":true}");
    
    ThunderStub::setString(THUNDER_DI_FRAMERATE, true, "Framerate6000");

    HOSTIF_MsgData_t msg = makeMsg();
    int rc = m_iface->handleGetMsg("ResolutionValue", &msg);

    EXPECT_EQ(rc, OK);
    EXPECT_THAT(std::string(msg.paramValue), ::testing::HasSubstr("3840x2160p/60Hz"));
}

/* getEnable with pChanged: detects change */
TEST_F(HDMIThunderTest, GetEnable_WithChangeDetection)
{
    /* First call: enabled=true */
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_VIDEO_PORT, true, true);
    HOSTIF_MsgData_t msg1 = makeMsg();
    m_iface->handleGetMsg("Enable", &msg1);

    /* Second call: enabled=false (changed) */
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_VIDEO_PORT, true, false);
    HOSTIF_MsgData_t msg2 = makeMsg();
    int rc = m_iface->handleGetMsg("Enable", &msg2);

    EXPECT_EQ(rc, OK);
    EXPECT_EQ(msg2.paramValue[0], '0');
}

/* getStatus with pChanged: detects change */
TEST_F(HDMIThunderTest, GetStatus_WithChangeDetection)
{
    /* First call: Enabled */
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_VIDEO_PORT, true, true);
    HOSTIF_MsgData_t msg1 = makeMsg();
    m_iface->handleGetMsg("Status", &msg1);

    /* Second call: Disabled (changed) */
    ThunderStub::setBool(THUNDER_DS_GET_ENABLE_VIDEO_PORT, true, false);
    HOSTIF_MsgData_t msg2 = makeMsg();
    int rc = m_iface->handleGetMsg("Status", &msg2);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg2.paramValue, "Disabled");
}

/* getName with pChanged: detects no change */
TEST_F(HDMIThunderTest, GetName_NoChange)
{
    /* First call */
    HOSTIF_MsgData_t msg1 = makeMsg();
    m_iface->handleGetMsg("Name", &msg1);

    /* Second call: name doesn't change */
    HOSTIF_MsgData_t msg2 = makeMsg();
    int rc = m_iface->handleGetMsg("Name", &msg2);

    EXPECT_EQ(rc, OK);
    EXPECT_STREQ(msg2.paramValue, "HDMI0");
}

/* getResolutionValue with pChanged: detects change */
TEST_F(HDMIThunderTest, GetResolutionValue_WithChangeDetection)
{
    /* First call: 1080p60 */
    ThunderStub::setRaw(THUNDER_DS_GET_CURRENT_RESOLUTION, true,
        "{\"resolution\":\"1080p60\",\"w\":1920,\"h\":1080,\"progressive\":true,\"success\":true}");
    ThunderStub::setString(THUNDER_DI_FRAMERATE, true, "Framerate6000");
    HOSTIF_MsgData_t msg1 = makeMsg();
    m_iface->handleGetMsg("ResolutionValue", &msg1);

    /* Second call: 720p (changed) */
    ThunderStub::setRaw(THUNDER_DS_GET_CURRENT_RESOLUTION, true,
        "{\"resolution\":\"720p\",\"w\":1280,\"h\":720,\"progressive\":true,\"success\":true}");
    ThunderStub::setString(THUNDER_DI_FRAMERATE, true, "Framerate5994");
    HOSTIF_MsgData_t msg2 = makeMsg();
    int rc = m_iface->handleGetMsg("ResolutionValue", &msg2);

    EXPECT_EQ(rc, OK);
    EXPECT_THAT(std::string(msg2.paramValue), ::testing::HasSubstr("1280x720p"));
}

/* ====================================================================
 * main
 * ==================================================================== */
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
