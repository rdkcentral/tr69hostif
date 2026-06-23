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
 * @brief setCancelMute() migrated to org.rdk.DisplaySettings.setMuted (Thunder plugin).
 *
 * Plugin  : org.rdk.DisplaySettings  (entservices-displaysettings)
 * Method  : setMuted
 * Params  : { "audioPort": "<name>", "muted": <true|false> }
 * Response: { "success": <true|false> }
 */


#include "hostIf_main.h"
#include "hostIf_utils.h"           /* invokeThunderPluginMethodAndExtractBoolField */
#include "Components_AudioOutput.h"
#include "dsTypes.h"
#include "illegalArgumentException.hpp"
#include "exception.hpp"
#include "safec_lib.h"
#include <sstream>

/* String constants used by handleSetMsg() dispatcher */
#define CANCELMUTE_STRING           "CancelMute"
#define AUDIOLEVEL_STRING           "AudioLevel"
#define COMCAST_AUDIODB_STRING      "X_COMCAST-COM_AudioDB"
#define COMCAST_AUDIOSTEREOMODE_STRING  "X_COMCAST-COM_AudioStereoMode"
#define COMCAST_AUDIOLOOPTHRU_STRING    "X_COMCAST-COM_AudioLoopThru"
#define COMCAST_AUDIOENCODING_STRING    "X_COMCAST-COM_AudioEncoding"
#define COMCAST_AUDIOCOMPRESSION_STRING "X_COMCAST-COM_AudioCompression"
#define COMCAST_DIALOGENHANCEMENT_STRING "X_COMCAST-COM_DialogEnhancement"

/* Thunder plugin method identifiers – DisplaySettings audio operations */
#define THUNDER_DISPLAYSETTINGS_SET_MUTED              "org.rdk.DisplaySettings.setMuted"
#define THUNDER_DISPLAYSETTINGS_GET_MUTED              "org.rdk.DisplaySettings.getMuted"
#define THUNDER_DISPLAYSETTINGS_GET_ENABLE_AUDIO_PORT  "org.rdk.DisplaySettings.getEnableAudioPort"
#define THUNDER_DISPLAYSETTINGS_GET_SOUND_MODE         "org.rdk.DisplaySettings.getSoundMode"
#define THUNDER_DISPLAYSETTINGS_SET_SOUND_MODE         "org.rdk.DisplaySettings.setSoundMode"
#define THUNDER_DISPLAYSETTINGS_GET_AUDIO_ENCODING     "org.rdk.DisplaySettings.getAudioEncoding"
#define THUNDER_DISPLAYSETTINGS_SET_AUDIO_ENCODING     "org.rdk.DisplaySettings.setAudioEncoding"
#define THUNDER_DISPLAYSETTINGS_GET_MS12_COMPRESSION   "org.rdk.DisplaySettings.getMS12AudioCompression"
#define THUNDER_DISPLAYSETTINGS_SET_MS12_COMPRESSION   "org.rdk.DisplaySettings.setMS12AudioCompression"
#define THUNDER_DISPLAYSETTINGS_GET_VOLUME_LEVEL       "org.rdk.DisplaySettings.getVolumeLevel"
#define THUNDER_DISPLAYSETTINGS_SET_VOLUME_LEVEL       "org.rdk.DisplaySettings.setVolumeLevel"
#define THUNDER_DISPLAYSETTINGS_GET_GAIN               "org.rdk.DisplaySettings.getGain"
#define THUNDER_DISPLAYSETTINGS_GET_DIALOG_ENHANCE     "org.rdk.DisplaySettings.getDialogEnhancement"
#define THUNDER_DISPLAYSETTINGS_SET_DIALOG_ENHANCE     "org.rdk.DisplaySettings.setDialogEnhancement"
#define THUNDER_DISPLAYSETTINGS_GET_SUPPORTED_AUDIO_PORTS "org.rdk.DisplaySettings.getSupportedAudioPorts"

/* Parameter name string constants for handleGetMsg() and doUpdates() */
#define STATUS_STRING                    "Status"
#define ENABLED_STRING                   "Enabled"
#define ENABLE_STRING                    "Enable"
#define AUDIOFORMAT_STRING               "AudioFormat"
#define NAME_STRING                      "Name"
#define COMCAST_AUDIOOPTIMALLEVEL_STRING "X_COMCAST-COM_AudioOptimalLevel"
#define COMCAST_MINAUDIODB_STRING        "X_COMCAST-COM_MinAudioDB"
#define COMCAST_MAXAUDIODB_STRING        "X_COMCAST-COM_MaxAudioDB"
#define COMCAST_AUDIOGAIN_STRING         "X_COMCAST-COM_AudioGain"
#define BASE_NAME                        "Device.Services.STBService.1.Components.AudioOutput"
#define UPDATE_FORMAT_STRING             "%s.%d.%s"

GHashTable * hostIf_STBServiceAudioInterface::ifHash = NULL;         /* dev_id → instance */
GMutex hostIf_STBServiceAudioInterface::m_mutex;

#define DEV_NAME "AudioOutput"

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
            hostIf_STBServiceAudioInterface* pDev = (hostIf_STBServiceAudioInterface *)current->data;
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
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] Unlocking mutex...  \n", __FUNCTION__, __LINE__);
    g_mutex_unlock(&hostIf_STBServiceAudioInterface::m_mutex);
}

/**
 * @brief Class Constructor of the class hostIf_STBServiceAudioInterface.
 *
 * It will initialize the device id and audio output port.
 *
 * @param[in] devid Identification number of the device.
 * @param[in] port Audio output port number.
 */
