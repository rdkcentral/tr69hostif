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
int hostIf_WiFi_SSID::get_Device_WiFi_SSID_Fields(int ssidIndex)
{
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
int hostIf_WiFi_SSID::get_Device_WiFi_SSID_Fields(int ssidIndex)
{
    errno_t rc = -1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    hostIf_WiFi_SSID *pDev = hostIf_WiFi_SSID::getInstance(dev_id);
    if (pDev)
    {
        std::string postData = "{\"jsonrpc\":\"2.0\",\"id\":\"42\",\"method\": \"org.rdk.NetworkManager.GetConnectedSSID\"}";
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Sending GetConnectedSSID request: %s\n", __FUNCTION__, postData.c_str());
        string response = getJsonRPCData(std::move(postData));
        if(response.c_str())
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: curl response string = %s\n", __FUNCTION__, response.c_str());
            cJSON* root = cJSON_Parse(response.c_str());
            if(root)
            {
                cJSON* jsonObj = cJSON_GetObjectItem(root, "result");

                if (jsonObj)
                {
                    cJSON *bssid = cJSON_GetObjectItem(jsonObj, "bssid");
	            cJSON *ssid = cJSON_GetObjectItem(jsonObj, "ssid");
                    
                    if (!bssid || !ssid) {
                        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: NULL pointer - bssid=%p, ssid=%p\n", __FUNCTION__, bssid, ssid);
                        cJSON_Delete(root);
                        return NOK;
                    }
                    
                    //ASSIGN TO OP HERE
                    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: Copying BSSID value: %s\n", __FUNCTION__, bssid->valuestring);
		    rc=strcpy_s(BSSID,sizeof(BSSID),bssid->valuestring);
		    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: BSSID = %s, strcpy_s return code = %d\n", __FUNCTION__, BSSID, rc);
		    if(rc!=EOK)
        	    {
            	        ERR_CHK(rc);
                        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: strcpy_s failed for BSSID with rc=%d\n", __FUNCTION__, rc);
        	    }
                    
                    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: Copying SSID value: %s\n", __FUNCTION__, ssid->valuestring);
		    rc=strcpy_s(SSID,sizeof(SSID),ssid->valuestring);
                    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: SSID = %s, strcpy_s return code = %d\n", __FUNCTION__, SSID, rc);
                    if(rc!=EOK)
                    {
                        ERR_CHK(rc);
                        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: strcpy_s failed for SSID with rc=%d\n", __FUNCTION__, rc);
                    }
		 }
                 else
                 {
                    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] json parse error, no \"result\" in the output from Thunder plugin\n", __FUNCTION__);
                    cJSON_Delete(root);
                    return NOK;
                 }
                 cJSON_Delete(root);
	    }
            else
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: json parse error for GetConnectedSSID, response was: %s\n", __FUNCTION__, response.c_str());
                return NOK;
            }
	}
        else
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: getJsonRPCData() returned empty response for GetConnectedSSID\n", __FUNCTION__);
            return NOK;
        }

        postData = "{\"jsonrpc\":\"2.0\",\"id\":\"42\",\"method\": \"org.rdk.NetworkManager.GetAvailableInterfaces\"}";
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Sending GetAvailableInterfaces request: %s\n", __FUNCTION__, postData.c_str());
        response = getJsonRPCData(postData);
            
        if(response.c_str())
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: GetAvailableInterfaces response = %s\n", __FUNCTION__, response.c_str());
            cJSON* root = cJSON_Parse(response.c_str());
            if(root)
            {
                cJSON* jsonObj    = cJSON_GetObjectItem(root, "result");

                if (jsonObj)
                {
                    cJSON *interfaces = cJSON_GetObjectItem(jsonObj, "interfaces");
                    if (!interfaces) {
                        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: interfaces array is NULL\n", __FUNCTION__);
                        cJSON_Delete(root);
                        return NOK;
                    }
                    
	            cJSON *interface = NULL; 
		    cJSON *interfaceType = NULL;
                    int arraySize = cJSON_GetArraySize(interfaces);
                    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Found %d interfaces\n", __FUNCTION__, arraySize);

		     for (int i = 0; i < arraySize; i++) {
                        interface = cJSON_GetArrayItem(interfaces, i);
                        if (!interface) {
                            RDK_LOG (RDK_LOG_WARN, LOG_TR69HOSTIF, "%s: interface at index %d is NULL\n", __FUNCTION__, i);
                            continue;
                        }
			interfaceType = cJSON_GetObjectItem(interface, "type");
                        if (!interfaceType) {
                            RDK_LOG (RDK_LOG_WARN, LOG_TR69HOSTIF, "%s: interfaceType at index %d is NULL\n", __FUNCTION__, i);
                            continue;
                        }
                        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: Checking interface %d, type=%s\n", __FUNCTION__, i, interfaceType->valuestring);
			if (strcmp(interfaceType->valuestring, "WIFI") == 0) {
			    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Found WiFi Interface at index %d\n", __FUNCTION__, i);
			    break;
			}
		    }
                    
                    if (!interface) {
                        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: WiFi interface not found\n", __FUNCTION__);
                        cJSON_Delete(root);
                        return NOK;
                    }
                    
                    //ASSIGN TO OP HERE
		    cJSON *result = cJSON_GetObjectItem(interface, "mac");
                    if (!result) {
                        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: mac field is NULL\n", __FUNCTION__);
                        cJSON_Delete(root);
                        return NOK;
                    }
                    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: Copying MACAddress value: %s\n", __FUNCTION__, result->valuestring);
		    rc=strcpy_s(MACAddress,sizeof(MACAddress),result->valuestring);
		    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: MACAddress = %s, strcpy_s return code = %d\n", __FUNCTION__, MACAddress, rc);
        	    if(rc!=EOK)
        	    {
            	        ERR_CHK(rc);
                        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: strcpy_s failed for MACAddress with rc=%d\n", __FUNCTION__, rc);
        	    }
		    cJSON *isEnabled = cJSON_GetObjectItem(interface, "enabled");
                    if (!isEnabled) {
                        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: enabled field is NULL\n", __FUNCTION__);
                        cJSON_Delete(root);
                        return NOK;
                    }
		    enable=isEnabled->type;
		    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: ENABLE = %d (cJSON type)\n", __FUNCTION__, enable);
                }
                else
                {
                    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] json parse error, no \"result\" in the output from Thunder plugin\n", __FUNCTION__);
                    cJSON_Delete(root);
                    return NOK;
                }
                cJSON_Delete(root);
             }
             else
             {
                  RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: json parse error for GetAvailableInterfaces, response was: %s\n", __FUNCTION__, response.c_str());
		  return NOK;
             }
	}
        else
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: getJsonRPCData() returned empty response for GetAvailableInterfaces\n", __FUNCTION__);
	    return NOK;
        }
	
        postData = "{\"jsonrpc\":\"2.0\",\"id\":\"42\",\"method\": \"org.rdk.NetworkManager.GetWifiState\"}";
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Sending GetWifiState request: %s\n", __FUNCTION__, postData.c_str());
        response = getJsonRPCData(std::move(postData));

        if(response.c_str())
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: GetWifiState response = %s\n", __FUNCTION__, response.c_str());
            cJSON* root = cJSON_Parse(response.c_str());
            if(root)
            {
                cJSON* jsonObj    = cJSON_GetObjectItem(root, "result");

                if (jsonObj)
                {
	            cJSON *state = cJSON_GetObjectItem(jsonObj, "state");
                    if (!state) {
                        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: state field is NULL\n", __FUNCTION__);
                        cJSON_Delete(root);
                        return NOK;
                    }
		    //ASSIGN TO OP HERE
		    int res = state->valueint;
                    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: WiFi state value = %d\n", __FUNCTION__, res);
		    switch (res) {
			case 0:
                            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: WiFi state = UNINSTALLED\n", __FUNCTION__);
			    rc=strcpy_s(status,sizeof(status),"UNINSTALLED");
			    break;
			case 1:
                            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: WiFi state = DISABLED\n", __FUNCTION__);
			    rc=strcpy_s(status,sizeof(status),"DISABLED");
			    break;
			case 2:
                            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: WiFi state = DISCONNECTED\n", __FUNCTION__);
			    rc=strcpy_s(status,sizeof(status),"DISCONNECTED");
			    break;
			case 3:
                            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: WiFi state = PAIRING\n", __FUNCTION__);
		            rc=strcpy_s(status,sizeof(status),"PAIRING");
			    break;
			case 4:
                            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: WiFi state = CONNECTING\n", __FUNCTION__);
			    rc=strcpy_s(status,sizeof(status),"CONNECTING");
			    break;
			case 5:
                            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: WiFi state = CONNECTED\n", __FUNCTION__);
			    rc=strcpy_s(status,sizeof(status),"CONNECTED");
			    break;
			case 6:
                            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: WiFi state = SSID_NOT_FOUND\n", __FUNCTION__);
			    rc=strcpy_s(status,sizeof(status),"SSID_NOT_FOUND");
			    break;
			case 7:
                            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: WiFi state = SSID_CHANGED\n", __FUNCTION__);
			    rc=strcpy_s(status,sizeof(status),"SSID_CHANGED");
			    break;
			case 8:
                            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: WiFi state = CONNECTION_LOST\n", __FUNCTION__);
			    rc=strcpy_s(status,sizeof(status),"CONNECTION_LOST");
			    break;
			case 9:
                            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: WiFi state = CONNECTION_FAILED\n", __FUNCTION__);
			    rc=strcpy_s(status,sizeof(status),"CONNECTION_FAILED");
			    break;
			case 10:
                            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: WiFi state = CONNECTION_INTERRUPTED\n", __FUNCTION__);
			    rc=strcpy_s(status,sizeof(status),"CONNECTION_INTERRUPTED");
			    break;
			case 11:
                            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: WiFi state = INVALID_CREDENTIALS\n", __FUNCTION__);
			    rc=strcpy_s(status,sizeof(status),"INVALID_CREDENTIALS");
			    break;
			case 12:
                            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: WiFi state = AUTHENTICATION_FAILED\n", __FUNCTION__);
			    rc=strcpy_s(status,sizeof(status),"AUTHENTICATION_FAILED");
			    break;
			case 13:
                            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: WiFi state = ERROR\n", __FUNCTION__);
			    rc=strcpy_s(status,sizeof(status),"ERROR");
			    break;
                        default:
                            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Unknown WiFi state = %d\n", __FUNCTION__, res);
			    rc=strcpy_s(status,sizeof(status),"UNKNOWN");
			    break;
			}
			RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: STATUS = %s, strcpy_s return code = %d\n", __FUNCTION__, status, rc);
			if(rc!=EOK)
			{
			    ERR_CHK(rc);
                            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: strcpy_s failed for status with rc=%d\n", __FUNCTION__, rc);
			}
		}
	  	else
		{
		    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] json parse error, no \"result\" in the output from Thunder plugin\n", __FUNCTION__);
		    cJSON_Delete(root);
		    return NOK;
		}
		cJSON_Delete(root);
	    }
            else
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: json parse error for GetWifiState, response was: %s\n", __FUNCTION__, response.c_str());
                return NOK;
            }
	}
        else
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: getJsonRPCData() returned empty response for GetWifiState\n", __FUNCTION__);
            return NOK;
        }
        
	firstExTime = time (NULL);
        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting with return value OK\n", __FUNCTION__, __FILE__);
        return OK;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s]Error!  Unable to connect to wifi instance\n", __FILE__, __FUNCTION__);
        return NOK;
    }
}
#endif

void hostIf_WiFi_SSID::checkWifiSSIDFetch(int ssidIndex)
{
    int ret = NOK;
    time_t currExTime = time (NULL);
    if ((currExTime - firstExTime ) > QUERY_INTERVAL)
    {
        ret = get_Device_WiFi_SSID_Fields(ssidIndex);
        if( OK != ret)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to fetch   : %d.\n", __FILE__, __FUNCTION__, ret);
        }
    }
}

int hostIf_WiFi_SSID::get_Device_WiFi_SSID_Enable(HOSTIF_MsgData_t *stMsgData )
{
    int ssidIndex=1;
    int ret=OK;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiSSIDFetch(ssidIndex);
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
    checkWifiSSIDFetch(ssidIndex);
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
    checkWifiSSIDFetch(ssidIndex);
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
    checkWifiSSIDFetch(ssidIndex);
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
    checkWifiSSIDFetch(ssidIndex);
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
    checkWifiSSIDFetch(ssidIndex);
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

