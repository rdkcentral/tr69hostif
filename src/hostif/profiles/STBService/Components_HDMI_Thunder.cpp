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
 * @file Components_HDMI_Thunder.cpp
 * @brief Components_HDMI APIs migrated from libds to org.rdk.DisplaySettings Thunder plugin.
 *
 * Plugin  : org.rdk.DisplaySettings  (entservices-displaysettings)
 * Port    : HDMI0, HDMI1, ... (derived from getSupportedVideoDisplays)
 *
 * Migrated APIs:
 *   handleSetMsg()       — SET dispatcher
 *   handleGetMsg()       — GET dispatcher
 *   getEnable()          → org.rdk.DisplaySettings.getEnableVideoPort  ("enable" bool)
 *   setEnableVideoPort() → org.rdk.DisplaySettings.setEnableVideoPort
 *   getStatus()          → org.rdk.DisplaySettings.getEnableVideoPort  ("enable" → "Enabled"/"Disabled")
 *   getName()            — in-memory: returns m_portName (e.g. "HDMI0")
 *   getResolutionValue() → org.rdk.DisplaySettings.getCurrentResolution ("resolution" string)
 *   setResolution()      → org.rdk.DisplaySettings.setCurrentResolution
 *   setHDMIResolutionMode() — in-memory static, triggers setCurrentResolution on "Auto"
 *   DisplayDevice.*      — delegated to hostIf_STBServiceDisplayDevice (Thunder)
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "Components_HDMI.h"
#include "Components_DisplayDevice.h"
#include "safec_lib.h"
#include <sstream>

#define DEV_NAME             "HDMI"
#define BASE_NAME            "Device.Services.STBService.1.Components.HDMI"
#define UPDATE_FORMAT_STRING "%s.%d.%s"

/* Parameter name string constants */
#define STATUS_STRING     "Status"
#define ENABLE_STRING     "Enable"
#define RES_MODE_STRING   "ResolutionMode"
#define RES_VAL_STRING    "ResolutionValue"
#define NAME_STRING       "Name"
#define ENABLED_STRING    "Enabled"
#define DISABLED_STRING   "Disabled"

/* DisplayDevice sub-object prefix (handled by hostIf_STBServiceDisplayDevice) */
#define DISPLAYDEVICE_OBJECT_NAME "DisplayDevice."

/* Default resolution applied when switching to Auto mode — values from Components_HDMI.h */

/* Thunder plugin method identifiers */
#define THUNDER_DS_GET_SUPPORTED_VIDEO_DISPLAYS "org.rdk.DisplaySettings.getSupportedVideoDisplays"
#define THUNDER_DS_GET_ENABLE_VIDEO_PORT        "org.rdk.DisplaySettings.getEnableVideoPort"
#define THUNDER_DS_SET_ENABLE_VIDEO_PORT        "org.rdk.DisplaySettings.setEnableVideoPort"
#define THUNDER_DS_GET_CURRENT_RESOLUTION       "org.rdk.DisplaySettings.getCurrentResolution"
#define THUNDER_DS_SET_CURRENT_RESOLUTION       "org.rdk.DisplaySettings.setCurrentResolution"
#define THUNDER_DI_FRAMERATE                    "DisplayInfo.1.framerate"

/* Per-process resolution-mode state (matches the original static) */
char hostIf_STBServiceHDMI::dsHDMIResolutionMode[10] = HDMI_RESOLUTION_MODE_MANUAL;

GHashTable * hostIf_STBServiceHDMI::ifHash = NULL;
GMutex       hostIf_STBServiceHDMI::m_mutex;

/* ─────────────────────────────────────────────────────────────────────────────
 * Instance lifecycle
 * ───────────────────────────────────────────────────────────────────────────*/

hostIf_STBServiceHDMI* hostIf_STBServiceHDMI::getInstance(int dev_id)
{
    if (!ifHash)
        buildPortNameHash();

    hostIf_STBServiceHDMI* pRet =
        (hostIf_STBServiceHDMI*)g_hash_table_lookup(ifHash, (gpointer)(intptr_t)dev_id);

    if (!pRet)
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                "[%s:%s:%d]: No instance for dev_id=%d\n",
                __FILE__, __FUNCTION__, __LINE__, dev_id);
    }
    return pRet;
}

