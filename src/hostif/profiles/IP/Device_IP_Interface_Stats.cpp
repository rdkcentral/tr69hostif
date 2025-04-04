/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
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
 * @file Device_IP_Interface_Statss.cpp
 * @brief This source file contains the APIs of device IPv4 interface stats.
 */

/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/



/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "Device_IP_Interface_Stats.h"
#include "Device_IP.h"


IPInterfaceStats hostIf_IPInterfaceStats::curntIpStat = {0,};

GMutex hostIf_IPInterfaceStats::m_mutex;

GHashTable *hostIf_IPInterfaceStats::ifHash = NULL;


void hostIf_IPInterfaceStats::refreshInterfaceName ()
{
    nameOfInterface[0] = 0;
    if (NULL == hostIf_IP::getInterfaceName (dev_id, nameOfInterface))
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: error getting interface name for Device.IP.Interface.%d\n", __FUNCTION__, dev_id);
}

hostIf_IPInterfaceStats* hostIf_IPInterfaceStats::getInstance(int dev_id)
{
    hostIf_IPInterfaceStats* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_IPInterfaceStats *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_IPInterfaceStats(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_IPInterfaceStats instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }

    // make sure returned instance has interface name set
    if (pRet)
        pRet->refreshInterfaceName ();

    return pRet;
}

GList* hostIf_IPInterfaceStats::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_IPInterfaceStats::closeInstance(hostIf_IPInterfaceStats *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_IPInterfaceStats::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        GList* current = tmp_list;

        while(current)
        {
            hostIf_IPInterfaceStats* pDev = (hostIf_IPInterfaceStats *)current->data;
            current = current->next;
            closeInstance(pDev);
        }

        g_list_free(tmp_list);
    }
}

void hostIf_IPInterfaceStats::getLock()
{
    g_mutex_init(&hostIf_IPInterfaceStats::m_mutex);
    g_mutex_lock(&hostIf_IPInterfaceStats::m_mutex);
}

void hostIf_IPInterfaceStats::releaseLock()
{
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] Unlocking mutex...  \n", __FUNCTION__, __LINE__);
    g_mutex_unlock(&hostIf_IPInterfaceStats::m_mutex);
}

/**
 * @brief Class Constructor of the class hostIf_IPInterfaceStats.
 *
 * It will initialize the device id.
 *
 * @param[in] dev_id Identification number of the device to communicate.
 */
hostIf_IPInterfaceStats::hostIf_IPInterfaceStats(int dev_id):
    dev_id(dev_id),
    backupBytesSent(0),
    backupBytesReceived(0),
    backupPacketsSent(0),
    backupPacketsReceived(0),
    backupErrorsSent(0),
    backupErrorsReceived(0),
    backupUnicastPacketsSent(0),
    backupUnicastPacketsReceived(0),
    backupDiscardPacketsSent(0),
    backupDiscardPacketsReceived(0),
    backupMulticastPacketsSent(0),
    backupMulticastPacketsReceived(0),
    backupBroadcastPacketsSent(0),
    backupBroadcastPacketsReceived(0),
    backupUnknownProtoPacketsReceived(0),
    bCalledBytesSent(false),
    bCalledBytesReceived(false),
    bCalledPacketsSent(false),
    bCalledPacketsReceived(false),
    bCalledErrorsSent(false),
    bCalledErrorsReceived(false),
    bCalledUnicastPacketsSent(false),
    bCalledUnicastPacketsReceived(false),
    bCalledDiscardPacketsSent(false),
    bCalledDiscardPacketsReceived(false),
    bCalledMulticastPacketsSent(false),
    bCalledMulticastPacketsReceived(false),
    bCalledBroadcastPacketsSent(false),
    bCalledBroadcastPacketsReceived(false),
    bCalledUnknownProtoPacketsReceived(false)
{
}

