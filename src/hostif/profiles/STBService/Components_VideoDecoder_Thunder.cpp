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
 * @file Components_VideoDecoder_Thunder.cpp
 * @brief Components_VideoDecoder APIs migrated from libds to org.rdk.PowerManager Thunder plugin.
 *
 * Plugin  : org.rdk.PowerManager  (entservices-powermanager)
 *           Auto-generated from Exchange::JPowerManager
 *
 * Migrated APIs:
 *   getX_COMCAST_COM_Standby()  → org.rdk.PowerManager.GetPowerState  ("currentState" == "STANDBY")
 *   setX_COMCAST_COM_Standby()  → org.rdk.PowerManager.SetPowerState  (powerState "STANDBY"/"ON")
 *   getStatus()                 → org.rdk.PowerManager.GetPowerState
 *                                 ON → "Enabled", STANDBY → "X_COMCAST-COM_Standby",
 *                                 OFF/UNKNOWN → "Disabled"
 *   getContentAspectRatio()     → org.rdk.DisplayInfo.DisplayInfo.1.displayinfo
 *                                 (width/height → "16x9" or "4x3")
 *
 * Not migrated (hardcoded / no HAL call in original):
 *   Enable        — hardcoded true
 *   Name          — hardcoded "VideoDecoderHDMI0"
 *   X_RDKCENTRAL-COM_MPEGHPart2 — hardcoded profile path
 *
 * Power state string values (from PowerUtils::str()):
 *   "ON", "STANDBY", "DEEP_SLEEP", "LIGHT_SLEEP", "OFF", "UNKNOWN"
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "Components_VideoDecoder.h"

#define BASE_NAME            "Device.Services.STBService.1.Components.VideoDecoder"
#define UPDATE_FORMAT_STRING "%s.%d.%s"

/* Parameter name string constants */
#define STATUS_STRING           "Status"
#define CONTENT_AR_STRING       "ContentAspectRatio"
#define COMCAST_STANDBY_STRING  "X_COMCAST-COM_Standby"
#define HEVC_STRING             "X_RDKCENTRAL-COM_MPEGHPart2"
#define HEVC_PROFILE_PATH       ".Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1"
#define NAME_STRING             "Name"
#define ENABLE_STRING           "Enable"
#define ENABLED_STRING          "Enabled"
#define DISABLED_STRING         "Disabled"
#define ERROR_STRING            "Error"

/* Thunder plugin method identifiers */
#define THUNDER_PM_GET_POWER_STATE  "org.rdk.PowerManager.GetPowerState"
#define THUNDER_PM_SET_POWER_STATE  "org.rdk.PowerManager.SetPowerState"
#define THUNDER_DI_DISPLAYINFO      "DisplayInfo.1.displayinfo"

/* Power state strings returned by org.rdk.PowerManager.GetPowerState "currentState" field */
#define PM_STATE_ON           "ON"
#define PM_STATE_STANDBY      "STANDBY"
#define PM_STATE_DEEP_SLEEP   "DEEP_SLEEP"
#define PM_STATE_LIGHT_SLEEP  "LIGHT_SLEEP"
#define PM_STATE_OFF          "OFF"

GHashTable * hostIf_STBServiceVideoDecoder::ifHash = NULL;
GMutex       hostIf_STBServiceVideoDecoder::m_mutex;

/* ─────────────────────────────────────────────────────────────────────────────
 * Instance lifecycle
 * ───────────────────────────────────────────────────────────────────────────*/

hostIf_STBServiceVideoDecoder* hostIf_STBServiceVideoDecoder::getInstance(int dev_id)
{
    if (!ifHash)
        buildSingleInstance();

    hostIf_STBServiceVideoDecoder* pRet =
        (hostIf_STBServiceVideoDecoder*)g_hash_table_lookup(ifHash, (gpointer)(intptr_t)dev_id);

    if (!pRet)
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                "[%s:%s:%d]: No instance for dev_id=%d\n",
                __FILE__, __FUNCTION__, __LINE__, dev_id);
    }
    return pRet;
}