GList* hostIf_STBServiceHDMI::getAllInstances()
{
    if (ifHash) return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_STBServiceHDMI::closeInstance(hostIf_STBServiceHDMI *pDev)
{
    if (pDev)
    {
        if (ifHash)
            g_hash_table_remove(ifHash, (gconstpointer)(intptr_t)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBServiceHDMI::closeAllInstances()
{
    if (ifHash)
    {
        GList* tmp_list = g_hash_table_get_values(ifHash);
        GList* current  = tmp_list;
        while (current)
        {
            hostIf_STBServiceHDMI* pDev = (hostIf_STBServiceHDMI*)current->data;
            current = current->next;
            delete pDev;
        }
        g_list_free(tmp_list);
        g_hash_table_destroy(ifHash);
        ifHash = NULL;
    }
}

void hostIf_STBServiceHDMI::getLock()
{
    g_mutex_init(&hostIf_STBServiceHDMI::m_mutex);
    g_mutex_lock(&hostIf_STBServiceHDMI::m_mutex);
}

void hostIf_STBServiceHDMI::releaseLock()
{
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s:%d] Unlocking mutex...\n",
            __FUNCTION__, __LINE__);
    g_mutex_unlock(&hostIf_STBServiceHDMI::m_mutex);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * buildPortNameHash — discover HDMI video ports via Thunder
 * ───────────────────────────────────────────────────────────────────────────*/

void hostIf_STBServiceHDMI::buildPortNameHash()
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
        if (!portName.empty() && strcasestr(portName.c_str(), "hdmi"))
        {
            hostIf_STBServiceHDMI* pInst = new hostIf_STBServiceHDMI(devId, portName);
            g_hash_table_insert(ifHash, (gpointer)(intptr_t)devId, pInst);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
                    "[%s:%d] ifHash: dev_id=%d portName=%s\n",
                    __FUNCTION__, __LINE__, devId, portName.c_str());
            devId++;
        }
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Constructor / Destructor
 * ───────────────────────────────────────────────────────────────────────────*/

hostIf_STBServiceHDMI::hostIf_STBServiceHDMI(int devid, const std::string& portName)
    : dev_id(devid), m_portName(portName)
{
    errno_t rc = -1;
    backupEnable = false;
    rc = strcpy_s(backupStatus,          sizeof(backupStatus),          " "); ERR_CHK(rc);
    rc = strcpy_s(backupResolutionValue, sizeof(backupResolutionValue), " "); ERR_CHK(rc);
    rc = strcpy_s(backupName,            sizeof(backupName),            " "); ERR_CHK(rc);

    bCalledEnable          = false;
    bCalledStatus          = false;
    bCalledResolutionValue = false;
    bCalledName            = false;

    displayDevice = new hostIf_STBServiceDisplayDevice(devid, portName);
}

hostIf_STBServiceHDMI::~hostIf_STBServiceHDMI()
{
    delete displayDevice;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Dispatch — SET
 * ───────────────────────────────────────────────────────────────────────────*/

int hostIf_STBServiceHDMI::handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if (strcasecmp(paramName, RES_MODE_STRING) == 0)
    {
        ret = setHDMIResolutionMode(stMsgData->paramValue);
    }
    else if (strcasecmp(paramName, RES_VAL_STRING) == 0)
    {
        if (0 == strcasecmp(getHDMIResolutionMode(), HDMI_RESOLUTION_MODE_MANUAL))
        {
            ret = setResolution(stMsgData);
        }
        else
        {
            RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                    "[%s] Setting ResolutionValue has no effect in Auto mode.\n", __FUNCTION__);
            ret = NOK;
        }
    }
    else if (strcasecmp(paramName, ENABLE_STRING) == 0)
    {
        ret = setEnableVideoPort(stMsgData);
    }
    return ret;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Dispatch — GET
 * ───────────────────────────────────────────────────────────────────────────*/

int hostIf_STBServiceHDMI::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
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
    else if (strcasecmp(paramName, NAME_STRING) == 0)
    {
        ret = getName(stMsgData);
    }
    else if (strcasecmp(paramName, RES_MODE_STRING) == 0)
    {
        strncpy(stMsgData->paramValue, getHDMIResolutionMode(), sizeof(stMsgData->paramValue) - 1);
        stMsgData->paramValue[sizeof(stMsgData->paramValue) - 1] = '\0';
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen  = strlen(stMsgData->paramValue);
        ret = OK;
    }
    else if (strcasecmp(paramName, RES_VAL_STRING) == 0)
    {
        ret = getResolutionValue(stMsgData);
    }
    else if (strncasecmp(paramName, DISPLAYDEVICE_OBJECT_NAME, strlen(DISPLAYDEVICE_OBJECT_NAME)) == 0)
    {
        ret = displayDevice->handleGetMsg(paramName + strlen(DISPLAYDEVICE_OBJECT_NAME), stMsgData);
    }
    return ret;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * doUpdates
 * ───────────────────────────────────────────────────────────────────────────*/

void hostIf_STBServiceHDMI::checkForUpdates(updateCallback mUpdateCallback)
{
    (void)mUpdateCallback; /* Currently not implemented */
}

void hostIf_STBServiceHDMI::doUpdates(updateCallback mUpdateCallback)
{
    displayDevice->doUpdates(BASE_NAME, mUpdateCallback);

    HOSTIF_MsgData_t msgData;
    bool bChanged;
    char tmp_buff[PARAM_LEN];

#define HDMI_DO_UPDATE(getter, paramStr) \
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

    HDMI_DO_UPDATE(getResolutionValue, RES_VAL_STRING);
    HDMI_DO_UPDATE(getEnable,          ENABLE_STRING);
    HDMI_DO_UPDATE(getStatus,          STATUS_STRING);
    HDMI_DO_UPDATE(getName,            NAME_STRING);

#undef HDMI_DO_UPDATE
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Migrated accessors
 * ───────────────────────────────────────────────────────────────────────────*/

/************************************************************
 * Description  : Get HDMI port Enable state  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.DisplaySettings.getEnableVideoPort
 * Request      : { "videoDisplay": "<portName>" }
 * Response     : { "enable": <bool>, "success": true }
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → bool (true = enabled)
************************************************************/
int hostIf_STBServiceHDMI::getEnable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
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

    put_boolean(stMsgData->paramValue, isEnabled);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen  = sizeof(bool);

    if (bCalledEnable && pChanged && (backupEnable != isEnabled))
        *pChanged = true;
    bCalledEnable = true;
    backupEnable  = isEnabled;

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] Enable=%d (port=%s)\n", __FUNCTION__, isEnabled, m_portName.c_str());
    return OK;
}

/************************************************************
 * Description  : Enable or disable HDMI port  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.DisplaySettings.setEnableVideoPort
 * Request      : { "videoDisplay": "<portName>", "enable": <bool> }
 * Response     : { "success": true }
 *
 * @return OK on success, NOK on failure.
************************************************************/
int hostIf_STBServiceHDMI::setEnableVideoPort(const HOSTIF_MsgData_t *stMsgData)
{
    bool enable = get_boolean(stMsgData->paramValue);
    const std::string paramsJson = "{\"videoDisplay\":\"" + m_portName
                                   + "\",\"enable\":" + (enable ? "true" : "false") + "}";
    bool success = false;

    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DS_SET_ENABLE_VIDEO_PORT, paramsJson, "success", success)
        || !success)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s, enable=%d)\n",
                __FUNCTION__, THUNDER_DS_SET_ENABLE_VIDEO_PORT, m_portName.c_str(), enable);
        return NOK;
    }

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] setEnableVideoPort succeeded (port=%s, enable=%d)\n",
            __FUNCTION__, m_portName.c_str(), enable);
    return OK;
}

