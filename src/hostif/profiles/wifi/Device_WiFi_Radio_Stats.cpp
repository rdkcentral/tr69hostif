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
#ifdef RDKV_NM
extern "C" {
#include "wifiSrvMgrIarmIf.h"
    /* #include "c_only_header.h"*/
};
#endif

GHashTable* hostIf_WiFi_Radio_Stats::ifHash = NULL;
static time_t radioFirstExTime = 0;

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

static bool getNoise(int &noise_value)
{
    char cmd[50];
    snprintf(cmd, sizeof(cmd), "wpa_cli -i wlan0 signal_poll");

    FILE *fp = popen(cmd, "r");
    if (NULL == fp)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Error in popen() : signal_poll failed \n");
        return false;
    }

    char line[256];
    char noise[64] = { 0 };

    while (fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, "NOISE=", 6) == 0)
        {
            strncpy(noise, line + 6, sizeof(noise) - 1);
            // Strip trailing newline if present
            size_t len = strlen(noise);
            if (len > 0 && noise[len - 1] == '\n')
                noise[len - 1] = '\0';
        }
    }
    pclose(fp);

    if (noise[0] == '\0')
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "NOISE not found in signal_poll.\n");
        return false;
    }

    noise_value = atoi(noise);
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "\n noise = %d ", noise_value);

    return true;
}

struct wifi_radioTrafficStats_t
{
    unsigned long   bytesSent;
    unsigned long   bytesReceived;
    unsigned long   packetsSent;
    unsigned long   packetsReceived;
    unsigned int    errorsSent;
    unsigned int    errorsReceived;
    unsigned int    discardPacketsSent;
    unsigned int    discardPacketsReceived;
};

static int wifi_getRadioTrafficStats(int radioIndex, wifi_radioTrafficStats_t *output_struct)
{
    FILE *fp = NULL;
    char resultBuff[256];
    char cmd[50];
    char interfaceName[10] = "wlan0";
    long long int rx_bytes = 0,rx_packets = 0,rx_err = 0,rx_drop = 0;
    long long int tx_bytes = 0,tx_packets = 0,tx_err = 0,tx_drop = 0;
    int numParams = 0;

    if (!output_struct)
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "output struct is null");
        return NOK;
    }

    memset(resultBuff, 0, sizeof(resultBuff));
    memset(cmd, 0, sizeof(cmd));

    snprintf(cmd, sizeof(cmd), "cat /proc/net/dev | grep %s", interfaceName);

    if (NULL != (fp = popen(cmd, "r")))
    {
        if (fgets(resultBuff, sizeof (resultBuff), fp) != NULL)
        {
            numParams = sscanf(resultBuff, " %[^:]: %lld %lld %lld %lld %*u %*u %*u %*u %lld %lld %lld %lld %*u %*u %*u %*u",
                            interfaceName,
                            &rx_bytes, &rx_packets, &rx_err, &rx_drop,
                            &tx_bytes, &tx_packets, &tx_err, &tx_drop);
            if (numParams != 9)
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Error in parsing Radio Stats params \n");

            output_struct->packetsSent = tx_packets;
            output_struct->packetsReceived = rx_packets;
            output_struct->bytesSent = tx_bytes;
            output_struct->bytesReceived = rx_bytes;
            output_struct->errorsReceived = rx_err;
            output_struct->errorsSent = tx_err;
            output_struct->discardPacketsSent = tx_drop;
            output_struct->discardPacketsReceived = rx_drop;
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
                    "[tx_packets = %lld] [rx_packets =  %lld] "
                    "[tx_bytes = %lld] [rx_bytes = %lld] "
                    "[rx_err = %lld] [tx_err = %lld] "
                    "[tx_drop = %lld] [rx_drop = %lld] \n",
                    tx_packets, rx_packets, tx_bytes, rx_bytes,
                    rx_err, tx_err, tx_drop, rx_drop);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Error in reading /proc/net/dev file \n");
        }
        pclose(fp);
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Error in popen() : Opening /proc/net/dev failed \n");
    }

    return OK;
}

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_Props_Fields(int radioIndex)
{
#ifdef RDKV_NM
    IARM_Result_t retVal = IARM_RESULT_SUCCESS;
    IARM_BUS_WiFi_DiagsPropParam_t param = {0};
    int ret;
    radioIndex=1;

    hostIf_WiFi_Radio_Stats *pDev = hostIf_WiFi_Radio_Stats::getInstance(dev_id);
    if(pDev)
    {
        retVal = IARM_Bus_Call(IARM_BUS_NM_SRV_MGR_NAME, IARM_BUS_WIFI_MGR_API_getRadioStatsProps, (void *)&param, sizeof(param));
        if (IARM_RESULT_SUCCESS != retVal)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] IARM BUS CALL failed with  : %d.\n", __FILE__, __FUNCTION__, retVal);
            return NOK;
        }
        BytesSent = param.data.radio_stats.params.bytesSent;
        BytesReceived = param.data.radio_stats.params.bytesReceived;
        PacketsSent = param.data.radio_stats.params.packetsSent;
        PacketsReceived = param.data.radio_stats.params.packetsReceived;
        ErrorsSent = param.data.radio_stats.params.errorsSent;
        ErrorsReceived = param.data.radio_stats.params.errorsReceived;
        DiscardPacketsSent = param.data.radio_stats.params.discardPacketsSent;
        DiscardPacketsReceived = param.data.radio_stats.params.discardPacketsReceived;
        NoiseFloor = param.data.radio_stats.params.noiseFloor;
        radioFirstExTime = time (NULL);
        return OK;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s]Error!  Unable to connect to wifi instance\n", __FILE__, __FUNCTION__);
        return NOK;
    }
#else
    hostIf_WiFi_Radio_Stats *pDev = hostIf_WiFi_Radio_Stats::getInstance(dev_id);
    if (pDev)
    {
        int noise;
        wifi_radioTrafficStats_t stats = {0};
        wifi_getRadioTrafficStats(0, &stats);

        BytesSent = stats.bytesSent;
        BytesReceived = stats.bytesReceived;
        PacketsSent = stats.packetsSent;
        PacketsReceived = stats.packetsReceived;
        ErrorsSent = stats.errorsSent;
        ErrorsReceived = stats.errorsReceived;
        DiscardPacketsSent = stats.discardPacketsSent;
        DiscardPacketsReceived = stats.discardPacketsReceived;
        NoiseFloor = getNoise(noise) ? noise : 0;

        radioFirstExTime = time (NULL);
        return OK;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s]Error!  Unable to connect to wifi instance\n", __FILE__, __FUNCTION__);
        return NOK;
    }
#endif

}

void hostIf_WiFi_Radio_Stats::checkWifiRadioPropsFetch(int radioIndex)
{
    int ret = NOK;
    time_t currExTime = time (NULL);
    if ((currExTime - radioFirstExTime ) > QUERY_INTERVAL)
    {
        ret = get_Device_WiFi_Radio_Stats_Props_Fields(radioIndex);
        if( OK != ret)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to fetch   : %d.\n", __FILE__, __FUNCTION__, ret);
        }
    }
}

#ifdef RDKV_NM

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_BytesSent(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, BytesSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;

}


int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_BytesReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, BytesReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_PacketsSent(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, PacketsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

#endif

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_PacketsReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, PacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

#ifdef RDKV_NM

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_ErrorsSent(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, ErrorsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_ErrorsReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, ErrorsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_DiscardPacketsSent(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, DiscardPacketsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_DiscardPacketsReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, DiscardPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

#endif

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_NoiseFloor(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, NoiseFloor);
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

#endif /* #ifdef USE_WIFI_PROFILE */
