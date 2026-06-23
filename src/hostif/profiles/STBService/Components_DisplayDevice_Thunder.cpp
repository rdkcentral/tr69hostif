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
 * @file Components_DisplayDevice_Thunder.cpp
 * @brief Components_DisplayDevice APIs migrated from libds to Thunder plugins.
 *
 * Plugin  : org.rdk.DisplaySettings  (entservices-displaysettings)
 *
 * Migrated APIs:
 *   getStatus()               → org.rdk.DisplaySettings.getConnectedVideoDisplays
 *                               (check if m_portName appears in "connectedVideoDisplays" list)
 *   getEDID_BYTES()           → org.rdk.DisplaySettings.readEDID  ("EDID" base64 field)
 *   getX_COMCAST_COM_EDID()   → org.rdk.DisplaySettings.readEDID  ("EDID" base64 field)
 *   getSupportedResolutions() → org.rdk.DisplaySettings.getSupportedResolutions
 *                               ("supportedResolutions" comma-joined array)
 *   getPreferredResolution()  → org.rdk.DisplaySettings.getCurrentResolution ("resolution" field)
 *
 * Not migrated (no Thunder equivalent):
 *   VideoLatency, CECSupport, AutoLipSyncSupport, HDMI3DPresent → remain NOT_HANDLED
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#include "Components_DisplayDevice.h"
#include "hostIf_utils.h"
#include "safec_lib.h"
#include <sstream>

/* Parameter name string constants */
#define STATUS_STRING           "Status"
#define EEDID_STRING            "EEDID"
#define COMCAST_EDID_STRING     "X_COMCAST-COM_EDID"
#define EDID_BYTES_STRING       "EDID_BYTES"
#define SUPPORTED_RES_STRING    "SupportedResolutions"
#define PREF_RES_STRING         "PreferredResolution"
#define VIDEO_LATENCY_STRING    "VideoLatency"
#define CECSUPPORT_STRING       "CECSupport"
#define AUTO_LIP_SYNC_STRING    "AutoLipSyncSupport"
#define HDMI3D_STRING           "HDMI3DPresent"
#define PRESENT_STRING          "Present"
#define ABSENT_STRING           "Absent"

#define UPDATE_FORMAT_STRING    "%s.%d.%s%s"
#define DISPLAYDEVICE_OBJECT_NAME "DisplayDevice."

/* Thunder plugin method identifiers */
#define THUNDER_DS_GET_CONNECTED_VIDEO_DISPLAYS  "org.rdk.DisplaySettings.getConnectedVideoDisplays"
#define THUNDER_DS_READ_EDID                     "org.rdk.DisplaySettings.readEDID"
#define THUNDER_DS_GET_SUPPORTED_RESOLUTIONS     "org.rdk.DisplaySettings.getSupportedResolutions"
#define THUNDER_DS_GET_CURRENT_RESOLUTION        "org.rdk.DisplaySettings.getCurrentResolution"

/**
 * @brief Constructor — stores dev_id and port name; initialises backup fields.
 */
hostIf_STBServiceDisplayDevice::hostIf_STBServiceDisplayDevice(int devId, const std::string& portName)
    : dev_id(devId), m_portName(portName)
{
    errno_t rc = -1;
    rc = strcpy_s(backupDisplayDeviceStatus, sizeof(backupDisplayDeviceStatus), " ");
    ERR_CHK(rc);
    rc = strcpy_s(backupEDID, sizeof(backupEDID), " ");
    ERR_CHK(rc);
    rc = strcpy_s(backupEDIDBytes, sizeof(backupEDIDBytes), " ");
    ERR_CHK(rc);
    rc = strcpy_s(backupSupportedResolution, sizeof(backupSupportedResolution), " ");
    ERR_CHK(rc);
    rc = strcpy_s(backupPreferredResolution, sizeof(backupPreferredResolution), " ");
    ERR_CHK(rc);

    bCalledDisplayDeviceStatus = false;
    bCalledEDID                = false;
    bCalledEDIDBytes           = false;
    bCalledSupportedResolution = false;
    bCalledPreferredResolution = false;
}

/**
 * @brief SET dispatcher — no writeable parameters on DisplayDevice.
 */
int hostIf_STBServiceDisplayDevice::handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    stMsgData->faultCode = fcInvalidParameterName;
    return NOT_HANDLED;
}

/**
 * @brief GET dispatcher — routes to the Thunder-migrated accessors.
 */