/************************************************************
 * Description  : Get HDMI port status  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.DisplaySettings.getEnableVideoPort
 * Request      : { "videoDisplay": "<portName>" }
 * Response     : { "enable": <bool>, "success": true }
 *
 * Maps: enable=true → "Enabled", enable=false → "Disabled"
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → "Enabled" or "Disabled"
************************************************************/
int hostIf_STBServiceHDMI::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int ret = getEnable(stMsgData);
    if (ret == OK)
    {
        const char *statusStr = get_boolean(stMsgData->paramValue) ? ENABLED_STRING : DISABLED_STRING;
        strncpy(stMsgData->paramValue, statusStr, PARAM_LEN);
        stMsgData->paramValue[PARAM_LEN - 1] = '\0';
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen  = strlen(stMsgData->paramValue);

        if (bCalledStatus && pChanged && strcmp(backupStatus, stMsgData->paramValue))
            *pChanged = true;
        bCalledStatus = true;
        strncpy(backupStatus, stMsgData->paramValue, _BUF_LEN_16 - 1);
        backupStatus[_BUF_LEN_16 - 1] = '\0';
    }
    return ret;
}

/************************************************************
 * Description  : Get human-readable HDMI port name  [in-memory, no HAL call]
 *
 * Returns m_portName directly (e.g. "HDMI0", "HDMI1").
 *
 * @return OK always.
 * stMsgData->paramValue → port name string
************************************************************/
int hostIf_STBServiceHDMI::getName(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    snprintf(stMsgData->paramValue, PARAM_LEN, "%s", m_portName.c_str());
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = strlen(stMsgData->paramValue);

    if (bCalledName && pChanged && strcmp(backupName, stMsgData->paramValue))
        *pChanged = true;
    bCalledName = true;
    strncpy(backupName, stMsgData->paramValue, _BUF_LEN_256 - 1);
    backupName[_BUF_LEN_256 - 1] = '\0';
    return OK;
}

