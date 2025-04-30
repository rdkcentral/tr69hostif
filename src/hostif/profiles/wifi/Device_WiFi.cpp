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


#ifdef USE_WIFI_PROFILE

/**
 * @file Device_WiFi.c
 *
 * @brief MoCA_Interface API Implementation.
 *
 * This is the implementation of the MoCA_Interface API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */

/** @addtogroup MoCA_Interface Implementation
 *  This is the implementation of the Device Public API.
 *  @{
 */

/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/
#include "Device_WiFi.h"
#include <string.h>
#include <math.h>
#include <curl/curl.h>
#include "cJSON.h"
#include "hostIf_utils.h"

//char *moca_interface = NULL;
GHashTable* WiFiDevice::devHash = NULL;
GHashTable* hostIf_WiFi::ifHash = NULL;

WiFiDevice::WiFiDevice(int dev_id):dev_id(dev_id)
{
//    ctxt = WiFiCtl_Open(interface);

    if(!ctxt)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error!  Unable to connect to WiFi Device instance %d\n",dev_id);
        throw 1;
    }
}

WiFiDevice* WiFiDevice::getInstance(int dev_id)
{
    WiFiDevice* pRet = NULL;

    if(devHash)
    {
        pRet = (WiFiDevice *)g_hash_table_lookup(devHash, (gpointer) dev_id);
    }
    else
    {
        devHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new WiFiDevice(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create WiFi device instance..\n");
        }
        g_hash_table_insert(devHash,(gpointer)dev_id, pRet);
    }
    return pRet;
}
void* WiFiDevice::getContext()
{
    return ctxt;
}

void WiFiDevice::closeInstance(WiFiDevice *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(devHash, (gconstpointer)pDev->dev_id);
        if(pDev->ctxt)
        {
//           WiFiCtl_Close(pDev->ctxt);
        }
        delete pDev;
    }
}

void WiFiDevice::closeAllInstances()
{
    if(devHash)
    {
        GList* tmp_list = g_hash_table_get_values (devHash);

        GList* current = tmp_list;

        while(current)
        {
            WiFiDevice* pDev = (WiFiDevice *)current->data;
            current = current->next;
            closeInstance(pDev);
        }

        g_list_free(tmp_list);
    }
}

//------------------------------------------------------------------------------
// init: Perform the necessary operations to initialise the WiFi device.
//       Returns 0 on success, -1 on failure.
//------------------------------------------------------------------------------
int WiFiDevice::init()
{
    /* Initialise the WiFi HAL */
    /*   int ret = wifi_init();

       if (ret != 0)
       {
          RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error! Unable to initialise WiFi HAL\n");
          throw 1;
       }*/
    return 1;
}

//------------------------------------------------------------------------------
// shutdown: Perform the necessary operations to shut down the WiFi device.
//------------------------------------------------------------------------------
void WiFiDevice::shutdown()
{
    /* Shut down the WiFi HAL */
//   (void)wifi_down();
}

hostIf_WiFi::hostIf_WiFi(int dev_id):
    dev_id(dev_id),
    uiRadioNumberOfEntries(0),
    uiSSIDNumberOfEntries(0),
    uiAccessPointNumberOfEntries(0),
    uiEndPointNumberOfEntries(0)  //CID:103645 - UNINIT_CTOR
{

}


hostIf_WiFi* hostIf_WiFi::getInstance(int dev_id)
{
    hostIf_WiFi* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create Wifi Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }

    return pRet;
}

GList* hostIf_WiFi::getAllIntefaces()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_WiFi::closeInstance(hostIf_WiFi *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_WiFi::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        GList* current = tmp_list;

        while(current)
        {
            hostIf_WiFi* pDev = (hostIf_WiFi *)current->data;
            current = current->next;
            closeInstance(pDev);
        }

        g_list_free(tmp_list);
    }
}

/****************************************************************************************************************************************************/
// Device.WiFi. Profile. Getters:
/****************************************************************************************************************************************************/

int hostIf_WiFi::get_Device_WiFi_AccessPointNumberOfEntries(HOSTIF_MsgData_t *stMsgData)
{
    unsigned int accessPointNumOfEntries = 1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    put_int(stMsgData->paramValue, accessPointNumOfEntries);

    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof (unsigned int);

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);

    return OK;
}

int hostIf_WiFi::get_Device_WiFi_EndPointNumberOfEntries(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    unsigned int endPointNumOfEntries = 1;
    put_int(stMsgData->paramValue, endPointNumOfEntries);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof (unsigned int);

    return OK;
}

int hostIf_WiFi::get_Device_WiFi_EnableWiFi(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    std::string postData = "{\"jsonrpc\":\"2.0\",\"id\":\"42\",\"method\": \"org.rdk.NetworkManager.GetAvailableInterfaces\"}";
    string response = getJsonRPCData(std::move(postData));

    if(response.c_str())
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: curl response string = %s\n", __FUNCTION__, response.c_str());
        cJSON* root = cJSON_Parse(response.c_str());
        if(root)
        {
            cJSON* jsonObj    = cJSON_GetObjectItem(root, "result");

            if (jsonObj)
            {
                cJSON *interfaces = cJSON_GetObjectItem(jsonObj, "interfaces");
	        cJSON *interface = nullptr, *interfaceType;
	        for (int i = 0; i < cJSON_GetArraySize(interfaces); i++) {
                    interface = cJSON_GetArrayItem(interfaces, i);
	            interfaceType = cJSON_GetObjectItem(interface, "type");
	            if (strcmp(interfaceType->valuestring, "WIFI") == 0)
	                break;
	        }

                //ASSIGN TO OP HERE
		cJSON *result = cJSON_GetObjectItem(interface, "isEnabled");
		put_boolean(stMsgData->paramValue, result->type);
                stMsgData->paramtype = hostIf_BooleanType;
                stMsgData->paramLen=1;
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
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: json parse error\n", __FUNCTION__);
	    return NOK;
        }
    }
    else
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: getJsonRPCData() failed\n", __FUNCTION__);
	return NOK;
    }
    return OK;
}

int hostIf_WiFi::set_Device_WiFi_EnableWiFi(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    if (stMsgData->paramtype != hostIf_BooleanType)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Failed due to wrong data type for %s, please use boolean(0/1) to set.\n", __FUNCTION__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterType;
        return NOK;
    }

    std::string postData;
    if(get_boolean(stMsgData->paramValue)) {
        postData = "{\"jsonrpc\":\"2.0\",\"id\":\"42\",\"method\": \"org.rdk.NetworkManager.EnableInterface\", \"params\" : { \"type\" : \"WIFI\"}}";
    }
    else {
        postData = "{\"jsonrpc\":\"2.0\",\"id\":\"42\",\"method\": \"org.rdk.NetworkManager.DisableInterface\", \"params\" : { \"type\" : \"WIFI\"}}";
    }  

    string response = getJsonRPCData(std::move(postData));
    if(response.c_str())
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: curl response string = %s\n", __FUNCTION__, response.c_str());
	cJSON* root = cJSON_Parse(response.c_str());
        if(root)
        {
            cJSON* jsonObj    = cJSON_GetObjectItem(root, "result");

            if (jsonObj)
            {
	        cJSON *CheckResultObj = cJSON_GetObjectItem(jsonObj, "success");
	        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Result of Set operation = %s\n", __FUNCTION__, cJSON_IsTrue(CheckResultObj) ? "true" : "false");
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
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: json parse error\n", __FUNCTION__);
	    return NOK;
        }
    }
    else
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: getJsonRPCData() failed\n", __FUNCTION__);
        return NOK;
    }
    return OK;
}

#endif
/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */
