/*
 * Copyright 2016 RDK Management — Apache-2.0
 * Components_HDMI_Thunder.cpp: Thunder-backed HDMI implementation.
 */
#include <sstream>
#include "Components_HDMI.h"

#define DEV_NAME "HDMI"
#define BASE_NAME "Device.Services.STBService.1.Components.HDMI"
#define UPDATE_FORMAT_STRING "%s.%d.%s"

#define STATUS_STRING     "Status"
#define ENABLE_STRING     "Enable"
#define RES_MODE_STRING   "ResolutionMode"
#define RES_VAL_STRING    "ResolutionValue"
#define NAME_STRING       "Name"
#define ENABLED_STRING    "Enabled"
#define DISABLED_STRING   "Disabled"

#define THUNDER_DS_GET_SUPPORTED_VIDEO_DISPLAYS "org.rdk.DisplaySettings.getSupportedVideoDisplays"
#define THUNDER_DS_GET_CURRENT_RESOLUTION       "org.rdk.DisplaySettings.getCurrentResolution"
#define THUNDER_DS_SET_CURRENT_RESOLUTION       "org.rdk.DisplaySettings.setCurrentResolution"
#define THUNDER_DS_SET_ENABLE_VIDEO_PORT        "org.rdk.DisplaySettings.setEnableVideoPort"
#define THUNDER_DS_GET_ENABLE_VIDEO_PORT        "org.rdk.DisplaySettings.getEnableVideoPort"
#define THUNDER_DS_GET_MUTED                    "org.rdk.DisplaySettings.getMuted"
#define THUNDER_DI_FRAMERATE                    "DisplayInfo.1.framerate"

#define MUTED_STRING      "Muted"

char hostIf_STBServiceHDMI::dsHDMIResolutionMode[10] = HDMI_RESOLUTION_MODE_MANUAL;
GHashTable * hostIf_STBServiceHDMI::ifHash = NULL;
GMutex hostIf_STBServiceHDMI::m_mutex;

void hostIf_STBServiceHDMI::buildPortNameHash()
{
    if (ifHash) g_hash_table_destroy(ifHash);
    ifHash = g_hash_table_new(NULL, NULL);

    std::string delimitedPorts;
    if (!invokeThunderPluginMethodAndExtractDelimitedStringArrayField(
            THUNDER_DS_GET_SUPPORTED_VIDEO_DISPLAYS, "{}", "supportedVideoDisplays", ",", delimitedPorts))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s:%d] Failed to get video displays\n", __FUNCTION__, __LINE__);
        return;
    }

    int devId = 1;
    std::istringstream ss(delimitedPorts);
    std::string portName;
    while (std::getline(ss, portName, ','))
    {
        if (!portName.empty())
        {
            hostIf_STBServiceHDMI *pInst = new hostIf_STBServiceHDMI(devId, portName);
            g_hash_table_insert(ifHash, (gpointer)(intptr_t)devId, pInst);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s:%d] dev_id=%d portName=%s\n",
                    __FUNCTION__, __LINE__, devId, portName.c_str());
            devId++;
        }
    }
}

hostIf_STBServiceHDMI* hostIf_STBServiceHDMI::getInstance(int dev_id)
{
    if (!ifHash) buildPortNameHash();
    hostIf_STBServiceHDMI *pRet =
        (hostIf_STBServiceHDMI*)g_hash_table_lookup(ifHash, (gpointer)(intptr_t)dev_id);
    if (!pRet)
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s:%s:%d]: No instance for dev_id=%d\n",
                __FILE__, __FUNCTION__, __LINE__, dev_id);
    return pRet;
}

