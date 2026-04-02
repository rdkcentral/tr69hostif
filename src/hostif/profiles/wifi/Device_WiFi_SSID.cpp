/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2018 RDK Management
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
 * @file Device_WiFi_SSID.cpp
 *
 * @brief Device_WiFi_SSID API Implementation.
 *
 * This is the implementation of the WiFi API.
 *
 * @par Document
 */
/** @addtogroup TR-069 WiFi Implementation
 *  This is the implementation of the Device Public API.
 *  @{
 */

/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/
#include <cstddef>
#include "safec_lib.h"
#ifdef USE_WIFI_PROFILE

#include "Device_WiFi_SSID.h"
#ifdef RDKV_NM
extern "C" {
    /* #include "c_only_header.h"*/
#include "wifi_common_hal.h"
#include "wifiSrvMgrIarmIf.h"
};
#else
#include <curl/curl.h>
#include "cJSON.h"
#include "hostIf_utils.h"
#endif

static time_t firstExTime = 0;
enum WiFiSSIDFetchMask {
    WIFI_SSID_FETCH_CONNECTED_SSID = 1 << 0,
    WIFI_SSID_FETCH_AVAILABLE_INTERFACES = 1 << 1,
    WIFI_SSID_FETCH_WIFI_STATE = 1 << 2
};

#ifndef RDKV_NM
static time_t connectedSsidFetchTime = 0;
static time_t availableInterfacesFetchTime = 0;
static time_t wifiStateFetchTime = 0;
#endif

GHashTable* hostIf_WiFi_SSID::ifHash = NULL;

