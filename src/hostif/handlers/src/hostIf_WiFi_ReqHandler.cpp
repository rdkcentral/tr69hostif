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
 * @file hostIf_WiFi_ReqHandler.cpp
 * @brief The header file provides HostIf IP WiFi request handler information APIs.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifdef USE_WIFI_PROFILE
#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_msgHandler.h"

#include "hostIf_WiFi_ReqHandler.h"
#include "Device_WiFi.h"
#include "Device_WiFi_Radio.h"
#include "Device_WiFi_Radio_Stats.h"
#include "Device_WiFi_EndPoint.h"
#include "Device_WiFi_EndPoint_WPS.h"
#include "Device_WiFi_EndPoint_Security.h"
#include "Device_WiFi_EndPoint_Profile.h"
#include "Device_WiFi_EndPoint_Profile_Security.h"
#include "Device_WiFi_AccessPoint.h"
#include "Device_WiFi_AccessPoint_WPS.h"
#include "Device_WiFi_AccessPoint_Security.h"
#include "Device_WiFi_AccessPoint_AssociatedDevice.h"
#include "Device_WiFi_SSID.h"
#include "Device_WiFi_SSID_Stats.h"

#ifdef WIFI_CLIENT_ROAMING
#include "Device_WiFi_X_RDKCENTRAL_COM_ClientRoaming.h"
#endif

WiFiReqHandler* WiFiReqHandler::pInstance = NULL;
updateCallback WiFiReqHandler::mUpdateCallback = NULL;
int WiFiReqHandler::savedSSIDNumberOfEntries = 0;
int WiFiReqHandler::savedRadioNumberOfEntries = 0;
int WiFiReqHandler::savedEndPointNumberOfEntries = 0;
static bool bfirstInstance=false;

msgHandler* WiFiReqHandler::getInstance()
{
    if(!pInstance)
        pInstance = new WiFiReqHandler();
    return pInstance;
}

/**
 * @brief This function is use to initialize. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successfully .
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_WIFI_REQHANDLER_CLASSES
 */
bool WiFiReqHandler::init()
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] WiFi manager Initializing\n", __FUNCTION__);
    return true;
}

void WiFiReqHandler::reset()
{
    bfirstInstance=false;
    savedSSIDNumberOfEntries = 0;
    savedRadioNumberOfEntries = 0;
    savedEndPointNumberOfEntries = 0;
}

/**
 * @brief This function is used to close all the instances of WiFi, WiFi Radio,
 * SSID, AccessPoint, EndPoint etc..
 *
 * @return Returns the status of the operation.
 *
 * @retval true if it is successfully close all the instances.
 * @retval false if not able to close all the instances.
 * @ingroup TR-069HOSTIF_WIFI_REQHANDLER_CLASSES
 */
bool WiFiReqHandler::unInit()
{

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] WiFi manager Deinitializing. \n", __FUNCTION__);

    hostIf_WiFi::closeAllInstances();

    hostIf_WiFi_Radio::closeAllInstances();
    hostIf_WiFi_Radio_Stats::closeAllInstances();

    hostIf_WiFi_SSID::closeAllInstances();
    hostIf_WiFi_SSID_Stats::closeAllInstances();

    hostIf_WiFi_AccessPoint::closeAllInstances();
    hostIf_WiFi_AccessPoint_WPS::closeAllInstances();
    hostIf_WiFi_AccessPoint_Security::closeAllInstances();
    hostIf_WiFi_AccessPoint_AssociatedDevice::closeAllInstances();

    hostIf_WiFi_EndPoint::closeAllInstances();
    hostIf_WiFi_EndPoint_WPS::closeAllInstances();
    hostIf_WiFi_EndPoint_Security::closeAllInstances();
    hostIf_WiFi_EndPoint_Profile::closeAllInstances();
    hostIf_WiFi_EndPoint_Profile_Security::closeAllInstances();
#ifdef WIFI_CLIENT_ROAMING
    hostIf_WiFi_Xrdk_ClientRoaming::closeAllInstances();
#endif

    WiFiDevice::closeAllInstances();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

/**
 * @brief This function use to handle the set message request of WiFi.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_WIFI_REQHANDLER_CLASSES
 */
int WiFiReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
/*    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s] Found string as %s. Set command not supported.\n",
             __FUNCTION__, __FILE__, stMsgData->paramName);
    stMsgData->faultCode = fcAttemptToSetaNonWritableParameter;*/
    if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_WiFiEnable") == 0)
    {
        hostIf_WiFi *pIface = hostIf_WiFi::getInstance (1);

        if(!pIface)
        {
            return NOK;
        }
        ret = pIface->set_Device_WiFi_EnableWiFi(stMsgData);
    }