hostIf_STBServiceAudioInterface::hostIf_STBServiceAudioInterface(int devid, const std::string& portName) : dev_id(devid), m_portName(portName)
{
    errno_t rc = -1;
    rc=strcpy_s(backupStatus,sizeof(backupStatus), " ");
    ERR_CHK(rc);
    backupCancelMute = false;
    rc=strcpy_s(backupAudioStereoMode,sizeof(backupAudioStereoMode)," ");
    ERR_CHK(rc);
    backupAudioLevel=0;
    rc=strcpy_s(backupAudioDB,sizeof(backupAudioDB)," ");
    ERR_CHK(rc);
    rc=strcpy_s(backupAudioLoopThru,sizeof(backupAudioLoopThru)," ");
    ERR_CHK(rc);
    backupAudioCompression=0;
    rc=strcpy_s(backupAudioEncoding,sizeof(backupAudioEncoding)," ");
    ERR_CHK(rc);
    rc=strcpy_s(backupAudioGain,sizeof(backupAudioGain)," ");
    ERR_CHK(rc);
    rc=strcpy_s(backupMinAudioDB,sizeof(backupMinAudioDB)," ");
    ERR_CHK(rc);
    rc=strcpy_s(backupMaxAudioDB,sizeof(backupMaxAudioDB)," ");
    ERR_CHK(rc);
    rc=strcpy_s(backupAudioOptimalLevel,sizeof(backupAudioOptimalLevel)," ");
    ERR_CHK(rc);
    backupDialogEnhancement=0;
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

void hostIf_STBServiceAudioInterface::buildPortNameHash()
{
    if (ifHash)
        g_hash_table_destroy(ifHash);
    ifHash = g_hash_table_new(NULL, NULL);

    std::string delimitedPorts;
    if (!invokeThunderPluginMethodAndExtractDelimitedStringArrayField(
            THUNDER_DISPLAYSETTINGS_GET_SUPPORTED_AUDIO_PORTS,
            "{}", "supportedAudioPorts", ",", delimitedPorts))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                "[%s:%d] Failed to get supportedAudioPorts from Thunder\n", __FUNCTION__, __LINE__);
        return;
    }

    int devId = 1;
    std::istringstream ss(delimitedPorts);
    std::string portName;
    while (std::getline(ss, portName, ','))
    {
        if (!portName.empty())
        {
            hostIf_STBServiceAudioInterface* pInst = new hostIf_STBServiceAudioInterface(devId, portName);
            g_hash_table_insert(ifHash, (gpointer)(intptr_t)devId, pInst);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
                    "[%s:%d] ifHash: dev_id=%d portName=%s\n",
                    __FUNCTION__, __LINE__, devId, portName.c_str());
            devId++;
        }
    }
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

/************************************************************
 * Description  : Dispatch SET requests for AudioOutput parameters.
 *                Routes CancelMute to the Thunder-migrated setCancelMute().
 *                All other SET parameters remain delegated to their libds
 *                implementations in Components_AudioOutput.cpp.
 *
 * @param[in] pSetting  Parameter name string (e.g. "CancelMute").
 * @param[in] stMsgData HostIf Message Request containing the new value.
 *
 * @return OK on success, NOK on failure, NOT_HANDLED if unrecognised.
 * @ingroup TR69_HOSTIF_STBSERVICES_AUDIOOUTPUT_API
************************************************************/

int hostIf_STBServiceAudioInterface::handleSetMsg(const char *pSetting, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;

    if (strcasecmp(pSetting, CANCELMUTE_STRING) == 0)
    {
        /* ── MIGRATED: calls Thunder org.rdk.DisplaySettings.setMuted ── */
        ret = setCancelMute(stMsgData);
    }
    else if (strcasecmp(pSetting, AUDIOLEVEL_STRING) == 0)
    {
        /* ── MIGRATED: calls Thunder org.rdk.DisplaySettings.setVolumeLevel ── */
        ret = setAudioLevel(stMsgData);
    }
    else if (strcasecmp(pSetting, COMCAST_AUDIOSTEREOMODE_STRING) == 0)
    {
        /* ── MIGRATED: calls Thunder org.rdk.DisplaySettings.setSoundMode ── */
        ret = setX_COMCAST_COM_AudioStereoMode(stMsgData);
    }
#if 0
    else if (strcasecmp(pSetting, COMCAST_AUDIODB_STRING) == 0)
    {
        /* pending migration – no Thunder equivalent for setDB */
        ret = setX_COMCAST_COM_AudioDB(stMsgData);
    }
    else if (strcasecmp(pSetting, COMCAST_AUDIOLOOPTHRU_STRING) == 0)
    {
        /* pending migration – no Thunder equivalent for setLoopThru */
        ret = setX_COMCAST_COM_AudioLoopThru(stMsgData);
    }
#endif
    else if (strcasecmp(pSetting, COMCAST_AUDIOENCODING_STRING) == 0)
    {
        /* ── MIGRATED: calls Thunder org.rdk.DisplaySettings.setAudioEncoding ── */
        ret = setAudioEncoding(stMsgData);
    }
    else if (strcasecmp(pSetting, COMCAST_AUDIOCOMPRESSION_STRING) == 0)
    {
        /* ── MIGRATED: calls Thunder org.rdk.DisplaySettings.setMS12AudioCompression ── */
        ret = setX_COMCAST_COM_AudioCompression(stMsgData);
    }
    else if (strcasecmp(pSetting, COMCAST_DIALOGENHANCEMENT_STRING) == 0)
    {
        /* ── MIGRATED: calls Thunder org.rdk.DisplaySettings.setDialogEnhancement ── */
        ret = setX_COMCAST_COM_DialogEnhancement(stMsgData);
    }

    return ret;
}

