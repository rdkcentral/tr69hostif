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
 * @brief Thunder-backed APIs of TR069 Components SPDIF.
 */

#include <sstream>
#include "Components_SPDIF.h"
#include "safec_lib.h"

#define DEV_NAME "SPDIF"
#define BASE_NAME "Device.Services.STBService.1.Components.SPDIF"
#define UPDATE_FORMAT_STRING "%s.%d.%s"

#define ENABLE_STRING "Enable"
#define STATUS_STRING "Status"
#define ALIAS_STRING "Alias"
#define NAME_STRING "Name"
#define FORCEPCM_STRING "ForcePCM"
#define PASSTHROUGH_STRING "Passthrough"
#define AUDIODELAY_STRING "AudioDelay"

#define ENABLED_STRING "Enabled"
#define DISABLED_STRING "Disabled"

#define THUNDER_DS_GET_SUPPORTED_AUDIO_PORTS  "org.rdk.DisplaySettings.getSupportedAudioPorts"
#define THUNDER_DS_GET_ENABLE_AUDIO_PORT      "org.rdk.DisplaySettings.getEnableAudioPort"
#define THUNDER_DS_GET_AUDIO_ENCODING         "org.rdk.DisplaySettings.getAudioEncoding"
#define THUNDER_DS_SET_AUDIO_ENCODING         "org.rdk.DisplaySettings.setAudioEncoding"
#define THUNDER_DS_GET_AUDIO_DELAY            "org.rdk.DisplaySettings.getAudioDelay"

GHashTable * hostIf_STBServiceSPDIF::ifHash = NULL;
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

void hostIf_STBServiceSPDIF::buildPortNameHash()
{
    if (ifHash)
        g_hash_table_destroy(ifHash);

    ifHash = g_hash_table_new(NULL, NULL);

    std::string delimitedPorts;
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
        if (!portName.empty() && strcasestr(portName.c_str(), "spdif"))
        {
            hostIf_STBServiceSPDIF *pInst = new hostIf_STBServiceSPDIF(devId, portName);
            g_hash_table_insert(ifHash, (gpointer)(intptr_t)devId, pInst);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
                    "[%s:%d] dev_id=%d portName=%s\n", __FUNCTION__, __LINE__, devId, portName.c_str());
            devId++;
        }
    }
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

hostIf_STBServiceSPDIF::hostIf_STBServiceSPDIF(int devid, const std::string& portName)
    : dev_id(devid), m_portName(portName)
{
    backupEnable = false;
    errno_t rc = -1;
    rc = strcpy_s(backupStatus, sizeof(backupStatus), " ");
    ERR_CHK(rc);
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

int hostIf_STBServiceSPDIF::handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if (strcasecmp(paramName, FORCEPCM_STRING) == 0)
    {
        ret = setForcePCM(stMsgData);
    }
    return ret;
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
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getEnable(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, ENABLE_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getStatus(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, STATUS_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getAlias(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, ALIAS_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getName(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, NAME_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getForcePCM(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, FORCEPCM_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getPassthrough(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, PASSTHROUGH_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getAudioDelay(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, AUDIODELAY_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
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

int hostIf_STBServiceSPDIF::setEnable(HOSTIF_MsgData_t *stMsgData)
{
    (void)stMsgData;
    return NOT_HANDLED;
}

int hostIf_STBServiceSPDIF::setAlias(HOSTIF_MsgData_t *stMsgData)
{
    (void)stMsgData;
    return NOT_HANDLED;
}

int hostIf_STBServiceSPDIF::setForcePCM(HOSTIF_MsgData_t *stMsgData)
{
    const bool forcePcm = get_boolean(stMsgData->paramValue);
    const std::string encValue = forcePcm ? "PCM" : "DISPLAY";
    const std::string paramsJson = std::string("{\"audioPort\":\"") + m_portName
        + "\",\"audioEncoding\":\"" + encValue + "\"}";
    std::string response;
    if (!invokeThunderPluginMethod(THUNDER_DS_SET_AUDIO_ENCODING, paramsJson, response))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder setAudioEncoding failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }
    return OK;
}

int hostIf_STBServiceSPDIF::getEnable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool enabled = false;
    const std::string paramsJson = std::string("{\"audioPort\":\"") + m_portName + "\"}";
    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DS_GET_ENABLE_AUDIO_PORT, paramsJson, "enable", enabled))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder getEnableAudioPort failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }

    put_boolean(stMsgData->paramValue, enabled);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    if(bCalledEnable && pChanged && (backupEnable != enabled))
        *pChanged = true;

    bCalledEnable = true;
    backupEnable = enabled;
    return OK;
}

int hostIf_STBServiceSPDIF::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool enabled = false;
    const std::string paramsJson = std::string("{\"audioPort\":\"") + m_portName + "\"}";
    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DS_GET_ENABLE_AUDIO_PORT, paramsJson, "enable", enabled))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder getEnableAudioPort failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }

    const char *status = enabled ? ENABLED_STRING : DISABLED_STRING;
    strncpy(stMsgData->paramValue, status, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(status);

    if(bCalledStatus && pChanged && strcmp(backupStatus, stMsgData->paramValue))
        *pChanged = true;

    bCalledStatus = true;
    strncpy(backupStatus, stMsgData->paramValue, _BUF_LEN_16-1);
    backupStatus[_BUF_LEN_16-1] = '\0';
    return OK;
}

