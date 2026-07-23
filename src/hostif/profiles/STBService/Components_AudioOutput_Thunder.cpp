/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
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
 * @file Components_AudioOutput_Thunder.cpp
 * @brief Thunder-backed implementation of TR069 Components AudioOutput.
 */

#include <sstream>
#include "Components_AudioOutput.h"

#define DEV_NAME "AudioOutput"
#define BASE_NAME "Device.Services.STBService.1.Components.AudioOutput"
#define UPDATE_FORMAT_STRING "%s.%d.%s"

#define STATUS_STRING                 "Status"
#define ENABLED_STRING                "Enabled"
#define ENABLE_STRING                 "Enable"
#define CANCELMUTE_STRING             "CancelMute"
#define AUDIOFORMAT_STRING            "AudioFormat"
#define NAME_STRING                   "Name"
#define AUDIOLEVEL_STRING             "AudioLevel"
#define COMCAST_AUDIOOPTIMALLEVEL_STRING "X_COMCAST-COM_AudioOptimalLevel"
#define COMCAST_MINAUDIODB_STRING     "X_COMCAST-COM_MinAudioDB"
#define COMCAST_MAXAUDIODB_STRING     "X_COMCAST-COM_MaxAudioDB"
#define COMCAST_AUDIODB_STRING        "X_COMCAST-COM_AudioDB"
#define COMCAST_AUDIOSTEREOMODE_STRING "X_COMCAST-COM_AudioStereoMode"
#define COMCAST_AUDIOLOOPTHRU_STRING  "X_COMCAST-COM_AudioLoopThru"
#define COMCAST_AUDIOENCODING_STRING  "X_COMCAST-COM_AudioEncoding"
#define COMCAST_AUDIOCOMPRESSION_STRING "X_COMCAST-COM_AudioCompression"
#define COMCAST_AUDIOGAIN_STRING      "X_COMCAST-COM_AudioGain"
#define COMCAST_DIALOGENHANCEMENT_STRING "X_COMCAST-COM_DialogEnhancement"

#define THUNDER_DS_GET_SUPPORTED_AUDIO_PORTS   "org.rdk.DisplaySettings.getSupportedAudioPorts"
#define THUNDER_DS_GET_ENABLE_AUDIO_PORT       "org.rdk.DisplaySettings.getEnableAudioPort"
#define THUNDER_DS_GET_MUTED                   "org.rdk.DisplaySettings.getMuted"
#define THUNDER_DS_GET_VOLUME_LEVEL            "org.rdk.DisplaySettings.getVolumeLevel"
#define THUNDER_DS_GET_AUDIO_ENCODING          "org.rdk.DisplaySettings.getAudioEncoding"
#define THUNDER_DS_GET_AUDIO_FORMAT            "org.rdk.DisplaySettings.getAudioFormat"
#define THUNDER_DS_GET_SOUND_MODE              "org.rdk.DisplaySettings.getSoundMode"
#define THUNDER_DS_GET_MS12_AUDIO_COMPRESSION  "org.rdk.DisplaySettings.getMS12AudioCompression"
#define THUNDER_DS_GET_AUDIO_DELAY             "org.rdk.DisplaySettings.getAudioDelay"
#define THUNDER_DS_GET_DIALOG_ENHANCEMENT      "org.rdk.DisplaySettings.getDialogEnhancement"

GHashTable * hostIf_STBServiceAudioInterface::ifHash = NULL;
GMutex hostIf_STBServiceAudioInterface::m_mutex;

void hostIf_STBServiceAudioInterface::buildPortNameHash()
{
    if (ifHash)
        g_hash_table_destroy(ifHash);

    ifHash = g_hash_table_new(NULL, NULL);

    std::string delimitedPorts;
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Calling Thunder API: %s\n", __FUNCTION__, THUNDER_DS_GET_SUPPORTED_AUDIO_PORTS);
    if (!invokeThunderPluginMethodAndExtractDelimitedStringArrayField(
            THUNDER_DS_GET_SUPPORTED_AUDIO_PORTS, "{}", "supportedAudioPorts", ",", delimitedPorts))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                "[%s:%d] Failed to get supported audio ports\n", __FUNCTION__, __LINE__);
        return;
    }

    int devId = 1;
    std::istringstream ss(delimitedPorts);
    std::string portName;
    while (std::getline(ss, portName, ','))
    {
        if (!portName.empty())
        {
            hostIf_STBServiceAudioInterface *pInst = new hostIf_STBServiceAudioInterface(devId, portName);
            g_hash_table_insert(ifHash, (gpointer)(intptr_t)devId, pInst);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
                    "[%s:%d] dev_id=%d portName=%s\n", __FUNCTION__, __LINE__, devId, portName.c_str());
            devId++;
        }
    }
}

