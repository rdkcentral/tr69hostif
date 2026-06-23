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
 * @file Components_VideoOutput_Thunder.cpp
 * @brief Components_VideoOutput APIs migrated from libds to Thunder plugins.
 *
 * Plugin mapping:
 *   org.rdk.DisplaySettings  — port enable/status, resolution, zoom setting
 *   org.rdk.HdcpProfile      — HDCP authentication status
 *
 * Port: HDMI0, HDMI1, ... (derived from getSupportedVideoDisplays, all ports)
 *
 * Migrated APIs:
 *   getStatus()              → org.rdk.DisplaySettings.getEnableVideoPort  ("enable" bool)
 *   getDisplayFormat()       → org.rdk.DisplaySettings.getCurrentResolution ("resolution" string)
 *   getAspectRatioBehaviour()→ org.rdk.DisplaySettings.getZoomSetting      ("zoomSetting" string)
 *   getHDCP()                → org.rdk.HdcpProfile.getHDCPStatus nested "HDCPStatus.isHDCPEnabled"
 *   getName()                — in-memory: returns m_portName (e.g. "HDMI0")
 *
 * Not migrated (hardcoded / no HAL call in original):
 *   Enable                   — hardcoded true (no HAL call in original either)
 *   VideoFormat              — hardcoded "HDMI" (no HAL call in original either)
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "Components_VideoOutput.h"
#include "safec_lib.h"
#include <sstream>

#define DEV_NAME             "VideoOutput"
#define BASE_NAME            "Device.Services.STBService.1.Components.VideoOutput"
#define UPDATE_FORMAT_STRING "%s.%d.%s"

/* Parameter name string constants */
#define STATUS_STRING         "Status"
#define ENABLE_STRING         "Enable"
#define DISPLAY_FORMAT_STRING "DisplayFormat"
#define VIDEO_FORMAT_STRING   "VideoFormat"
#define AR_BEHAVIOR_STRING    "AspectRatioBehaviour"
#define HDCP_STRING           "HDCP"
#define DISPLAY_NAME_STRING   "Name"
#define ENABLED_STRING        "Enabled"
#define DISABLED_STRING       "Disabled"

/* Thunder plugin method identifiers */
#define THUNDER_DS_GET_SUPPORTED_VIDEO_DISPLAYS "org.rdk.DisplaySettings.getSupportedVideoDisplays"
#define THUNDER_DS_GET_ENABLE_VIDEO_PORT        "org.rdk.DisplaySettings.getEnableVideoPort"
#define THUNDER_DS_GET_CURRENT_RESOLUTION       "org.rdk.DisplaySettings.getCurrentResolution"
#define THUNDER_DS_GET_ZOOM_SETTING             "org.rdk.DisplaySettings.getZoomSetting"
#define THUNDER_HDCP_GET_STATUS                 "org.rdk.HdcpProfile.getHDCPStatus"

GHashTable * hostIf_STBServiceVideoOutput::ifHash = NULL;
GMutex       hostIf_STBServiceVideoOutput::m_mutex;

/* ─────────────────────────────────────────────────────────────────────────────
 * Instance lifecycle
 * ───────────────────────────────────────────────────────────────────────────*/

hostIf_STBServiceVideoOutput* hostIf_STBServiceVideoOutput::getInstance(int dev_id)
{
    if (!ifHash)
        buildPortNameHash();

    hostIf_STBServiceVideoOutput* pRet =
        (hostIf_STBServiceVideoOutput*)g_hash_table_lookup(ifHash, (gpointer)(intptr_t)dev_id);

    if (!pRet)
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                "[%s:%s:%d]: No instance for dev_id=%d\n",
                __FILE__, __FUNCTION__, __LINE__, dev_id);
    }
    return pRet;
}