/************************************************************
 * Description  : Cancel Audio Mute  [MIGRATED to Thunder]
 * Precondition : None
 * Input        : stMsgData->paramValue -> 1: cancels audio output mute status
 *                                      -> 0: Setting it to false has no effect
 *
 * Thunder plugin : org.rdk.DisplaySettings (entservices-displaysettings)
 * Thunder method : setMuted
 * JSON params    : { "audioPort": "<portName>", "muted": <true|false> }
 * JSON response  : { "success": <true|false> }
 *
 * Return       : OK -> Success
 *                NOK -> Failure
************************************************************/

int hostIf_STBServiceAudioInterface::setCancelMute(const HOSTIF_MsgData_t *stMsgData)
{
    bool toMute = get_boolean(stMsgData->paramValue);
    bool success = false;

    std::string audioPortName = m_portName;

    std::string paramsJson = "{\"audioPort\":\"" + audioPortName + "\","
                             "\"muted\":" + (toMute ? "true" : "false") + "}";

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] Calling Thunder %s with params: %s\n",
            __FUNCTION__, THUNDER_DISPLAYSETTINGS_SET_MUTED, paramsJson.c_str());

    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DISPLAYSETTINGS_SET_MUTED, paramsJson, "success", success)
        || !success)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder plugin call %s failed (muted=%s, port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_SET_MUTED,
                toMute ? "true" : "false", audioPortName.c_str());
        return NOK;
    }

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] Thunder %s succeeded (muted=%s, port=%s)\n",
            __FUNCTION__, THUNDER_DISPLAYSETTINGS_SET_MUTED,
            toMute ? "true" : "false", audioPortName.c_str());

    return OK;
}

/************************************************************
 * Description  : Get Audio Port Status  [MIGRATED to Thunder]
 * Thunder methods: getEnableAudioPort  →  { "enable": <bool> }
 *                  getMuted            →  { "muted":  <bool> }
 * Return       : OK -> Success, NOK -> Failure
 * stMsgData->paramValue: "Disabled" | "Enabled" | "Muted"
************************************************************/
int hostIf_STBServiceAudioInterface::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string audioPortName = m_portName;
    std::string paramsJson = "{\"audioPort\":\"" + audioPortName + "\"}";
    bool isEnabled = false;

    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DISPLAYSETTINGS_GET_ENABLE_AUDIO_PORT, paramsJson, "enable", isEnabled))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_GET_ENABLE_AUDIO_PORT, audioPortName.c_str());
        return NOK;
    }

    const char *audioPortStatus = isEnabled ? "Enabled" : "Disabled";

    if (isEnabled)
    {
        bool isMuted = false;
        if (!invokeThunderPluginMethodAndExtractBoolField(
                THUNDER_DISPLAYSETTINGS_GET_MUTED, paramsJson, "muted", isMuted))
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                    "[%s] Thunder %s failed (port=%s)\r\n",
                    __FUNCTION__, THUNDER_DISPLAYSETTINGS_GET_MUTED, audioPortName.c_str());
            return NOK;
        }
        audioPortStatus = isMuted ? "Muted" : "Enabled";
    }

    strncpy(stMsgData->paramValue, audioPortStatus, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);

    if (bCalledStatus && pChanged && strcmp(backupStatus, stMsgData->paramValue))
        *pChanged = true;
    bCalledStatus = true;
    strncpy(backupStatus, stMsgData->paramValue, sizeof(backupStatus) - 1);
    backupStatus[sizeof(backupStatus) - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s()] [Value: %s]\n", __FUNCTION__, stMsgData->paramValue);
    return OK;
}

/************************************************************
 * Description  : Get Audio Mute Status  [MIGRATED to Thunder]
 * Thunder method : getMuted
 * JSON params    : { "audioPort": "<portName>" }
 * JSON response  : { "muted": <bool>, "success": <bool> }
 * Return       : OK -> Success, NOK -> Failure
************************************************************/
int hostIf_STBServiceAudioInterface::getCancelMute(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string audioPortName = m_portName;
    std::string paramsJson = "{\"audioPort\":\"" + audioPortName + "\"}";
    bool isMuted = false;

    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DISPLAYSETTINGS_GET_MUTED, paramsJson, "muted", isMuted))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_GET_MUTED, audioPortName.c_str());
        return NOK;
    }

    put_boolean(stMsgData->paramValue, isMuted);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    if (bCalledCancelMute && pChanged && (backupCancelMute != get_boolean(stMsgData->paramValue)))
        *pChanged = true;
    bCalledCancelMute = true;
    backupCancelMute = get_boolean(stMsgData->paramValue);

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s()] getMute = %d\n", __FUNCTION__, isMuted);
    return OK;
}

/************************************************************
 * Description  : Set Stereo Mode  [MIGRATED to Thunder]
 * Thunder method : setSoundMode
 * JSON params    : { "soundMode": "<mode>", "audioPort": "<portName>" }
 * JSON response  : { "success": <bool> }
 * Input mapping : 1=Mono → "MONO", 2=Stereo → "STEREO", 3=Surround → "SURROUND"
 * Return       : OK -> Success, NOK -> Failure
************************************************************/
int hostIf_STBServiceAudioInterface::setX_COMCAST_COM_AudioStereoMode(const HOSTIF_MsgData_t *stMsgData)
{
    int mode = get_int(stMsgData->paramValue);
    const char *soundModeStr;
    switch (mode)
    {
        case 1:  soundModeStr = "MONO";     break;
        case 2:  soundModeStr = "STEREO";   break;
        case 3:  soundModeStr = "SURROUND"; break;
        default: soundModeStr = "STEREO";   break;  /* default to stereo for unknown */
    }

    std::string audioPortName = m_portName;
    std::string paramsJson = "{\"soundMode\":\"" + std::string(soundModeStr) + "\","
                             "\"audioPort\":\"" + audioPortName + "\"}";
    bool success = false;

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] Calling Thunder %s with params: %s\n",
            __FUNCTION__, THUNDER_DISPLAYSETTINGS_SET_SOUND_MODE, paramsJson.c_str());

    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DISPLAYSETTINGS_SET_SOUND_MODE, paramsJson, "success", success)
        || !success)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (mode=%s, port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_SET_SOUND_MODE, soundModeStr, audioPortName.c_str());
        return NOK;
    }
    return OK;
}