hostIf_WiFi_SSID* hostIf_WiFi_SSID::getInstance(int dev_id)
{
    static hostIf_WiFi_SSID* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi_SSID *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi_SSID(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_WiFi_SSID instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_WiFi_SSID::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_WiFi_SSID::closeInstance(hostIf_WiFi_SSID *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_WiFi_SSID::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        GList* current = tmp_list;

        while(current)
        {
            hostIf_WiFi_SSID* pDev = (hostIf_WiFi_SSID *)current->data;
            current = current->next;
            closeInstance(pDev);
        }

        g_list_free(tmp_list);
    }
}



hostIf_WiFi_SSID::hostIf_WiFi_SSID(int dev_id):
    enable(false),
    LastChange(0)
{
    memset(status,0 , sizeof(status));  //CID:103996 - OVERRUN
    memset(alias, 0, sizeof(alias));  //CID:103531 - OVERRUN
    memset(name,0, sizeof(name));
    memset(LowerLayers,0, sizeof(LowerLayers));
    memset(BSSID,0, sizeof(BSSID));
    memset(MACAddress,0, sizeof(MACAddress));
    memset(SSID,0, sizeof(SSID));  //CID:103108 - OVERRUN
}
#ifdef RDKV_NM
int hostIf_WiFi_SSID::get_Device_WiFi_SSID_Fields(int ssidIndex, unsigned int fetchMask)
{
    (void)fetchMask;
    errno_t rc = -1;
    IARM_Result_t retVal = IARM_RESULT_SUCCESS;
    IARM_BUS_WiFi_DiagsPropParam_t param = {0};
    int ret;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    hostIf_WiFi_SSID *pDev = hostIf_WiFi_SSID::getInstance(dev_id);
    if (pDev)
    {
        retVal = IARM_Bus_Call(IARM_BUS_NM_SRV_MGR_NAME, IARM_BUS_WIFI_MGR_API_getSSIDProps, (void *)&param, sizeof(param));
        if (IARM_RESULT_SUCCESS != retVal)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] IARM BUS CALL failed with  : %d.\n", __FILE__, __FUNCTION__, retVal);
            return NOK;
        }
        rc=strcpy_s(name,sizeof(name),param.data.ssid.params.name);
	if(rc!=EOK)
    	{
	    ERR_CHK(rc);
    	}
        rc=strcpy_s(BSSID,sizeof(BSSID),param.data.ssid.params.bssid);
	if(rc!=EOK)
        {
            ERR_CHK(rc);
        }
        rc=strcpy_s(MACAddress,sizeof(MACAddress),param.data.ssid.params.macaddr);
	if(rc!=EOK)
    	{
	    ERR_CHK(rc);
    	}
        rc=strcpy_s(SSID,sizeof(SSID),param.data.ssid.params.ssid);
	if(rc!=EOK)
        {
            ERR_CHK(rc);
        }
        rc=strcpy_s(status,sizeof(status),param.data.ssid.params.status);
	if(rc!=EOK)
        {
            ERR_CHK(rc);
        }
        enable=param.data.ssid.params.enable;
        firstExTime = time (NULL);
        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
        return OK;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s]Error!  Unable to connect to wifi instance\n", __FILE__, __FUNCTION__);
        return NOK;
    }
}
#else
int hostIf_WiFi_SSID::get_Device_WiFi_SSID_Fields(int ssidIndex, unsigned int fetchMask)
{
    errno_t rc = -1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    hostIf_WiFi_SSID *pDev = hostIf_WiFi_SSID::getInstance(dev_id);
    if (pDev)
    {
        if ((fetchMask & WIFI_SSID_FETCH_CONNECTED_SSID) != 0)
        {
            std::string ssidResponse;
            if (!invokeThunderPluginMethod("org.rdk.NetworkManager.GetConnectedSSID", "", ssidResponse))
            {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: failed to invoke NetworkManager.GetConnectedSSID\n", __FUNCTION__);
                return NOK;
            }

            std::string bssid;
            if (!thunderExtractResultStringField(ssidResponse, "bssid", bssid))
            {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: failed to fetch bssid from NetworkManager.GetConnectedSSID\n", __FUNCTION__);
                return NOK;
            }

            std::string ssid;
            if (!thunderExtractResultStringField(ssidResponse, "ssid", ssid))
            {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: failed to fetch ssid from NetworkManager.GetConnectedSSID\n", __FUNCTION__);
                return NOK;
            }

            //ASSIGN TO OP HERE
            rc=strcpy_s(BSSID,sizeof(BSSID),bssid.c_str());
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: BSSID = %s \n", __FUNCTION__, BSSID);
            if(rc!=EOK)
            {
                ERR_CHK(rc);
            }
            rc=strcpy_s(SSID,sizeof(SSID),ssid.c_str());
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: SSID = %s \n", __FUNCTION__, SSID);
            if(rc!=EOK)
            {
                ERR_CHK(rc);
            }
            rc = strcpy_s(name, sizeof(name), ssid.c_str());
            if (rc != EOK)
            {
                ERR_CHK(rc);
            }
        }

        if ((fetchMask & WIFI_SSID_FETCH_AVAILABLE_INTERFACES) != 0)
        {
            std::string response;
            if (invokeThunderPluginMethod("org.rdk.NetworkManager.GetAvailableInterfaces", "", response))
            {
                std::string macAddressValue;
                if (readThunderArrayItemByKey(response, "interfaces", "type", "WIFI", "mac", macAddressValue))
                {
                     RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Found WiFi Interface\n", __FUNCTION__);
                }
                else
                {
                    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: WIFI interface not found\n", __FUNCTION__);
                    return NOK;
                }

                rc = strcpy_s(MACAddress, sizeof(MACAddress), macAddressValue.c_str());
                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: MACAddress = %s \n", __FUNCTION__, MACAddress);
                if (rc != EOK)
                {
                    ERR_CHK(rc);
                }

                if (!readThunderArrayItemByKey(response, "interfaces", "type", "WIFI", "enabled", enable))
                {
                    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Invalid or missing enabled for WIFI interface\n", __FUNCTION__);
                    return NOK;
                }

                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: ENABLE = %d \n", __FUNCTION__, enable);
            }
            else
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: failed to fetch interfaces from NetworkManager.GetAvailableInterfaces\n", __FUNCTION__);
                return NOK;
            }
        }

        if ((fetchMask & WIFI_SSID_FETCH_WIFI_STATE) != 0)
        {
            int res = 0;
            if (!invokeThunderPluginMethodAndExtractNumberField("org.rdk.NetworkManager.GetWifiState", "", "state", res))
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: failed to fetch state from NetworkManager.GetWifiState\n", __FUNCTION__);
                return NOK;
            }

            switch (res) {
                case 0:
                    rc=strcpy_s(status,sizeof(status),"UNINSTALLED");
                    break;
                case 1:
                    rc=strcpy_s(status,sizeof(status),"DISABLED");
                    break;
                case 2:
                    rc=strcpy_s(status,sizeof(status),"DISCONNECTED");
                    break;
                case 3:
                    rc=strcpy_s(status,sizeof(status),"PAIRING");
                    break;
                case 4:
                    rc=strcpy_s(status,sizeof(status),"CONNECTING");
                    break;
                case 5:
                    rc=strcpy_s(status,sizeof(status),"CONNECTED");
                    break;
                case 6:
                    rc=strcpy_s(status,sizeof(status),"SSID_NOT_FOUND");
                    break;
                case 7:
                    rc=strcpy_s(status,sizeof(status),"SSID_CHANGED");
                    break;
                case 8:
                    rc=strcpy_s(status,sizeof(status),"CONNECTION_LOST");
                    break;
                case 9:
                    rc=strcpy_s(status,sizeof(status),"CONNECTION_FAILED");
                    break;
                case 10:
                    rc=strcpy_s(status,sizeof(status),"CONNECTION_INTERRUPTED");
                    break;
                case 11:
                    rc=strcpy_s(status,sizeof(status),"INVALID_CREDENTIALS");
                    break;
                case 12:
                    rc=strcpy_s(status,sizeof(status),"AUTHENTICATION_FAILED");
                    break;
                case 13:
                    rc=strcpy_s(status,sizeof(status),"ERROR");
                    break;
            }
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: STATUS = %s \n", __FUNCTION__, status);
            if(rc!=EOK)
            {
                ERR_CHK(rc);
            }
        }
        
	firstExTime = time (NULL);
        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
        return OK;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s]Error!  Unable to connect to wifi instance\n", __FILE__, __FUNCTION__);
        return NOK;
    }
}
#endif