#ifdef WIFI_CLIENT_ROAMING
    else if(strncasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming",strlen("Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming"))==0)
    {
        //Setting Roaming params
        hostIf_WiFi_Xrdk_ClientRoaming* clntRoamInst =  hostIf_WiFi_Xrdk_ClientRoaming::getInstance(stMsgData->instanceNum);
        if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.Enable") == 0)
        {
            ret = clntRoamInst->set_Device_WiFi_X_Rdkcentral_clientRoaming_Enable(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PreAssn_BestThresholdLevel") == 0)
        {
            ret = clntRoamInst->set_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestThresholdLevel(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PreAssn_BestDeltaLevel") == 0)
        {
            ret = clntRoamInst->set_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestDeltaLevel(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_BestDeltaLevelConnected") == 0)
        {
            ret = clntRoamInst->set_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_BestDeltaLevelConnected(stMsgData);
        }
        else if(strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_BestDeltaLevelDisconnected") == 0)
        {
            ret = clntRoamInst->set_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_BestDeltaLevelDisconnected(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_SelfSteerThresholdLevel") == 0)
        {
            ret = clntRoamInst->set_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_SelfSteerThreshold(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_SelfSteerTimeframe") == 0)
        {
            ret = clntRoamInst->set_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_SelfSteerTimeframe(stMsgData);
        }
       /* else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_SelfSteerBeaconsMissedTime") == 0)
        {
            ret = clntRoamInst->set_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_SelfSteerBeaconsMissedTime(stMsgData);
        }*/
        else if(strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_APcontrolThresholdLevel") == 0)
        {
            ret = clntRoamInst->set_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_APcontrolThresholdLevel(stMsgData);
        }
        else if(strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_APcontrolTimeframe") == 0)
        {
            ret = clntRoamInst->set_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_APcontrolTimeframe(stMsgData);
        }
        else if(strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.SelfSteer_OverrideEnable") == 0)
        {
            ret = clntRoamInst->set_Device_WiFi_X_Rdkcentral_clientRoaming_SelfSteerOverride(stMsgData);
        }
        else if(strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_BackOffTime") == 0)
        {
            ret = clntRoamInst->set_Device_WiFi_X_Rdkcentral_clientRoaming_postAssnBackOffTime(stMsgData);
        }
        else if(strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.80211kvrEnable") == 0)
        {
            ret = clntRoamInst->set_Device_WiFi_X_Rdkcentral_clientRoaming_80211kvrEnable(stMsgData);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]  Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
            stMsgData->faultCode = fcInvalidParameterName;
            ret = NOK;
        }
    }
#endif
    else
    {
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s] Found string as %s. Set command not supported.\n", __FUNCTION__, __FILE__, stMsgData->paramName);
        stMsgData->faultCode = fcAttemptToSetaNonWritableParameter;
    }
    return ret;
}

/**
 * @brief This function use to handle the get message request of WiFi interface
 * and get the attributes of "Radio", "SSID", "AccessPoint", "Endpoint" etc.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to get data from the device.
 * @ingroup TR-069HOSTIF_WIFI_REQHANDLER_CLASSES
 */
int WiFiReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    const int maxSSID_Instances = 1;
    int instanceNum = 0;

    if (strcasecmp(stMsgData->paramName,"Device.WiFi.AccessPointNumberOfEntries") == 0)
    {
        stMsgData->instanceNum = 0;
        hostIf_WiFi *pIface = hostIf_WiFi::getInstance(stMsgData->instanceNum);

        if(!pIface)
        {
            return NOK;
        }

        ret = pIface->get_Device_WiFi_AccessPointNumberOfEntries(stMsgData);
    }
    else if (strcasecmp(stMsgData->paramName,"Device.WiFi.EndPointNumberOfEntries") == 0)
    {
        hostIf_WiFi *pIface = hostIf_WiFi::getInstance (1);

        if(!pIface)
        {
            return NOK;
        }
        ret = pIface->get_Device_WiFi_EndPointNumberOfEntries(stMsgData);
    }
    else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_WiFiEnable") == 0)
    {
        hostIf_WiFi *pIface = hostIf_WiFi::getInstance (1);

        if(!pIface)
        {
            return NOK;
        }
        ret = pIface->get_Device_WiFi_EnableWiFi(stMsgData);
    }
    else if (matchComponent(stMsgData->paramName, "Device.WiFi.Endpoint", &pSetting, instanceNum))
    {
        stMsgData->instanceNum = instanceNum;
        hostIf_WiFi_EndPoint *pWifiEndPoint = hostIf_WiFi_EndPoint::getInstance(stMsgData->instanceNum);
        hostIf_WiFi_EndPoint_WPS *pWifiEndPointWps  = hostIf_WiFi_EndPoint_WPS::getInstance(stMsgData->instanceNum);
        hostIf_WiFi_EndPoint_Security *pWifiEndpointSec = hostIf_WiFi_EndPoint_Security::getInstance(stMsgData->instanceNum);

        if ((!pWifiEndPoint) || (!pWifiEndPointWps) || (!pWifiEndpointSec))
        {
            return NOK;
        }

        if (strcasecmp(pSetting,"Enable") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_Enable(stMsgData);
        }
        else if (strcasecmp(pSetting,"Status") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_Status(stMsgData);
        }
        else if (strcasecmp(pSetting,"Alias") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_Alias(stMsgData);
        }
        else if (strcasecmp(pSetting,"ProfileReference") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_ProfileReference(stMsgData);
        }
        else if (strcasecmp(pSetting,"SSIDReference") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_SSIDReference(stMsgData);
        }
        else if (strcasecmp(pSetting,"ProfileNumberOfEntries") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_ProfileNumberOfEntries(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.LastDataDownlinkRate") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_Stats_LastDataDownlinkRate(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.LastDataUplinkRate") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_Stats_LastDataUplinkRate(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.SignalStrength") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_Stats_SignalStrength(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.Retransmissions") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_Stats_Retransmissions(stMsgData);
        }
        else if(strcasecmp(pSetting,"Security.ModesEnabled") == 0)
        {
            ret = pWifiEndpointSec->get_hostIf_WiFi_EndPoint_Security_ModesEnabled(stMsgData);
        }
        else
        {
           RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]  Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
           stMsgData->faultCode = fcInvalidParameterName;
           ret = NOK;
        }

    }
    else if (matchComponent(stMsgData->paramName, "Device.WiFi.AccessPoint", &pSetting, instanceNum))
    {
        stMsgData->instanceNum = instanceNum;
        hostIf_WiFi_AccessPoint *pIfaceAccessPoint = hostIf_WiFi_AccessPoint::getInstance(stMsgData->instanceNum);
        hostIf_WiFi_AccessPoint_WPS *pIfaceAccessPointWps = hostIf_WiFi_AccessPoint_WPS::getInstance(stMsgData->instanceNum);
        hostIf_WiFi_AccessPoint_Security *pIfaceAccessPointSec = hostIf_WiFi_AccessPoint_Security::getInstance(stMsgData->instanceNum);
        hostIf_WiFi_AccessPoint_AssociatedDevice *pIfaceAccessPointAssDev = hostIf_WiFi_AccessPoint_AssociatedDevice::getInstance(stMsgData->instanceNum);

        if ((!pIfaceAccessPoint) || (!pIfaceAccessPointWps) || (!pIfaceAccessPointSec) || (!pIfaceAccessPointAssDev))
        {
            return NOK;
        }
    }
    else if (matchComponent(stMsgData->paramName, "Device.WiFi.SSID", &pSetting, instanceNum))
    {
        if ((instanceNum <= 0) || (instanceNum > maxSSID_Instances))
        {
            return NOK;
        }

        stMsgData->instanceNum = instanceNum;
        hostIf_WiFi_SSID *pIfaceSsid = hostIf_WiFi_SSID::getInstance(stMsgData->instanceNum);
        hostIf_WiFi_SSID_Stats *pIfaceSsidStats = hostIf_WiFi_SSID_Stats::getInstance(stMsgData->instanceNum);

        if ((!pIfaceSsid) || (!pIfaceSsidStats))
        {
            return NOK;
        }

        if (strcasecmp(pSetting, "Enable") == 0)
        {
            ret = pIfaceSsid->get_Device_WiFi_SSID_Enable(stMsgData);
        }
        else if (strcasecmp(pSetting, "Status") == 0)
        {
            ret = pIfaceSsid->get_Device_WiFi_SSID_Status(stMsgData);
        }
        else if (strcasecmp(pSetting, "BSSID") == 0)
        {
            ret = pIfaceSsid->get_Device_WiFi_SSID_BSSID(stMsgData);
        }
        else if (strcasecmp(pSetting, "MACAddress") == 0)
        {
            ret = pIfaceSsid->get_Device_WiFi_SSID_MACAddress(stMsgData);
        }
        else if (strcasecmp(pSetting, "SSID") == 0)
        {
            ret = pIfaceSsid->get_Device_WiFi_SSID_SSID(stMsgData);
        }
        else if (strcasecmp(pSetting, "Name") == 0)
        {
            ret = pIfaceSsid->get_Device_WiFi_SSID_Name(stMsgData);
        }
        else if (strcasecmp(pSetting, "Alias") == 0)
        {
            ret = pIfaceSsid->get_Device_WiFi_SSID_Alias(stMsgData);
        }
        else
        {
           RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]  Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
           stMsgData->faultCode = fcInvalidParameterName;
           ret = NOK;
        }

    }
#ifdef WIFI_CLIENT_ROAMING
    else if (strncasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming",strlen("Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming")) == 0)
    {
        // Get Client Roaming Settings
        hostIf_WiFi_Xrdk_ClientRoaming* clntRoamInst =  hostIf_WiFi_Xrdk_ClientRoaming::getInstance(stMsgData->instanceNum);
        if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.Enable") == 0)
        {
            ret = clntRoamInst->get_Device_WiFi_X_Rdkcentral_clientRoaming_Enable(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PreAssn_BestThresholdLevel") == 0)
        {
            ret = clntRoamInst->get_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestThresholdLevel(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PreAssn_BestDeltaLevel") == 0)
        {
            ret = clntRoamInst->get_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestDeltaLevel(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.SelfSteer_Override") == 0)
        {
            ret = clntRoamInst->get_Device_WiFi_X_Rdkcentral_clientRoaming_SelfSteerOverride(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_BestDeltaLevelConnected") == 0)
        {
            ret = clntRoamInst->get_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_BestDeltaLevelConnected(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_BestDeltaLevelDisconnected") == 0)
        {
            ret = clntRoamInst->get_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_BestDeltaLevelDisconnected(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_SelfSteerThresholdLevel") == 0)
        {
            ret = clntRoamInst->get_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_SelfSteerThreshold(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_SelfSteerTimeframe") == 0)
        {
            ret = clntRoamInst->get_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_SelfSteerTimeframe(stMsgData);
        }
       /* else if (strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_SelfSteerBeaconsMissedTime") == 0)
        {
            ret = clntRoamInst->get_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_SelfSteerBeaconsMissedTime(stMsgData);
        }*/
        else if(strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_APcontrolThresholdLevel") == 0)
        {
            ret = clntRoamInst->get_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_APcontrolThresholdLevel(stMsgData);
        }
        else if(strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_APcontrolTimeframe") == 0)
        {
            ret = clntRoamInst->get_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_APcontrolTimeframe(stMsgData);
        }
        else if(strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.SelfSteer_OverrideEnable") == 0)
        {
            ret = clntRoamInst->get_Device_WiFi_X_Rdkcentral_clientRoaming_SelfSteerOverride(stMsgData);
        }
        else if(strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_BackOffTime") == 0)
        {
            ret = clntRoamInst->get_Device_WiFi_X_Rdkcentral_clientRoaming_postAssnBackOffTime(stMsgData);
        }
        else if(strcasecmp(stMsgData->paramName,"Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.80211kvrEnable") == 0)
        {
            ret = clntRoamInst->get_Device_WiFi_X_Rdkcentral_clientRoaming_80211kvrEnable(stMsgData);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]  Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
            stMsgData->faultCode = fcInvalidParameterName;
            ret = NOK;
        }
    }
#endif
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]  Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOK;
    }

    return ret;
}

int WiFiReqHandler::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] TODO Entering  Parameter value = %s \n", __FILE__, __FUNCTION__,stMsgData->paramName);
    // TODO: Retrieve notification value from DeviceInfo structure for given parameter
    return ret;
}

int WiFiReqHandler::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{

        int ret = NOT_HANDLED;
/*	int instanceNumber = 0;
	hostIf_WiFi::getLock();
	// TODO: Set notification value from DeviceInfo structure for given parameter
	RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] WiFiReqHandler get para as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
	hostIf_WiFi *pIface = hostIf_WiFi::getInstance(instanceNumber);
	stMsgData->instanceNum = instanceNumber;
	if(!pIface)
	{
	    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] pIface is failed. For param as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
	    hostIf_WiFi::releaseLock();
	    return NOK;
	}
	GHashTable* notifyhash = pIface->getNotifyHash();
	if(notifyhash != NULL)
	{
	    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] notifyhash is not Null %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
	    int notifyvalue = get_int(stMsgData->paramValue);
	    g_hash_table_insert(notifyhash,stMsgData->paramName,(gpointer) notifyvalue);
	    ret = OK;
	}
	else
	{
	   RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] notifyhash is Null %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
	   ret = NOK;
	}
	hostIf_WiFi::releaseLock();*/
	return ret;
}
void WiFiReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
}

void WiFiReqHandler::checkForUpdates()
{
    LOG_ENTRY_EXIT;

    if (NULL == mUpdateCallback)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s:%s] mUpdateCallback is NULL\n", __FILE__, __FUNCTION__);
        return;
    }

    hostIf_WiFi *pIface = hostIf_WiFi::getInstance(1);
    if (NULL == pIface)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s:%s] hostIf_WiFi::getInstance(1) returned NULL\n", __FILE__, __FUNCTION__);
        return;
    }
}

#endif /* #ifdef USE_WIFI_PROFILE */


/** @} */
/** @} */