GList* hostIf_STBServiceVideoOutput::getAllInstances()
{
    if (ifHash) return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_STBServiceVideoOutput::closeInstance(hostIf_STBServiceVideoOutput *pDev)
{
    if (pDev)
    {
        if (ifHash)
            g_hash_table_remove(ifHash, (gconstpointer)(intptr_t)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBServiceVideoOutput::closeAllInstances()
{
    if (ifHash)
    {
        GList* tmp_list = g_hash_table_get_values(ifHash);
        GList* current  = tmp_list;
        while (current)
        {
            hostIf_STBServiceVideoOutput* pDev = (hostIf_STBServiceVideoOutput*)current->data;
            current = current->next;
            delete pDev;
        }
        g_list_free(tmp_list);
        g_hash_table_destroy(ifHash);
        ifHash = NULL;
    }
}

void hostIf_STBServiceVideoOutput::getLock()
{
    g_mutex_init(&hostIf_STBServiceVideoOutput::m_mutex);
    g_mutex_lock(&hostIf_STBServiceVideoOutput::m_mutex);
}

void hostIf_STBServiceVideoOutput::releaseLock()
{
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s:%d] Unlocking mutex...\n",
            __FUNCTION__, __LINE__);
    g_mutex_unlock(&hostIf_STBServiceVideoOutput::m_mutex);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * buildPortNameHash — all video ports (not just HDMI-filtered)
 * ───────────────────────────────────────────────────────────────────────────*/

void hostIf_STBServiceVideoOutput::buildPortNameHash()
{
    if (ifHash) g_hash_table_destroy(ifHash);
    ifHash = g_hash_table_new(NULL, NULL);

    std::string delimitedPorts;
    if (!invokeThunderPluginMethodAndExtractDelimitedStringArrayField(
            THUNDER_DS_GET_SUPPORTED_VIDEO_DISPLAYS,
            "{}", "supportedVideoDisplays", ",", delimitedPorts))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                "[%s:%d] Failed to get supportedVideoDisplays from Thunder\n",
                __FUNCTION__, __LINE__);
        return;
    }

    int devId = 1;
    std::istringstream ss(delimitedPorts);
    std::string portName;
    while (std::getline(ss, portName, ','))
    {
        if (!portName.empty())
        {
            hostIf_STBServiceVideoOutput* pInst = new hostIf_STBServiceVideoOutput(devId, portName);
            g_hash_table_insert(ifHash, (gpointer)(intptr_t)devId, pInst);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
                    "[%s:%d] ifHash: dev_id=%d portName=%s\n",
                    __FUNCTION__, __LINE__, devId, portName.c_str());
            devId++;
        }
    }
}

int hostIf_STBServiceVideoOutput::getNumberOfInstances(HOSTIF_MsgData_t *stMsgData)
{
    if (!ifHash) buildPortNameHash();
    put_int(stMsgData->paramValue, (int)g_hash_table_size(ifHash));
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen  = sizeof(unsigned int);
    return OK;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Constructor
 * ───────────────────────────────────────────────────────────────────────────*/

hostIf_STBServiceVideoOutput::hostIf_STBServiceVideoOutput(int devid, const std::string& portName)
    : dev_id(devid), m_portName(portName)
{
    errno_t rc = -1;
    rc = strcpy_s(backupAspectRatioBehaviour, sizeof(backupAspectRatioBehaviour), " "); ERR_CHK(rc);
    rc = strcpy_s(backupDisplayFormat,        sizeof(backupDisplayFormat),        " "); ERR_CHK(rc);
    rc = strcpy_s(backupDisplayName,          sizeof(backupDisplayName),          " "); ERR_CHK(rc);
    rc = strcpy_s(backupVideoFormat,          sizeof(backupVideoFormat),          " "); ERR_CHK(rc);
    backupHDCP = false;
    rc = strcpy_s(backupVideoOutputStatus,    sizeof(backupVideoOutputStatus),    " "); ERR_CHK(rc);

    bCalledAspectRatioBehaviour = false;
    bCalledDisplayFormat        = false;
    bCalledDisplayName          = false;
    bCalledVideoFormat          = false;
    bCalledHDCP                 = false;
    bCalledVideoOutputStatus    = false;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Dispatch — SET (no writable parameters for VideoOutput)
 * ───────────────────────────────────────────────────────────────────────────*/

int hostIf_STBServiceVideoOutput::handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    (void)paramName; (void)stMsgData;
    return NOT_HANDLED;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Dispatch — GET
 * ───────────────────────────────────────────────────────────────────────────*/

int hostIf_STBServiceVideoOutput::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if (strcasecmp(paramName, STATUS_STRING) == 0)
    {
        ret = getStatus(stMsgData);
    }
    else if (strcasecmp(paramName, ENABLE_STRING) == 0)
    {
        /* Always enabled — no HAL call in original either */
        put_boolean(stMsgData->paramValue, true);
        stMsgData->paramtype = hostIf_BooleanType;
        stMsgData->paramLen  = sizeof(bool);
        ret = OK;
    }
    else if (strcasecmp(paramName, DISPLAY_FORMAT_STRING) == 0)
    {
        ret = getDisplayFormat(stMsgData);
    }
    else if (strcasecmp(paramName, VIDEO_FORMAT_STRING) == 0)
    {
        ret = getVideoFormat(stMsgData);
    }
    else if (strcasecmp(paramName, AR_BEHAVIOR_STRING) == 0)
    {
        ret = getAspectRatioBehaviour(stMsgData);
    }
    else if (strcasecmp(paramName, HDCP_STRING) == 0)
    {
        ret = getHDCP(stMsgData);
    }
    else if (strcasecmp(paramName, DISPLAY_NAME_STRING) == 0)
    {
        ret = getName(stMsgData);
    }
    return ret;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * doUpdates
 * ───────────────────────────────────────────────────────────────────────────*/

void hostIf_STBServiceVideoOutput::checkForUpdates(updateCallback mUpdateCallback)
{
    (void)mUpdateCallback; /* Currently not implemented */
}

void hostIf_STBServiceVideoOutput::doUpdates(updateCallback mUpdateCallback)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    char tmp_buff[PARAM_LEN];

#define VO_DO_UPDATE(getter, paramStr) \
    do { \
        memset(&msgData, 0, sizeof(msgData)); \
        memset(tmp_buff, 0, PARAM_LEN); \
        bChanged = false; \
        msgData.instanceNum = dev_id; \
        getter(&msgData, &bChanged); \
        if (bChanged) { \
            snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, paramStr); \
            if (mUpdateCallback) \
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, \
                                tmp_buff, msgData.paramValue, msgData.paramtype); \
        } \
    } while (0)

    VO_DO_UPDATE(getStatus,               STATUS_STRING);
    VO_DO_UPDATE(getDisplayFormat,        DISPLAY_FORMAT_STRING);
    VO_DO_UPDATE(getName,                 DISPLAY_NAME_STRING);
    VO_DO_UPDATE(getVideoFormat,          VIDEO_FORMAT_STRING);
    VO_DO_UPDATE(getAspectRatioBehaviour, AR_BEHAVIOR_STRING);
    VO_DO_UPDATE(getHDCP,                 HDCP_STRING);