GList* hostIf_STBServiceVideoDecoder::getAllInstances()
{
    if (ifHash) return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_STBServiceVideoDecoder::closeInstance(hostIf_STBServiceVideoDecoder *pDev)
{
    if (pDev)
    {
        if (ifHash)
            g_hash_table_remove(ifHash, (gconstpointer)(intptr_t)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBServiceVideoDecoder::closeAllInstances()
{
    if (ifHash)
    {
        GList* tmp_list = g_hash_table_get_values(ifHash);
        GList* current  = tmp_list;
        while (current)
        {
            hostIf_STBServiceVideoDecoder* pDev = (hostIf_STBServiceVideoDecoder*)current->data;
            current = current->next;
            delete pDev;
        }
        g_list_free(tmp_list);
        g_hash_table_destroy(ifHash);
        ifHash = NULL;
    }
}

void hostIf_STBServiceVideoDecoder::getLock()
{
    g_mutex_init(&hostIf_STBServiceVideoDecoder::m_mutex);
    g_mutex_lock(&hostIf_STBServiceVideoDecoder::m_mutex);
}

void hostIf_STBServiceVideoDecoder::releaseLock()
{
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s:%d] Unlocking mutex...\n",
            __FUNCTION__, __LINE__);
    g_mutex_unlock(&hostIf_STBServiceVideoDecoder::m_mutex);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * buildSingleInstance — VideoDecoder is device-wide, one instance only
 * ───────────────────────────────────────────────────────────────────────────*/

void hostIf_STBServiceVideoDecoder::buildSingleInstance()
{
    if (ifHash) g_hash_table_destroy(ifHash);
    ifHash = g_hash_table_new(NULL, NULL);

    hostIf_STBServiceVideoDecoder* pInst = new hostIf_STBServiceVideoDecoder(1);
    g_hash_table_insert(ifHash, (gpointer)(intptr_t)1, pInst);
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
            "[%s:%d] Single VideoDecoder instance created (dev_id=1)\n",
            __FUNCTION__, __LINE__);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Constructor
 * ───────────────────────────────────────────────────────────────────────────*/

hostIf_STBServiceVideoDecoder::hostIf_STBServiceVideoDecoder(int devid)
{
    dev_id = devid;

    strncpy(backupContentAspectRatio, " ", _BUF_LEN_16 - 1);
    backupStandby = false;
    strncpy(backupVideoDecoderStatus, " ", _BUF_LEN_32 - 1);

    bCalledContentAspectRatio = false;
    bCalledStandby            = false;
    bCalledVideoDecoderStatus = false;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Dispatch — SET
 * ───────────────────────────────────────────────────────────────────────────*/

int hostIf_STBServiceVideoDecoder::handleSetMsg(const char *pSetting, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if (strcasecmp(pSetting, COMCAST_STANDBY_STRING) == 0)
    {
        ret = setX_COMCAST_COM_Standby(stMsgData); /* MIGRATED: org.rdk.PowerManager.SetPowerState */
    }
    return ret;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Dispatch — GET
 * ───────────────────────────────────────────────────────────────────────────*/

int hostIf_STBServiceVideoDecoder::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if (strcasecmp(paramName, COMCAST_STANDBY_STRING) == 0)
    {
        ret = getX_COMCAST_COM_Standby(stMsgData); /* MIGRATED */
    }
    else if (strcasecmp(paramName, ENABLE_STRING) == 0)
    {
        put_boolean(stMsgData->paramValue, true);
        stMsgData->paramtype = hostIf_BooleanType;
        stMsgData->paramLen  = sizeof(bool);
        ret = OK;
    }
    else if (strcasecmp(paramName, STATUS_STRING) == 0)
    {
        ret = getStatus(stMsgData); /* MIGRATED */
    }
    else if (strcasecmp(paramName, NAME_STRING) == 0)
    {
        strncpy(stMsgData->paramValue, "VideoDecoderHDMI0", strlen("VideoDecoderHDMI0") + 1);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen  = strlen(stMsgData->paramValue);
        ret = OK;
    }
    else if (strcasecmp(paramName, CONTENT_AR_STRING) == 0)
    {
        ret = getContentAspectRatio(stMsgData); /* MIGRATED */
    }
    else if (strcasecmp(paramName, HEVC_STRING) == 0)
    {
        strncpy(stMsgData->paramValue, HEVC_PROFILE_PATH, strlen(HEVC_PROFILE_PATH) + 1);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen  = strlen(stMsgData->paramValue);
        ret = OK;
    }
    return ret;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * doUpdates
 * ───────────────────────────────────────────────────────────────────────────*/

void hostIf_STBServiceVideoDecoder::checkForUpdates(updateCallback mUpdateCallback)
{
    (void)mUpdateCallback; /* Currently not implemented */
}

void hostIf_STBServiceVideoDecoder::doUpdates(updateCallback mUpdateCallback)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    char tmp_buff[PARAM_LEN];

#define VD_DO_UPDATE(getter, paramStr) \
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

    VD_DO_UPDATE(getX_COMCAST_COM_Standby, COMCAST_STANDBY_STRING);
    VD_DO_UPDATE(getStatus,               STATUS_STRING);
    VD_DO_UPDATE(getContentAspectRatio,   CONTENT_AR_STRING);

#undef VD_DO_UPDATE
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Helper — extract currentState from GetPowerState response
 *
 * org.rdk.PowerManager.GetPowerState response:
 *   { "currentState": "ON"|"STANDBY"|"DEEP_SLEEP"|"LIGHT_SLEEP"|"OFF"|"UNKNOWN",
 *     "prevState": "...", "success": true }
 * ───────────────────────────────────────────────────────────────────────────*/
static bool getPowerStateString(std::string& currentState)
{
    std::string response;
    if (!invokeThunderPluginMethod(THUNDER_PM_GET_POWER_STATE, "{}", response))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[getPowerStateString] Thunder %s failed\n", THUNDER_PM_GET_POWER_STATE);
        return false;
    }

    const char *key = "\"currentState\":\"";
    const char *pos = strstr(response.c_str(), key);
    if (!pos)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[getPowerStateString] \"currentState\" not found in: %s\n",
                response.c_str());
        return false;
    }
    pos += strlen(key);
    const char *end = strchr(pos, '"');
    if (!end)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[getPowerStateString] Malformed response: %s\n", response.c_str());
        return false;
    }
    currentState = std::string(pos, end - pos);
    return true;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Migrated accessors
 * ───────────────────────────────────────────────────────────────────────────*/

/************************************************************
 * Description  : Get VideoDecoder standby state  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.PowerManager.GetPowerState
 * Request      : {}
 * Response     : { "currentState": "ON"|"STANDBY"|..., "prevState": "...", "success": true }
 *
 * Returns true if the device is in any standby state (STANDBY, DEEP_SLEEP, LIGHT_SLEEP).
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → bool (true = in standby)
************************************************************/
int hostIf_STBServiceVideoDecoder::getX_COMCAST_COM_Standby(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string currentState;
    if (!getPowerStateString(currentState))
        return NOK;

    bool isStandby = (currentState == PM_STATE_STANDBY    ||
                      currentState == PM_STATE_DEEP_SLEEP  ||
                      currentState == PM_STATE_LIGHT_SLEEP);

    put_boolean(stMsgData->paramValue, isStandby);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen  = sizeof(bool);

    if (bCalledStandby && pChanged && (backupStandby != isStandby))
        *pChanged = true;
    bCalledStandby = true;
    backupStandby  = isStandby;

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] Standby=%d (currentState=%s)\n",
            __FUNCTION__, isStandby, currentState.c_str());
    return OK;
}