int hostIf_STBServiceSPDIF::getAlias(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    (void)pChanged;
    errno_t rc = strcpy_s(stMsgData->paramValue, sizeof(stMsgData->paramValue), m_portName.c_str());
    ERR_CHK(rc);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    bCalledAlias = true;
    return OK;
}

int hostIf_STBServiceSPDIF::getName(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bCalledName = true;
    return getAlias(stMsgData, pChanged);
}

int hostIf_STBServiceSPDIF::getForcePCM(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string encoding;
    const std::string paramsJson = std::string("{\"audioPort\":\"") + m_portName + "\"}";
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_GET_AUDIO_ENCODING, paramsJson, "audioEncoding", encoding))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder getAudioEncoding failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }

    bool forcePcm = (0 == strcasecmp(encoding.c_str(), "PCM"));
    put_boolean(stMsgData->paramValue, forcePcm);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    if(bCalledForcePCM && pChanged && (backupForcePCM != forcePcm))
        *pChanged = true;

    bCalledForcePCM = true;
    backupForcePCM = forcePcm;
    return OK;
}

int hostIf_STBServiceSPDIF::getPassthrough(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string encoding;
    const std::string paramsJson = std::string("{\"audioPort\":\"") + m_portName + "\"}";
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_GET_AUDIO_ENCODING, paramsJson, "audioEncoding", encoding))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder getAudioEncoding failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }

    bool passthrough = (0 != strcasecmp(encoding.c_str(), "PCM"));
    put_boolean(stMsgData->paramValue, passthrough);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    if(bCalledPassthrough && pChanged && (backupPassthrough != passthrough))
        *pChanged = true;

    bCalledPassthrough = true;
    backupPassthrough = passthrough;
    return OK;
}

int hostIf_STBServiceSPDIF::getAudioDelay(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int delay = 0;
    const std::string paramsJson = std::string("{\"audioPort\":\"") + m_portName + "\"}";
    if (!invokeThunderPluginMethodAndExtractNumberField(
            THUNDER_DS_GET_AUDIO_DELAY, paramsJson, "audioDelay", delay))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder getAudioDelay failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }

    if (delay < 0)
        delay = 0;

    put_uint(stMsgData->paramValue, (uint)delay);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    if (bCalledAudioDelay && pChanged && (backupAudioDelay != (unsigned)delay))
        *pChanged = true;

    bCalledAudioDelay = true;
    backupAudioDelay = (unsigned)delay;
    return OK;
}

/** @} */
/** @} */