int hostIf_STBServiceDisplayDevice::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if (strcasecmp(paramName, STATUS_STRING) == 0)
    {
        ret = getStatus(stMsgData);
    }
    else if (strcasecmp(paramName, EEDID_STRING) == 0 ||
             strcasecmp(paramName, COMCAST_EDID_STRING) == 0)
    {
        ret = getX_COMCAST_COM_EDID(stMsgData);
    }
    else if (strcasecmp(paramName, EDID_BYTES_STRING) == 0)
    {
        ret = getEDID_BYTES(stMsgData);
    }
    else if (strcasecmp(paramName, SUPPORTED_RES_STRING) == 0)
    {
        ret = getSupportedResolutions(stMsgData);
    }
    else if (strcasecmp(paramName, PREF_RES_STRING) == 0)
    {
        ret = getPreferredResolution(stMsgData);
    }
    else if (strcasecmp(paramName, VIDEO_LATENCY_STRING) == 0 ||
             strcasecmp(paramName, CECSUPPORT_STRING) == 0 ||
             strcasecmp(paramName, AUTO_LIP_SYNC_STRING) == 0 ||
             strcasecmp(paramName, HDMI3D_STRING) == 0)
    {
        /* No Thunder equivalent — not migrated */
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOT_HANDLED;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s:%d] Parameter \'%s\' is Not Supported\n",
                __FUNCTION__, __LINE__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOK;
    }
    return ret;
}

/**
 * @brief Propagate changed values via the update callback.
 */
void hostIf_STBServiceDisplayDevice::doUpdates(const char *baseName, updateCallback mUpdateCallback)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    char tmp_buff[PARAM_LEN];

    memset(&msgData, 0, sizeof(msgData));
    memset(tmp_buff, 0, PARAM_LEN);
    bChanged = false;
    msgData.instanceNum = dev_id;

    getStatus(&msgData, &bChanged);
    if (bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, baseName, dev_id,
                 DISPLAYDEVICE_OBJECT_NAME, STATUS_STRING);
        if (mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, tmp_buff,
                            msgData.paramValue, msgData.paramtype);
    }

    memset(&msgData, 0, sizeof(msgData));
    memset(tmp_buff, 0, PARAM_LEN);
    bChanged = false;
    msgData.instanceNum = dev_id;

    getX_COMCAST_COM_EDID(&msgData, &bChanged);
    if (bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, baseName, dev_id,
                 DISPLAYDEVICE_OBJECT_NAME, EEDID_STRING);
        if (mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, tmp_buff,
                            msgData.paramValue, msgData.paramtype);
    }

    memset(&msgData, 0, sizeof(msgData));
    memset(tmp_buff, 0, PARAM_LEN);
    bChanged = false;
    msgData.instanceNum = dev_id;

    getEDID_BYTES(&msgData, &bChanged);
    if (bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, baseName, dev_id,
                 DISPLAYDEVICE_OBJECT_NAME, EDID_BYTES_STRING);
        if (mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, tmp_buff,
                            msgData.paramValue, msgData.paramtype);
    }

    memset(&msgData, 0, sizeof(msgData));
    memset(tmp_buff, 0, PARAM_LEN);
    bChanged = false;
    msgData.instanceNum = dev_id;

    getSupportedResolutions(&msgData, &bChanged);
    if (bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, baseName, dev_id,
                 DISPLAYDEVICE_OBJECT_NAME, SUPPORTED_RES_STRING);
        if (mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, tmp_buff,
                            msgData.paramValue, msgData.paramtype);
    }

    memset(&msgData, 0, sizeof(msgData));
    memset(tmp_buff, 0, PARAM_LEN);
    bChanged = false;
    msgData.instanceNum = dev_id;

    getPreferredResolution(&msgData, &bChanged);
    if (bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, baseName, dev_id,
                 DISPLAYDEVICE_OBJECT_NAME, PREF_RES_STRING);
        if (mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, tmp_buff,
                            msgData.paramValue, msgData.paramtype);
    }
}

/************************************************************
 * Description  : Get HDMI display device connection status  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.DisplaySettings.getConnectedVideoDisplays
 * Request      : {}
 * Response     : { "connectedVideoDisplays": ["HDMI0", ...], "success": true }
 *
 * Maps: port name present in list → "Present"; absent → "Absent"
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → "Present" or "Absent"
************************************************************/
int hostIf_STBServiceDisplayDevice::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string response;

    if (!invokeThunderPluginMethod(THUNDER_DS_GET_CONNECTED_VIDEO_DISPLAYS, "{}", response))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed\n", __FUNCTION__,
                THUNDER_DS_GET_CONNECTED_VIDEO_DISPLAYS);
        return NOK;
    }

    /* Check if our port name appears in the JSON response array */
    bool isConnected = (response.find(m_portName) != std::string::npos);
    const char *statusStr = isConnected ? PRESENT_STRING : ABSENT_STRING;

    strncpy(stMsgData->paramValue, statusStr, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = strlen(stMsgData->paramValue);

    if (bCalledDisplayDeviceStatus && pChanged && strcmp(backupDisplayDeviceStatus, stMsgData->paramValue))
        *pChanged = true;
    bCalledDisplayDeviceStatus = true;
    strncpy(backupDisplayDeviceStatus, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupDisplayDeviceStatus[_BUF_LEN_16 - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] DisplayDevice Status: %s (port=%s)\n",
            __FUNCTION__, stMsgData->paramValue, m_portName.c_str());
    return OK;
}

