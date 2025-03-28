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
 * @file Device_IP_ActivePort.cpp
 * @brief This source file contains the APIs of device IP active ports.
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

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <net/if.h>
#include <arpa/inet.h>
#include "Device_IP_ActivePort.h"

#define         _LENGTH_PARAMETER               64
#define         _LENGTH_IPADDR                   16
#define         _LENGTH_STATUS                  12
#define         _HEX_STATE_LEN                  3
#define         _BUF_LEN            1024

GHashTable *hostIf_IPActivePort::ifHash = NULL;

GMutex hostIf_IPActivePort::m_mutex;

struct Device_IP_ActivePort hostIf_IPActivePort::activePort = {0,};

struct TcpConnection {
    std::string index; // Need it for Parsing Aligment
    std::string localAddress;
    std::string remoteAddress;
    std::string status;
    // Add more fields as needed
};

int hostIf_IPActivePort::getActivePortsFields(unsigned int activePortNo)
{
    char local_address[_LENGTH_IPADDR]= {'\0'}, rem_address[_LENGTH_IPADDR]= {'\0'}, state[_HEX_STATE_LEN] = {'\0'};
    unsigned int activePortCount = 1;
    int ret = -1;
    unsigned long ipAddr = 0;
    char *addrPtr = NULL;

    std::ifstream tcpFile("/proc/net/tcp");
    if (!tcpFile.is_open()) 
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"/proc/net/tcp File Open Error");
        return 0;
    }

    std::string line;
    std::getline(tcpFile, line); // Read and discard the header line

    while (std::getline(tcpFile, line)) 
    {
        std::istringstream iss(line);
        TcpConnection connection;

        if(activePortNo == activePortCount) 
	{
            // Parse the line
            iss >> connection.index >> connection.localAddress >> connection.remoteAddress >> connection.status;
            ret = OK;
            // Do something with the connection data
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Local Address: %s" , (connection.localAddress).c_str());
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Remote Address: %s" , (connection.remoteAddress).c_str());
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Status: %s", (connection.status).c_str());

	    (connection.localAddress).copy(local_address, _LENGTH_IPADDR);
	    local_address[(connection.localAddress).size()] = '\0';

	    (connection.remoteAddress).copy(rem_address, _LENGTH_IPADDR);
            rem_address[(connection.remoteAddress).size()] = '\0';

            (connection.status).copy(state, _HEX_STATE_LEN);
            state[(connection.status).size()] = '\0';
            break;
        }
        activePortCount++;
    }

    tcpFile.close();
    memset(activePort.localIpAddress, '\0', _LENGTH_IPADDR);
    memset(activePort.remoteIpAddress, '\0', _LENGTH_IPADDR);
    memset(activePort.status, '\0', _LENGTH_STATUS);
    activePort.localPort = 0;
    activePort.remotePort = 0;

    if(ret != OK)
        return ret;

    // STATUS
    /* st = 0A = LISTEN
       st = 01 = ESTABLISHED */
    if (strtoul(state, NULL, 16) == 10)
    {
	strncpy(activePort.status, "LISTEN", sizeof(activePort.status) - 1);
	activePort.status[sizeof(activePort.status) - 1] = '\0';
    }
    else if (strtoul(state, NULL, 16) == 1)
    {
        strncpy(activePort.status, "ESTABLISHED", sizeof(activePort.status) - 1);
        activePort.status[sizeof(activePort.status) - 1] = '\0';
    }
    // LOCAL IP ADDRESS and LOCAL PORT
    addrPtr = local_address;

    ipAddr = strtoul(strsep(&addrPtr, ":"), NULL, 16);
    inet_ntop(AF_INET, &ipAddr, activePort.localIpAddress, INET_ADDRSTRLEN);
    activePort.localPort = strtoul(addrPtr, NULL, 16);

    // REMOTE IP ADDRESS and REMOTE PORT
    addrPtr = rem_address;

    ipAddr = strtoul(strsep(&addrPtr, ":"), NULL, 16);
    inet_ntop(AF_INET, &ipAddr, activePort.remoteIpAddress, INET_ADDRSTRLEN);
    activePort.remotePort = strtoul(addrPtr, NULL, 16);

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"ActivePort: %d local addr: %s:%d remote addr: %s:%d Status: %s\n", activePortNo, activePort.localIpAddress,
             activePort.localPort, activePort.remoteIpAddress, activePort.remotePort, activePort.status);

    return ret;
}

/**
 * @brief Class Constructor of the class hostIf_IPActivePort.
 *
 * It will initialize the device id. And it will initialize the local IP address , remote IP address to
 * empty string.
 *
 * @param[in] dev_id Device identification number.
 */
hostIf_IPActivePort::hostIf_IPActivePort(int dev_id):
    dev_id(dev_id),
    bCalledLocalIPAddress(false),
    bCalledLocalPort(false),
    bCalledRemoteIPAddress(false),
    bCalledRemotePort(false),
    bCalledStatus(false),
    backupLocalPort(0),
    backupRemotePort(0)
{
    backupLocalIPAddress[0]='\0';
    backupRemoteIPAddress[0]='\0';
    backupStatus[0]='\0';

}