/************************************************************
 * Description  : Get Stereo Mode  [MIGRATED to Thunder]
 * Thunder method : getSoundMode
 * JSON params    : { "audioPort": "<portName>" }
 * JSON response  : { "soundMode": "<mode>", "success": <bool> }
 * Output mapping: "MONO"→"Mono", "STEREO"→"Stereo", "SURROUND"→"Surround",
 *                 "PASSTHRU"→"Expertmode", "AUTO*"→"Surround"
 * Return       : OK -> Success, NOK -> Failure
************************************************************/
int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioStereoMode(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string audioPortName = m_portName;
    std::string paramsJson = "{\"audioPort\":\"" + audioPortName + "\"}";
    std::string soundMode;

    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DISPLAYSETTINGS_GET_SOUND_MODE, paramsJson, "soundMode", soundMode))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_GET_SOUND_MODE, audioPortName.c_str());
        return NOK;
    }

    const char *tr69Mode;
    if      (soundMode == "MONO")                    tr69Mode = "Mono";
    else if (soundMode == "STEREO")                  tr69Mode = "Stereo";
    else if (soundMode == "SURROUND")                tr69Mode = "Surround";
    else if (soundMode == "PASSTHRU")                tr69Mode = "Expertmode";
    else if (soundMode.rfind("AUTO", 0) == 0)        tr69Mode = "Surround";
    else                                             tr69Mode = "Unknown";

    strncpy(stMsgData->paramValue, tr69Mode, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);

    if (bCalledAudioStereoMode && pChanged && strcmp(backupAudioStereoMode, stMsgData->paramValue))
        *pChanged = true;
    bCalledAudioStereoMode = true;
    strncpy(backupAudioStereoMode, stMsgData->paramValue, sizeof(backupAudioStereoMode) - 1);
    backupAudioStereoMode[sizeof(backupAudioStereoMode) - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Value:%s\n", __FUNCTION__, stMsgData->paramValue);
    return OK;
}

/************************************************************
 * Description  : Set Audio Encoding  [MIGRATED to Thunder]
 * Thunder method : setAudioEncoding
 * JSON params    : { "encoding": "<mode>", "audioPort": "<portName>" }
 * JSON response  : { "success": <bool> }
 * Input mapping : 0=None, 1=Display, 2=PCM, 3=AC3
 * Return       : OK -> Success, NOK -> Failure
************************************************************/
int hostIf_STBServiceAudioInterface::setAudioEncoding(const HOSTIF_MsgData_t *stMsgData)
{
    int newEncoding = atoi((const char *)stMsgData->paramValue);
    const char *encodingStr;
    switch (newEncoding)
    {
        case 0:  encodingStr = "NONE";    break;
        case 1:  encodingStr = "DISPLAY"; break;
        case 2:  encodingStr = "PCM";     break;
        case 3:  encodingStr = "AC3";     break;
        default:
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                    "[%s] Invalid encoding value: %d\n", __FUNCTION__, newEncoding);
            return NOK;
    }

    std::string audioPortName = m_portName;
    std::string paramsJson = "{\"encoding\":\"" + std::string(encodingStr) + "\","
                             "\"audioPort\":\"" + audioPortName + "\"}";
    bool success = false;

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] Calling Thunder %s with params: %s\n",
            __FUNCTION__, THUNDER_DISPLAYSETTINGS_SET_AUDIO_ENCODING, paramsJson.c_str());

    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DISPLAYSETTINGS_SET_AUDIO_ENCODING, paramsJson, "success", success)
        || !success)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (encoding=%s, port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_SET_AUDIO_ENCODING, encodingStr, audioPortName.c_str());
        return NOK;
    }
    return OK;
}

/************************************************************
 * Description  : Get Audio Encoding  [MIGRATED to Thunder]
 * Thunder method : getAudioEncoding
 * JSON params    : { "audioPort": "<portName>" }
 * JSON response  : { "encoding": "<mode>", "encodingId": <int>, "success": <bool> }
 * Output mapping: "NONE"→"None", "DISPLAY"→"Display", "PCM"→"PCM", "AC3"→"AC3"
 * Return       : OK -> Success, NOK -> Failure
************************************************************/
int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioEncoding(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string audioPortName = m_portName;
    std::string paramsJson = "{\"audioPort\":\"" + audioPortName + "\"}";
    std::string encoding;

    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DISPLAYSETTINGS_GET_AUDIO_ENCODING, paramsJson, "encoding", encoding))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_GET_AUDIO_ENCODING, audioPortName.c_str());
        return NOK;
    }

    const char *tr69Enc;
    if      (encoding == "NONE")    tr69Enc = "None";
    else if (encoding == "DISPLAY") tr69Enc = "Display";
    else if (encoding == "PCM")     tr69Enc = "PCM";
    else if (encoding == "AC3")     tr69Enc = "AC3";
    else
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Unknown encoding string from Thunder: %s\r\n",
                __FUNCTION__, encoding.c_str());
        return NOK;
    }

    strncpy(stMsgData->paramValue, tr69Enc, _BUF_LEN_16);
    stMsgData->paramValue[_BUF_LEN_16 - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);

    if (bCalledAudioEncoding && pChanged && strcmp(backupAudioEncoding, stMsgData->paramValue))
        *pChanged = true;
    bCalledAudioEncoding = true;
    strncpy(backupAudioEncoding, stMsgData->paramValue, _BUF_LEN_16);
    backupAudioEncoding[_BUF_LEN_16 - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] stMsgData->paramValue [%s]\n",
            __FUNCTION__, stMsgData->paramValue);
    return OK;
}

