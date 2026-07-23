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
#include <sstream>
#include "Components_VideoDecoder.h"

#define DEV_NAME "VideoDecoder"
#define BASE_NAME "Device.Services.STBService.1.Components.VideoDecoder"
#define UPDATE_FORMAT_STRING "%s.%d.%s"

#define STATUS_STRING         "Status"
#define CONTENT_AR_STRING     "ContentAspectRatio"
#define COMCAST_STANDBY_STRING "X_COMCAST-COM_Standby"
#define HEVC_STRING           "X_RDKCENTRAL-COM_MPEGHPart2"
#define HEVC_PROFILE_PATH     ".Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1"
#define NAME_STRING           "Name"
#define ENABLE_STRING         "Enable"
#define ENABLED_STRING        "Enabled"
#define DISABLED_STRING       "Disabled"

#define THUNDER_PM_GET_POWER_STATE "org.rdk.PowerManager.GetPowerState"

#define THUNDER_DS_GET_DISPLAY_ASPECT_RATIO "org.rdk.DisplaySettings.getDisplayAspectRatio"

#define THUNDER_DS_GET_SUPPORTED_VIDEO_DISPLAYS "org.rdk.DisplaySettings.getSupportedVideoDisplays"

GHashTable * hostIf_STBServiceVideoDecoder::ifHash = NULL;
GMutex hostIf_STBServiceVideoDecoder::m_mutex;

void hostIf_STBServiceVideoDecoder::buildPortNameHash()
{
    if (ifHash) g_hash_table_destroy(ifHash);
    ifHash = g_hash_table_new(NULL, NULL);

    std::string delimitedPorts;
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Calling Thunder API: %s\n", __FUNCTION__, THUNDER_DS_GET_SUPPORTED_VIDEO_DISPLAYS);
    if (!invokeThunderPluginMethodAndExtractDelimitedStringArrayField(
            THUNDER_DS_GET_SUPPORTED_VIDEO_DISPLAYS, "{}", "supportedVideoDisplays", ",", delimitedPorts))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s:%d] Failed to get video displays, using default HDMI0\n",
                __FUNCTION__, __LINE__);
        hostIf_STBServiceVideoDecoder *pInst = new hostIf_STBServiceVideoDecoder(1, "HDMI0");
        g_hash_table_insert(ifHash, (gpointer)(intptr_t)1, pInst);
        return;
    }

    int devId = 1;
    std::istringstream ss(delimitedPorts);
    std::string portName;
    while (std::getline(ss, portName, ','))
    {
        if (!portName.empty())
        {
            hostIf_STBServiceVideoDecoder *pInst = new hostIf_STBServiceVideoDecoder(devId, portName);
            g_hash_table_insert(ifHash, (gpointer)(intptr_t)devId, pInst);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s:%d] dev_id=%d portName=%s\n",
                    __FUNCTION__, __LINE__, devId, portName.c_str());
            devId++;
        }
    }
}

hostIf_STBServiceVideoDecoder* hostIf_STBServiceVideoDecoder::getInstance(int dev_id)
{
    if (!ifHash) buildPortNameHash();
    hostIf_STBServiceVideoDecoder *pRet =
        (hostIf_STBServiceVideoDecoder*)g_hash_table_lookup(ifHash, (gpointer)(intptr_t)dev_id);
    if (!pRet)
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s:%s:%d]: No instance for dev_id=%d\n",
                __FILE__, __FUNCTION__, __LINE__, dev_id);
    return pRet;
}

GList* hostIf_STBServiceVideoDecoder::getAllInstances() { if(ifHash) return g_hash_table_get_keys(ifHash); return NULL; }

