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
 * @file Device_MoCA_Interface.cpp
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
#ifdef USE_WIFI_PROFILE
/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/
#include "Device_WiFi_Radio.h"

GHashTable* hostIf_WiFi_Radio::ifHash = NULL;

/*hostIf_WiFi_Radio::hostIf_WiFi_Radio(int dev_id):dev_id(dev_id)
{
}*/

hostIf_WiFi_Radio *hostIf_WiFi_Radio::getInstance(int dev_id)
{
    static hostIf_WiFi_Radio* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi_Radio *)g_hash_table_lookup(ifHash, (gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi_Radio(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_WiFi_Radio instance..\n");
        }
        g_hash_table_insert(ifHash,(gpointer)dev_id, pRet);
    }

    return pRet;
}

/*void* hostIf_WiFi_Radio::getContext()
{
    return ctxt;
}*/



GList* hostIf_WiFi_Radio::getAllAssociateDevs()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_WiFi_Radio::closeInstance(hostIf_WiFi_Radio *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
//        if(pDev->ctxt)
        delete pDev;
    }
}

void hostIf_WiFi_Radio::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        GList* current = tmp_list;

        while(current)
        {
            hostIf_WiFi_Radio* pDev = (hostIf_WiFi_Radio *)current->data;
            current = current->next;
            closeInstance(pDev);
        }

        g_list_free(tmp_list);
    }
}

hostIf_WiFi_Radio::hostIf_WiFi_Radio(int dev_id):
    dev_id(dev_id),
    radioFirstExTime(0),
    Enable(false),
    LastChange(0),
    Upstream (false),
    MaxBitRate(0),
    Channel(0),
    AutoChannelSupported(false),
    AutoChannelEnable(false),
    AutoChannelRefreshPeriod (0),
    mcs (0),
    TransmitPower (0),
    IEEE80211hSupported (false),
    IEEE80211hEnabled (false)
{
    memset(Status, 0, sizeof(Status));
    memset(Alias,0,sizeof(Alias));
    memset(Name,0,sizeof(Name));
    memset(LowerLayers,0,sizeof(LowerLayers));
    memset(SupportedFrequencyBands,0,sizeof(SupportedFrequencyBands));
    memset(OperatingFrequencyBand,0,sizeof(OperatingFrequencyBand));
    memset(SupportedStandards, 0, sizeof(SupportedStandards));
    memset(OperatingStandards, 0, sizeof(OperatingStandards));
    memset(PossibleChannels, 0, sizeof(PossibleChannels));
    memset(ChannelsInUse, 0, sizeof(ChannelsInUse));
    memset(OperatingChannelBandwidth, 0, sizeof(OperatingChannelBandwidth));
    memset(ExtensionChannel, 0, sizeof(ExtensionChannel));
    memset(GuardInterval, 0,sizeof(GuardInterval));
    memset(TransmitPowerSupported, 0, sizeof(TransmitPowerSupported));
    memset(RegulatoryDomain, 0, sizeof(RegulatoryDomain));
}

#endif /* #ifdef USE_WIFI_PROFILE */