#undef VO_DO_UPDATE
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Migrated accessors
 * ───────────────────────────────────────────────────────────────────────────*/

/************************************************************
 * Description  : Get VideoOutput port status  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.DisplaySettings.getEnableVideoPort
 * Request      : { "videoDisplay": "<portName>" }
 * Response     : { "enable": <bool>, "success": true }
 *
 * Maps: enable=true → "Enabled", false → "Disabled"
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → "Enabled" or "Disabled"
************************************************************/
int hostIf_STBServiceVideoOutput::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool isEnabled = false;
    const std::string paramsJson = "{\"videoDisplay\":\"" + m_portName + "\"}";

    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DS_GET_ENABLE_VIDEO_PORT, paramsJson, "enable", isEnabled))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s)\n",
                __FUNCTION__, THUNDER_DS_GET_ENABLE_VIDEO_PORT, m_portName.c_str());
        return NOK;
    }

    const char *statusStr = isEnabled ? ENABLED_STRING : DISABLED_STRING;
    strncpy(stMsgData->paramValue, statusStr, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = strlen(stMsgData->paramValue);

    if (bCalledVideoOutputStatus && pChanged && strcmp(backupVideoOutputStatus, stMsgData->paramValue))
        *pChanged = true;
    bCalledVideoOutputStatus = true;
    strncpy(backupVideoOutputStatus, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupVideoOutputStatus[_BUF_LEN_16 - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] Status=%s (port=%s)\n", __FUNCTION__, statusStr, m_portName.c_str());
    return OK;
}

/************************************************************
 * Description  : Get VideoOutput display format (current resolution name)  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.DisplaySettings.getCurrentResolution
 * Request      : { "videoDisplay": "<portName>" }
 * Response     : { "resolution": "<name>", "success": true }
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → resolution name string, e.g. "1080p"
************************************************************/
int hostIf_STBServiceVideoOutput::getDisplayFormat(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    const std::string paramsJson = "{\"videoDisplay\":\"" + m_portName + "\"}";
    std::string resolution;

    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_GET_CURRENT_RESOLUTION, paramsJson, "resolution", resolution))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s)\n",
                __FUNCTION__, THUNDER_DS_GET_CURRENT_RESOLUTION, m_portName.c_str());
        return NOK;
    }

    strncpy(stMsgData->paramValue, resolution.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = strlen(stMsgData->paramValue);

    if (bCalledDisplayFormat && pChanged && strcmp(backupDisplayFormat, stMsgData->paramValue))
        *pChanged = true;
    bCalledDisplayFormat = true;
    strncpy(backupDisplayFormat, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupDisplayFormat[_BUF_LEN_16 - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] DisplayFormat: %s (port=%s)\n",
            __FUNCTION__, stMsgData->paramValue, m_portName.c_str());
    return OK;
}

