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
#include "Components_DisplayDevice.h"

#define STATUS_STRING        "Status"
#define EEDID_STRING         "EEDID"
#define COMCAST_EDID_STRING  "X_COMCAST-COM_EDID"
#define EDID_BYTES_STRING    "EDID_BYTES"
#define SUPPORTED_RES_STRING "SupportedResolutions"
#define PREF_RES_STRING      "PreferredResolution"

#define THUNDER_DI_CONNECTED   "DisplayInfo.1.connected"
#define THUNDER_DS_READ_EDID   "org.rdk.DisplaySettings.readEDID"
#define THUNDER_DS_GET_SUPPORTED_RESOLUTIONS        "org.rdk.DisplaySettings.getSupportedResolutions"
#define THUNDER_DS_GET_DEFAULT_RESOLUTION           "org.rdk.DisplaySettings.getDefaultResolution"

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
    if (strcasecmp(paramName, EEDID_STRING) == 0)
        return getX_COMCAST_COM_EDID(stMsgData);  /* parsed EDID format, matching libds */
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
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Calling Thunder API: %s\n", __FUNCTION__, THUNDER_DI_CONNECTED);
    if (!invokeThunderPluginMethodAndExtractBoolField(THUNDER_DI_CONNECTED, "{}", "isconnected", connected))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] DisplayInfo.1.connected failed\n", __FUNCTION__);
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

/* Convert DisplaySettings resolution code (e.g. "2160p60") to the full TR-069 format
 * (e.g. "3840x2160p/59.94Hz"), matching the format libds produced. */
static const char *resolveResolutionCode(const char *code, char *buf, size_t bufLen)
{
    static const struct { const char *code; const char *full; } kTable[] = {
        { "480p",    "720x480p/59.94Hz"   },
        { "576i",    "720x576i/50Hz"      },
        { "576p",    "720x576p/50Hz"      },
        { "720p50",  "1280x720p/50Hz"     },
        { "720p",    "1280x720p/59.94Hz"  },
        { "1080i50", "1920x1080i/50Hz"    },
        { "1080i",   "1920x1080i/59.94Hz" },
        { "1080p24", "1920x1080p/23.98Hz" },
        { "1080p25", "1920x1080p/25Hz"    },
        { "1080p30", "1920x1080p/30Hz"    },
        { "1080p50", "1920x1080p/50Hz"    },
        { "1080p60", "1920x1080p/59.94Hz" },
        { "2160p24", "3840x2160p/23.98Hz" },
        { "2160p25", "3840x2160p/25Hz"    },
        { "2160p30", "3840x2160p/30Hz"    },
        { "2160p50", "3840x2160p/50Hz"    },
        { "2160p60", "3840x2160p/59.94Hz" },
        { NULL, NULL }
    };
    for (int i = 0; kTable[i].code; i++)
    {
        if (strcmp(code, kTable[i].code) == 0)
        {
            snprintf(buf, bufLen, "%s", kTable[i].full);
            return buf;
        }
    }
    snprintf(buf, bufLen, "%s", code);  /* unknown code — return as-is */
    return buf;
}

int hostIf_STBServiceDisplayDevice::getSupportedResolutions(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    const std::string params = std::string("{\"videoDisplay\":\"") + m_portName + "\"}";
    std::string resolutionsCsv;

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Calling Thunder API: %s\n", __FUNCTION__, THUNDER_DS_GET_SUPPORTED_RESOLUTIONS);
    if (!invokeThunderPluginMethodAndExtractDelimitedStringArrayField(
            THUNDER_DS_GET_SUPPORTED_RESOLUTIONS,
            params, "supportedResolutions", ",", resolutionsCsv))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed (port=%s)\n",
                __FUNCTION__, THUNDER_DS_GET_SUPPORTED_RESOLUTIONS, m_portName.c_str());
        return NOK;
    }

    /* Convert each raw plugin code to the full format (e.g. "720p" -> "1280x720p/59.94Hz") */
    std::string formatted;
    {
        std::istringstream ss(resolutionsCsv);
        std::string token;
        char fmtBuf[64];
        while (std::getline(ss, token, ','))
        {
            if (token.empty()) continue;
            if (!formatted.empty()) formatted += ',';
            formatted += resolveResolutionCode(token.c_str(), fmtBuf, sizeof(fmtBuf));
        }
    }

    strncpy(stMsgData->paramValue, formatted.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (bCalledSupportedResolution && pChanged && strcmp(backupSupportedResolution, stMsgData->paramValue))
        *pChanged = true;
    bCalledSupportedResolution = true;
    strncpy(backupSupportedResolution, stMsgData->paramValue, sizeof(backupSupportedResolution) - 1);
    backupSupportedResolution[sizeof(backupSupportedResolution) - 1] = '\0';
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] SupportedResolutions: %s (port=%s)\n",
            __FUNCTION__, stMsgData->paramValue, m_portName.c_str());
    return OK;
}