/************************************************************
 * Description  : Set Audio Compression  [MIGRATED to Thunder]
 * Thunder method : setMS12AudioCompression
 * JSON params    : { "compresionLevel": <int>, "audioPort": "<portName>" }
 *                  (note: single 's' typo matches the DisplaySettings plugin param name)
 * JSON response  : { "success": <bool> }
 * Input range   : 0 – 10
 * Return       : OK -> Success, NOK -> Failure
************************************************************/
int hostIf_STBServiceAudioInterface::setX_COMCAST_COM_AudioCompression(const HOSTIF_MsgData_t *stMsgData)
{
    int newComp = get_int(stMsgData->paramValue);
    if (newComp < 0 || newComp > 10)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Invalid compression value: %d\n", __FUNCTION__, newComp);
        return NOK;
    }

    std::string audioPortName = m_portName;
    std::string paramsJson = "{\"compresionLevel\":" + std::to_string(newComp) + ","
                             "\"audioPort\":\"" + audioPortName + "\"}";
    bool success = false;

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] Calling Thunder %s with params: %s\n",
            __FUNCTION__, THUNDER_DISPLAYSETTINGS_SET_MS12_COMPRESSION, paramsJson.c_str());

    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DISPLAYSETTINGS_SET_MS12_COMPRESSION, paramsJson, "success", success)
        || !success)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (level=%d, port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_SET_MS12_COMPRESSION, newComp, audioPortName.c_str());
        return NOK;
    }
    return OK;
}

/************************************************************
 * Description  : Get Audio Compression  [MIGRATED to Thunder]
 * Thunder method : getMS12AudioCompression
 * JSON params    : { "audioPort": "<portName>" }
 * JSON response  : { "compressionlevel": <int>, "enable": <bool>, "success": <bool> }
 * Return       : OK -> Success, NOK -> Failure
************************************************************/
int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioCompression(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string audioPortName = m_portName;
    std::string paramsJson = "{\"audioPort\":\"" + audioPortName + "\"}";
    int compression = 0;

    if (!invokeThunderPluginMethodAndExtractNumberField(
            THUNDER_DISPLAYSETTINGS_GET_MS12_COMPRESSION, paramsJson, "compressionlevel", compression))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_GET_MS12_COMPRESSION, audioPortName.c_str());
        return NOK;
    }

    put_int(stMsgData->paramValue, compression);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    if (bCalledAudioCompression && pChanged &&
        (backupAudioCompression != (unsigned int)get_int(stMsgData->paramValue)))
        *pChanged = true;
    bCalledAudioCompression = true;
    backupAudioCompression = get_int(stMsgData->paramValue);

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Value: %s\n", __FUNCTION__, stMsgData->paramValue);
    return OK;
}

/************************************************************
 * Description  : Set Audio Level  [MIGRATED to Thunder]
 * Thunder method : setVolumeLevel
 * JSON params    : { "volumeLevel": <float>, "audioPort": "<portName>" }
 * JSON response  : { "success": <bool> }
 * Input range   : 0 – 100
 * Return       : OK -> Success, NOK -> Failure
************************************************************/
int hostIf_STBServiceAudioInterface::setAudioLevel(const HOSTIF_MsgData_t *stMsgData)
{
    float newLevel = (float)get_int(stMsgData->paramValue);
    if (newLevel < 0.0f || newLevel > 100.0f)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Invalid audio level: %f\n", __FUNCTION__, newLevel);
        return NOK;
    }

    std::string audioPortName = m_portName;
    std::string paramsJson = "{\"volumeLevel\":" + std::to_string(newLevel) + ","
                             "\"audioPort\":\"" + audioPortName + "\"}";
    bool success = false;

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] Calling Thunder %s with params: %s\n",
            __FUNCTION__, THUNDER_DISPLAYSETTINGS_SET_VOLUME_LEVEL, paramsJson.c_str());

    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DISPLAYSETTINGS_SET_VOLUME_LEVEL, paramsJson, "success", success)
        || !success)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (level=%f, port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_SET_VOLUME_LEVEL, newLevel, audioPortName.c_str());
        return NOK;
    }
    return OK;
}

/************************************************************
 * Description  : Get Audio Level  [MIGRATED to Thunder]
 * Thunder method : getVolumeLevel
 * JSON params    : { "audioPort": "<portName>" }
 * JSON response  : { "volumeLevel": "<float-string>", "success": <bool> }
 * Return       : OK -> Success, NOK -> Failure
************************************************************/
int hostIf_STBServiceAudioInterface::getAudioLevel(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string audioPortName = m_portName;
    std::string paramsJson = "{\"audioPort\":\"" + audioPortName + "\"}";
    std::string levelStr;

    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DISPLAYSETTINGS_GET_VOLUME_LEVEL, paramsJson, "volumeLevel", levelStr))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_GET_VOLUME_LEVEL, audioPortName.c_str());
        return NOK;
    }

    float level = 0.0f;
    try { level = std::stof(levelStr); } catch (...) { return NOK; }

    put_int(stMsgData->paramValue, (int)(level + 0.5f));
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    if (bCalledAudioLevel && pChanged &&
        (backupAudioLevel != (unsigned int)get_int(stMsgData->paramValue)))
        *pChanged = true;
    bCalledAudioLevel = true;
    backupAudioLevel = get_int(stMsgData->paramValue);

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] volumeLevel=%s\n", __FUNCTION__, levelStr.c_str());
    return OK;
}