GList* hostIf_STBServiceHDMI::getAllInstances()
{
    if (ifHash) return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_STBServiceHDMI::closeInstance(hostIf_STBServiceHDMI *pDev)
{
    if (pDev) {
        if (ifHash) g_hash_table_remove(ifHash, (gconstpointer)(intptr_t)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBServiceHDMI::closeAllInstances()
{
    if (ifHash) {
        GList *tmp = g_hash_table_get_values(ifHash), *cur = tmp;
        while (cur) { delete (hostIf_STBServiceHDMI*)cur->data; cur = cur->next; }
        g_list_free(tmp);
        g_hash_table_destroy(ifHash);
        ifHash = NULL;
    }
}

void hostIf_STBServiceHDMI::getLock() { g_mutex_init(&m_mutex); g_mutex_lock(&m_mutex); }
void hostIf_STBServiceHDMI::releaseLock() { g_mutex_unlock(&m_mutex); }

hostIf_STBServiceHDMI::hostIf_STBServiceHDMI(int devid, const std::string& portName)
    : dev_id(devid), m_portName(portName)
{
    displayDevice = new hostIf_STBServiceDisplayDevice(devid, portName);
    backupEnable = false;
    strncpy(backupStatus, " ", sizeof(backupStatus));
    strncpy(backupResolutionValue, " ", sizeof(backupResolutionValue));
    strncpy(backupName, " ", sizeof(backupName));
    bCalledEnable = false;
    bCalledStatus = false;
    bCalledResolutionValue = false;
    bCalledName = false;
}

hostIf_STBServiceHDMI::~hostIf_STBServiceHDMI()
{
    if (displayDevice) { delete displayDevice; displayDevice = NULL; }
}

void hostIf_STBServiceHDMI::checkForUpdates(updateCallback) {}

int hostIf_STBServiceHDMI::handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    if (strcasecmp(paramName, RES_MODE_STRING) == 0)
        return setHDMIResolutionMode(stMsgData->paramValue);
    if (strcasecmp(paramName, RES_VAL_STRING) == 0) {
        if (0 == strcasecmp(getHDMIResolutionMode(), HDMI_RESOLUTION_MODE_MANUAL))
            return setResolution(stMsgData);
        return OK;
    }
    if (strcasecmp(paramName, ENABLE_STRING) == 0)
        return setEnableVideoPort(stMsgData);
    return NOT_HANDLED;
}

int hostIf_STBServiceHDMI::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    if (strcasecmp(paramName, ENABLE_STRING) == 0)
        return getEnable(stMsgData);
    if (strcasecmp(paramName, STATUS_STRING) == 0)
        return getStatus(stMsgData);
    if (strcasecmp(paramName, NAME_STRING) == 0)
        return getName(stMsgData);
    if (strcasecmp(paramName, RES_MODE_STRING) == 0) {
        strncpy(stMsgData->paramValue, getHDMIResolutionMode(), sizeof(stMsgData->paramValue) - 1);
        stMsgData->paramValue[sizeof(stMsgData->paramValue) - 1] = '\0';
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        return OK;
    }
    if (strcasecmp(paramName, RES_VAL_STRING) == 0)
        return getResolutionValue(stMsgData);
    if (strncasecmp(paramName, DISPLAYDEVICE_OBJECT_NAME, strlen(DISPLAYDEVICE_OBJECT_NAME)) == 0)
        return displayDevice->handleGetMsg(paramName + strlen(DISPLAYDEVICE_OBJECT_NAME), stMsgData);
    return NOT_HANDLED;
}

void hostIf_STBServiceHDMI::doUpdates(updateCallback mUpdateCallback)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    char tmp_buff[PARAM_LEN];

#define DO_UPD(fn, param) \
    memset(&msgData,0,sizeof(msgData)); bChanged=false; msgData.instanceNum=dev_id; \
    fn(&msgData,&bChanged); \
    if(bChanged){ snprintf(tmp_buff,PARAM_LEN,UPDATE_FORMAT_STRING,BASE_NAME,dev_id,param); \
    if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff,msgData.paramValue,msgData.paramtype); }

    DO_UPD(getResolutionValue, RES_VAL_STRING)
    DO_UPD(getStatus, STATUS_STRING)
    DO_UPD(getName, NAME_STRING)
#undef DO_UPD
}

/* ---- private ---- */

int hostIf_STBServiceHDMI::setResolution(const HOSTIF_MsgData_t *stMsgData)
{
    const std::string params = std::string("{\"videoDisplay\":\"") + m_portName
        + "\",\"resolution\":\"" + stMsgData->paramValue + "\"}";
    std::string response;
    if (!invokeThunderPluginMethod(THUNDER_DS_SET_CURRENT_RESOLUTION, params, response))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder setCurrentResolution failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }
    return OK;
}