void hostIf_STBServiceVideoDecoder::closeInstance(hostIf_STBServiceVideoDecoder *pDev)
{
    if (pDev) {
        if (ifHash) g_hash_table_remove(ifHash, (gconstpointer)(intptr_t)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBServiceVideoDecoder::closeAllInstances()
{
    if (ifHash) {
        GList *tmp = g_hash_table_get_values(ifHash), *cur = tmp;
        while (cur) { delete (hostIf_STBServiceVideoDecoder*)cur->data; cur = cur->next; }
        g_list_free(tmp);
        g_hash_table_destroy(ifHash);
        ifHash = NULL;
    }
}

void hostIf_STBServiceVideoDecoder::getLock() { g_mutex_init(&m_mutex); g_mutex_lock(&m_mutex); }
void hostIf_STBServiceVideoDecoder::releaseLock() { g_mutex_unlock(&m_mutex); }
void hostIf_STBServiceVideoDecoder::checkForUpdates(updateCallback) {}

hostIf_STBServiceVideoDecoder::hostIf_STBServiceVideoDecoder(int devid, const std::string& portName)
    : dev_id(devid), m_portName(portName)
{
    strncpy(backupContentAspectRatio, " ", sizeof(backupContentAspectRatio));
    strncpy(backupVideoDecoderStatus, " ", sizeof(backupVideoDecoderStatus));
    backupStandby = false;
    bCalledContentAspectRatio = bCalledStandby = bCalledVideoDecoderStatus = false;
}

int hostIf_STBServiceVideoDecoder::handleSetMsg(const char *pSetting, HOSTIF_MsgData_t *stMsgData)
{
    (void)pSetting; (void)stMsgData;
    return NOT_HANDLED;
}

int hostIf_STBServiceVideoDecoder::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Getting VideoDecoder param: %s\n", __FUNCTION__, paramName);
    if (strcasecmp(paramName, ENABLE_STRING) == 0) {
        put_boolean(stMsgData->paramValue, true);
        stMsgData->paramtype = hostIf_BooleanType;
        stMsgData->paramLen = sizeof(bool);
        return OK;
    } else if (strcasecmp(paramName, STATUS_STRING) == 0) {
        return getStatus(stMsgData);
    } else if (strcasecmp(paramName, NAME_STRING) == 0) {
        snprintf(stMsgData->paramValue, PARAM_LEN, "VideoDecoder%s", m_portName.c_str());
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        return OK;
    } else if (strcasecmp(paramName, CONTENT_AR_STRING) == 0) {
        /* ContentAspectRatio: no Thunder equivalent, use DisplaySettings aspect ratio */
        return getContentAspectRatio(stMsgData);
    } else if (strcasecmp(paramName, HEVC_STRING) == 0) {
        strncpy(stMsgData->paramValue, HEVC_PROFILE_PATH, strlen(HEVC_PROFILE_PATH)+1);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        return OK;
    }
    return NOT_HANDLED;
}

void hostIf_STBServiceVideoDecoder::doUpdates(updateCallback mUpdateCallback)
{
    HOSTIF_MsgData_t msgData; bool bChanged; char tmp_buff[PARAM_LEN];
#define DO_UPD(fn, param) \
    memset(&msgData,0,sizeof(msgData)); bChanged=false; msgData.instanceNum=dev_id; \
    fn(&msgData,&bChanged); \
    if(bChanged){ snprintf(tmp_buff,PARAM_LEN,UPDATE_FORMAT_STRING,BASE_NAME,dev_id,param); \
    if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff,msgData.paramValue,msgData.paramtype); }

    DO_UPD(getStatus, STATUS_STRING)
#undef DO_UPD
}

/* ---- private ---- */

/************************************************************
 * Description  : Get if Video decoder is in Standby or not.
 * Precondition : None
 * Input        : stMsgData for result return.
 *                pChanged
 *
 * Return       : OK -> Success
 *                NOK -> Failure
 *              stMsgData->paramValue -> "Enabled", "Disabled", "Error", "X_COMCAST-COM_Standby"
************************************************************/
int hostIf_STBServiceVideoDecoder::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string currentState;
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_PM_GET_POWER_STATE, "{}", "currentState", currentState))
    {
        strncpy(stMsgData->paramValue, "Error", PARAM_LEN);
        stMsgData->paramValue[PARAM_LEN - 1] = '\0';
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        return NOK;
    }

    const char *status;
    if (currentState == "ON")
        status = ENABLED_STRING;
    else if (currentState == "STANDBY" || currentState == "LIGHT_SLEEP" || currentState == "DEEP_SLEEP")
        status = COMCAST_STANDBY_STRING;
    else /* UNKNOWN, OFF */
        status = DISABLED_STRING;

    strncpy(stMsgData->paramValue, status, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(status);
    if (bCalledVideoDecoderStatus && pChanged && strcmp(backupVideoDecoderStatus, stMsgData->paramValue))
        *pChanged = true;
    bCalledVideoDecoderStatus = true;
    strncpy(backupVideoDecoderStatus, stMsgData->paramValue, sizeof(backupVideoDecoderStatus) - 1);
    backupVideoDecoderStatus[sizeof(backupVideoDecoderStatus) - 1] = '\0';
    return OK;
}

int hostIf_STBServiceVideoDecoder::getContentAspectRatio(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string aspectRatio;
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_GET_DISPLAY_ASPECT_RATIO,
            std::string("{\"videoDisplay\":\"") + m_portName + "\"}", "aspectRatio", aspectRatio))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder %s failed\n",
                __FUNCTION__, THUNDER_DS_GET_DISPLAY_ASPECT_RATIO);
        aspectRatio = "16:9";
    }
    strncpy(stMsgData->paramValue, aspectRatio.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (bCalledContentAspectRatio && pChanged && strcmp(backupContentAspectRatio, stMsgData->paramValue))
        *pChanged = true;
    bCalledContentAspectRatio = true;
    strncpy(backupContentAspectRatio, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupContentAspectRatio[_BUF_LEN_16 - 1] = '\0';
    return OK;
}
