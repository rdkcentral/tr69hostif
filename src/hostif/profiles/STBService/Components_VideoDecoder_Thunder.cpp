/*
 * Copyright 2016 RDK Management — Apache-2.0
 * Components_VideoDecoder_Thunder.cpp: Thunder-backed VideoDecoder implementation.
 */
#include "Components_VideoDecoder.h"

#define DEV_NAME "VideoDecoder"
#define BASE_NAME "Device.Services.STBService.1.Components.VideoDecoder"
#define UPDATE_FORMAT_STRING "%s.%d.%s"

#define STATUS_STRING         "Status"
#define CONTENT_AR_STRING     "ContentAspectRatio"
#define COMCAST_STANDBY_STRING "X_COMCAST-COM_Standby"
#define HEVC_STRING           "X_RDKCENTRAL-COM_MPEGHPart2"
#define NAME_STRING           "Name"
#define ENABLE_STRING         "Enable"
#define ENABLED_STRING        "Enabled"
#define DISABLED_STRING       "Disabled"

#define THUNDER_PM_GET_POWER_STATE "org.rdk.PowerManager.GetPowerState"
#define THUNDER_PM_SET_POWER_STATE "org.rdk.PowerManager.SetPowerState"
#define THUNDER_DI_GET_DISPLAY_ASPECT_RATIO "org.rdk.DisplayInfo.getDisplayAspectRatio"

GHashTable * hostIf_STBServiceVideoDecoder::ifHash = NULL;
GMutex hostIf_STBServiceVideoDecoder::m_mutex;

hostIf_STBServiceVideoDecoder* hostIf_STBServiceVideoDecoder::getInstance(int dev_id)
{
    if (!ifHash)
    {
        ifHash = g_hash_table_new(NULL, NULL);
        /* Single instance for the decoder domain */
        hostIf_STBServiceVideoDecoder *pInst = new hostIf_STBServiceVideoDecoder(1);
        g_hash_table_insert(ifHash, (gpointer)(intptr_t)1, pInst);
    }
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

hostIf_STBServiceVideoDecoder::hostIf_STBServiceVideoDecoder(int devid) : dev_id(devid)
{
    strncpy(backupContentAspectRatio, " ", sizeof(backupContentAspectRatio));
    strncpy(backupVideoDecoderStatus, " ", sizeof(backupVideoDecoderStatus));
    backupStandby = false;
    bCalledContentAspectRatio = bCalledStandby = bCalledVideoDecoderStatus = false;
}

int hostIf_STBServiceVideoDecoder::handleSetMsg(const char *pSetting, HOSTIF_MsgData_t *stMsgData)
{
    if (strcasecmp(pSetting, COMCAST_STANDBY_STRING) == 0)
        return setX_COMCAST_COM_Standby(stMsgData);
    return NOT_HANDLED;
}

int hostIf_STBServiceVideoDecoder::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    if (strcasecmp(paramName, COMCAST_STANDBY_STRING) == 0)  return getX_COMCAST_COM_Standby(stMsgData);
    if (strcasecmp(paramName, ENABLE_STRING) == 0)           return getStatus(stMsgData);
    if (strcasecmp(paramName, STATUS_STRING) == 0)           return getStatus(stMsgData);
    if (strcasecmp(paramName, NAME_STRING) == 0) {
        snprintf(stMsgData->paramValue, PARAM_LEN, "VideoDecoder%d", dev_id);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        return OK;
    }
    /* ContentAspectRatio and HEVC: no Thunder equivalent */
    if (strcasecmp(paramName, CONTENT_AR_STRING) == 0 || strcasecmp(paramName, HEVC_STRING) == 0)
        return getContentAspectRatio(stMsgData);
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
    DO_UPD(getX_COMCAST_COM_Standby, COMCAST_STANDBY_STRING)
#undef DO_UPD
}

/* ---- private ---- */

int hostIf_STBServiceVideoDecoder::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string currentState;
    if (!invokeThunderPluginMethodAndExtractStringField(
            THUNDER_PM_GET_POWER_STATE, "{}", "currentState", currentState))
    {
        /* default to Enabled if power state unknown */
        currentState = "POWER_STATE_ON";
    }
    const char *status = (currentState == "POWER_STATE_ON") ? ENABLED_STRING : DISABLED_STRING;
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
            THUNDER_DI_GET_DISPLAY_ASPECT_RATIO, "{}", "aspectRatio", aspectRatio))
    {
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

int hostIf_STBServiceVideoDecoder::getX_COMCAST_COM_Standby(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    std::string currentState;
    invokeThunderPluginMethodAndExtractStringField(THUNDER_PM_GET_POWER_STATE, "{}", "currentState", currentState);
    const bool standby = (currentState.find("STANDBY") != std::string::npos);
    put_boolean(stMsgData->paramValue, standby);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);
    if (bCalledStandby && pChanged && (backupStandby != standby)) *pChanged = true;
    bCalledStandby = true;
    backupStandby = standby;
    return OK;
}

int hostIf_STBServiceVideoDecoder::setX_COMCAST_COM_Standby(const HOSTIF_MsgData_t *stMsgData)
{
    const bool wantStandby = get_boolean(stMsgData->paramValue);
    const std::string state = wantStandby ? "POWER_STATE_STANDBY" : "POWER_STATE_ON";
    const std::string params = std::string("{\"keyCode\":0,\"powerState\":\"") + state + "\",\"reason\":\"STBService\"}";
    std::string response;
    if (!invokeThunderPluginMethod(THUNDER_PM_SET_POWER_STATE, params, response))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder SetPowerState failed\n", __FUNCTION__);
        return NOK;
    }
    return OK;
}