hostIf_IPActivePort* hostIf_IPActivePort::getInstance(int dev_id)
{
    hostIf_IPActivePort* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_IPActivePort *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_IPActivePort(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create MoCA Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_IPActivePort::getAllInstances()
{
    if(ifHash)
       return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_IPActivePort::closeInstance(hostIf_IPActivePort *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_IPActivePort::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list_original = g_hash_table_get_values (ifHash);
        GList* tmp_list = tmp_list_original;

        while(tmp_list)
        {
            hostIf_IPActivePort* pDev = (hostIf_IPActivePort *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
        g_list_free(tmp_list_original);
    }
}

void hostIf_IPActivePort::getLock()
{
    g_mutex_init(&hostIf_IPActivePort::m_mutex);
    g_mutex_lock(&hostIf_IPActivePort::m_mutex);
}

void hostIf_IPActivePort::releaseLock()
{
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] Unlocking mutex...  \n", __FUNCTION__, __LINE__);
    g_mutex_unlock(&hostIf_IPActivePort::m_mutex);
}

int hostIf_IPActivePort::handleGetMsg (const char* pSetting, HOSTIF_MsgData_t* stMsgData)
{
    int ret = NOT_HANDLED;

    if (!strcasecmp (pSetting, "LocalIPAddress"))
    {
        ret = get_Device_IP_ActivePort_LocalIPAddress (stMsgData);
    }
    else if (!strcasecmp (pSetting, "LocalPort"))
    {
        ret = get_Device_IP_ActivePort_LocalPort (stMsgData);
    }
    else if (!strcasecmp (pSetting, "RemoteIPAddress"))
    {
        ret = get_Device_IP_ActivePort_RemoteIPAddress (stMsgData);
    }
    else if (!strcasecmp (pSetting, "RemotePort"))
    {
        ret = get_Device_IP_ActivePort_RemotePort (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Status"))
    {
        ret = get_Device_IP_ActivePort_Status (stMsgData);
    }

    return ret;
}

/****************************************************************************************************************************************************/
// Device.IP.ActivePort. Profile. Getters:
/****************************************************************************************************************************************************/

/*
 * Parameter Name: Device.IP.ActivePort.
*/

/**
 * @brief This function gets the local IP Address of the connection.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_ACTIVEPORT_API
 */
int hostIf_IPActivePort::get_Device_IP_ActivePort_LocalIPAddress(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    /*Retrieving value */
    getActivePortsFields(dev_id);

    if(bCalledLocalIPAddress && pChanged && strncmp(activePort.localIpAddress,backupLocalIPAddress,_LENGTH_IPADDR ))
    {
        *pChanged =  true;
    }
    bCalledLocalIPAddress = true;
    strncpy(backupLocalIPAddress,activePort.localIpAddress,sizeof(backupLocalIPAddress) -1);  //CID:136360 - Buffer size warning
    backupLocalIPAddress[sizeof(backupLocalIPAddress) -1] = '\0';
    strncpy(stMsgData->paramValue,activePort.localIpAddress,sizeof(stMsgData->paramValue) -1);
    stMsgData->paramValue[sizeof(stMsgData->paramValue) -1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen( activePort.localIpAddress);

    return OK;
}

/**
 * @brief This function gets the local port number of the connection.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_ACTIVEPORT_API
 */
int hostIf_IPActivePort::get_Device_IP_ActivePort_LocalPort(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    /*Retrieving value */
    getActivePortsFields(dev_id);
    if(bCalledLocalPort && pChanged && (backupLocalPort != activePort.localPort))
    {
        *pChanged = true;
    }
    bCalledLocalPort = true;
    backupLocalPort = activePort.localPort;
    put_int(stMsgData->paramValue,activePort.localPort);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the remote IP Address of the established connection.
 * It provides the remote IP address of the source of inbound packets.
 *
 * @note This will be an empty string for listening connections  ESTABLISHED state
 * have remote addresses.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_ACTIVEPORT_API
 */
int hostIf_IPActivePort::get_Device_IP_ActivePort_RemoteIPAddress(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    /*Retrieving value */
    getActivePortsFields(dev_id);
    if(bCalledRemoteIPAddress && pChanged && strncmp(activePort.remoteIpAddress,backupRemoteIPAddress,_LENGTH_IPADDR ))
    {
        *pChanged =  true;
    }
    bCalledRemoteIPAddress = true;
    strncpy(backupRemoteIPAddress,activePort.remoteIpAddress,sizeof(backupRemoteIPAddress) -1);  //CID:136383 - Buffer size warning
    backupRemoteIPAddress[sizeof(backupRemoteIPAddress) -1] = '\0';
    strncpy(stMsgData->paramValue,activePort.remoteIpAddress,sizeof(stMsgData->paramValue) -1);
    stMsgData->paramValue[sizeof(stMsgData->paramValue) -1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen( activePort.remoteIpAddress);
    return OK;
}

/**
 * @brief This function gets the remote port number of the established connection. It provides the
 * remote port number of the source of inbound packets.
 *
 * @note  This will be 0 for listening connections only ESTABLISHED the state have remote addresses.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_ACTIVEPORT_API
 */
int hostIf_IPActivePort::get_Device_IP_ActivePort_RemotePort(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    /*Retrieving value */
    getActivePortsFields(dev_id);
    if(bCalledRemotePort && pChanged && (backupRemotePort != activePort.remotePort))
    {
        *pChanged = true;
    }
    bCalledRemotePort = true;
    backupRemotePort = activePort.remotePort;
    put_int(stMsgData->paramValue,activePort.remotePort);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;
    return OK;
}

/**
 * @brief This function gets the current operational status of the connection. The possible values are
 * 'LISTEN', 'ESTABLISHED'.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_ACTIVEPORT_API
 */
int hostIf_IPActivePort::get_Device_IP_ActivePort_Status(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    /*Retrieving value */
    getActivePortsFields(dev_id);
    if(bCalledStatus && pChanged && strncmp(activePort.status,backupStatus,_LENGTH_IPADDR ))
    {
        *pChanged =  true;
    }
    bCalledStatus = true;
    strncpy(backupStatus,activePort.status,_LENGTH_STATUS-1);
    strncpy(stMsgData->paramValue,activePort.status,_LENGTH_STATUS-1 );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen( activePort.status);
    return OK;
}
/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */


/** @} */
/** @} */
