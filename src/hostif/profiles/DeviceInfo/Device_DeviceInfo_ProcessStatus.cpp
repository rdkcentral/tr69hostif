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
 * @file Device_DeviceInfo_ProcessStatus.cpp
 * @brief This source file contains the APIs for getting device processor status information.
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


#include "Device_DeviceInfo_ProcessStatus.h"
#include "safec_lib.h"

GHashTable* hostIf_DeviceProcessStatusInterface::ifHash = NULL;
GMutex hostIf_DeviceProcessStatusInterface::m_mutex;

hostIf_DeviceProcessStatusInterface* hostIf_DeviceProcessStatusInterface::getInstance(int dev_id)
{
    hostIf_DeviceProcessStatusInterface* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_DeviceProcessStatusInterface *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_DeviceProcessStatusInterface(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create Ethernet Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_DeviceProcessStatusInterface::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_DeviceProcessStatusInterface::closeInstance(hostIf_DeviceProcessStatusInterface *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_DeviceProcessStatusInterface::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        GList* current = tmp_list;

        while(current)
        {
            hostIf_DeviceProcessStatusInterface* pDev = (hostIf_DeviceProcessStatusInterface *)current->data;
            current = current->next;
            closeInstance(pDev);
        }

        g_list_free(tmp_list);
    }
}

void hostIf_DeviceProcessStatusInterface::getLock()
{
    g_mutex_init(&hostIf_DeviceProcessStatusInterface::m_mutex);
    g_mutex_lock(&hostIf_DeviceProcessStatusInterface::m_mutex);
}

void hostIf_DeviceProcessStatusInterface::releaseLock()
{
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] Unlocking mutex...  \n", __FUNCTION__, __LINE__);
    g_mutex_unlock(&hostIf_DeviceProcessStatusInterface::m_mutex);
}

hostIf_DeviceProcessStatusInterface::hostIf_DeviceProcessStatusInterface(int devid)
{
    dev_id = devid;
    bCalledCPUUsage = false;
    bCalledProcessNumberOfEntries = false;
    backupCPUUsage = 0;
    backupProcessNumberOfEntries = 0;
}

int hostIf_DeviceProcessStatusInterface::getProcessStatParam(long long unsigned int *mUser, long long unsigned int *mNice, long long unsigned int *mSystem, 
		                                             long long unsigned int *mIdle, long long unsigned int *mIOwait, long long unsigned int *mIrq, 
							     long long unsigned int *mSoftirq) 
{
    int ret = -1;
    std::string filePath = "/proc/stat";
    std::ifstream file(filePath);

    if (!file.is_open()) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): /proc/stat File open Error\n", __FUNCTION__);
        return -1;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("cpu ") == 0) {
            sscanf(line.c_str(),"%*s %llu %llu %llu %llu %llu %llu %llu", mUser, mNice, mSystem, mIdle, mIOwait, mIrq, mSoftirq);
	    ret = 0;
	    break;
        }
    }

    file.close();

    return ret;
    
}
int hostIf_DeviceProcessStatusInterface::getProcessStatusCPUUsage() {

    errno_t rc = -1;
    unsigned long total_jiffies_1 = 0, total_jiffies_2 = 0;
    unsigned long work_jiffies_1 = 0, work_jiffies_2 = 0;
    long long unsigned int mUser = 0, mNice = 0, mSystem = 0, mIdle = 0, mIOwait = 0, mIrq = 0, mSoftirq = 0;
    unsigned int uiCpuUsage = 0;
    unsigned int temp = 0;

    /* --------Get CPU Stats-------------- */
    rc = getProcessStatParam(&mUser, &mNice, &mSystem, &mIdle, &mIOwait, &mIrq, &mSoftirq);
    if(rc == -1)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): CPU Stat param Fetch Failed.\n", __FUNCTION__);
	return -1;
    }
    /* ---------------------- */

    //CPUUsage

    total_jiffies_1 =  mUser + mNice + mSystem + mIdle + mIOwait + mIrq + mSoftirq;
    work_jiffies_1 =  mUser + mNice + mSystem;

    sleep(1);
    
    /* --------Get CPU Stats-------------- */
    rc = getProcessStatParam(&mUser, &mNice, &mSystem, &mIdle, &mIOwait, &mIrq, &mSoftirq);
    if(rc == -1)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): CPU Stat param Fetch Failed.\n", __FUNCTION__);
	return -1;
    }
    /* ---------------------- */


    total_jiffies_2 =  mUser + mNice + mSystem + mIdle + mIOwait + mIrq + mSoftirq;
    work_jiffies_2 =  mUser + mNice + mSystem;

    temp = total_jiffies_2 - total_jiffies_1;

    if(temp != 0)
    {
        uiCpuUsage = ((work_jiffies_2 - work_jiffies_1)*100)/(temp);

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): CpuUsage = [%u]\n", __FUNCTION__, uiCpuUsage);
    }
    return uiCpuUsage;

}


/****************************************************************************************************************************************************/
// Device_DeviceInfo_ProcessStatus. Profile. Getters:
/****************************************************************************************************************************************************/

/**
 * @brief This function provides the total amount of cpu usage, in percentage.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_PROCESSORSTATUS_API
 */
int hostIf_DeviceProcessStatusInterface::get_Device_DeviceInfo_ProcessStatus_CPUUsage(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int uiCpuUsage = getProcessStatusCPUUsage();
    if (uiCpuUsage < 0)
        uiCpuUsage = 0;

    put_int(stMsgData->paramValue,uiCpuUsage);
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen = 32;
    if(bCalledCPUUsage && pChanged &&(backupCPUUsage != uiCpuUsage))
    {
        *pChanged = true;
    }
    bCalledCPUUsage = true;
    backupCPUUsage = uiCpuUsage;


    return OK;
}

/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */


/** @} */
/** @} */