void hostIf_WiFi_SSID::checkWifiSSIDFetch(int ssidIndex, unsigned int fetchMask)
{
    int ret = NOK;
    time_t currExTime = time (NULL);
#ifdef RDKV_NM
    if ((currExTime - firstExTime ) > QUERY_INTERVAL)
    {
        ret = get_Device_WiFi_SSID_Fields(ssidIndex, fetchMask);
        if( OK != ret)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to fetch   : %d.\n", __FILE__, __FUNCTION__, ret);
        }
    }
#else
    unsigned int refreshMask = 0;

    if (((fetchMask & WIFI_SSID_FETCH_CONNECTED_SSID) != 0) && ((currExTime - connectedSsidFetchTime) > QUERY_INTERVAL))
    {
        refreshMask |= WIFI_SSID_FETCH_CONNECTED_SSID;
    }

    if (((fetchMask & WIFI_SSID_FETCH_AVAILABLE_INTERFACES) != 0) && ((currExTime - availableInterfacesFetchTime) > QUERY_INTERVAL))
    {
        refreshMask |= WIFI_SSID_FETCH_AVAILABLE_INTERFACES;
    }

    if (((fetchMask & WIFI_SSID_FETCH_WIFI_STATE) != 0) && ((currExTime - wifiStateFetchTime) > QUERY_INTERVAL))
    {
        refreshMask |= WIFI_SSID_FETCH_WIFI_STATE;
    }

    if (refreshMask != 0)
    {
        ret = get_Device_WiFi_SSID_Fields(ssidIndex, refreshMask);
        if( OK != ret)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to fetch   : %d.\n", __FILE__, __FUNCTION__, ret);
        }
        else
        {
            if ((refreshMask & WIFI_SSID_FETCH_CONNECTED_SSID) != 0)
            {
                connectedSsidFetchTime = currExTime;
            }

            if ((refreshMask & WIFI_SSID_FETCH_AVAILABLE_INTERFACES) != 0)
            {
                availableInterfacesFetchTime = currExTime;
            }

            if ((refreshMask & WIFI_SSID_FETCH_WIFI_STATE) != 0)
            {
                wifiStateFetchTime = currExTime;
            }
        }
    }
#endif
}

int hostIf_WiFi_SSID::get_Device_WiFi_SSID_Enable(HOSTIF_MsgData_t *stMsgData )
{
    int ssidIndex=1;
    int ret=OK;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiSSIDFetch(ssidIndex, WIFI_SSID_FETCH_AVAILABLE_INTERFACES);
    put_boolean(stMsgData->paramValue, enable);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);

    return ret;
}
int hostIf_WiFi_SSID::set_Device_WiFi_SSID_Enable(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_SSID::get_Device_WiFi_SSID_Status(HOSTIF_MsgData_t *stMsgData )
{
    int             ret = OK;
    int ssidIndex=1;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiSSIDFetch(ssidIndex, WIFI_SSID_FETCH_WIFI_STATE);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(status);
    snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN, status);

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);

    return ret;
}

int hostIf_WiFi_SSID::get_Device_WiFi_SSID_Alias(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID::set_Device_WiFi_SSID_Alias(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_SSID::get_Device_WiFi_SSID_Name(HOSTIF_MsgData_t *stMsgData )
{
    int ssidIndex=1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiSSIDFetch(ssidIndex, WIFI_SSID_FETCH_CONNECTED_SSID);
    snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN, name);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(name);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_SSID::get_Device_WiFi_SSID_LastChange(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_SSID::get_Device_WiFi_SSID_LowerLayers(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_SSID::set_Device_WiFi_SSID_LowerLayers(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_SSID::hostIf_WiFi_SSID::get_Device_WiFi_SSID_BSSID(HOSTIF_MsgData_t *stMsgData )
{

    int ssidIndex=1;
    int ret=OK;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiSSIDFetch(ssidIndex, WIFI_SSID_FETCH_CONNECTED_SSID);
    snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN, BSSID);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(BSSID);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);

    return ret;
}

int hostIf_WiFi_SSID::hostIf_WiFi_SSID::get_Device_WiFi_SSID_MACAddress(HOSTIF_MsgData_t *stMsgData )
{
    int ssidIndex=1;
    int ret=OK;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiSSIDFetch(ssidIndex, WIFI_SSID_FETCH_AVAILABLE_INTERFACES);
    snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN, MACAddress);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(MACAddress);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return ret;
}

int hostIf_WiFi_SSID::get_Device_WiFi_SSID_SSID(HOSTIF_MsgData_t *stMsgData )
{
    int ssidIndex=1;
    int ret=OK;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiSSIDFetch(ssidIndex, WIFI_SSID_FETCH_CONNECTED_SSID);
    snprintf(stMsgData->paramValue,TR69HOSTIFMGR_MAX_PARAM_LEN, SSID);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(SSID);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);

    return ret;
}
int hostIf_WiFi_SSID::set_Device_WiFi_SSID_SSID(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

#endif /* #ifdef USE_WIFI_PROFILE */

