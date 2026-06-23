/*
 * Copyright 2016 RDK Management — Apache-2.0
 * Components_HDMI_Thunder.cpp: Thunder-backed HDMI implementation.
 */
#include <sstream>
#include <cctype>
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
#define THUNDER_DS_GET_ENABLE_VIDEO_PORT        "org.rdk.DisplaySettings.getEnableVideoPort"
#define THUNDER_DS_SET_ENABLE_VIDEO_PORT        "org.rdk.DisplaySettings.setEnableVideoPort"
#define THUNDER_DI_CONNECTED                    "org.rdk.DisplayInfo.connected"

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
    DO_UPD(getEnable, ENABLE_STRING)
    DO_UPD(getStatus, STATUS_STRING)
    DO_UPD(getName, NAME_STRING)
#undef DO_UPD
}

static bool normalizeResolutionFormat(const std::string& input, std::string& normalized)
{
    if (input.empty())
        return false;

    std::string s = input;
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
        s.pop_back();
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front())))
        s.erase(s.begin());

    if (s.empty())
        return false;

    const size_t xPos = s.find('x');
    if (xPos == std::string::npos)
        return false;

    const size_t slashPos = s.find('/');
    if (slashPos != std::string::npos)
    {
        const size_t pPos = s.find('p', xPos + 1);
        const size_t iPos = s.find('i', xPos + 1);
        const bool hasMode = ((pPos != std::string::npos && pPos < slashPos) ||
                              (iPos != std::string::npos && iPos < slashPos));

        if (!hasMode)
            s.insert(slashPos, "p");

        const size_t hzPos = s.find("Hz");
        if (hzPos == std::string::npos)
            s += "Hz";

        normalized = s;
        return true;
    }

    const size_t pPos = s.find('p', xPos + 1);
    const size_t iPos = s.find('i', xPos + 1);
    size_t modePos = std::string::npos;
    char mode = 'p';
    if (pPos != std::string::npos && iPos != std::string::npos)
    {
        modePos = (pPos < iPos) ? pPos : iPos;
        mode = (pPos < iPos) ? 'p' : 'i';
    }
    else if (pPos != std::string::npos)
    {
        modePos = pPos;
        mode = 'p';
    }
    else if (iPos != std::string::npos)
    {
        modePos = iPos;
        mode = 'i';
    }

    if (modePos == std::string::npos)
        return false;

    const std::string prefix = s.substr(0, modePos);
    std::string rate = s.substr(modePos + 1);
    if (rate.empty())
        return false;

    while (!rate.empty() && (rate.front() == '-' || rate.front() == '_' || rate.front() == ' '))
        rate.erase(rate.begin());
    if (rate.empty())
        return false;

    if (rate.size() >= 2 && (rate.substr(rate.size() - 2) == "hz" || rate.substr(rate.size() - 2) == "Hz"))
        rate = rate.substr(0, rate.size() - 2);

    if (rate.empty())
        return false;

    normalized = prefix + mode + "/" + rate + "Hz";
    return true;
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
    std::string res;
    const std::string params = std::string("{\"videoDisplay\":\"") + m_portName + "\"}";
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_DS_GET_CURRENT_RESOLUTION, params, "resolution", res))
    {
        res = HDMI_RESOLUTION_VALUE_DEFAULT;
    }

    std::string normalized;
    if (normalizeResolutionFormat(res, normalized))
        res = normalized;
    strncpy(stMsgData->paramValue, res.c_str(), PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    if (bCalledResolutionValue && pChanged && strcmp(backupResolutionValue, stMsgData->paramValue))
        *pChanged = true;
    bCalledResolutionValue = true;
    strncpy(backupResolutionValue, stMsgData->paramValue, _BUF_LEN_16 - 1);
    backupResolutionValue[_BUF_LEN_16 - 1] = '\0';
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

int hostIf_STBServiceHDMI::getEnable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool enabled = false;
    const std::string params = std::string("{\"videoDisplay\":\"") + m_portName + "\"}";
    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DS_GET_ENABLE_VIDEO_PORT, params, "enable", enabled))
    {
        /* Fallback keeps prior behavior if plugin does not expose enable state. */
        invokeThunderPluginMethodAndExtractBoolField(THUNDER_DI_CONNECTED, "{}", "connected", enabled);
    }
    put_boolean(stMsgData->paramValue, enabled);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);
    if (bCalledEnable && pChanged && (backupEnable != enabled)) *pChanged = true;
    bCalledEnable = true;
    backupEnable = enabled;
    return OK;
}

int hostIf_STBServiceHDMI::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool connected = false;
    invokeThunderPluginMethodAndExtractBoolField(THUNDER_DI_CONNECTED, "{}", "connected", connected);
    const char *status = connected ? ENABLED_STRING : DISABLED_STRING;
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
    snprintf(stMsgData->paramValue, PARAM_LEN, "HDMI%d", stMsgData->instanceNum - 1);
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
