/*
 * Copyright 2016 RDK Management — Apache-2.0
 * Components_DisplayDevice_Thunder.cpp: Thunder-backed DisplayDevice implementation.
 */
#include <sstream>
#include "Components_DisplayDevice.h"

#define STATUS_STRING        "Status"
#define EEDID_STRING         "EEDID"
#define COMCAST_EDID_STRING  "X_COMCAST-COM_EDID"
#define EDID_BYTES_STRING    "EDID_BYTES"
#define SUPPORTED_RES_STRING "SupportedResolutions"
#define PREF_RES_STRING      "PreferredResolution"

#define THUNDER_DI_DISPLAYINFO "org.rdk.DisplayInfo.displayinfo"
#define THUNDER_DI_CONNECTED   "org.rdk.DisplayInfo.connected"
#define THUNDER_DS_READ_EDID   "org.rdk.DisplaySettings.readEDID"
#define THUNDER_DS_GET_SUPPORTED_SETTOP_RESOLUTIONS "org.rdk.DisplaySettings.getSupportedSettopResolutions"
#define THUNDER_DS_GET_DEFAULT_RESOLUTION "org.rdk.DisplaySettings.getDefaultResolution"
#define THUNDER_DS_GET_CURRENT_RESOLUTION "org.rdk.DisplaySettings.getCurrentResolution"

static const char* normalizeSettopResolution(const std::string& value)
{
    if (value == "480i") return "720x480i/29.97Hz";
    if (value == "480p") return "720x480p/59.94Hz";
    if (value == "576p50") return "720x576p/50Hz";
    if (value == "720p") return "1280x720p/59.94Hz";
    if (value == "720p50") return "1280x720p/50Hz";
    if (value == "768p60") return "1366x768p/59.94Hz";
    if (value == "1080p24") return "1920x1080p/24Hz";
    if (value == "1080p") return "1920x1080p/59.94Hz";
    if (value == "1080i50") return "1920x1080i/50Hz";
    if (value == "1080i") return "1920x1080i/59.94Hz";
    if (value == "2160p30") return "3840x2160p/30Hz";
    if (value == "2160p60") return "3840x2160p/59.94Hz";
    return NULL;
}

static std::string normalizeSettopResolutionList(const std::string& resolutions)
{
    std::string response;
    cJSON* root = cJSON_Parse(resolutions.c_str());
    if (!cJSON_IsArray(root))
    {
        if (root)
            cJSON_Delete(root);
        return resolutions;
    }

    const int count = cJSON_GetArraySize(root);
    for (int i = 0; i < count; ++i)
    {
        cJSON* item = cJSON_GetArrayItem(root, i);
        if (!cJSON_IsString(item) || (item->valuestring == NULL))
            continue;

        const char* mapped = normalizeSettopResolution(item->valuestring);
        if (mapped == NULL)
            mapped = item->valuestring;

        if (!response.empty())
            response += ",";
        response += mapped;
    }

    cJSON_Delete(root);
    return response;
}

static bool getNormalizedResolutionValue(const std::string& method,
                                         const std::string& params,
                                         const char* field,
                                         std::string& normalized)
{
    std::string value;
    if (!invokeThunderPluginMethodAndExtractStringField(method, params, field, value))
        return false;

    const char* mapped = normalizeSettopResolution(value);
    normalized = (mapped != NULL) ? mapped : value;
    return true;
}

hostIf_STBServiceDisplayDevice::hostIf_STBServiceDisplayDevice(int devId, const std::string& portName)
    : dev_id(devId), m_portName(portName)
{
    strncpy(backupDisplayDeviceStatus, " ", sizeof(backupDisplayDeviceStatus));
    strncpy(backupEDID, " ", sizeof(backupEDID));
    strncpy(backupEDIDBytes, " ", sizeof(backupEDIDBytes));
    strncpy(backupSupportedResolution, " ", sizeof(backupSupportedResolution));
    strncpy(backupPreferredResolution, " ", sizeof(backupPreferredResolution));
    bCalledDisplayDeviceStatus = false;
    bCalledEDID = false;
    bCalledEDIDBytes = false;
    bCalledSupportedResolution = false;
    bCalledPreferredResolution = false;
}

int hostIf_STBServiceDisplayDevice::handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    (void)paramName; (void)stMsgData;
    return NOT_HANDLED;
}

int hostIf_STBServiceDisplayDevice::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    if (strcasecmp(paramName, STATUS_STRING) == 0)
        return getStatus(stMsgData);
    if (strcasecmp(paramName, SUPPORTED_RES_STRING) == 0)
        return getSupportedResolutions(stMsgData);
    if (strcasecmp(paramName, PREF_RES_STRING) == 0)
        return getPreferredResolution(stMsgData);
    if (strcasecmp(paramName, EEDID_STRING) == 0 ||
        strcasecmp(paramName, EDID_BYTES_STRING) == 0)
        return getEDID_BYTES(stMsgData);
    if (strcasecmp(paramName, COMCAST_EDID_STRING) == 0)
        return getX_COMCAST_COM_EDID(stMsgData);
    return NOT_HANDLED;
}

void hostIf_STBServiceDisplayDevice::doUpdates(const char *baseName, updateCallback mUpdateCallback)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    char tmp_buff[PARAM_LEN];
    memset(&msgData, 0, sizeof(msgData)); msgData.instanceNum = dev_id;
    bChanged = false;
    getStatus(&msgData, &bChanged);
    if (bChanged) {
        snprintf(tmp_buff, PARAM_LEN, "%s%s", baseName, STATUS_STRING);
        if (mUpdateCallback)
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, tmp_buff, msgData.paramValue, msgData.paramtype);
    }
}