/************************************************************
 * Description  : Get VideoOutput name  [in-memory, no HAL call]
 *
 * Returns m_portName directly.
 *
 * @return OK always.
 * stMsgData->paramValue → port name string, e.g. "HDMI0"
************************************************************/
int hostIf_STBServiceVideoOutput::getName(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    snprintf(stMsgData->paramValue, PARAM_LEN, "%s", m_portName.c_str());
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = strlen(stMsgData->paramValue);

    if (bCalledDisplayName && pChanged && strcmp(backupDisplayName, stMsgData->paramValue))
        *pChanged = true;
    bCalledDisplayName = true;
    strncpy(backupDisplayName, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupDisplayName[_BUF_LEN_16 - 1] = '\0';
    return OK;
}

/************************************************************
 * Description  : Get VideoOutput video format  [no HAL call — hardcoded]
 *
 * Returns "HDMI" — same as original implementation (no HAL call there either).
 *
 * @return OK always.
 * stMsgData->paramValue → "HDMI"
************************************************************/
int hostIf_STBServiceVideoOutput::getVideoFormat(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    snprintf(stMsgData->paramValue, PARAM_LEN, "%s", "HDMI");
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = strlen(stMsgData->paramValue);

    if (bCalledVideoFormat && pChanged && strcmp(backupVideoFormat, stMsgData->paramValue))
        *pChanged = true;
    bCalledVideoFormat = true;
    strncpy(backupVideoFormat, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupVideoFormat[_BUF_LEN_16 - 1] = '\0';
    return OK;
}

/************************************************************
 * Description  : Get VideoOutput aspect ratio behaviour (zoom/DFC mode)  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.DisplaySettings.getZoomSetting
 * Request      : {}  (device-wide setting)
 * Response     : { "zoomSetting": "<mode>", "success": true }
 *
 * Original used device::VideoDevice::getDFC().getName() — same underlying data via Thunder.
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → zoom/DFC name string, e.g. "Full", "None", "16x9 Zoom"
************************************************************/
int hostIf_STBServiceVideoOutput::getAspectRatioBehaviour(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string zoomSetting;

    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_GET_ZOOM_SETTING, "{}", "zoomSetting", zoomSetting))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed\n", __FUNCTION__, THUNDER_DS_GET_ZOOM_SETTING);
        return NOK;
    }

    strncpy(stMsgData->paramValue, zoomSetting.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = strlen(stMsgData->paramValue);

    if (bCalledAspectRatioBehaviour && pChanged &&
        strncmp(backupAspectRatioBehaviour, stMsgData->paramValue, _BUF_LEN_16 - 1))
        *pChanged = true;
    bCalledAspectRatioBehaviour = true;
    strncpy(backupAspectRatioBehaviour, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupAspectRatioBehaviour[_BUF_LEN_16 - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] AspectRatioBehaviour: %s\n", __FUNCTION__, stMsgData->paramValue);
    return OK;
}

/************************************************************
 * Description  : Get HDCP status for this VideoOutput  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.HdcpProfile.getHDCPStatus
 * Request      : {}
 * Response     : { "HDCPStatus": { "isConnected": <bool>, "isHDCPEnabled": <bool>,
 *                                  "isHDCPCompliant": <bool>, ... }, "success": true }
 *
 * Returns true if HDCP is currently enabled (content protection active).
 *
 * Note: invokeThunderPluginMethod is used to get raw JSON because isHDCPEnabled is
 * nested inside the "HDCPStatus" object.
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → bool (true = HDCP enabled)
************************************************************/
int hostIf_STBServiceVideoOutput::getHDCP(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string response;

    if (!invokeThunderPluginMethod(THUNDER_HDCP_GET_STATUS, "{}", response))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed\n", __FUNCTION__, THUNDER_HDCP_GET_STATUS);
        return NOK;
    }

    /* Parse "isHDCPEnabled":true|false from the nested HDCPStatus object.
     * Simple string search is sufficient here — the JSON keys are unique. */
    bool isHDCPEnabled = false;
    const char *key = "\"isHDCPEnabled\":";
    const char *pos = strstr(response.c_str(), key);
    if (pos)
    {
        pos += strlen(key);
        while (*pos == ' ') pos++;
        isHDCPEnabled = (strncmp(pos, "true", 4) == 0);
    }
    else
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                "[%s] \"isHDCPEnabled\" not found in response: %s\n",
                __FUNCTION__, response.c_str());
        return NOK;
    }

    put_boolean(stMsgData->paramValue, isHDCPEnabled);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen  = sizeof(bool);

    if (bCalledHDCP && pChanged && (backupHDCP != isHDCPEnabled))
        *pChanged = true;
    bCalledHDCP = true;
    backupHDCP  = isHDCPEnabled;

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] HDCP=%d (port=%s)\n", __FUNCTION__, isHDCPEnabled, m_portName.c_str());
    return OK;
}

/** @} */
/** @} */
