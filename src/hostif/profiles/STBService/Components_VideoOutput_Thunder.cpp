/*
 * Copyright 2016 RDK Management — Apache-2.0
 * Components_VideoOutput_Thunder.cpp: Thunder-backed VideoOutput implementation.
 */
#include <sstream>
#include "Components_VideoOutput.h"
#include "safec_lib.h"

#define DEV_NAME "VideoOutput"
#define BASE_NAME "Device.Services.STBService.1.Components.VideoOutput"
#define UPDATE_FORMAT_STRING "%s.%d.%s"

#define STATUS_STRING          "Status"
#define ENABLE_STRING          "Enable"
#define DISPLAY_FORMAT_STRING  "DisplayFormat"
#define VIDEO_FORMAT_STRING    "VideoFormat"
#define AR_BEHAVIOR_STRING     "AspectRatioBehaviour"
#define HDCP_STRING            "HDCP"
#define DISPLAY_NAME_STRING    "Name"
#define ENABLED_STRING         "Enabled"
#define DISABLED_STRING        "Disabled"

#define THUNDER_DS_GET_SUPPORTED_VIDEO_DISPLAYS "org.rdk.DisplaySettings.getSupportedVideoDisplays"
#define THUNDER_DS_GET_CURRENT_RESOLUTION       "org.rdk.DisplaySettings.getCurrentResolution"
#define THUNDER_DS_GET_VIDEO_FORMAT             "org.rdk.DisplaySettings.getVideoFormat"
#define THUNDER_DS_GET_ENABLE_VIDEO_PORT        "org.rdk.DisplaySettings.getEnableVideoPort"
#define THUNDER_AVO_GET_ZOOM_MODE               "org.rdk.AVOutput.getZoomMode"
#define THUNDER_HDCP_GET_STATUS                 "org.rdk.HdcpProfile.getHDCPStatus"
#define THUNDER_DI_CONNECTED                    "org.rdk.DisplayInfo.connected"

GHashTable * hostIf_STBServiceVideoOutput::ifHash = NULL;
GMutex hostIf_STBServiceVideoOutput::m_mutex;

void hostIf_STBServiceVideoOutput::buildPortNameHash()
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
            hostIf_STBServiceVideoOutput *pInst = new hostIf_STBServiceVideoOutput(devId, portName);
            g_hash_table_insert(ifHash, (gpointer)(intptr_t)devId, pInst);
            devId++;
        }
    }
}

hostIf_STBServiceVideoOutput* hostIf_STBServiceVideoOutput::getInstance(int dev_id)
{
    if (!ifHash) buildPortNameHash();
    hostIf_STBServiceVideoOutput *pRet =
        (hostIf_STBServiceVideoOutput*)g_hash_table_lookup(ifHash, (gpointer)(intptr_t)dev_id);
    if (!pRet)
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s:%s:%d]: No instance for dev_id=%d\n",
                __FILE__, __FUNCTION__, __LINE__, dev_id);
    return pRet;
}

GList* hostIf_STBServiceVideoOutput::getAllInstances() { if(ifHash) return g_hash_table_get_keys(ifHash); return NULL; }

void hostIf_STBServiceVideoOutput::closeInstance(hostIf_STBServiceVideoOutput *pDev)
{
    if (pDev) {
        if (ifHash) g_hash_table_remove(ifHash, (gconstpointer)(intptr_t)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBServiceVideoOutput::closeAllInstances()
{
    if (ifHash) {
        GList *tmp = g_hash_table_get_values(ifHash), *cur = tmp;
        while (cur) { delete (hostIf_STBServiceVideoOutput*)cur->data; cur = cur->next; }
        g_list_free(tmp);
        g_hash_table_destroy(ifHash);
        ifHash = NULL;
    }
}

void hostIf_STBServiceVideoOutput::getLock() { g_mutex_init(&m_mutex); g_mutex_lock(&m_mutex); }
void hostIf_STBServiceVideoOutput::releaseLock() { g_mutex_unlock(&m_mutex); }
void hostIf_STBServiceVideoOutput::checkForUpdates(updateCallback) {}

hostIf_STBServiceVideoOutput::hostIf_STBServiceVideoOutput(int devid, const std::string& portName)
    : dev_id(devid), m_portName(portName)
{
    errno_t rc = -1;
    rc = strcpy_s(backupAspectRatioBehaviour, sizeof(backupAspectRatioBehaviour), " "); ERR_CHK(rc);
    rc = strcpy_s(backupDisplayFormat, sizeof(backupDisplayFormat), " "); ERR_CHK(rc);
    rc = strcpy_s(backupDisplayName, sizeof(backupDisplayName), " "); ERR_CHK(rc);
    rc = strcpy_s(backupVideoFormat, sizeof(backupVideoFormat), " "); ERR_CHK(rc);
    rc = strcpy_s(backupVideoOutputStatus, sizeof(backupVideoOutputStatus), " "); ERR_CHK(rc);
    backupHDCP = false;
    bCalledAspectRatioBehaviour = bCalledDisplayFormat = bCalledDisplayName = false;
    bCalledVideoFormat = bCalledHDCP = bCalledVideoOutputStatus = false;
}

int hostIf_STBServiceVideoOutput::handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    (void)paramName; (void)stMsgData;
    return NOT_HANDLED;
}

int hostIf_STBServiceVideoOutput::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    if (strcasecmp(paramName, STATUS_STRING) == 0)       return getStatus(stMsgData);
    if (strcasecmp(paramName, ENABLE_STRING) == 0)       return getEnable(stMsgData);
    if (strcasecmp(paramName, DISPLAY_FORMAT_STRING) == 0) return getDisplayFormat(stMsgData);
    if (strcasecmp(paramName, VIDEO_FORMAT_STRING) == 0) return getVideoFormat(stMsgData);
    if (strcasecmp(paramName, AR_BEHAVIOR_STRING) == 0)  return getAspectRatioBehaviour(stMsgData);
    if (strcasecmp(paramName, HDCP_STRING) == 0)         return getHDCP(stMsgData);
    if (strcasecmp(paramName, DISPLAY_NAME_STRING) == 0) return getName(stMsgData);
    return NOT_HANDLED;
}

void hostIf_STBServiceVideoOutput::doUpdates(updateCallback mUpdateCallback)
{
    HOSTIF_MsgData_t msgData; bool bChanged; char tmp_buff[PARAM_LEN];
#define DO_UPD(fn, param) \
    memset(&msgData,0,sizeof(msgData)); bChanged=false; msgData.instanceNum=dev_id; \
    fn(&msgData,&bChanged); \
    if(bChanged){ snprintf(tmp_buff,PARAM_LEN,UPDATE_FORMAT_STRING,BASE_NAME,dev_id,param); \
    if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff,msgData.paramValue,msgData.paramtype); }

    DO_UPD(getStatus, STATUS_STRING)
    DO_UPD(getDisplayFormat, DISPLAY_FORMAT_STRING)
    DO_UPD(getVideoFormat, VIDEO_FORMAT_STRING)
    DO_UPD(getHDCP, HDCP_STRING)
#undef DO_UPD
}

