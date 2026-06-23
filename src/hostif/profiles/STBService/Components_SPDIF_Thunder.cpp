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
 * @file Components_SPDIF_Thunder.cpp
 * @brief Components_SPDIF APIs migrated from libds to org.rdk.DisplaySettings Thunder plugin.
 *
 * Plugin  : org.rdk.DisplaySettings  (entservices-displaysettings)
 * Port    : SPDIF0, SPDIF1, ... (derived from aPort.getName())
 *
 * Migrated APIs:
 *   handleSetMsg()     — SET dispatcher (moved from Components_SPDIF.cpp)
 *   getStatus()        → org.rdk.DisplaySettings.getEnableAudioPort ("enable" bool → "Enabled"/"Disabled")
 *   getEnable()        → org.rdk.DisplaySettings.getEnableAudioPort ("enable" bool)
 *   setEnable()        → org.rdk.DisplaySettings.setEnableAudioPort
 *   getName()          — aPort.getName() in-memory (no HAL call; returns "SPDIF0", "SPDIF1", ...)
 *   setForcePCM()      → org.rdk.DisplaySettings.setAudioEncoding ("PCM" when true, "DISPLAY" when false)
 *   getForcePCM()      → org.rdk.DisplaySettings.getAudioEncoding  (encoding == "PCM")
 *   getPassthrough()   → org.rdk.DisplaySettings.getAudioEncoding  (encoding == "DISPLAY" or "AC3")
 *   getAudioDelay()    — NOT supported for SPDIF; returns NOT_HANDLED
 *   setAudioDelay()    → org.rdk.DisplaySettings.setAudioDelay     (new — not in original handleSetMsg)
 *
 * Not migrated (no DisplaySettings equivalent):
 *   getAlias()         — remains NOT_HANDLED in Components_SPDIF.cpp
 *   setAlias()         — remains NOT_HANDLED in Components_SPDIF.cpp
 */

#include "hostIf_main.h"
#include "hostIf_utils.h"           /* invokeThunderPluginMethod*, invokeThunderPluginMethodAndExtract* */
#include "Components_SPDIF.h"
#include <sstream>

/* String constants used by handleSetMsg() and handleGetMsg() dispatchers */
#define ENABLE_STRING       "Enable"
#define STATUS_STRING       "Status"
#define ALIAS_STRING        "Alias"
#define NAME_STRING         "Name"
#define FORCEPCM_STRING     "ForcePCM"
#define PASSTHROUGH_STRING  "Passthrough"
#define AUDIODELAY_STRING   "AudioDelay"

#define BASE_NAME            "Device.Services.STBService.1.Components.SPDIF"
#define UPDATE_FORMAT_STRING "%s.%d.%s"
#define DEV_NAME             "SPDIF"

/* Thunder plugin method identifiers — DisplaySettings SPDIF operations */
#define THUNDER_DISPLAYSETTINGS_GET_ENABLE_AUDIO_PORT      "org.rdk.DisplaySettings.getEnableAudioPort"
#define THUNDER_DISPLAYSETTINGS_SET_ENABLE_AUDIO_PORT      "org.rdk.DisplaySettings.setEnableAudioPort"
#define THUNDER_DISPLAYSETTINGS_GET_AUDIO_ENCODING         "org.rdk.DisplaySettings.getAudioEncoding"
#define THUNDER_DISPLAYSETTINGS_SET_AUDIO_ENCODING         "org.rdk.DisplaySettings.setAudioEncoding"
#define THUNDER_DISPLAYSETTINGS_GET_AUDIO_DELAY            "org.rdk.DisplaySettings.getAudioDelay"
#define THUNDER_DISPLAYSETTINGS_SET_AUDIO_DELAY            "org.rdk.DisplaySettings.setAudioDelay"
#define THUNDER_DISPLAYSETTINGS_GET_SUPPORTED_AUDIO_PORTS  "org.rdk.DisplaySettings.getSupportedAudioPorts"

/* Audio encoding string values (from DisplaySettings::encodingToString) */
#define ENCODING_PCM     "PCM"
#define ENCODING_DISPLAY "DISPLAY"
#define ENCODING_AC3     "AC3"

/* Status strings */
#define ENABLED_STRING   "Enabled"
#define DISABLED_STRING  "Disabled"