/************************************************************
 * Description  : Set VideoDecoder standby state  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.PowerManager.SetPowerState
 * Request      : { "keyCode": 0, "powerState": "STANDBY"|"ON", "reason": "TR69" }
 * Response     : { "success": true }
 *
 * true  → request STANDBY
 * false → request ON
 *
 * @return OK on success, NOK on failure.
************************************************************/
int hostIf_STBServiceVideoDecoder::setX_COMCAST_COM_Standby(const HOSTIF_MsgData_t *stMsgData)
{
    bool goStandby = get_boolean(stMsgData->paramValue);
    const char *targetState = goStandby ? PM_STATE_STANDBY : PM_STATE_ON;

    const std::string paramsJson =
        std::string("{\"keyCode\":0,\"powerState\":\"") + targetState
        + "\",\"reason\":\"TR69\"}";

    std::string response;
    if (!invokeThunderPluginMethod(THUNDER_PM_SET_POWER_STATE, paramsJson, response))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (powerState=%s)\n",
                __FUNCTION__, THUNDER_PM_SET_POWER_STATE, targetState);
        return NOK;
    }

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] SetPowerState succeeded (powerState=%s)\n",
            __FUNCTION__, targetState);
    return OK;
}

/************************************************************
 * Description  : Get VideoDecoder status from power state  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.PowerManager.GetPowerState
 * Request      : {}
 * Response     : { "currentState": "ON"|"STANDBY"|..., "success": true }
 *
 * Maps:
 *   "ON"                     → "Enabled"
 *   "STANDBY"                → "X_COMCAST-COM_Standby"
 *   "DEEP_SLEEP"/"LIGHT_SLEEP" → "X_COMCAST-COM_Standby"
 *   "OFF"                    → "Disabled"
 *   any other                → "Error"
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → status string
************************************************************/
int hostIf_STBServiceVideoDecoder::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string currentState;
    if (!getPowerStateString(currentState))
        return NOK;

    const char *statusStr;
    if (currentState == PM_STATE_ON)
        statusStr = ENABLED_STRING;
    else if (currentState == PM_STATE_STANDBY    ||
             currentState == PM_STATE_DEEP_SLEEP  ||
             currentState == PM_STATE_LIGHT_SLEEP)
        statusStr = COMCAST_STANDBY_STRING;
    else if (currentState == PM_STATE_OFF)
        statusStr = DISABLED_STRING;
    else
        statusStr = ERROR_STRING;

    strncpy(stMsgData->paramValue, statusStr, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = strlen(stMsgData->paramValue);

    if (bCalledVideoDecoderStatus && pChanged && strcmp(backupVideoDecoderStatus, stMsgData->paramValue))
        *pChanged = true;
    bCalledVideoDecoderStatus = true;
    strncpy(backupVideoDecoderStatus, stMsgData->paramValue, _BUF_LEN_32 - 1);
    backupVideoDecoderStatus[_BUF_LEN_32 - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] Status=%s (currentState=%s)\n",
            __FUNCTION__, statusStr, currentState.c_str());
    return OK;
}