hostIf_STBServiceAudioInterface* hostIf_STBServiceAudioInterface::getInstance(int dev_id)
{
    if (!ifHash)
        buildPortNameHash();

    hostIf_STBServiceAudioInterface* pRet =
        (hostIf_STBServiceAudioInterface*)g_hash_table_lookup(ifHash, (gpointer)(intptr_t)dev_id);

    if (!pRet)
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                "[%s:%s:%d]: No instance for dev_id=%d\n", __FILE__, __FUNCTION__, __LINE__, dev_id);
    }
    return pRet;
}

GList* hostIf_STBServiceAudioInterface::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_STBServiceAudioInterface::closeInstance(hostIf_STBServiceAudioInterface *pDev)
{
    if(pDev)
    {
        if (ifHash)
            g_hash_table_remove(ifHash, (gconstpointer)(intptr_t)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBServiceAudioInterface::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values(ifHash);
        GList* current = tmp_list;

        while(current)
        {
            hostIf_STBServiceAudioInterface* pDev = (hostIf_STBServiceAudioInterface*)current->data;
            current = current->next;
            delete pDev;
        }

        g_list_free(tmp_list);
        g_hash_table_destroy(ifHash);
        ifHash = NULL;
    }
}

void hostIf_STBServiceAudioInterface::getLock()
{
    g_mutex_init(&hostIf_STBServiceAudioInterface::m_mutex);
    g_mutex_lock(&hostIf_STBServiceAudioInterface::m_mutex);
}

void hostIf_STBServiceAudioInterface::releaseLock()
{
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s:%d] Unlocking mutex...\n", __FUNCTION__, __LINE__);
    g_mutex_unlock(&hostIf_STBServiceAudioInterface::m_mutex);
}

hostIf_STBServiceAudioInterface::hostIf_STBServiceAudioInterface(int devid, const std::string& portName)
    : dev_id(devid), m_portName(portName)
{
    strncpy(backupStatus, " ", sizeof(backupStatus));
    backupCancelMute = false;
    strncpy(backupAudioStereoMode, " ", sizeof(backupAudioStereoMode));
    backupAudioLevel = 0;
    strncpy(backupAudioDB, " ", sizeof(backupAudioDB));
    strncpy(backupAudioLoopThru, " ", sizeof(backupAudioLoopThru));
    backupAudioCompression = 0;
    strncpy(backupAudioEncoding, " ", sizeof(backupAudioEncoding));
    strncpy(backupAudioGain, " ", sizeof(backupAudioGain));
    strncpy(backupMinAudioDB, " ", sizeof(backupMinAudioDB));
    strncpy(backupMaxAudioDB, " ", sizeof(backupMaxAudioDB));
    strncpy(backupAudioOptimalLevel, " ", sizeof(backupAudioOptimalLevel));
    backupDialogEnhancement = 0;

    bCalledStatus = false;
    bCalledCancelMute = false;
    bCalledAudioStereoMode = false;
    bCalledAudioLevel = false;
    bCalledAudioDB = false;
    bCalledAudioLoopThru = false;
    bCalledAudioCompression = false;
    bCalledAudioEncoding = false;
    bCalledAudioGain = false;
    bCalledMinAudioDB = false;
    bCalledMaxAudioDB = false;
    bCalledAudioOptimalLevel = false;
    bCalledDialogEnhancement = false;
}

int hostIf_STBServiceAudioInterface::getNumberOfInstances(HOSTIF_MsgData_t *stMsgData)
{
    if (!ifHash)
        buildPortNameHash();

    put_int(stMsgData->paramValue, g_hash_table_size(ifHash));
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);
    return OK;
}

int hostIf_STBServiceAudioInterface::handleSetMsg(const char *pSetting, HOSTIF_MsgData_t *stMsgData)
{
    (void)pSetting; (void)stMsgData;
    return NOT_HANDLED;
}

int hostIf_STBServiceAudioInterface::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if (strcasecmp(paramName, STATUS_STRING) == 0)
        ret = getStatus(stMsgData);
    else if (strcasecmp(paramName, ENABLE_STRING) == 0)
        ret = getEnable(stMsgData);
    else if (strcasecmp(paramName, CANCELMUTE_STRING) == 0)
        ret = getCancelMute(stMsgData);
    else if (strcasecmp(paramName, AUDIOFORMAT_STRING) == 0)
        ret = getX_COMCAST_COM_AudioFormat(stMsgData);
    else if (strcasecmp(paramName, NAME_STRING) == 0)
        ret = getName(stMsgData);
    else if (strcasecmp(paramName, AUDIOLEVEL_STRING) == 0)
        ret = getAudioLevel(stMsgData);
    else if (strcasecmp(paramName, COMCAST_AUDIOOPTIMALLEVEL_STRING) == 0)
        ret = getX_COMCAST_COM_AudioOptimalLevel(stMsgData);
    else if (strcasecmp(paramName, COMCAST_AUDIOSTEREOMODE_STRING) == 0)
        ret = getX_COMCAST_COM_AudioStereoMode(stMsgData);
    else if (strcasecmp(paramName, COMCAST_AUDIOENCODING_STRING) == 0)
        ret = getX_COMCAST_COM_AudioEncoding(stMsgData);
    else if (strcasecmp(paramName, COMCAST_AUDIOCOMPRESSION_STRING) == 0)
        ret = getX_COMCAST_COM_AudioCompression(stMsgData);
    return ret;
}