GHashTable * hostIf_STBServiceSPDIF::ifHash = NULL;   /* dev_id → hostIf_STBServiceSPDIF* */
GMutex hostIf_STBServiceSPDIF::m_mutex;

hostIf_STBServiceSPDIF* hostIf_STBServiceSPDIF::getInstance(int dev_id)
{
    if (!ifHash)
        buildPortNameHash();

    hostIf_STBServiceSPDIF* pRet =
        (hostIf_STBServiceSPDIF*)g_hash_table_lookup(ifHash, (gpointer)(intptr_t)dev_id);

    if (!pRet)
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                "[%s:%s:%d]: No instance for dev_id=%d\n", __FILE__, __FUNCTION__, __LINE__, dev_id);
    }
    return pRet;
}

GList* hostIf_STBServiceSPDIF::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_STBServiceSPDIF::closeInstance(hostIf_STBServiceSPDIF *pDev)
{
    if(pDev)
    {
        if (ifHash)
            g_hash_table_remove(ifHash, (gconstpointer)(intptr_t)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBServiceSPDIF::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values(ifHash);
        GList* current = tmp_list;
        while(current)
        {
            hostIf_STBServiceSPDIF* pDev = (hostIf_STBServiceSPDIF *)current->data;
            current = current->next;
            delete pDev;
        }
        g_list_free(tmp_list);
        g_hash_table_destroy(ifHash);
        ifHash = NULL;
    }
}

void hostIf_STBServiceSPDIF::getLock()
{
    g_mutex_init(&hostIf_STBServiceSPDIF::m_mutex);
    g_mutex_lock(&hostIf_STBServiceSPDIF::m_mutex);
}

void hostIf_STBServiceSPDIF::releaseLock()
{
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] Unlocking mutex...  \n", __FUNCTION__, __LINE__);
    g_mutex_unlock(&hostIf_STBServiceSPDIF::m_mutex);
}

hostIf_STBServiceSPDIF::hostIf_STBServiceSPDIF(int devid, const std::string& portName) : dev_id(devid), m_portName(portName)
{
    backupEnable = false;
    strncpy(backupStatus, " ", _BUF_LEN_16 - 1);
    backupStatus[_BUF_LEN_16 - 1] = '\0';
    backupForcePCM = false;
    backupPassthrough = false;
    backupAudioDelay = 0;

    bCalledEnable = false;
    bCalledStatus = false;
    bCalledAlias = false;
    bCalledName = false;
    bCalledForcePCM = false;
    bCalledPassthrough = false;
    bCalledAudioDelay = false;
}

void hostIf_STBServiceSPDIF::buildPortNameHash()
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
        if (!portName.empty() && strcasestr(portName.c_str(), "spdif"))
        {
            hostIf_STBServiceSPDIF* pInst = new hostIf_STBServiceSPDIF(devId, portName);
            g_hash_table_insert(ifHash, (gpointer)(intptr_t)devId, pInst);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
                    "[%s:%d] ifHash: dev_id=%d portName=%s\n",
                    __FUNCTION__, __LINE__, devId, portName.c_str());
            devId++;
        }
    }
}

int hostIf_STBServiceSPDIF::getNumberOfInstances(HOSTIF_MsgData_t *stMsgData)
{
    if (!ifHash)
        buildPortNameHash();
    put_int(stMsgData->paramValue, g_hash_table_size(ifHash));
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);
    return OK;
}

/************************************************************
 * Description  : Dispatch SET requests for SPDIF parameters.
 *
 *   Enable      → MIGRATED: org.rdk.DisplaySettings.setEnableAudioPort
 *   ForcePCM    → MIGRATED: org.rdk.DisplaySettings.setAudioEncoding
 *   AudioDelay  → MIGRATED: org.rdk.DisplaySettings.setAudioDelay
 *
 * @param[in] paramName  Parameter name string (e.g. "ForcePCM").
 * @param[in] stMsgData  HostIf Message Request containing the new value.
 *
 * @return OK on success, NOK on failure, NOT_HANDLED if unrecognised.
 * @ingroup TR69_HOSTIF_STBSERVICES_SPDIF_API
************************************************************/
int hostIf_STBServiceSPDIF::handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if (strcasecmp(paramName, ENABLE_STRING) == 0)
    {
        ret = setEnable(stMsgData);     /* MIGRATED: org.rdk.DisplaySettings.setEnableAudioPort */
    }
    else if (strcasecmp(paramName, FORCEPCM_STRING) == 0)
    {
        ret = setForcePCM(stMsgData);   /* MIGRATED: org.rdk.DisplaySettings.setAudioEncoding */
    }
    else if (strcasecmp(paramName, AUDIODELAY_STRING) == 0)
    {
        ret = setAudioDelay(stMsgData); /* MIGRATED: org.rdk.DisplaySettings.setAudioDelay */
    }
    return ret;
}