/************************************************************
 * Description  : Get Audio Gain  [MIGRATED to Thunder]
 * Thunder method : getGain
 * JSON params    : { "audioPort": "<portName>" }
 * JSON response  : { "gain": "<float-string>", "success": <bool> }
 * Return       : OK -> Success, NOK -> Failure
************************************************************/
int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioGain(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string audioPortName = m_portName;
    std::string paramsJson = "{\"audioPort\":\"" + audioPortName + "\"}";
    std::string gainStr;

    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DISPLAYSETTINGS_GET_GAIN, paramsJson, "gain", gainStr))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_GET_GAIN, audioPortName.c_str());
        return NOK;
    }

    snprintf(stMsgData->paramValue, PARAM_LEN, "%s", gainStr.c_str());
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);

    if (bCalledAudioGain && pChanged && strcmp(backupAudioGain, stMsgData->paramValue))
        *pChanged = true;
    bCalledAudioGain = true;
    strncpy(backupAudioGain, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupAudioGain[_BUF_LEN_16 - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] gain=%s\n", __FUNCTION__, gainStr.c_str());
    return OK;
}

/************************************************************
 * Description  : Set Dialog Enhancement Level  [MIGRATED to Thunder]
 * Thunder method : setDialogEnhancement
 * JSON params    : { "enhancerlevel": <int>, "audioPort": "<portName>" }
 * JSON response  : { "success": <bool> }
 * Input range   : 0 – 16
 * Return       : OK -> Success, NOK -> Failure
************************************************************/
int hostIf_STBServiceAudioInterface::setX_COMCAST_COM_DialogEnhancement(const HOSTIF_MsgData_t *stMsgData)
{
    int newLevel = get_int(stMsgData->paramValue);
    if (newLevel < 0 || newLevel > 16)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Invalid dialog enhancement level: %d\n", __FUNCTION__, newLevel);
        return NOK;
    }

    std::string audioPortName = m_portName;
    std::string paramsJson = "{\"enhancerlevel\":" + std::to_string(newLevel) + ","
                             "\"audioPort\":\"" + audioPortName + "\"}";
    bool success = false;

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] Calling Thunder %s with params: %s\n",
            __FUNCTION__, THUNDER_DISPLAYSETTINGS_SET_DIALOG_ENHANCE, paramsJson.c_str());

    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DISPLAYSETTINGS_SET_DIALOG_ENHANCE, paramsJson, "success", success)
        || !success)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (level=%d, port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_SET_DIALOG_ENHANCE, newLevel, audioPortName.c_str());
        return NOK;
    }
    return OK;
}

/************************************************************
 * Description  : Get Dialog Enhancement Level  [MIGRATED to Thunder]
 * Thunder method : getDialogEnhancement
 * JSON params    : { "audioPort": "<portName>" }
 * JSON response  : { "enhancerlevel": <int>, "enable": <bool>, "success": <bool> }
 * Return       : OK -> Success, NOK -> Failure
************************************************************/
int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_DialogEnhancement(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string audioPortName = m_portName;
    std::string paramsJson = "{\"audioPort\":\"" + audioPortName + "\"}";
    int enhancerLevel = 0;

    if (!invokeThunderPluginMethodAndExtractNumberField(
            THUNDER_DISPLAYSETTINGS_GET_DIALOG_ENHANCE, paramsJson, "enhancerlevel", enhancerLevel))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_GET_DIALOG_ENHANCE, audioPortName.c_str());
        return NOK;
    }

    put_int(stMsgData->paramValue, enhancerLevel);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    if (bCalledDialogEnhancement && pChanged &&
        (backupDialogEnhancement != (unsigned int)get_int(stMsgData->paramValue)))
        *pChanged = true;
    bCalledDialogEnhancement = true;
    backupDialogEnhancement = get_int(stMsgData->paramValue);

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] enhancerLevel=%d\n", __FUNCTION__, enhancerLevel);
    return OK;
}