void hostIf_STBServiceAudioInterface::doUpdates(updateCallback mUpdateCallback)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    char tmp_buff[PARAM_LEN];

#define DO_UPDATE(fn, paramStr) \
    memset(&msgData, 0, sizeof(msgData)); memset(tmp_buff, 0, PARAM_LEN); \
    bChanged = false; msgData.instanceNum = dev_id; \
    fn(&msgData, &bChanged); \
    if (bChanged) { \
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, paramStr); \
        if (mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, tmp_buff, msgData.paramValue, msgData.paramtype); \
    }

    DO_UPDATE(getCancelMute, CANCELMUTE_STRING)
    DO_UPDATE(getX_COMCAST_COM_AudioEncoding, COMCAST_AUDIOENCODING_STRING)
    DO_UPDATE(getAudioLevel, AUDIOLEVEL_STRING)
    DO_UPDATE(getX_COMCAST_COM_AudioOptimalLevel, COMCAST_AUDIOOPTIMALLEVEL_STRING)
    DO_UPDATE(getX_COMCAST_COM_AudioStereoMode, COMCAST_AUDIOSTEREOMODE_STRING)
    DO_UPDATE(getX_COMCAST_COM_AudioCompression, COMCAST_AUDIOCOMPRESSION_STRING)

#undef DO_UPDATE
}

/* ---- helpers ---- */

static std::string portParam(const std::string& portName)
{
    return std::string("{\"audioPort\":\"") + portName + "\"}";
}

/* ---- getters ---- */

int hostIf_STBServiceAudioInterface::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool enabled = false;
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Calling Thunder API: %s\n", __FUNCTION__, THUNDER_DS_GET_ENABLE_AUDIO_PORT);
    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DS_GET_ENABLE_AUDIO_PORT, portParam(m_portName), "enable", enabled))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder getEnableAudioPort failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }

    const char *status = "Disabled";
    if (enabled)
    {
        bool muted = false;
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Calling Thunder API: %s\n", __FUNCTION__, THUNDER_DS_GET_MUTED);
        invokeThunderPluginMethodAndExtractBoolField(THUNDER_DS_GET_MUTED, portParam(m_portName), "muted", muted);
        status = muted ? "Muted" : "Enabled";
    }

    strncpy(stMsgData->paramValue, status, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(status);

    if (bCalledStatus && pChanged && strcmp(backupStatus, stMsgData->paramValue))
        *pChanged = true;
    bCalledStatus = true;
    strncpy(backupStatus, stMsgData->paramValue, sizeof(backupStatus) - 1);
    backupStatus[sizeof(backupStatus) - 1] = '\0';
    return OK;
}

int hostIf_STBServiceAudioInterface::getEnable(HOSTIF_MsgData_t *stMsgData)
{
    put_boolean(stMsgData->paramValue, true);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);
    return OK;
}

int hostIf_STBServiceAudioInterface::getName(HOSTIF_MsgData_t *stMsgData)
{
    snprintf(stMsgData->paramValue, PARAM_LEN, "AudioOutputPort%s%d", m_portName.c_str(), dev_id);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    return OK;
}

int hostIf_STBServiceAudioInterface::getCancelMute(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool muted = false;
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Calling Thunder API: %s\n", __FUNCTION__, THUNDER_DS_GET_MUTED);
    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DS_GET_MUTED, portParam(m_portName), "muted", muted))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder getMuted failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }
    put_boolean(stMsgData->paramValue, muted);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);
    if (bCalledCancelMute && pChanged && (backupCancelMute != muted))
        *pChanged = true;
    bCalledCancelMute = true;
    backupCancelMute = muted;
    return OK;
}

