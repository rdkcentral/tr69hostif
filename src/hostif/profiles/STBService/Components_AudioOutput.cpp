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
 * @file Components_AudioOutput.cpp
 * @brief This source file contains the APIs of TR069 Components AudioOutput.
 */


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include "hostIf_main.h"
#include "dsTypes.h"
#include "illegalArgumentException.hpp"
#include "exception.hpp"
#include "Components_AudioOutput.h"
#include "safec_lib.h"

/* All functions moved to Components_AudioOutput_Thunder.cpp */

/* getInstance() moved to Components_AudioOutput_Thunder.cpp */
#if 0
hostIf_STBServiceAudioInterface* hostIf_STBServiceAudioInterface::getInstance(int dev_id)
{
    hostIf_STBServiceAudioInterface* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_STBServiceAudioInterface *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d]: dev_id=[%d];\n", __FILE__, __FUNCTION__, __LINE__, dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        if (!portNameHash)
            buildPortNameHash();

        const char* portNameCStr = (const char*)g_hash_table_lookup(portNameHash, (gpointer)(intptr_t)dev_id);
        if (!portNameCStr)
        {
            RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                    "[%s:%s:%d]: No port name for dev_id=%d\n", __FILE__, __FUNCTION__, __LINE__, dev_id);
            return NULL;
        }
        std::string portName(portNameCStr);

        try 
        {
            device::List<device::AudioOutputPort> aPorts = device::Host::getInstance().getAudioOutputPorts();
            pRet = new hostIf_STBServiceAudioInterface(dev_id, aPorts.at(dev_id-1), portName);
            g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s:%s:%d]: AudioOutput dev_id=%d portName=%s\n",
                    __FILE__, __FUNCTION__, __LINE__, dev_id, portName.c_str());
        }
        catch (const device::IllegalArgumentException &e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught device::IllegalArgumentException, not able create STB service %s Interface instance %d..\n", DEV_NAME, dev_id);
        }
        catch (const int &e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create STB service %s Interface instance %d..\n", DEV_NAME, dev_id);
        }
        catch (const dsError_t &e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught dsError_t %d, not able create STB service %s Interface instance %d..\n", e, DEV_NAME, dev_id);
        }
        catch (const device::Exception &e) 
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Caught device::Exception %d \"%s\", not able create STB service %s Interface instance %d..\n", e.getCode(), e.getMessage().c_str(), DEV_NAME, dev_id);
        }
    }
    return pRet;
}

GList* hostIf_STBServiceAudioInterface::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_STBServiceAudioInterface::closeInstance(hostIf_STBServiceAudioInterface *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBServiceAudioInterface::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        GList* current = tmp_list;

        while(current)
        {
            hostIf_STBServiceAudioInterface* pDev = (hostIf_STBServiceAudioInterface *)current->data;
            current = current->next;
            closeInstance(pDev);
        }

        g_list_free(tmp_list);
    }
}

void hostIf_STBServiceAudioInterface::getLock()
{
    g_mutex_init(&hostIf_STBServiceAudioInterface::m_mutex);
    g_mutex_lock(&hostIf_STBServiceAudioInterface::m_mutex);
}

void hostIf_STBServiceAudioInterface::releaseLock()
{
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] Unlocking mutex...  \n", __FUNCTION__, __LINE__);
    g_mutex_unlock(&hostIf_STBServiceAudioInterface::m_mutex);
}

/**
 * @brief Class Constructor of the class hostIf_STBServiceAudioInterface.
 *
 * It will initialize the device id and audio output port.
 *
 * @param[in] devid Identification number of the device.
 * @param[in] port Audio output port number.
 */
hostIf_STBServiceAudioInterface::hostIf_STBServiceAudioInterface(int devid, device::AudioOutputPort& port, const std::string& portName) : dev_id(devid), aPort(port), m_portName(portName)
{
    errno_t rc = -1;
    rc=strcpy_s(backupStatus,sizeof(backupStatus), " ");
    ERR_CHK(rc);
    backupCancelMute = false;
    rc=strcpy_s(backupAudioStereoMode,sizeof(backupAudioStereoMode)," ");
    ERR_CHK(rc);
    backupAudioLevel=0;
    rc=strcpy_s(backupAudioDB,sizeof(backupAudioDB)," ");
    ERR_CHK(rc);
    rc=strcpy_s(backupAudioLoopThru,sizeof(backupAudioLoopThru)," ");
    ERR_CHK(rc);
    backupAudioCompression=0;
    rc=strcpy_s(backupAudioEncoding,sizeof(backupAudioEncoding)," ");
    ERR_CHK(rc);
    rc=strcpy_s(backupAudioGain,sizeof(backupAudioGain)," ");
    ERR_CHK(rc);
    rc=strcpy_s(backupMinAudioDB,sizeof(backupMinAudioDB)," ");
    ERR_CHK(rc);
    rc=strcpy_s(backupMaxAudioDB,sizeof(backupMaxAudioDB)," ");
    ERR_CHK(rc);
    rc=strcpy_s(backupAudioOptimalLevel,sizeof(backupAudioOptimalLevel)," ");
    ERR_CHK(rc);
    backupDialogEnhancement=0;
    bCalledStatus = false;
    bCalledCancelMute = false;
    bCalledAudioStereoMode = false;
    bCalledAudioLevel = false;
    bCalledAudioDB = false;
    bCalledAudioLoopThru = false;
    bCalledAudioCompression = false;
    bCalledAudioEncoding = false;
    bCalledAudioGain = false;
    bCalledMinAudioDB = false;
    bCalledMaxAudioDB = false;
    bCalledAudioOptimalLevel = false;
    bCalledDialogEnhancement = false;
}

/* handleSetMsg() moved to Components_AudioOutput_Thunder.cpp */
/* handleGetMsg() moved to Components_AudioOutput_Thunder.cpp */
/* doUpdates() moved to Components_AudioOutput_Thunder.cpp */
/* getNumberOfInstances() moved to Components_AudioOutput_Thunder.cpp */
/* All parameter getter/setter functions moved to Components_AudioOutput_Thunder.cpp */
#endif

/** @} */
/** @} */