/************************************************************
 * Description  : Get enabled state of the SPDIF port via Thunder.
 * Thunder API  : org.rdk.DisplaySettings.getEnableAudioPort
 * Request      : { "audioPort": "<portName>" }
 * Response     : { "enable": <true|false>, "success": true }
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → true if port is enabled, false otherwise.
 * @ingroup TR69_HOSTIF_STBSERVICES_SPDIF_API
************************************************************/
int hostIf_STBServiceSPDIF::getEnable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool isEnabled = false;
    const std::string portName = m_portName;
    const std::string paramsJson = "{\"audioPort\":\"" + portName + "\"}";

    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DISPLAYSETTINGS_GET_ENABLE_AUDIO_PORT,
            paramsJson, "enable", isEnabled))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder call failed for port %s\n",
                __FUNCTION__, portName.c_str());
        return NOK;
    }

    put_boolean(stMsgData->paramValue, isEnabled);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    if (bCalledEnable && pChanged && (backupEnable != isEnabled))
    {
        *pChanged = true;
    }
    bCalledEnable = true;
    backupEnable = isEnabled;

    return OK;
}

/************************************************************
 * Description  : Enable or disable the SPDIF port via Thunder.
 * Thunder API  : org.rdk.DisplaySettings.setEnableAudioPort
 * Request      : { "audioPort": "<portName>", "enable": <true|false> }
 * Response     : { "success": true }
 *
 * @return OK on success, NOK on failure.
 * @ingroup TR69_HOSTIF_STBSERVICES_SPDIF_API
************************************************************/
int hostIf_STBServiceSPDIF::setEnable(HOSTIF_MsgData_t *stMsgData)
{
    bool enable = get_boolean(stMsgData->paramValue);
    const std::string portName = m_portName;
    const std::string paramsJson = "{\"audioPort\":\"" + portName + "\",\"enable\":"
                                   + (enable ? "true" : "false") + "}";
    std::string response;

    if (!invokeThunderPluginMethod(
            THUNDER_DISPLAYSETTINGS_SET_ENABLE_AUDIO_PORT, paramsJson, response))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder call failed for port %s\n",
                __FUNCTION__, portName.c_str());
        return NOK;
    }
    return OK;
}