/************************************************************
 * Description  : Get current HDMI resolution value  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.DisplaySettings.getCurrentResolution
 * Request      : { "videoDisplay": "<portName>" }
 * Response     : { "resolution": "<name>", "w": <int>, "h": <int>,
 *                  "progressive": <bool>, "success": true }
 * Frame rate   : org.rdk.DisplayInfo / DisplayInfo.1.framerate
 *
 * Reconstructs the full libds-compatible format string:
 *   "<W>x<H>p/<F>Hz"  (e.g. "1920x1080p/60Hz", "1920x1080i/59.94Hz")
 * using w, h, progressive from getCurrentResolution and framerate from DisplayInfo.
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → e.g. "1920x1080p/60Hz"
************************************************************/
int hostIf_STBServiceHDMI::getResolutionValue(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    /* Step 1: get w, h, progressive from getCurrentResolution full response */
    const std::string paramsJson = "{\"videoDisplay\":\"" + m_portName + "\"}";
    std::string rawResponse;

    if (!invokeThunderPluginMethod(THUNDER_DS_GET_CURRENT_RESOLUTION, paramsJson, rawResponse))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s)\n",
                __FUNCTION__, THUNDER_DS_GET_CURRENT_RESOLUTION, m_portName.c_str());
        return NOK;
    }

    int w = 0, h = 0;
    bool isProgressive = true;
    const char *pos;

    pos = strstr(rawResponse.c_str(), "\"w\":");
    if (pos) w = (int)strtol(pos + 4, NULL, 10);

    pos = strstr(rawResponse.c_str(), "\"h\":");
    if (pos) h = (int)strtol(pos + 4, NULL, 10);

    pos = strstr(rawResponse.c_str(), "\"progressive\":");
    if (pos)
    {
        pos += strlen("\"progressive\":");
        while (*pos == ' ') pos++;
        isProgressive = (strncmp(pos, "true", 4) == 0);
    }

    /* Step 2: get frame rate from DisplayInfo.1.framerate */
    double frameRate = 0.0;
    invokeThunderPluginMethodAndExtractNumberField(
            THUNDER_DI_FRAMERATE, "{}", "framerate", frameRate);

    /* Step 3: reconstruct full format string matching original libds format */
    char resStr[PARAM_LEN];
    if (frameRate > 0.0 && frameRate == (double)(int)frameRate)
        snprintf(resStr, sizeof(resStr), "%dx%d%s/%dHz",
                 w, h, isProgressive ? "p" : "i", (int)frameRate);
    else if (frameRate > 0.0)
        snprintf(resStr, sizeof(resStr), "%dx%d%s/%.2fHz",
                 w, h, isProgressive ? "p" : "i", frameRate);
    else
        snprintf(resStr, sizeof(resStr), "%dx%d%s",
                 w, h, isProgressive ? "p" : "i");

    strncpy(stMsgData->paramValue, resStr, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = strlen(stMsgData->paramValue);

    if (bCalledResolutionValue && pChanged && strcmp(backupResolutionValue, stMsgData->paramValue))
        *pChanged = true;
    bCalledResolutionValue = true;
    strncpy(backupResolutionValue, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupResolutionValue[_BUF_LEN_16 - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] ResolutionValue: %s (port=%s)\n",
            __FUNCTION__, stMsgData->paramValue, m_portName.c_str());
    return OK;
}