/************************************************************
 * Description  : Get EDID of connected display device  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.DisplaySettings.readEDID
 * Request      : {}  (uses default video port)
 * Response     : { "EDID": "<base64>", "success": true }
 *
 * Returns the base64-encoded EDID string.
 * Note: original returned "pcode=0x%x,pserial=0x%x,year=%d,week=%d" from libds;
 *       Thunder returns the full base64 EDID blob via readEDID.
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → base64 EDID string
************************************************************/
int hostIf_STBServiceDisplayDevice::getX_COMCAST_COM_EDID(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string edidBase64;

    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_READ_EDID, "{}", "EDID", edidBase64))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed\n", __FUNCTION__, THUNDER_DS_READ_EDID);
        return NOK;
    }

    strncpy(stMsgData->paramValue, edidBase64.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = strlen(stMsgData->paramValue);

    if (bCalledEDID && pChanged && strcmp(backupEDID, stMsgData->paramValue))
        *pChanged = true;
    bCalledEDID = true;
    strncpy(backupEDID, stMsgData->paramValue, _BUF_LEN_256 - 1);
    backupEDID[_BUF_LEN_256 - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] EDID (base64, len=%zu)\n", __FUNCTION__, edidBase64.size());
    return OK;
}

/************************************************************
 * Description  : Get EDID bytes of connected display device  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.DisplaySettings.readEDID
 * Request      : {}  (uses default video port)
 * Response     : { "EDID": "<base64>", "success": true }
 *
 * Returns the base64-encoded EDID string (same as getX_COMCAST_COM_EDID).
 * Note: original returned uppercase hex; Thunder readEDID returns base64.
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → base64 EDID string
************************************************************/
int hostIf_STBServiceDisplayDevice::getEDID_BYTES(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string edidBase64;

    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_READ_EDID, "{}", "EDID", edidBase64))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed\n", __FUNCTION__, THUNDER_DS_READ_EDID);
        return NOK;
    }

    strncpy(stMsgData->paramValue, edidBase64.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = strlen(stMsgData->paramValue);

    if (bCalledEDIDBytes && pChanged && strcmp(backupEDIDBytes, stMsgData->paramValue))
        *pChanged = true;
    bCalledEDIDBytes = true;
    strncpy(backupEDIDBytes, stMsgData->paramValue, _BUF_LEN_256 - 1);
    backupEDIDBytes[_BUF_LEN_256 - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] EDID bytes (base64, len=%zu)\n", __FUNCTION__, edidBase64.size());
    return OK;
}

/************************************************************
 * Description  : Get supported resolutions of the connected display  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.DisplaySettings.getSupportedResolutions
 * Request      : { "videoDisplay": "<portName>" }
 * Response     : { "supportedResolutions": ["720p", "1080i", ...], "success": true }
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → comma-separated resolution list, e.g. "720p,1080i,1080p60"
************************************************************/
int hostIf_STBServiceDisplayDevice::getSupportedResolutions(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    const std::string paramsJson = "{\"videoDisplay\":\"" + m_portName + "\"}";
    std::string resolutionsCsv;

    if (!invokeThunderPluginMethodAndExtractDelimitedStringArrayField(
            THUNDER_DS_GET_SUPPORTED_RESOLUTIONS,
            paramsJson, "supportedResolutions", ",", resolutionsCsv))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s)\n",
                __FUNCTION__, THUNDER_DS_GET_SUPPORTED_RESOLUTIONS, m_portName.c_str());
        return NOK;
    }

    strncpy(stMsgData->paramValue, resolutionsCsv.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = strlen(stMsgData->paramValue);

    if (bCalledSupportedResolution && pChanged && strcmp(backupSupportedResolution, stMsgData->paramValue))
        *pChanged = true;
    bCalledSupportedResolution = true;
    strncpy(backupSupportedResolution, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupSupportedResolution[_BUF_LEN_16 - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] SupportedResolutions: %s (port=%s)\n",
            __FUNCTION__, stMsgData->paramValue, m_portName.c_str());
    return OK;
}

/************************************************************
 * Description  : Get preferred (current) resolution of the display  [MIGRATED to Thunder]
 * Thunder API  : org.rdk.DisplaySettings.getCurrentResolution
 * Request      : { "videoDisplay": "<portName>" }
 * Response     : { "resolution": "<name>", "w": <int>, "h": <int>, "progressive": <bool>,
 *                  "success": true }
 *
 * @return OK on success, NOK on failure.
 * stMsgData->paramValue → resolution string, e.g. "1080p"
************************************************************/
int hostIf_STBServiceDisplayDevice::getPreferredResolution(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    const std::string paramsJson = "{\"videoDisplay\":\"" + m_portName + "\"}";
    std::string resolution;

    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_GET_CURRENT_RESOLUTION,
            paramsJson, "resolution", resolution))
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

    if (bCalledPreferredResolution && pChanged && strcmp(backupPreferredResolution, stMsgData->paramValue))
        *pChanged = true;
    bCalledPreferredResolution = true;
    strncpy(backupPreferredResolution, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupPreferredResolution[_BUF_LEN_16 - 1] = '\0';

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] PreferredResolution: %s (port=%s)\n",
            __FUNCTION__, stMsgData->paramValue, m_portName.c_str());
    return OK;
}

/** @} */
/** @} */