/* ---- private ---- */

int hostIf_STBServiceVideoOutput::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool connected = false;
    invokeThunderPluginMethodAndExtractBoolField(THUNDER_DI_CONNECTED, "{}", "connected", connected);
    const char *status = connected ? ENABLED_STRING : DISABLED_STRING;
    strncpy(stMsgData->paramValue, status, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(status);
    if (bCalledVideoOutputStatus && pChanged && strcmp(backupVideoOutputStatus, stMsgData->paramValue))
        *pChanged = true;
    bCalledVideoOutputStatus = true;
    strncpy(backupVideoOutputStatus, stMsgData->paramValue, sizeof(backupVideoOutputStatus) - 1);
    backupVideoOutputStatus[sizeof(backupVideoOutputStatus) - 1] = '\0';
    return OK;
}

int hostIf_STBServiceVideoOutput::getEnable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool enabled = false;
    const std::string params = std::string("{\"videoDisplay\":\"") + m_portName + "\"}";
    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DS_GET_ENABLE_VIDEO_PORT, params, "enable", enabled))
    {
        invokeThunderPluginMethodAndExtractBoolField(THUNDER_DI_CONNECTED, "{}", "connected", enabled);
    }
    put_boolean(stMsgData->paramValue, enabled);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);
    if (bCalledVideoOutputStatus && pChanged)
    {
        const char *status = enabled ? ENABLED_STRING : DISABLED_STRING;
        if (strcmp(backupVideoOutputStatus, status))
            *pChanged = true;
    }
    return OK;
}

int hostIf_STBServiceVideoOutput::getDisplayFormat(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string res;
    const std::string params = std::string("{\"videoDisplay\":\"") + m_portName + "\"}";
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_GET_CURRENT_RESOLUTION, params, "resolution", res))
        res = "1280x720p";
    strncpy(stMsgData->paramValue, res.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (bCalledDisplayFormat && pChanged && strcmp(backupDisplayFormat, stMsgData->paramValue))
        *pChanged = true;
    bCalledDisplayFormat = true;
    strncpy(backupDisplayFormat, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupDisplayFormat[_BUF_LEN_16 - 1] = '\0';
    return OK;
}

int hostIf_STBServiceVideoOutput::getVideoFormat(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string fmt;
    const std::string params = std::string("{\"videoDisplay\":\"") + m_portName + "\"}";
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_GET_VIDEO_FORMAT, params, "videoFormat", fmt))
        fmt = "Unknown";
    strncpy(stMsgData->paramValue, fmt.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (bCalledVideoFormat && pChanged && strcmp(backupVideoFormat, stMsgData->paramValue))
        *pChanged = true;
    bCalledVideoFormat = true;
    strncpy(backupVideoFormat, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupVideoFormat[_BUF_LEN_16 - 1] = '\0';
    return OK;
}

int hostIf_STBServiceVideoOutput::getAspectRatioBehaviour(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string mode;
    if (!invokeThunderPluginMethodAndExtractStringField(THUNDER_AVO_GET_ZOOM_MODE, "{}", "zoomMode", mode))
        mode = "None";
    strncpy(stMsgData->paramValue, mode.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (bCalledAspectRatioBehaviour && pChanged && strcmp(backupAspectRatioBehaviour, stMsgData->paramValue))
        *pChanged = true;
    bCalledAspectRatioBehaviour = true;
    strncpy(backupAspectRatioBehaviour, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupAspectRatioBehaviour[_BUF_LEN_16 - 1] = '\0';
    return OK;
}

int hostIf_STBServiceVideoOutput::getHDCP(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool hdcpEnabled = false;
    invokeThunderPluginMethodAndExtractBoolField(THUNDER_HDCP_GET_STATUS, "{}", "isHDCPEnabled", hdcpEnabled);
    put_boolean(stMsgData->paramValue, hdcpEnabled);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);
    if (bCalledHDCP && pChanged && (backupHDCP != hdcpEnabled)) *pChanged = true;
    bCalledHDCP = true;
    backupHDCP = hdcpEnabled;
    return OK;
}

int hostIf_STBServiceVideoOutput::getName(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    (void)pChanged;
    strncpy(stMsgData->paramValue, m_portName.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    return OK;
}