/************************************************************
 * Description  : Get status of the SPDIF port via Thunder.
 * Thunder API  : org.rdk.DisplaySettings.getEnableAudioPort
 * Request      : { "audioPort": "<portName>" }
 * Response     : { "enable": <true|false>, "success": true }
 *
 * Maps "enable" bool → "Enabled" / "Disabled" string.
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → "Enabled" or "Disabled"
 * @ingroup TR69_HOSTIF_STBSERVICES_SPDIF_API
************************************************************/
int hostIf_STBServiceSPDIF::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool isEnabled = false;
    const std::string portName = m_portName;
    const std::string paramsJson = "{\"audioPort\":\"" + portName + "\"}";

    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DISPLAYSETTINGS_GET_ENABLE_AUDIO_PORT,
            paramsJson, "enable", isEnabled))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder call failed for port %s\n",
                __FUNCTION__, portName.c_str());
        return NOK;
    }

    const char *statusStr = isEnabled ? ENABLED_STRING : DISABLED_STRING;
    strncpy(stMsgData->paramValue, statusStr, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(statusStr);

    if (bCalledStatus && pChanged && strcmp(backupStatus, stMsgData->paramValue))
    {
        *pChanged = true;
    }
    bCalledStatus = true;
    strncpy(backupStatus, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupStatus[_BUF_LEN_16 - 1] = '\0';

    return OK;
}

/************************************************************
 * Description  : Get human readable name of the SPDIF port.
 *                Uses in-memory aPort.getName() — no HAL/IPC call.
 *                Returns port name e.g. "SPDIF0", "SPDIF1".
 *
 * @return OK on success.
 * stMsgData->paramValue → port name string
 * @ingroup TR69_HOSTIF_STBSERVICES_SPDIF_API
************************************************************/
int hostIf_STBServiceSPDIF::getName(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    snprintf(stMsgData->paramValue, PARAM_LEN, "%s", m_portName.c_str());
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    return OK;
}

/************************************************************
 * Description  : Force SPDIF audio encoding to PCM via Thunder.
 * Thunder API  : org.rdk.DisplaySettings.setAudioEncoding
 * Request      : { "audioPort": "<portName>", "encoding": "PCM" | "DISPLAY" }
 * Response     : { "success": true }
 *
 * ForcePCM = true  → encoding "PCM"
 * ForcePCM = false → encoding "DISPLAY" (platform-selected passthrough)
 *
 * @return OK on success, NOK on failure.
 * @ingroup TR69_HOSTIF_STBSERVICES_SPDIF_API
************************************************************/
int hostIf_STBServiceSPDIF::setForcePCM(HOSTIF_MsgData_t *stMsgData)
{
    bool forcePCM = get_boolean(stMsgData->paramValue);
    const std::string portName = m_portName;
    const std::string encoding = forcePCM ? ENCODING_PCM : ENCODING_DISPLAY;
    const std::string paramsJson = "{\"audioPort\":\"" + portName
                                   + "\",\"encoding\":\"" + encoding + "\"}";
    std::string response;

    if (!invokeThunderPluginMethod(
            THUNDER_DISPLAYSETTINGS_SET_AUDIO_ENCODING, paramsJson, response))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder call failed for port %s encoding %s\n",
                __FUNCTION__, portName.c_str(), encoding.c_str());
        return NOK;
    }
    return OK;
}

/************************************************************
 * Description  : Get ForcePCM state of the SPDIF port via Thunder.
 * Thunder API  : org.rdk.DisplaySettings.getAudioEncoding
 * Request      : { "audioPort": "<portName>" }
 * Response     : { "encoding": "PCM" | "DISPLAY" | "AC3" | "NONE", "success": true }
 *
 * Returns true if current encoding is "PCM", false otherwise.
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → true if ForcePCM is active, false otherwise.
 * @ingroup TR69_HOSTIF_STBSERVICES_SPDIF_API
************************************************************/
int hostIf_STBServiceSPDIF::getForcePCM(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string encoding;
    const std::string portName = m_portName;
    const std::string paramsJson = "{\"audioPort\":\"" + portName + "\"}";

    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DISPLAYSETTINGS_GET_AUDIO_ENCODING,
            paramsJson, "encoding", encoding))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder call failed for port %s\n",
                __FUNCTION__, portName.c_str());
        return NOK;
    }

    bool isForcePCM = (encoding == ENCODING_PCM);
    put_boolean(stMsgData->paramValue, isForcePCM);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    if (bCalledForcePCM && pChanged && (backupForcePCM != isForcePCM))
    {
        *pChanged = true;
    }
    bCalledForcePCM = true;
    backupForcePCM = isForcePCM;

    return OK;
}

/************************************************************
 * Description  : Get Passthrough state of the SPDIF port via Thunder.
 * Thunder API  : org.rdk.DisplaySettings.getAudioEncoding
 * Request      : { "audioPort": "<portName>" }
 * Response     : { "encoding": "PCM" | "DISPLAY" | "AC3" | "NONE", "success": true }
 *
 * Returns true if encoding is "DISPLAY" or "AC3" (passthrough modes).
 * Returns false if encoding is "PCM" (decoded mode).
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → true if passthrough is active, false otherwise.
 * @ingroup TR69_HOSTIF_STBSERVICES_SPDIF_API
************************************************************/
int hostIf_STBServiceSPDIF::getPassthrough(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string encoding;
    const std::string portName = m_portName;
    const std::string paramsJson = "{\"audioPort\":\"" + portName + "\"}";

    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DISPLAYSETTINGS_GET_AUDIO_ENCODING,
            paramsJson, "encoding", encoding))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder call failed for port %s\n",
                __FUNCTION__, portName.c_str());
        return NOK;
    }

    /* Passthrough: audio stream sent without decoding (DISPLAY = platform-selected, AC3 = compressed) */
    bool isPassthrough = (encoding == ENCODING_DISPLAY || encoding == ENCODING_AC3);
    put_boolean(stMsgData->paramValue, isPassthrough);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    if (bCalledPassthrough && pChanged && (backupPassthrough != isPassthrough))
    {
        *pChanged = true;
    }
    bCalledPassthrough = true;
    backupPassthrough = isPassthrough;

    return OK;
}