int hostIf_STBServiceDisplayDevice::getPreferredResolution(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string resolution;

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Calling Thunder API: %s\n", __FUNCTION__, THUNDER_DS_GET_DEFAULT_RESOLUTION);
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_GET_DEFAULT_RESOLUTION,
            "{}", "defaultResolution", resolution))
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                "[%s] Thunder %s failed\n",
                __FUNCTION__, THUNDER_DS_GET_DEFAULT_RESOLUTION);
        return NOK;
    }

    /* Convert raw plugin code to full format (e.g. "720p" -> "1280x720p/59.94Hz") */
    char fmtBuf[64];
    resolveResolutionCode(resolution.c_str(), fmtBuf, sizeof(fmtBuf));

    strncpy(stMsgData->paramValue, fmtBuf, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (bCalledPreferredResolution && pChanged && strcmp(backupPreferredResolution, stMsgData->paramValue))
        *pChanged = true;
    bCalledPreferredResolution = true;
    strncpy(backupPreferredResolution, stMsgData->paramValue, sizeof(backupPreferredResolution) - 1);
    backupPreferredResolution[sizeof(backupPreferredResolution) - 1] = '\0';
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] PreferredResolution: %s\n",
            __FUNCTION__, stMsgData->paramValue);
    return OK;
}

int hostIf_STBServiceDisplayDevice::getX_COMCAST_COM_EDID(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    static const char kB64Table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    /* readEDID returns "EDID":""  when display is not connected — treat empty
     * EDID as not-connected, matching the original libds isDisplayConnected() check. */
    std::string edidBase64;
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Calling Thunder API: %s\n", __FUNCTION__, THUNDER_DS_READ_EDID);
    invokeThunderPluginMethodAndExtractStringField(THUNDER_DS_READ_EDID, "{}", "EDID", edidBase64);

    if (edidBase64.empty()) {
        memset(stMsgData->paramValue, '\0', sizeof(stMsgData->paramValue));
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Display not connected or EDID unavailable\n", __FUNCTION__);
    } else {
        /* Base64 decode */
        std::string edid;
        int val = 0, bits = -8;
        for (unsigned char c : edidBase64) {
            if (c == '=') break;
            const char *p = strchr(kB64Table, (char)c);
            if (!p) continue;
            val = (val << 6) + (int)(p - kB64Table);
            bits += 6;
            if (bits >= 0) {
                edid += (char)((val >> bits) & 0xFF);
                bits -= 8;
            }
        }

        if (edid.size() < 18) {
            RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] EDID too short (%zu bytes)\n",
                    __FUNCTION__, edid.size());
            return NOK;
        }

        /* EDID byte layout (standard):
         *   [10-11] productCode     (little-endian uint16)
         *   [12-15] serialNumber    (little-endian uint32)
         *   [16]    manufactureWeek
         *   [17]    manufactureYear offset (+1990) */
        int productCode     = (unsigned char)edid[10] | ((unsigned char)edid[11] << 8);
        int serialNumber    = (unsigned char)edid[12] | ((unsigned char)edid[13] << 8)
                            | ((unsigned char)edid[14] << 16) | ((unsigned char)edid[15] << 24);
        int manufactureWeek = (unsigned char)edid[16];
        int manufactureYear = (unsigned char)edid[17] + 1990;

        snprintf(stMsgData->paramValue, PARAM_LEN,
                 "pcode=0x%x,pserial=0x%x,year=%d,week=%d",
                 productCode, serialNumber, manufactureYear, manufactureWeek);
    }

    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = strlen(stMsgData->paramValue);
    if (bCalledEDID && pChanged && strcmp(backupEDID, stMsgData->paramValue))
        *pChanged = true;
    bCalledEDID = true;
    strncpy(backupEDID, stMsgData->paramValue, sizeof(backupEDID) - 1);
    backupEDID[sizeof(backupEDID) - 1] = '\0';
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,
            "[%s] X_COMCAST_COM_EDID: %s\n", __FUNCTION__, stMsgData->paramValue);
    return OK;
}