/************************************************************
 * Description  : Get Audio Interface parameter values.
 *                Dispatches to individual getter methods.
 *
 * @param[in] paramName  Parameter name string.
 * @param[in] stMsgData  HostIf Message Request.
 *
 * @return OK / NOK / NOT_HANDLED
 * @ingroup TR69_HOSTIF_STBSERVICES_AUDIOOUTPUT_API
************************************************************/
int hostIf_STBServiceAudioInterface::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if(strcasecmp(paramName, STATUS_STRING) == 0)
    {
        ret = getStatus(stMsgData);
    }
    else if(strcasecmp(paramName, ENABLE_STRING) == 0)
    {
        ret = getEnable(stMsgData);
    }
    else if(strcasecmp(paramName, CANCELMUTE_STRING) == 0)
    {
        ret = getCancelMute(stMsgData);
    }
    else if(strcasecmp(paramName, AUDIOFORMAT_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioFormat(stMsgData);
    }
    else if(strcasecmp(paramName, NAME_STRING) == 0)
    {
        ret = getName(stMsgData);
    }
    else if(strcasecmp(paramName, AUDIOLEVEL_STRING) == 0)
    {
        ret = getAudioLevel(stMsgData);
    }
    else if(strcasecmp(paramName,COMCAST_AUDIOSTEREOMODE_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioStereoMode(stMsgData);
    }
#if 0
    else if(strcasecmp(paramName, COMCAST_AUDIOOPTIMALLEVEL_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioOptimalLevel(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_MINAUDIODB_STRING) == 0)
    {
        ret = getX_COMCAST_COM_MinAudioDB(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_MAXAUDIODB_STRING) == 0)
    {
        ret = getX_COMCAST_COM_MaxAudioDB(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_AUDIODB_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioDB(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_AUDIOLOOPTHRU_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioLoopThru(stMsgData);
    }
#endif
    else if(strcasecmp(paramName, COMCAST_AUDIOENCODING_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioEncoding(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_AUDIOCOMPRESSION_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioCompression(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_AUDIOGAIN_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioGain(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_DIALOGENHANCEMENT_STRING) == 0)
    {
        ret = getX_COMCAST_COM_DialogEnhancement(stMsgData);
    }
    return ret;
}

void hostIf_STBServiceAudioInterface::doUpdates(updateCallback mUpdateCallback)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    char tmp_buff[PARAM_LEN];

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getCancelMute(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, CANCELMUTE_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
    memset(&msgData,0,sizeof(msgData)); memset(tmp_buff,0,PARAM_LEN); bChanged=false; msgData.instanceNum=dev_id;
    getX_COMCAST_COM_AudioEncoding(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_AUDIOENCODING_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
    memset(&msgData,0,sizeof(msgData)); memset(tmp_buff,0,PARAM_LEN); bChanged=false; msgData.instanceNum=dev_id;
    getAudioLevel(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, AUDIOLEVEL_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
    memset(&msgData,0,sizeof(msgData)); memset(tmp_buff,0,PARAM_LEN); bChanged=false; msgData.instanceNum=dev_id;
    getX_COMCAST_COM_AudioStereoMode(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_AUDIOSTEREOMODE_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
#if 0
    memset(&msgData,0,sizeof(msgData)); memset(tmp_buff,0,PARAM_LEN); bChanged=false; msgData.instanceNum=dev_id;
    getX_COMCAST_COM_AudioOptimalLevel(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_AUDIOOPTIMALLEVEL_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
    memset(&msgData,0,sizeof(msgData)); memset(tmp_buff,0,PARAM_LEN); bChanged=false; msgData.instanceNum=dev_id;
    getX_COMCAST_COM_MinAudioDB(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_MINAUDIODB_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
    memset(&msgData,0,sizeof(msgData)); memset(tmp_buff,0,PARAM_LEN); bChanged=false; msgData.instanceNum=dev_id;
    getX_COMCAST_COM_MaxAudioDB(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_MAXAUDIODB_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
    memset(&msgData,0,sizeof(msgData)); memset(tmp_buff,0,PARAM_LEN); bChanged=false; msgData.instanceNum=dev_id;
    getX_COMCAST_COM_AudioDB(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_AUDIODB_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
    memset(&msgData,0,sizeof(msgData)); memset(tmp_buff,0,PARAM_LEN); bChanged=false; msgData.instanceNum=dev_id;
    getX_COMCAST_COM_AudioLoopThru(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_AUDIOLOOPTHRU_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
#endif
    memset(&msgData,0,sizeof(msgData)); memset(tmp_buff,0,PARAM_LEN); bChanged=false; msgData.instanceNum=dev_id;
    getX_COMCAST_COM_AudioCompression(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_AUDIOCOMPRESSION_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
    memset(&msgData,0,sizeof(msgData)); memset(tmp_buff,0,PARAM_LEN); bChanged=false; msgData.instanceNum=dev_id;
    getX_COMCAST_COM_AudioGain(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_AUDIOGAIN_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
    memset(&msgData,0,sizeof(msgData)); memset(tmp_buff,0,PARAM_LEN); bChanged=false; msgData.instanceNum=dev_id;
    getX_COMCAST_COM_DialogEnhancement(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_DIALOGENHANCEMENT_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
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

/* MIGRATED: getX_COMCAST_COM_AudioFormat → Thunder org.rdk.DisplaySettings.getAudioEncoding
 * Response field: "encoding" → "NONE"|"DISPLAY"|"PCM"|"AC3"|"UNKNOWN"
 * TR-181 mapping: NONE→"None", DISPLAY→"Other", PCM→"PCM", AC3→"AC3"
 * Note: dsAUDIO_ENC_EAC3 has no plugin equivalent (plugin returns "UNKNOWN").
 */
int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioFormat(HOSTIF_MsgData_t *stMsgData)
{
    std::string audioPortName = m_portName;
    std::string paramsJson = "{\"audioPort\":\"" + audioPortName + "\"}";
    std::string encoding;

    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DISPLAYSETTINGS_GET_AUDIO_ENCODING, paramsJson, "encoding", encoding))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s)\r\n",
                __FUNCTION__, THUNDER_DISPLAYSETTINGS_GET_AUDIO_ENCODING, audioPortName.c_str());
        return NOK;
    }

    const char *tr69Format;
    if      (encoding == "NONE")    tr69Format = "None";
    else if (encoding == "DISPLAY") tr69Format = "Other";
    else if (encoding == "PCM")     tr69Format = "PCM";
    else if (encoding == "AC3")     tr69Format = "AC3";
    else
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                "[%s] Unknown encoding from Thunder: %s\r\n", __FUNCTION__, encoding.c_str());
        return NOK;
    }

    strncpy(stMsgData->paramValue, tr69Format, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Value=%s\n", __FUNCTION__, stMsgData->paramValue);
    return OK;
}

#if 0
/* TODO: migrate setX_COMCAST_COM_AudioDB to Thunder (no direct DisplaySettings equivalent yet) */
int hostIf_STBServiceAudioInterface::setX_COMCAST_COM_AudioDB(const HOSTIF_MsgData_t *stMsgData)
{
    float newDb = (float)get_int(stMsgData->paramValue);
    try {
        device::List<device::AudioOutputPort> aPorts = device::Host::getInstance().getAudioOutputPorts();
        device::AudioOutputPort& aPort = aPorts.at(dev_id - 1);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Value: %s\n",__FUNCTION__, stMsgData->paramValue);
        aPort.setDB(newDb);
    }
    catch (const std::exception &e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }
    return OK;
}


/* TODO: migrate getX_COMCAST_COM_AudioDB to Thunder (no direct DisplaySettings equivalent yet) */
int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioDB(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    try {
        device::List<device::AudioOutputPort> aPorts = device::Host::getInstance().getAudioOutputPorts();
        device::AudioOutputPort& aPort = aPorts.at(dev_id - 1);
        float dbValue = aPort.getDB();
        snprintf(stMsgData->paramValue, PARAM_LEN, "%f", dbValue);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledAudioDB && pChanged && strcmp(backupAudioDB, stMsgData->paramValue))
            *pChanged = true;
        bCalledAudioDB = true;
        strncpy(backupAudioDB, stMsgData->paramValue, sizeof(backupAudioDB)-1);
        backupAudioDB[sizeof(backupAudioDB)-1] = '\0';
    }
    catch (const std::exception &e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }
    return OK;
}

/* TODO: migrate getX_COMCAST_COM_MinAudioDB to Thunder (no direct DisplaySettings equivalent yet) */
int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_MinAudioDB(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    try {
        device::List<device::AudioOutputPort> aPorts = device::Host::getInstance().getAudioOutputPorts();
        device::AudioOutputPort& aPort = aPorts.at(dev_id - 1);
        float minDbValue = aPort.getMinDB();
        snprintf(stMsgData->paramValue, PARAM_LEN, "%f", minDbValue);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledMinAudioDB && pChanged && strcmp(backupMinAudioDB, stMsgData->paramValue))
            *pChanged = true;
        bCalledMinAudioDB = true;
        strncpy(backupMinAudioDB, stMsgData->paramValue, _BUF_LEN_16-1);
        backupMinAudioDB[_BUF_LEN_16-1] = '\0';
    }
    catch (const std::exception &e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }
    return OK;
}

/* TODO: migrate getX_COMCAST_COM_MaxAudioDB to Thunder (no direct DisplaySettings equivalent yet) */
int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_MaxAudioDB(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    try {
        device::List<device::AudioOutputPort> aPorts = device::Host::getInstance().getAudioOutputPorts();
        device::AudioOutputPort& aPort = aPorts.at(dev_id - 1);
        float maxDbValue = aPort.getMaxDB();
        snprintf(stMsgData->paramValue, PARAM_LEN, "%f", maxDbValue);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledMaxAudioDB && pChanged && strcmp(backupMaxAudioDB, stMsgData->paramValue))
            *pChanged = true;
        bCalledMaxAudioDB = true;
        strncpy(backupMaxAudioDB, stMsgData->paramValue, _BUF_LEN_16-1);
        backupMaxAudioDB[_BUF_LEN_16-1] = '\0';
    }
    catch (const std::exception &e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }
    return OK;
}

/* TODO: migrate setX_COMCAST_COM_AudioLoopThru to Thunder (no direct DisplaySettings equivalent yet) */
int hostIf_STBServiceAudioInterface::setX_COMCAST_COM_AudioLoopThru(const HOSTIF_MsgData_t *stMsgData)
{
    bool loopThru = get_boolean(stMsgData->paramValue);
    try {
        device::List<device::AudioOutputPort> aPorts = device::Host::getInstance().getAudioOutputPorts();
        device::AudioOutputPort& aPort = aPorts.at(dev_id - 1);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Value: %s\n",__FUNCTION__, stMsgData->paramValue);
        aPort.setLoopThru(loopThru);
    }
    catch (const std::exception &e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }
    return OK;
}

/* TODO: migrate getX_COMCAST_COM_AudioLoopThru to Thunder (no direct DisplaySettings equivalent yet) */
int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioLoopThru(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    try {
        device::List<device::AudioOutputPort> aPorts = device::Host::getInstance().getAudioOutputPorts();
        device::AudioOutputPort& aPort = aPorts.at(dev_id - 1);
        bool isLoopThru = aPort.isLoopThru();
        snprintf(stMsgData->paramValue, PARAM_LEN, "%d", isLoopThru);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledAudioLoopThru && pChanged && strcmp(backupAudioLoopThru, stMsgData->paramValue))
            *pChanged = true;
        bCalledAudioLoopThru = true;
        strncpy(backupAudioLoopThru, stMsgData->paramValue, sizeof(backupAudioLoopThru)-1);
        backupAudioLoopThru[sizeof(backupAudioLoopThru)-1] = '\0';
    }
    catch (const std::exception &e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }
    return OK;
}

/* TODO: migrate getX_COMCAST_COM_AudioOptimalLevel to Thunder (no direct DisplaySettings equivalent yet) */
int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioOptimalLevel(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    try {
        device::List<device::AudioOutputPort> aPorts = device::Host::getInstance().getAudioOutputPorts();
        device::AudioOutputPort& aPort = aPorts.at(dev_id - 1);
        float optimalLevel = aPort.getOptimalLevel();
        snprintf(stMsgData->paramValue, PARAM_LEN, "%f", optimalLevel);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledAudioOptimalLevel && pChanged && strcmp(backupAudioOptimalLevel, stMsgData->paramValue))
            *pChanged = true;
        bCalledAudioOptimalLevel = true;
        strncpy(backupAudioOptimalLevel, stMsgData->paramValue, _BUF_LEN_16-1);
        backupAudioOptimalLevel[_BUF_LEN_16-1] = '\0';
    }
    catch (const std::exception &e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }
    return OK;
}
#endif