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
 * @file Device_WiFi_Radio_Stats.cpp
 *
 * @brief Device.WiFi.Radio.Stats API Implementation.
 *
 * This is the implementation of the WiFi API.
 *
 * @par Document
 */
/** @addtogroup TR-069 WiFi Implementation
 *  This is the implementation of the Device Public API.
 *  @{
 */
#ifdef USE_WIFI_PROFILE
/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/
#include "Device_WiFi_Radio_Stats.h"


GHashTable* hostIf_WiFi_Radio_Stats::ifHash = NULL;

hostIf_WiFi_Radio_Stats *hostIf_WiFi_Radio_Stats::getInstance(int dev_id)
{
    static hostIf_WiFi_Radio_Stats* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi_Radio_Stats *)g_hash_table_lookup(ifHash, (gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi_Radio_Stats(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_WiFi_Radio_Stats instance..\n");
        }
        g_hash_table_insert(ifHash,(gpointer)dev_id, pRet);
    }
    return pRet;
}

void hostIf_WiFi_Radio_Stats::closeInstance(hostIf_WiFi_Radio_Stats *pDev)
{
    if(pDev)
    {
//        g_hash_table_remove(devHash, (gconstpointer)pDev->dev_id);
//        if(pDev->ctxt)
        delete pDev;
    }
}

void hostIf_WiFi_Radio_Stats::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        GList* current = tmp_list;

        while(current)
        {
            hostIf_WiFi_Radio_Stats* pDev = (hostIf_WiFi_Radio_Stats *)current->data;
            current = current->next;
            closeInstance(pDev);
        }

        g_list_free(tmp_list);
    }
}


hostIf_WiFi_Radio_Stats::hostIf_WiFi_Radio_Stats(int dev_id):
    dev_id(dev_id),
    BytesSent(0),
    BytesReceived(0),
    PacketsSent(0),
    PacketsReceived(0),
    ErrorsSent(0),
    ErrorsReceived(0),
    DiscardPacketsSent(0),
    DiscardPacketsReceived(0),
    NoiseFloor(0)
{

}

#endif /* #ifdef USE_WIFI_PROFILE */