/************************************************************
 * Description  : Set HDMI resolution  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.DisplaySettings.setCurrentResolution
 * Request      : { "videoDisplay": "<portName>", "resolution": "<name>" }
 * Response     : { "success": true }
 *
 * Input: stMsgData->paramValue = resolution string (e.g. "1080p", "720p")
 *
 * @return OK on success, NOK on failure.
************************************************************/
int hostIf_STBServiceHDMI::setResolution(const HOSTIF_MsgData_t *stMsgData)
{
    const std::string resolution = stMsgData->paramValue;
    if (resolution.empty())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Empty resolution value\n", __FUNCTION__);
        return NOK;
    }

    const std::string paramsJson = "{\"videoDisplay\":\"" + m_portName
                                   + "\",\"resolution\":\"" + resolution + "\"}";
    bool success = false;

    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DS_SET_CURRENT_RESOLUTION, paramsJson, "success", success)
        || !success)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s, resolution=%s)\n",
                __FUNCTION__, THUNDER_DS_SET_CURRENT_RESOLUTION,
                m_portName.c_str(), resolution.c_str());
        return NOK;
    }

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] setCurrentResolution succeeded (port=%s, resolution=%s)\n",
            __FUNCTION__, m_portName.c_str(), resolution.c_str());
    return OK;
}

/************************************************************
 * Description  : Set HDMI resolution mode (in-memory state)
 * Input        : value = "Auto" or "Manual"
 *
 * On "Auto", resets resolution to the default value via Thunder.
 *
 * @return OK on success, NOK on invalid input.
************************************************************/
int hostIf_STBServiceHDMI::setHDMIResolutionMode(const char* value)
{
    if ((0 != strcasecmp(value, HDMI_RESOLUTION_MODE_AUTO)) &&
        (0 != strcasecmp(value, HDMI_RESOLUTION_MODE_MANUAL)))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Invalid ResolutionMode [%s]. Expected: Auto|Manual\n",
                __FUNCTION__, value);
        return NOK;
    }

    strncpy(dsHDMIResolutionMode, value, sizeof(dsHDMIResolutionMode) - 1);
    dsHDMIResolutionMode[sizeof(dsHDMIResolutionMode) - 1] = '\0';

    if (0 == strcasecmp(value, HDMI_RESOLUTION_MODE_AUTO))
    {
        /* Apply default resolution when switching to Auto */
        HOSTIF_MsgData_t msgData;
        memset(&msgData, 0, sizeof(msgData));
        strncpy(msgData.paramValue, HDMI_RESOLUTION_VALUE_DEFAULT, PARAM_LEN);
        msgData.paramValue[PARAM_LEN - 1] = '\0';
        msgData.instanceNum = dev_id;
        setResolution(&msgData);
    }
    return OK;
}

const char* hostIf_STBServiceHDMI::getHDMIResolutionMode()
{
    return dsHDMIResolutionMode;
}

/** @} */
/** @} */