int hostIf_STBServiceHDMI::getResolutionValue(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    /* Step 1: get w, h, progressive from getCurrentResolution full response */
    const std::string params = std::string("{\"videoDisplay\":\"") + m_portName + "\"}";
    std::string rawResponse;

    if (!invokeThunderPluginMethod(THUNDER_DS_GET_CURRENT_RESOLUTION, params, rawResponse))
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

    /* Step 2: get frame rate.
     * Primary  : query DisplayInfo.1.framerate (THUNDER_DI_FRAMERATE).
     *            Response is {"result":"FramerateXXXX"} where XXXX = framerate * 100.
     *            e.g. "Framerate5994" -> 59.94 Hz, "Framerate6000" -> 60 Hz.
     * Fallback : parse from the "resolution" string in the getCurrentResolution
     *            response (e.g. "2160p60" -> 60, "1080i50" -> 50). */
    double frameRateD = 0.0;
    std::string framerateStr;
    if (invokeThunderPluginMethodAndExtractScalarStringResult(THUNDER_DI_FRAMERATE, "{}", framerateStr))
    {
        /* Parse "FramerateXXXX" -> XXXX / 100.0 */
        const char *p = framerateStr.c_str();
        while (*p && (*p < '0' || *p > '9')) p++;
        if (*p)
            frameRateD = strtol(p, NULL, 10) / 100.0;
    }
    if (frameRateD <= 0.0)
    {
        const char *resField = strstr(rawResponse.c_str(), "\"resolution\":\"");
        if (resField)
        {
            resField += 14; /* advance past "\"resolution\":\"" */
            while (*resField && *resField != '"')
            {
                if ((*resField == 'p' || *resField == 'i') &&
                    (*(resField + 1) >= '0') && (*(resField + 1) <= '9'))
                {
                    frameRateD = (double)strtol(resField + 1, NULL, 10);
                    break;
                }
                resField++;
            }
        }
    }

    /* Step 3: reconstruct full format string matching original libds format.
     * Use decimal notation for non-integer framerates (e.g. 59.94Hz),
     * integer for whole-number framerates (e.g. 60Hz). */
    char resStr[PARAM_LEN];
    if (frameRateD > 0.0)
    {
        long frameRateCentis = (long)(frameRateD * 100.0 + 0.5);
        if (frameRateCentis % 100 == 0)
            snprintf(resStr, sizeof(resStr), "%dx%d%s/%ldHz",
                     w, h, isProgressive ? "p" : "i", frameRateCentis / 100);
        else
            snprintf(resStr, sizeof(resStr), "%dx%d%s/%.2fHz",
                     w, h, isProgressive ? "p" : "i", frameRateD);
    }
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

int hostIf_STBServiceHDMI::setEnableVideoPort(const HOSTIF_MsgData_t *stMsgData)
{
    const bool enable = get_boolean(stMsgData->paramValue);
    const std::string params = std::string("{\"videoDisplay\":\"") + m_portName
        + "\",\"enable\":" + (enable ? "true" : "false") + "}";
    std::string response;
    if (!invokeThunderPluginMethod(THUNDER_DS_SET_ENABLE_VIDEO_PORT, params, response))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder setEnableVideoPort failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }
    return OK;
}

/************************************************************
 * Description  : Get the enabled status for the HDMI port.
 * Precondition : None
 * Input        : stMsgData for result return.

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> true
************************************************************/
int hostIf_STBServiceHDMI::getEnable(HOSTIF_MsgData_t *stMsgData)
{
    put_boolean(stMsgData->paramValue, true);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);
    return OK;
}

/************************************************************
 * Description  : Get Audio Status
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> Disabled: Audio port diabled
                                        -> Enabled: Audio port enabled
                                        -> Muted: Audio port enabled and muted
                                        -> Error: Error
************************************************************/
int hostIf_STBServiceHDMI::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool enabled = false;
    const std::string params = std::string("{\"videoDisplay\":\"") + m_portName + "\"}";
    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DS_GET_ENABLE_VIDEO_PORT, params, "enable", enabled))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder getEnableVideoPort failed for %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s()] dev_id = %d, isEnabled = %s\n",
            __FUNCTION__, dev_id, enabled ? "true" : "false");

    const char *status = DISABLED_STRING;
    if (enabled)
    {
        bool muted = false;
        invokeThunderPluginMethodAndExtractBoolField(THUNDER_DS_GET_MUTED, params, "muted", muted);
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s()] dev_id = %d, isMute = %s\n",
                __FUNCTION__, dev_id, muted ? "true" : "false");
        status = muted ? MUTED_STRING : ENABLED_STRING;
    }
    strncpy(stMsgData->paramValue, status, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(status);
    if (bCalledStatus && pChanged && strcmp(backupStatus, stMsgData->paramValue)) *pChanged = true;
    bCalledStatus = true;
    strncpy(backupStatus, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupStatus[_BUF_LEN_16 - 1] = '\0';
    return OK;
}

int hostIf_STBServiceHDMI::getName(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    snprintf(stMsgData->paramValue, PARAM_LEN, "%s", m_portName.c_str());
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (bCalledName && pChanged && strcmp(backupName, stMsgData->paramValue)) *pChanged = true;
    bCalledName = true;
    strncpy(backupName, stMsgData->paramValue, _BUF_LEN_256 - 1);
    backupName[_BUF_LEN_256 - 1] = '\0';
    return OK;
}

int hostIf_STBServiceHDMI::setHDMIResolutionMode(const char* value)
{
    if ((0 != strcasecmp(value, HDMI_RESOLUTION_MODE_AUTO)) &&
        (0 != strcasecmp(value, HDMI_RESOLUTION_MODE_MANUAL)))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Invalid resolution mode: %s\n", __FUNCTION__, value);
        return NOK;
    }
    strncpy(dsHDMIResolutionMode, value, sizeof(dsHDMIResolutionMode) - 1);
    dsHDMIResolutionMode[sizeof(dsHDMIResolutionMode) - 1] = '\0';
    return OK;
}

const char* hostIf_STBServiceHDMI::getHDMIResolutionMode()
{
    return dsHDMIResolutionMode;
}