int hostIf_STBServiceDisplayDevice::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool connected = false;
    if (!invokeThunderPluginMethodAndExtractBoolField(THUNDER_DI_CONNECTED, "{}", "connected", connected))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] DisplayInfo.connected failed\n", __FUNCTION__);
        return NOK;
    }
    const char *status = connected ? "Present" : "Absent";
    strncpy(stMsgData->paramValue, status, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(status);
    if (bCalledDisplayDeviceStatus && pChanged && strcmp(backupDisplayDeviceStatus, stMsgData->paramValue))
        *pChanged = true;
    bCalledDisplayDeviceStatus = true;
    strncpy(backupDisplayDeviceStatus, stMsgData->paramValue, sizeof(backupDisplayDeviceStatus) - 1);
    backupDisplayDeviceStatus[sizeof(backupDisplayDeviceStatus) - 1] = '\0';
    return OK;
}

int hostIf_STBServiceDisplayDevice::getSupportedResolutions(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string resolutions;
    const std::string params = std::string("{\"videoDisplay\":\"") + m_portName + "\"}";
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_GET_SUPPORTED_SETTOP_RESOLUTIONS,
            params,
            "supportedSettopResolutions",
            resolutions))
    {
        /* Fallback to displayinfo width/height if settop list is unavailable. */
        int w = 0, h = 0;
        invokeThunderPluginMethodAndExtractNumberField(THUNDER_DI_DISPLAYINFO, "{}", "width", w);
        invokeThunderPluginMethodAndExtractNumberField(THUNDER_DI_DISPLAYINFO, "{}", "height", h);
        if (w > 0 && h > 0)
            snprintf(stMsgData->paramValue, PARAM_LEN, "%dx%d", w, h);
        else
            strncpy(stMsgData->paramValue, "1920x1080", PARAM_LEN);
    }
    else
    {
        const std::string normalized = normalizeSettopResolutionList(resolutions);
        strncpy(stMsgData->paramValue, normalized.c_str(), PARAM_LEN);
    }
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (bCalledSupportedResolution && pChanged && strcmp(backupSupportedResolution, stMsgData->paramValue))
        *pChanged = true;
    bCalledSupportedResolution = true;
    strncpy(backupSupportedResolution, stMsgData->paramValue, sizeof(backupSupportedResolution) - 1);
    backupSupportedResolution[sizeof(backupSupportedResolution) - 1] = '\0';
    return OK;
}

int hostIf_STBServiceDisplayDevice::getPreferredResolution(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string preferred;
    const std::string params = std::string("{\"videoDisplay\":\"") + m_portName + "\"}";

    if (!getNormalizedResolutionValue(THUNDER_DS_GET_DEFAULT_RESOLUTION, params, "defaultResolution", preferred) &&
        !getNormalizedResolutionValue(THUNDER_DS_GET_CURRENT_RESOLUTION, params, "resolution", preferred))
    {
        return getSupportedResolutions(stMsgData, pChanged);
    }

    strncpy(stMsgData->paramValue, preferred.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (bCalledPreferredResolution && pChanged && strcmp(backupPreferredResolution, stMsgData->paramValue))
        *pChanged = true;
    bCalledPreferredResolution = true;
    strncpy(backupPreferredResolution, stMsgData->paramValue, sizeof(backupPreferredResolution) - 1);
    backupPreferredResolution[sizeof(backupPreferredResolution) - 1] = '\0';
    return OK;
}

int hostIf_STBServiceDisplayDevice::getEDID_BYTES(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string edidBytes;
    const std::string params = std::string("{\"videoDisplay\":\"") + m_portName + "\"}";
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_READ_EDID, params, "EDIDBytes", edidBytes))
    {
        if (!invokeThunderPluginMethodAndExtractStringField(THUNDER_DS_READ_EDID, params, "edid", edidBytes))
        {
            RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder readEDID failed for %s\n",
                    __FUNCTION__, m_portName.c_str());
            return NOK;
        }
    }

    strncpy(stMsgData->paramValue, edidBytes.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (bCalledEDIDBytes && pChanged && strcmp(backupEDIDBytes, stMsgData->paramValue))
        *pChanged = true;
    bCalledEDIDBytes = true;
    strncpy(backupEDIDBytes, stMsgData->paramValue, sizeof(backupEDIDBytes) - 1);
    backupEDIDBytes[sizeof(backupEDIDBytes) - 1] = '\0';
    return OK;
}

int hostIf_STBServiceDisplayDevice::getX_COMCAST_COM_EDID(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string productCode;
    const std::string params = std::string("{\"videoDisplay\":\"") + m_portName + "\"}";
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_READ_EDID, params, "productCode", productCode))
    {
        /* Preserve operability by returning EDID blob when productCode is unavailable. */
        return getEDID_BYTES(stMsgData, pChanged);
    }

    strncpy(stMsgData->paramValue, productCode.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (bCalledEDID && pChanged && strcmp(backupEDID, stMsgData->paramValue))
        *pChanged = true;
    bCalledEDID = true;
    strncpy(backupEDID, stMsgData->paramValue, sizeof(backupEDID) - 1);
    backupEDID[sizeof(backupEDID) - 1] = '\0';
    return OK;
}