int hostIf_IPInterfaceStats::handleGetMsg (const char* pSetting, HOSTIF_MsgData_t* stMsgData)
{
    int ret = NOT_HANDLED;

    if (!strcasecmp (pSetting, "Stats.BytesSent"))
    {
        ret = get_Device_IP_Interface_Stats_BytesSent (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.BytesReceived"))
    {
        ret = get_Device_IP_Interface_Stats_BytesReceived (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.PacketsSent"))
    {
        ret = get_Device_IP_Interface_Stats_PacketsSent (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.PacketsReceived"))
    {
        ret = get_Device_IP_Interface_Stats_PacketsReceived (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.ErrorsSent"))
    {
        ret = get_Device_IP_Interface_Stats_ErrorsSent (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.ErrorsReceived"))
    {
        ret = get_Device_IP_Interface_Stats_ErrorsReceived (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.UnicastPacketsSent"))
    {
        ret = get_Device_IP_Interface_Stats_UnicastPacketsSent (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.UnicastPacketsReceived"))
    {
        ret = get_Device_IP_Interface_Stats_UnicastPacketsReceived (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.DiscardPacketsSent"))
    {
        ret = get_Device_IP_Interface_Stats_DiscardPacketsSent (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.DiscardPacketsReceived"))
    {
        ret = get_Device_IP_Interface_Stats_DiscardPacketsReceived (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.MulticastPacketsSent"))
    {
        ret = get_Device_IP_Interface_Stats_MulticastPacketsSent (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.MulticastPacketsReceived"))
    {
        ret = get_Device_IP_Interface_Stats_MulticastPacketsReceived (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.BroadcastPacketsSent"))
    {
        ret = get_Device_IP_Interface_Stats_BroadcastPacketsSent (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.BroadcastPacketsReceived"))
    {
        ret = get_Device_IP_Interface_Stats_BroadcastPacketsReceived (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.UnknownProtoPacketsReceived"))
    {
        ret = get_Device_IP_Interface_Stats_UnknownProtoPacketsReceived (stMsgData);
    }

    return ret;
}

int hostIf_IPInterfaceStats::getSysClassNetStatistic (const char* statistic, unsigned long* result)
{
    int ret = NOK;
    char filename[BUFF_LENGTH_64];
    snprintf (filename,BUFF_LENGTH_64, "/sys/class/net/%s/statistics/%s", nameOfInterface, statistic);

    FILE* fp = fopen (filename, "r");
    if (fp == NULL)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "\n[%s(), %d] Error in fopen(%s)\n", __FUNCTION__, __LINE__, filename);
        return NOK;
    }

    if (1 == fscanf (fp, "%ld", result))
        ret = OK;

    fclose (fp);

    return ret;
}

/****************************************************************************************************************************************************/
// Device_IP_Interface_Stats Profile. Getters:
/****************************************************************************************************************************************************/

/**
 * @brief This function gets the number of bytes sent on the IP Interface. It provides the
 * total number of bytes transmitted out of the interface, including framing characters.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_BytesSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.bytesSent = 0;
    getSysClassNetStatistic ("tx_bytes", &curntIpStat.bytesSent);

    if(bCalledBytesSent && pChanged && ((long unsigned int) backupBytesSent != curntIpStat.bytesSent))
    {
        *pChanged = true;
    }
    bCalledBytesSent = true;
    backupBytesSent = curntIpStat.bytesSent;
    put_int(stMsgData->paramValue,curntIpStat.bytesSent);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = sizeof(unsigned long);

    return OK;
}

/**
 * @brief This function gets the number of bytes received on the IP Interface. It provides
 * the total number of bytes received on the interface, including framing characters.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_BytesReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.bytesReceived = 0;
    getSysClassNetStatistic ("rx_bytes", &curntIpStat.bytesReceived);

    if(bCalledBytesReceived && pChanged && ((long unsigned int) backupBytesReceived != curntIpStat.bytesReceived))
    {
        *pChanged = true;
    }
    bCalledBytesReceived = true;
    backupBytesReceived = curntIpStat.bytesReceived;
    put_int(stMsgData->paramValue,curntIpStat.bytesReceived);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = sizeof(unsigned long);

    return OK;
}

/**
 * @brief This function gets the number of packets sent on the IP Interface. It provides
 * the total number of packets transmitted out of the interface.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_PacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.packetsSent = 0;
    getSysClassNetStatistic ("tx_packets", &curntIpStat.packetsSent);

    if(bCalledPacketsSent && pChanged && ((long unsigned int) backupPacketsSent != curntIpStat.packetsSent))
    {
        *pChanged = true;
    }
    bCalledPacketsSent = true;
    backupPacketsSent = curntIpStat.packetsSent;
    put_int(stMsgData->paramValue,curntIpStat.packetsSent);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = sizeof(unsigned long);

    return OK;
}

/**
 * @brief This function gets the total number of packets received on the IP Interface.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_PacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.packetsReceived = 0;
    getSysClassNetStatistic ("rx_packets", &curntIpStat.packetsReceived);
    // TODO: rx_packets only "Indicates the total number of good packets received by this network device."
    // as per https://www.kernel.org/doc/Documentation/ABI/testing/sysfs-class-net-statistics but
    // as per the TR069 spec, PacketsReceived is "The total number of packets received on the interface.".

    if(bCalledPacketsReceived && pChanged && ((long unsigned int) backupPacketsReceived != curntIpStat.packetsReceived))
    {
        *pChanged = true;
    }
    bCalledPacketsReceived = true;
    backupPacketsReceived = curntIpStat.packetsReceived;
    put_int(stMsgData->paramValue,curntIpStat.packetsReceived);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = sizeof(unsigned long);

    return OK;
}

/**
 * @brief  This function gets the number of send errors on the IP Interface. It provides
 * the total number of outbound packets that could not be transmitted because of errors.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_ErrorsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.errorsSent = 0;
    getSysClassNetStatistic ("tx_errors", &curntIpStat.errorsSent);

    if(bCalledErrorsSent && pChanged && ((long unsigned int) backupErrorsSent != curntIpStat.errorsSent))
    {
        *pChanged = true;
    }
    bCalledErrorsSent = true;
    backupErrorsSent = curntIpStat.errorsSent;
    put_int(stMsgData->paramValue,curntIpStat.errorsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of receive errors on the IP Interface. It provides
 * the total number of inbound packets that contained errors preventing them from being
 * delivered to a higher-layer protocol.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_ErrorsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.errorsReceived = 0;
    getSysClassNetStatistic ("rx_errors", &curntIpStat.errorsReceived);

    if(bCalledErrorsReceived && pChanged && ((long unsigned int) backupErrorsReceived != curntIpStat.errorsReceived))
    {
        *pChanged = true;
    }
    bCalledErrorsReceived = true;
    backupErrorsReceived = curntIpStat.errorsReceived;
    put_int(stMsgData->paramValue,curntIpStat.errorsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of unicast packets for which a request to send on the IP
 * Interface was received. It provides the total number of packets requested for transmission which
 * were not addressed to a multicast or broadcast address at this layer, including those that
 * were discarded or not sent.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_UnicastPacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.unicastPacketsSent = 0;
    getSysClassNetStatistic ("tx_packets", &curntIpStat.unicastPacketsSent);

    if(bCalledUnicastPacketsSent && pChanged && ((long unsigned int) backupUnicastPacketsSent != curntIpStat.unicastPacketsSent))
    {
        *pChanged = true;
    }
    bCalledUnicastPacketsSent = true;
    backupUnicastPacketsSent = curntIpStat.unicastPacketsSent;
    put_int(stMsgData->paramValue,curntIpStat.unicastPacketsSent);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = sizeof(unsigned long);

    return OK;
}

/**
 * @brief This function gets the number of unicast packets received on the IP Interface. Provides
 * the total number of received packets, delivered by this layer to a higher layer, which were
 * not addressed to a multicast or broadcast address at this layer.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_UnicastPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.unicastPacketsReceived = 0;
    getSysClassNetStatistic ("rx_packets", &curntIpStat.unicastPacketsReceived);

    if(bCalledUnicastPacketsReceived && pChanged && ((long unsigned int) backupUnicastPacketsReceived != curntIpStat.unicastPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledUnicastPacketsReceived = true;
    backupUnicastPacketsReceived = curntIpStat.unicastPacketsReceived;
    put_int(stMsgData->paramValue,curntIpStat.unicastPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = sizeof(unsigned long);

    return OK;
}

/**
 * @brief This function gets the number of discarded outbound packets on the IP Interface. It
 * provides the total number of outbound packets which were chosen to be discarded even though
 * no errors had been detected to prevent their being transmitted.
 *
 * @note One possible reason for discarding such a packet could be to free up buffer space.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_DiscardPacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.discardPacketsSent = 0;
    getSysClassNetStatistic ("tx_dropped", &curntIpStat.discardPacketsSent);

    if(bCalledDiscardPacketsSent && pChanged && ((long unsigned int) backupDiscardPacketsSent != curntIpStat.discardPacketsSent))
    {
        *pChanged = true;
    }
    bCalledDiscardPacketsSent = true;
    backupDiscardPacketsSent = curntIpStat.discardPacketsSent;
    put_int(stMsgData->paramValue,curntIpStat.discardPacketsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of discarded inbound packets on the IP Interface. It provides
 * the total number of inbound packets which were chosen to be discarded even though no errors had
 * been detected to prevent their being delivered.
 *
 * @note One possible reason for discarding such a packet could be to free up buffer space.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_DiscardPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.discardPacketsReceived = 0;
    getSysClassNetStatistic ("rx_dropped", &curntIpStat.discardPacketsReceived);

    if(bCalledDiscardPacketsReceived && pChanged && ((long unsigned int) backupDiscardPacketsReceived != curntIpStat.discardPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledDiscardPacketsReceived = true;
    backupDiscardPacketsReceived = curntIpStat.discardPacketsReceived;
    put_int(stMsgData->paramValue,curntIpStat.discardPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief  This function gets the number of Multicast Packets sent on the IP Interface. It provides
 * the total number of packets that higher-level protocols requested for transmission and which were
 * addressed to a multicast address at this layer, including those that were discarded or not sent.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_MulticastPacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.multicastPacketsSent = 0;
    getSysClassNetStatistic ("tx_packets", &curntIpStat.multicastPacketsSent);

    if(bCalledMulticastPacketsSent && pChanged && ((long unsigned int) backupMulticastPacketsSent != curntIpStat.multicastPacketsSent))
    {
        *pChanged = true;
    }
    bCalledMulticastPacketsSent = true;
    backupMulticastPacketsSent = curntIpStat.multicastPacketsSent;
    put_int(stMsgData->paramValue,curntIpStat.multicastPacketsSent);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = sizeof(unsigned long);

    return OK;
}

/**
 * @brief This function gets the number of Multicast Packets received on the IP Interface. It provides the
 * total number of received packets, delivered by this layer to a higher layer, which were addressed
 * to a multicast address at this layer.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_MulticastPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.multicastPacketsReceived = 0;
    getSysClassNetStatistic ("multicast", &curntIpStat.multicastPacketsReceived);

    if(bCalledMulticastPacketsReceived && pChanged && ((long unsigned int) backupMulticastPacketsReceived != curntIpStat.multicastPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledMulticastPacketsReceived = true;
    backupMulticastPacketsReceived = curntIpStat.multicastPacketsReceived;
    put_int(stMsgData->paramValue,curntIpStat.multicastPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = sizeof(unsigned long);

    return OK;
}

/**
 * @brief This function gets the number of broadcast packets sent on the IP Interface. It provides
 * the total number of packets that higher-level protocols requested for transmission and which
 * were addressed to a broadcast address at this layer, including those that were discarded or not sent.
 *
 * @note IPv6 does not define broadcast addresses, so IPv6 packets will never cause this counter to increment.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_BroadcastPacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.broadcastPacketsSent = 0;
    getSysClassNetStatistic ("tx_packets", &curntIpStat.broadcastPacketsSent);

    if(bCalledBroadcastPacketsSent && pChanged && ((long unsigned int) backupBroadcastPacketsSent != curntIpStat.broadcastPacketsSent))
    {
        *pChanged = true;
    }
    bCalledBroadcastPacketsSent = true;
    backupBroadcastPacketsSent = curntIpStat.broadcastPacketsSent;
    put_int(stMsgData->paramValue,curntIpStat.broadcastPacketsSent);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = sizeof(unsigned long);

    return OK;
}

/**
 * @brief This function gets the number of broadcast packets received on the IP interface.
 * It provides the total number of received packets, delivered by this layer to a higher layer,
 * which were addressed to a broadcast address at this layer.
 *
 * @note IPv6 does not define broadcast addresses, so IPv6 packets will never cause this counter
 * to increment.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_BroadcastPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.broadcastPacketsReceived = 0;
    getSysClassNetStatistic ("rx_packets", &curntIpStat.broadcastPacketsReceived);

    if(bCalledBroadcastPacketsReceived && pChanged && ((long unsigned int) backupBroadcastPacketsReceived != curntIpStat.broadcastPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledBroadcastPacketsReceived = true;
    backupBroadcastPacketsReceived = curntIpStat.broadcastPacketsReceived;
    put_int(stMsgData->paramValue,curntIpStat.broadcastPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of Packets of unidentified protocol received on
 * the IP Interface. It provides the total number of packets received via the interface
 * which were discarded because they were of an unknown or unsupported protocol.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_UnknownProtoPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    curntIpStat.unknownProtoPacketsReceived = 0;
    getSysClassNetStatistic ("rx_dropped", &curntIpStat.unknownProtoPacketsReceived);

    if(bCalledUnknownProtoPacketsReceived && pChanged && ((long unsigned int) backupUnknownProtoPacketsReceived != curntIpStat.unknownProtoPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledUnknownProtoPacketsReceived = true;
    backupUnknownProtoPacketsReceived = curntIpStat.unknownProtoPacketsReceived;
    put_int(stMsgData->paramValue,curntIpStat.unknownProtoPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}
/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */


/** @} */
/** @} */