int hostIf_STBServiceAudioInterface::getAudioLevel(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int volumeLevel = 0;
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Calling Thunder API: %s\n", __FUNCTION__, THUNDER_DS_GET_VOLUME_LEVEL);
    if (!invokeThunderPluginMethodAndExtractNumberField(
            THUNDER_DS_GET_VOLUME_LEVEL, portParam(m_portName), "volumeLevel", volumeLevel))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder getVolumeLevel failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }
    put_int(stMsgData->paramValue, volumeLevel);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);
    if (bCalledAudioLevel && pChanged && (backupAudioLevel != (unsigned)volumeLevel))
        *pChanged = true;
    bCalledAudioLevel = true;
    backupAudioLevel = (unsigned)volumeLevel;
    return OK;
}

int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioEncoding(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string encoding;
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Calling Thunder API: %s\n", __FUNCTION__, THUNDER_DS_GET_AUDIO_ENCODING);
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_GET_AUDIO_ENCODING, portParam(m_portName), "encoding", encoding))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder getAudioEncoding failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }
    /* Map Thunder UPPERCASE encoding to exact TR-135 legacy strings.
     * Legacy dsAUDIO_ENC_* values: NONE->"None", DISPLAY->"Display",
     * PCM->"PCM", AC3->"AC3", EAC3->"EAC3". Title-case normalization
     * is wrong for multi-char acronyms (AC3 -> Ac3, PCM -> Pcm). */
    const char *mapped = encoding.c_str();  /* default: pass through unknown values */
    if      (encoding == "NONE")    mapped = "None";
    else if (encoding == "DISPLAY") mapped = "Display";
    else if (encoding == "PCM")     mapped = "PCM";
    else if (encoding == "AC3")     mapped = "AC3";
    else if (encoding == "EAC3")    mapped = "EAC3";
    strncpy(stMsgData->paramValue, mapped, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (bCalledAudioEncoding && pChanged && strcmp(backupAudioEncoding, stMsgData->paramValue))
        *pChanged = true;
    bCalledAudioEncoding = true;
    strncpy(backupAudioEncoding, stMsgData->paramValue, _BUF_LEN_16);
    backupAudioEncoding[_BUF_LEN_16 - 1] = '\0';
    return OK;
}

int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioFormat(HOSTIF_MsgData_t *stMsgData)
{
    std::string encoding;
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Calling Thunder API: %s\n", __FUNCTION__, THUNDER_DS_GET_AUDIO_ENCODING);
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_GET_AUDIO_ENCODING, portParam(m_portName), "encoding", encoding))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder getAudioEncoding failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }
    /* Map Thunder encoding string to TR-181 AudioFormat — mirrors libds dsAudioEncoding_t switch:
     *   NONE    -> "None"
     *   DISPLAY -> "Other"  (platform-selected digital format)
     *   PCM     -> "PCM"
     *   AC3     -> "AC3"
     *   EAC3    -> "EAC3"
     */
    const char *fmt = "Other";
    if (encoding == "NONE")         fmt = "None";
    else if (encoding == "DISPLAY") fmt = "Other";
    else if (encoding == "PCM")     fmt = "PCM";
    else if (encoding == "AC3")     fmt = "AC3";
    else if (encoding == "EAC3")    fmt = "EAC3";

    strncpy(stMsgData->paramValue, fmt, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(fmt);
    return OK;
}

int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioStereoMode(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string mode;
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Calling Thunder API: %s\n", __FUNCTION__, THUNDER_DS_GET_SOUND_MODE);
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_GET_SOUND_MODE, portParam(m_portName), "soundMode", mode))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder getSoundMode failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }
    strncpy(stMsgData->paramValue, mode.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (bCalledAudioStereoMode && pChanged && strcmp(backupAudioStereoMode, stMsgData->paramValue))
        *pChanged = true;
    bCalledAudioStereoMode = true;
    strncpy(backupAudioStereoMode, stMsgData->paramValue, sizeof(backupAudioStereoMode) - 1);
    backupAudioStereoMode[sizeof(backupAudioStereoMode) - 1] = '\0';
    return OK;
}

int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioCompression(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int compression = 0;
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Calling Thunder API: %s\n", __FUNCTION__, THUNDER_DS_GET_MS12_AUDIO_COMPRESSION);
    if (!invokeThunderPluginMethodAndExtractNumberField(
            THUNDER_DS_GET_MS12_AUDIO_COMPRESSION, portParam(m_portName), "compressionlevel", compression))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder getAudioCompression failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }
    put_int(stMsgData->paramValue, compression);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);
    if (bCalledAudioCompression && pChanged && (backupAudioCompression != (unsigned)compression))
        *pChanged = true;
    bCalledAudioCompression = true;
    backupAudioCompression = (unsigned)compression;
    return OK;
}

// TODO: No Thunder API. Update after Operations team confirmation.
int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioOptimalLevel(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    (void)pChanged;
    snprintf(stMsgData->paramValue, PARAM_LEN, "0.000000");
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    return OK;
}