/************************************************************
 * Description  : Get audio delay of the SPDIF port.
 *                NOT supported for SPDIF — no DisplaySettings equivalent.
 *                Returns NOT_HANDLED.
 *
 * @return NOT_HANDLED always.
 * @ingroup TR69_HOSTIF_STBSERVICES_SPDIF_API
************************************************************/
int hostIf_STBServiceSPDIF::getAudioDelay(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int ret = NOT_HANDLED;
    return ret;
}

/************************************************************
 * Description  : Set audio delay on the SPDIF port via Thunder.
 * Thunder API  : org.rdk.DisplaySettings.setAudioDelay
 * Request      : { "audioPort": "<portName>", "audioDelay": "<ms>" }
 * Response     : { "success": true }
 *
 * @return OK on success, NOK on failure.
 * @ingroup TR69_HOSTIF_STBSERVICES_SPDIF_API
************************************************************/
int hostIf_STBServiceSPDIF::setAudioDelay(HOSTIF_MsgData_t *stMsgData)
{
    int audioDelayMs = get_int(stMsgData->paramValue);
    const std::string portName = m_portName;
    const std::string paramsJson = "{\"audioPort\":\"" + portName
                                   + "\",\"audioDelay\":\""
                                   + std::to_string(audioDelayMs) + "\"}";
    std::string response;

    if (!invokeThunderPluginMethod(
            THUNDER_DISPLAYSETTINGS_SET_AUDIO_DELAY, paramsJson, response))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder call failed for port %s delay %d ms\n",
                __FUNCTION__, portName.c_str(), audioDelayMs);
        return NOK;
    }
    return OK;
}

int hostIf_STBServiceSPDIF::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if (strcasecmp(paramName, ENABLE_STRING) == 0)
    {
        ret = getEnable(stMsgData);
    }
    else if (strcasecmp(paramName, STATUS_STRING) == 0)
    {
        ret = getStatus(stMsgData);
    }
    else if (strcasecmp(paramName, ALIAS_STRING) == 0)
    {
        ret = getAlias(stMsgData);
    }
    else if (strcasecmp(paramName, NAME_STRING) == 0)
    {
        ret = getName(stMsgData);
    }
    else if (strcasecmp(paramName, FORCEPCM_STRING) == 0)
    {
        ret = getForcePCM(stMsgData);
    }
    else if (strcasecmp(paramName, PASSTHROUGH_STRING) == 0)
    {
        ret = getPassthrough(stMsgData);
    }
    else if (strcasecmp(paramName, AUDIODELAY_STRING) == 0)
    {
        ret = getAudioDelay(stMsgData);
    }
    return ret;
}

void hostIf_STBServiceSPDIF::doUpdates(updateCallback mUpdateCallback)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    char tmp_buff[PARAM_LEN];

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged = false;
    msgData.instanceNum=dev_id;
    getEnable(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, ENABLE_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged = false;
    msgData.instanceNum=dev_id;
    getStatus(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, STATUS_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged = false;
    msgData.instanceNum=dev_id;
    getForcePCM(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, FORCEPCM_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged = false;
    msgData.instanceNum=dev_id;
    getPassthrough(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, PASSTHROUGH_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged = false;
    msgData.instanceNum=dev_id;
    getAudioDelay(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, AUDIODELAY_STRING);
        if(mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
}

/* getAlias/setAlias: no DisplaySettings equivalent — always NOT_HANDLED */
int hostIf_STBServiceSPDIF::getAlias(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    return NOT_HANDLED;
}

int hostIf_STBServiceSPDIF::setAlias(HOSTIF_MsgData_t *stMsgData)
{
    return NOT_HANDLED;
}

/** @} */
/** @} */