/************************************************************
 * Description  : Get VideoDecoder content aspect ratio  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.DisplayInfo.DisplayInfo.1.displayinfo  (property endpoint)
 * Request      : {}
 * Response     : { "totalgpuram": ..., "width": <uint>, "height": <uint>,
 *                  "connected": <bool>, ... }
 *
 * Derives aspect ratio from display width/height:
 *   width/height ≈ 16/9 → "16x9", otherwise → "4x3"
 *
 * Note: original returned the AR of the HDMI display device, not of the content.
 * This Thunder implementation is functionally equivalent but uses display info.
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → "16x9" or "4x3"
************************************************************/
int hostIf_STBServiceVideoDecoder::getContentAspectRatio(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string response;

    if (!invokeThunderPluginMethod(THUNDER_DI_DISPLAYINFO, "{}", response))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed\n", __FUNCTION__, THUNDER_DI_DISPLAYINFO);
        return NOK;
    }

    /* Extract "width" and "height" from the JSON response */
    int width  = 0;
    int height = 0;
    const char *wKey = "\"width\":";
    const char *hKey = "\"height\":";
    const char *pos;

    pos = strstr(response.c_str(), wKey);
    if (pos) width  = (int)strtol(pos + strlen(wKey), NULL, 10);

    pos = strstr(response.c_str(), hKey);
    if (pos) height = (int)strtol(pos + strlen(hKey), NULL, 10);

    /* Aspect ratio: 16:9 ≈ 1.777, 4:3 ≈ 1.333.  Threshold = 1.55 */
    const char *arStr = "4x3";
    if (height > 0 && ((float)width / (float)height) > 1.55f)
        arStr = "16x9";

    strncpy(stMsgData->paramValue, arStr, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = strlen(stMsgData->paramValue);

    if (bCalledContentAspectRatio && pChanged && strcmp(backupContentAspectRatio, stMsgData->paramValue))
        *pChanged = true;
    bCalledContentAspectRatio = true;
    strncpy(backupContentAspectRatio, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupContentAspectRatio[_BUF_LEN_16 - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] ContentAspectRatio=%s (width=%d, height=%d)\n",
            __FUNCTION__, arStr, width, height);
    return OK;
}

/** @} */
/** @} */
