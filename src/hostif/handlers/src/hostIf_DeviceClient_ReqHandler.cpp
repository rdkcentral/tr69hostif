/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2016-2019 RDK Management
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
 * @file hostIf_DeviceClient_ReqHandler.cpp
 * @brief The header file provides HostIf Device client request handler information APIs.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/
//#define HAVE_VALUE_CHANGE_EVENT


#include "hostIf_main.h"
#include "hostIf_DeviceClient_ReqHandler.h"
#include "hostIf_utils.h"
#include "host.hpp"
#include "dsError.h"
#include "libIBus.h"
#include "Device_DeviceInfo.h"
#include "Device_DeviceInfo_Processor.h"
#include "Device_DeviceInfo_ProcessStatus.h"
#include "Device_DeviceInfo_ProcessStatus_Process.h"
#include "hostIf_msgHandler.h"
#include "safec_lib.h"
#ifdef SNMP_ADAPTER_ENABLED
#include "hostIf_SNMPClient_ReqHandler.h"
#include "snmpAdapter.h"
#endif
#ifdef USE_XRDK_BT_PROFILE
#include "XrdkBlueTooth.h"
#endif

DeviceClientReqHandler* DeviceClientReqHandler::pInstance = NULL;
int DeviceClientReqHandler::curNumOfProcess[100] = {0,};
int DeviceClientReqHandler::curNumOfProcessor[10] = {0,};
updateCallback DeviceClientReqHandler::mUpdateCallback = NULL;

msgHandler* DeviceClientReqHandler::getInstance()
{

    if(!pInstance)
        pInstance = new DeviceClientReqHandler();
    return pInstance;
}

void DeviceClientReqHandler::reset()
{
    hostIf_DeviceInfo::getLock();
    memset(curNumOfProcess,0,sizeof(curNumOfProcess));
    memset(curNumOfProcessor,0,sizeof(curNumOfProcessor));
    hostIf_DeviceInfo::releaseLock();
}

/**
 * @brief This function is used to initialize all the current process and processor
 * to '0' using memset.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successful.
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_REQHANDLER_CLASSES
 */
bool DeviceClientReqHandler::init()
{
//    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
//    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    memset(curNumOfProcess,0,sizeof(curNumOfProcess));
    memset(curNumOfProcessor,0,sizeof(curNumOfProcessor));
    return true;
}

/**
 * @brief This function is used to close all the instances of device info, device processor
 * interface, device process status interface.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if it successfully close all the instances.
 * @retval false if not able to close all the instances.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_REQHANDLER_CLASSES
 */
bool DeviceClientReqHandler::unInit()
{
//    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] Device manager DeInitializing\n", __FUNCTION__);
    hostIf_DeviceInfo::closeAllInstances();
    hostIf_DeviceProcessorInterface::closeAllInstances();
    hostIf_DeviceProcessStatusInterface::closeAllInstances();
//    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

/**
 * @brief This function is used to set the Device reset, FirmwareDownload and FirmwareDownload Status.
 * The possible reset states of the device are "Cold", "Factory", "Warehouse" and "Customer".
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successful.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_REQHANDLER_CLASSES
 */
int DeviceClientReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    int instanceNumber = 0;  //CID:84325 - UNINIT
    hostIf_DeviceInfo::getLock();
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);

    if (stMsgData->bsUpdate != HOSTIF_NONE)
    {
        if ( (stMsgData->bsUpdate == HOSTIF_SRC_RFC && stMsgData->requestor == HOSTIF_SRC_RFC) ||
                (stMsgData->bsUpdate == HOSTIF_SRC_ALL && (stMsgData->requestor == HOSTIF_SRC_RFC || stMsgData->requestor == HOSTIF_SRC_WEBPA)) )
        {
            hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
            stMsgData->instanceNum = instanceNumber;
            if(!pIface)
            {
                hostIf_DeviceInfo::releaseLock();
                return NOK;
            }
            ret = pIface->set_xRDKCentralComBootstrap(stMsgData);
        }
        else
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] Not setting the bootstrap param:%s [bsUpdate=%d, requestor=%d]\n", __FUNCTION__, stMsgData->paramName, stMsgData->bsUpdate, stMsgData->requestor);
    }
#ifdef SNMP_ADAPTER_ENABLED
    else if(strncasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDK_SNMP",strlen("Device.DeviceInfo.X_RDK_SNMP"))==0)
     {
          hostIf_snmpAdapter *pIStatus = hostIf_snmpAdapter::getInstance(instanceNumber);
          stMsgData->instanceNum = instanceNumber;
          if(pIStatus){
              ret = pIStatus->set_ValueToSNMPAdapter(stMsgData);
          }
          else{
            ret = NOK;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] hostIf_snmpAdapter::getInstance is NULL for %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
             }
     }
#endif
    else if(strncasecmp(stMsgData->paramName,"Device.DeviceInfo",strlen("Device.DeviceInfo"))==0)
    {
        hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
        stMsgData->instanceNum = instanceNumber;
        if(!pIface)
        {
            hostIf_DeviceInfo::releaseLock();
            return NOK;
        }

        if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_Reset") == 0 ||
                strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_Reset") == 0)
        {
            ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_FirmwareToDownload") == 0 ||
                 strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload") == 0)
        {
            ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareToDownload(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_FirmwareDownloadStatus") == 0 ||
                 strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus") == 0)
        {
            ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadStatus(stMsgData);
        }
        /* Added as per RDK Requirement RDK-12720 */
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol") == 0)
        {
            ret = pIface->set_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadProtocol (stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL") == 0)
        {
            ret = pIface->set_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadURL (stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_PreferredGatewayType") == 0)
        {
            ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName, xOpsDMUploadLogsNow_STR) == 0)
        {
            ret = pIface->set_xOpsDMUploadLogsNow(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName, xOpsDMMoCALogEnabled) == 0)
        {
            ret = pIface->set_xOpsDMMoCALogEnabled(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName, xOpsDMMoCALogPeriod) == 0)
        {
            ret = pIface->set_xOpsDMMoCALogPeriod(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,xOpsReverseSshTrigger_STR) == 0)
        {
            ret = pIface->set_xOpsReverseSshTrigger(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,xOpsReverseSshArgs_STR) == 0)
        {
            ret = pIface->set_xOpsReverseSshArgs(stMsgData);
        }
        else if (strncmp(stMsgData->paramName,X_OPS_RPC_PROFILE, strlen(X_OPS_RPC_PROFILE)) == 0)
        {
            ret = pIface->set_xOpsRPC_Profile(stMsgData);
        }
        else if (strncasecmp(stMsgData->paramName,xOpsForwardSshEnable_STR,strlen(xOpsForwardSshEnable_STR)) == 0)
        {
            ret = pIface->set_xOpsDeviceMgmtForwardSSHEnable(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,X_RDKDownloadManager_InstallPackage) == 0)
        {
            ret = pIface->set_xRDKDownloadManager_InstallPackage(stMsgData);
        }
	else if (strcasecmp(stMsgData->paramName,X_RDKDownloadManager_DownloadStatus) == 0)
        {
            ret = pIface->set_xRDKDownloadManager_DownloadStatus(stMsgData);
        } 
        else if (strcasecmp(stMsgData->paramName,IPREMOTE_SUPPORT) == 0)
        {
            ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,PARTNER_ID) == 0)
        {
            ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,XRPOLLING_ACTION) == 0)
        {
            ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_XRPollingAction(stMsgData);
            if(OK == ret) {
                notifyIARM(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)stMsgData->paramName, stMsgData->paramValue, stMsgData->paramtype);
            }
        }
        else if (strstr(stMsgData->paramName,TR181_RFC_PREFIX) != NULL)
        {
            RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] parameter %s is being set with value %s \n", __FUNCTION__, stMsgData->paramName, stMsgData->paramValue);
            ret = pIface->set_xRDKCentralComRFC(stMsgData);
        }
#ifdef USE_XRDK_BT_PROFILE
        else if(strncasecmp(stMsgData->paramName,X_BT_ROOT_OBJ,strlen(X_BT_ROOT_OBJ))==0)
        {
            hostIf_DeviceInfoRdk_xBT *pIface = hostIf_DeviceInfoRdk_xBT::getInstance();

            if(!pIface)  {
                hostIf_DeviceInfo::releaseLock();
                return NOK;
            }
            ret = pIface->handleSetMsg(stMsgData);
        }
#endif
#ifdef USE_HWSELFTEST_PROFILE
        else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.ExecuteTest"))
        {
            ret = pIface->set_xOpsDeviceMgmt_hwHealthTest_ExecuteTest(stMsgData);
        }
        else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.SetTuneType"))
        {
            ret = pIface->set_xOpsDeviceMgmt_hwHealthTest_SetTuneType(stMsgData);
        }
        else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.ExecuteTuneTest"))
        {
            ret = pIface->set_xOpsDeviceMgmt_hwHealthTest_ExecuteTuneTest(stMsgData);
        }
#endif /* USE_HWSELFTEST_PROFILE */
        else if (!strcasecmp(stMsgData->paramName, xFirmwareDownloadNow_STR))
        {
            ret = pIface->set_xFirmwareDownloadNow(stMsgData);
        }
        else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadUseCodebig"))
        {
            ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadUseCodebig(stMsgData);
        }
        else if (!strcasecmp(stMsgData->paramName, FWDNLD_DEFER_REBOOT))
        {
            ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadDeferReboot(stMsgData);
        }
        else if (!strcasecmp(stMsgData->paramName, IUI_VERSION))
        {
            ret = pIface->set_Device_DeviceInfo_IUI_Version(stMsgData);
        }
        else
        {
            ret = NOK;
            stMsgData->faultCode = fcAttemptToSetaNonWritableParameter;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed, since Attempt To Set a NonWritable Parameter \"%s\"\n", __FUNCTION__, stMsgData->paramName);
        }

        if( ret == OK )
        {
            stMsgData->faultCode = fcNoFault;
        }

    }
    hostIf_DeviceInfo::releaseLock();
    return ret;
}

/**
 * @brief This function is used to get the Process information such as "PID", "Command"
 * "Size", "Priority", "CPUTime" and "State". It is also used to get the processor information,
 * process status such as total number of active process entries and CPUUsage and device info
 * such as "BootStatus", "Manufacturer", "ManufacturerOUI", "ModelName", "Description", "ProductClass"
 * "SerialNumber", "HardwareVersion", "SoftwareVersion", "AdditionalHardwareVersion" etc..
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is gets the data successfully.
 * @retval ERR_INTERNAL_ERROR if not able to get the data from the device.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_REQHANDLER_CLASSES
 */
int DeviceClientReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    int instanceNumber = 0;
    hostIf_DeviceInfo::getLock();
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);

    if (stMsgData->bsUpdate != HOSTIF_NONE)
    {
        hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
        stMsgData->instanceNum = instanceNumber;
        if(!pIface)
        {
            hostIf_DeviceInfo::releaseLock();
            return NOK;
        }
        ret = pIface->get_xRDKCentralComBootstrap(stMsgData);
    }
    else if(matchComponent(stMsgData->paramName,"Device.DeviceInfo.ProcessStatus.Process",&pSetting,instanceNumber))
    {
        hostIf_DeviceProcess *pIfaceProcess = hostIf_DeviceProcess::getInstance(instanceNumber);
        stMsgData->instanceNum = instanceNumber;
        if(!pIfaceProcess)
        {
            hostIf_DeviceInfo::releaseLock();
            return NOK;
        }
        if(strcasecmp(pSetting,"PID") == 0)
        {
            ret = pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_PID(stMsgData);
        }
        else if(strcasecmp(pSetting,"Command") == 0)
        {
            ret = pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_Command(stMsgData);
        }
        else if(strcasecmp(pSetting,"Size") == 0)
        {
            ret = pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_Size(stMsgData);
        }
        else if(strcasecmp(pSetting,"Priority") == 0)
        {
            ret = pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_Priority(stMsgData);
        }
        else if(strcasecmp(pSetting,"CPUTime") == 0)
        {
            ret = pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_CPUTime(stMsgData);
        }
        else if(strcasecmp(pSetting,"State") == 0)
        {
            ret = pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_State(stMsgData);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
            stMsgData->faultCode = fcInvalidParameterName;
            ret = NOK;
        }
    }
    else if(matchComponent(stMsgData->paramName,"Device.DeviceInfo.Processor",&pSetting,instanceNumber))
    {
        hostIf_DeviceProcessorInterface *pIfaceProcessor = hostIf_DeviceProcessorInterface::getInstance(instanceNumber);
        if(!pIfaceProcessor)
        {
            hostIf_DeviceInfo::releaseLock();
            return NOK;
        }
        stMsgData->instanceNum = instanceNumber;
        if(strcasecmp(pSetting,"Architecture")==0)
        {
            ret = pIfaceProcessor->get_Device_DeviceInfo_Processor_Architecture(stMsgData);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
            stMsgData->faultCode = fcInvalidParameterName;
            ret = NOK;
        }
    }
    else if(strncasecmp(stMsgData->paramName,"Device.DeviceInfo.ProcessStatus",strlen("Device.DeviceInfo.ProcessStatus"))==0)
    {
        hostIf_DeviceProcessStatusInterface *pIfaceStatus = hostIf_DeviceProcessStatusInterface::getInstance(instanceNumber);
        stMsgData->instanceNum = instanceNumber;
        if(!pIfaceStatus)
        {
            hostIf_DeviceInfo::releaseLock();
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] pIfaceStatus is NULL for %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
            return NOK;
        }
        if(strcasecmp(stMsgData->paramName,"Device.DeviceInfo.ProcessStatus.ProcessNumberOfEntries")==0)
        {
            ret = hostIf_DeviceProcess::getNumOfProcessEntries(stMsgData);
        }
        else if(strcasecmp(stMsgData->paramName,"Device.DeviceInfo.ProcessStatus.CPUUsage")==0)
        {
            ret = pIfaceStatus->get_Device_DeviceInfo_ProcessStatus_CPUUsage(stMsgData);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
            stMsgData->faultCode = fcInvalidParameterName;
            ret = NOK;
        }
    }
    else if(strncasecmp(stMsgData->paramName,xOpsForwardSshEnable_STR,strlen(xOpsForwardSshEnable_STR)) == 0)
    {
        hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
        ret = pIface->get_xOpsDeviceMgmtForwardSSHEnable(stMsgData);
    }
    else if(strncasecmp(stMsgData->paramName,xOpsReverseSshStatus_STR,strlen(xOpsReverseSshStatus_STR)) == 0)
    {
        hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
        ret = pIface->get_xOpsReverseSshStatus(stMsgData);
    }

    else if(strncasecmp(stMsgData->paramName,APPARMOR_BLOCKLIST_PROCESS,strlen(APPARMOR_BLOCKLIST_PROCESS)) == 0)
    {
        hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
        ret = pIface->get_ApparmorBlockListStatus(stMsgData);
    }

#ifdef USE_XRDK_BT_PROFILE
    else if(strncasecmp(stMsgData->paramName,X_BT_ROOT_OBJ,strlen(X_BT_ROOT_OBJ))==0)
    {
        hostIf_DeviceInfoRdk_xBT *pIface = hostIf_DeviceInfoRdk_xBT::getInstance();

        if(!pIface)  {
            hostIf_DeviceInfo::releaseLock();
            return NOK;
        }
        ret = pIface->handleGetMsg(stMsgData);
    }
#endif
#ifdef SNMP_ADAPTER_ENABLED
    else if(strncasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDK_SNMP",strlen("Device.DeviceInfo.X_RDK_SNMP"))==0)
        {
          hostIf_snmpAdapter *pIStatus = hostIf_snmpAdapter::getInstance(instanceNumber);
          stMsgData->instanceNum = instanceNumber;
          if(pIStatus){
              ret = pIStatus->get_ValueFromSNMPAdapter(stMsgData);
          }
          else{
            ret = NOK;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] hostIf_snmpAdapter::getInstance is NULL for %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
             }
        }
#endif
    else if(strncasecmp(stMsgData->paramName,"Device.DeviceInfo",strlen("Device.DeviceInfo"))==0)
    {
        hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
        stMsgData->instanceNum = instanceNumber;
        if(!pIface)
        {
            hostIf_DeviceInfo::releaseLock();
            return NOK;
        }
        if(strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM.BootStatus") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_BootStatus(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_PreferredGatewayType") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.Manufacturer") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_Manufacturer(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.ManufacturerOUI") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_ManufacturerOUI(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.ModelName") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_ModelName(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.Description") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_Description(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.ProductClass") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_ProductClass(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.SerialNumber") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_SerialNumber(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.HardwareVersion") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_HardwareVersion(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.SoftwareVersion") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_SoftwareVersion(stMsgData);
        }
	else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.MigrationPreparer.MigrationReady") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_MigrationPreparer_MigrationReady(stMsgData);
        }	
	else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.Migration.MigrationStatus") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_Migration_MigrationStatus(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,IUI_VERSION) == 0)
        {
            ret = pIface->get_Device_DeviceInfo_IUI_Version(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.AdditionalHardwareVersion") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_AdditionalHardwareVersion(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.AdditionalSoftwareVersion") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_AdditionalSoftwareVersion(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.ProvisioningCode") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_ProvisioningCode(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.UpTime") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_UpTime(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.FirstUseDate") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_FirstUseDate(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_STB_MAC") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_STB_MAC(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_STB_IP") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_STB_IP(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_FirmwareFilename") == 0 ||
                 strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareFilename") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_FirmwareToDownload") == 0 ||
                 strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareToDownload(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_FirmwareDownloadStatus") == 0 ||
                 strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadStatus(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadURL (stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadProtocol(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadUseCodebig") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadUseCodebig(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadPercent") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadPercent (stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareUpdateState") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareUpdateState (stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_RDKVersion") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_SoftwareVersion(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_Reset") == 0 ||
                 strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_Reset") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_PowerStatus") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_PowerStatus(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.VendorConfigFileNumberOfEntries") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_VendorConfigFileNumberOfEntries(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.SupportedDataModelNumberOfEntries") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_SupportedDataModelNumberOfEntries(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.ProcessorNumberOfEntries") == 0)
        {
            ret = hostIf_DeviceInfo::get_Device_DeviceInfo_ProcessorNumberOfEntries(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.VendorLogFileNumberOfEntries") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_VendorLogFileNumberOfEntries(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.MemoryStatus.Total") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_MemoryStatus_Total(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.MemoryStatus.Free") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_MemoryStatus_Free(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM.CPUTemp") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_CPUTemp(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,xOpsDMLogsUploadStatus_STR) == 0)
        {
            ret = pIface->get_xOpsDMLogsUploadStatus(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName, xOpsDMMoCALogEnabled) == 0)
        {
            ret = pIface->get_xOpsDMMoCALogEnabled(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName, xOpsDMMoCALogPeriod) == 0)
        {
            ret = pIface->get_xOpsDMMoCALogPeriod(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,xOpsDMUploadLogsNow_STR) == 0)
        {
            ret = pIface->get_xOpsDMUploadLogsNow(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,IPREMOTE_SUPPORT) == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,IPREMOTE_IPADDRESS) == 0)
        {
            ret= pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportIpaddress(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,IPREMOTE_MACADDRESS) == 0)
        {
            ret= pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportMACaddress(stMsgData);
        }
        else if ( !strcasecmp(stMsgData->paramName, PARTNER_ID) )
        {
            ret= pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,XRPOLLING_ACTION) == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_XRPollingAction(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,XRDK_BOOT_TIME) == 0)
        {
            ret = pIface->get_X_RDKCENTRAL_COM_BootTime(stMsgData);
        } else if (strstr(stMsgData->paramName,TR181_RFC_PREFIX) != NULL)
        {
            // RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] requst for parameter: %s of type: %d \n", __FUNCTION__, stMsgData->paramName, stMsgData->paramtype);
            ret = pIface->get_xRDKCentralComRFC(stMsgData);
            // RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] return for parameter: %s , is: %s \n", __FUNCTION__, stMsgData->paramName, stMsgData->paramValue);
        }
#ifdef USE_HWSELFTEST_PROFILE
        else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.Results"))
        {
            ret = pIface->get_xOpsDeviceMgmt_hwHealthTest_Results(stMsgData);
        }
        else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTestTune.TuneResults"))
        {
            ret = pIface->get_xOpsDeviceMgmt_hwHealthTestTune_TuneResults(stMsgData);
        }
#endif /* USE_HWSELFTEST_PROFILE */
        else if (strncmp(stMsgData->paramName,X_OPS_RPC_PROFILE, strlen(X_OPS_RPC_PROFILE)) == 0)
        {
            ret = pIface->get_xOpsRPC_Profile(stMsgData);
        }
        else if (strncmp(stMsgData->paramName,X_RDKCENTRAL_COM_LastRebootReason, strlen(X_RDKCENTRAL_COM_LastRebootReason)) == 0)
        {
            ret = pIface->get_X_RDKCENTRAL_COM_LastRebootReason(stMsgData);
        }
        else if(strncmp(stMsgData->paramName,X_RDKCENTRAL_COM_Experience, strlen(X_RDKCENTRAL_COM_Experience)) == 0)
        {
            ret = pIface->get_X_RDKCENTRAL_COM_experience(stMsgData);
        }
        else if (!strcasecmp(stMsgData->paramName, FWDNLD_DEFER_REBOOT))
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadDeferReboot(stMsgData);
        }
        else if (!strcasecmp(stMsgData->paramName, X_RDK_FirmwareName))
        {
            ret = pIface->get_X_RDK_FirmwareName(stMsgData);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
            stMsgData->faultCode = fcInvalidParameterName;
            ret = NOK;
        }
    }
    hostIf_DeviceInfo::releaseLock();
    return ret;
}

int DeviceClientReqHandler::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    int instanceNumber = 0;

    hostIf_DeviceInfo::getLock();
    stMsgData->instanceNum = instanceNumber;
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
    if(!pIface)
    {
        hostIf_DeviceInfo::releaseLock();
        return NOK;
    }

    GHashTable* notifyhash = pIface->getNotifyHash();
    if(notifyhash != NULL)
    {
        int* notifyvalue = (int*) g_hash_table_lookup(notifyhash,stMsgData->paramName);
        put_int(stMsgData->paramValue, *notifyvalue);
        stMsgData->paramtype = hostIf_IntegerType;
        ret = OK;
    }
    else
    {
        ret = NOK;
    }
    hostIf_DeviceInfo::releaseLock();
    return ret;
}

int DeviceClientReqHandler::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    errno_t rc = -1;
    int instanceNumber = 0;
    hostIf_DeviceInfo::getLock();
    hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);

    stMsgData->instanceNum = instanceNumber;
    if(!pIface)
    {
        hostIf_DeviceInfo::releaseLock();
        return NOK;
    }
    GHashTable* notifyhash = pIface->getNotifyHash();
    if(notifyhash != NULL)
    {
        int *notifyValuePtr;
        notifyValuePtr = (int*) malloc(1 * sizeof(int));

        // Inserting Notification parameter to Notify Hash Table,
        // Note that neither keys nor values are copied when inserted into the GHashTable, so they must exist for the lifetime of the GHashTable
        // There for allocating a memory for both Param name and param value. This should be freed whenever we disable Notification.
        char *notifyKey = NULL;
        notifyKey = (char*) calloc(sizeof(char),strlen(stMsgData->paramName)+1);
        if((NULL != notifyValuePtr) && (NULL != notifyKey))
        {
            *notifyValuePtr = 1;
            rc=strcpy_s(notifyKey,strlen(stMsgData->paramName)+1,stMsgData->paramName);
            if(rc!=EOK)
            {
                ERR_CHK(rc);
            }
            g_hash_table_insert(notifyhash,notifyKey,notifyValuePtr);
            ret = OK;
        }
        else
        {
            ret = NOK;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] Not able to allocate Notify pointer %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
        }
        free(notifyKey);//CID:86943 - Resource leak
        free(notifyValuePtr);
    }
    else
    {
        ret = NOK;
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] Not able to get notifyhash  %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    }
    hostIf_DeviceInfo::releaseLock();
    return ret;
}


void DeviceClientReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
}



void DeviceClientReqHandler::checkForUpdates()
{
    HOSTIF_MsgData_t msgData;
    int index = 1;
    const char *pSetting;
    int instanceNumber = 0;
    GHashTable* notifyhash = NULL;
    char tmp_buff[TR69HOSTIFMGR_MAX_PARAM_LEN];

    // Update ADD/REMOVE Events
    hostIf_DeviceProcess::getLock();
    index = 1;
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);

    if(hostIf_DeviceProcess::getNumOfProcessEntries(&msgData) == OK)
    {
        int tmpNoDev = get_int(msgData.paramValue);
        char *tmp = NULL;
        tmp = (char *)malloc(TR69HOSTIFMGR_MAX_PARAM_LEN * sizeof(char));
        snprintf(tmp_buff,TR69HOSTIFMGR_MAX_PARAM_LEN,"Device.DeviceInfo.ProcessStatus.Process");
        while(curNumOfProcess[index] > tmpNoDev)
        {
            asprintf(&tmp,"%s.%d.",tmp_buff,tmpNoDev);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE,tmp, NULL, hostIf_IntegerType);
            tmpNoDev++;
        }
        while(curNumOfProcess[index] < tmpNoDev)
        {
            asprintf(&tmp,"%s.",tmp_buff);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_ADD,tmp, NULL, hostIf_IntegerType);
            tmpNoDev--;
        }
		free(tmp);
        curNumOfProcess[index] = get_int(msgData.paramValue);
    }
    hostIf_DeviceProcess::releaseLock();
    hostIf_DeviceInfo::getLock();
    if(hostIf_DeviceInfo::get_Device_DeviceInfo_ProcessorNumberOfEntries(&msgData) == OK)
    {
        int tmpNoDev = get_int(msgData.paramValue);
        char *tmp = NULL;
	    tmp = (char *)malloc(TR69HOSTIFMGR_MAX_PARAM_LEN * sizeof(char));
        snprintf(tmp_buff,TR69HOSTIFMGR_MAX_PARAM_LEN,"Device.DeviceInfo.Processor");
        while(curNumOfProcessor[index] > tmpNoDev)
        {
            asprintf(&tmp,"%s.%d.",tmp_buff,tmpNoDev);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE,tmp, NULL, hostIf_IntegerType);
            tmpNoDev++;
        }
        while(curNumOfProcessor[index] < tmpNoDev)
        {
            asprintf(&tmp,"%s.",tmp_buff);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_ADD,tmp, NULL, hostIf_IntegerType);
            tmpNoDev--;
        }
        curNumOfProcessor[index] = get_int(msgData.paramValue);
        free(tmp);
    }
// Update for Value change events
#ifdef HAVE_VALUE_CHANGE_EVENT

    instanceNumber = 0;
    hostIf_DeviceInfo *dIface = hostIf_DeviceInfo::getInstance(instanceNumber);

    //Get Notify Hash from device Info
    if(NULL != dIface)
    {
        notifyhash = dIface->getNotifyHash();
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Unable to get Device Info Instance\n", __FUNCTION__, __FILE__);
    }

    // Iterate through Ghash Table
    if(NULL != notifyhash)
    {
        GHashTableIter notifyHashIterator;
        gpointer paramName;
        gpointer notifyEnable;
        bool  bChanged;

        g_hash_table_iter_init (&notifyHashIterator, notifyhash);
        while (g_hash_table_iter_next (&notifyHashIterator, &paramName, &notifyEnable))
        {
            int* isNotifyEnabled = (int *)notifyEnable;
            instanceNumber = 0;
            // Getting Update for Device.DeviceInfo.ProcessStatus.Process Parameters
            if(matchComponent((const char*)paramName,"Device.DeviceInfo.ProcessStatus.Process",&pSetting,instanceNumber))
            {
                hostIf_DeviceProcess *pIface = hostIf_DeviceProcess::getInstance(instanceNumber);
                if(pIface)
                {
                    if (strcasecmp(pSetting,"PID") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_ProcessStatus_Process_PID(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"Command") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_ProcessStatus_Process_Command(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"Size") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_ProcessStatus_Process_Size(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"Size") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_ProcessStatus_Process_Size(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"Priority") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_ProcessStatus_Process_Priority(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"CPUTime") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_ProcessStatus_Process_CPUTime(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"State") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_ProcessStatus_Process_State(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                }
            }
            else if(matchComponent((const char*)paramName,"Device.DeviceInfo.Processor",&pSetting,instanceNumber))
            {
                hostIf_DeviceProcessorInterface *pIfaceProcessor = hostIf_DeviceProcessorInterface::getInstance(instanceNumber);
                if(pIfaceProcessor)
                {
                    if (strcasecmp(pSetting,"Architecture") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceProcessor->get_Device_DeviceInfo_Processor_Architecture(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                }
            }
            else if(matchComponent((const char*)paramName,"Device.DeviceInfo.ProcessStatus",&pSetting,instanceNumber))
            {
                hostIf_DeviceProcessStatusInterface *pIfaceStatus = hostIf_DeviceProcessStatusInterface::getInstance(instanceNumber);
                if(pIfaceStatus)
                {
                    if (strcasecmp(pSetting,"CPUUsage") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStatus->get_Device_DeviceInfo_ProcessStatus_CPUUsage(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                }
            }
            else if(matchComponent((const char*)paramName,"Device.DeviceInfo",&pSetting,instanceNumber))
            {
                hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
                if(pIface)
                {
                    if (strcasecmp(pSetting,"Manufacturer") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_Manufacturer(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"ModelName") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_ModelName(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"Description") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_Description(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"ProductClass") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_ProductClass(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"SerialNumber") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_SerialNumber(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"HardwareVersion") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_HardwareVersion(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"SoftwareVersion") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_SoftwareVersion(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"AdditionalSoftwareVersion") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_AdditionalSoftwareVersion(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"ProvisioningCode") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_ProvisioningCode(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"UpTime") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_UpTime(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"FirstUseDate") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_FirstUseDate(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"X_COMCAST-COM_STB_MAC") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_X_COMCAST_COM_STB_MAC(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"X_COMCAST-COM_STB_IP") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_X_COMCAST_COM_STB_IP(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"X_COMCAST-COM_FirmwareFilename") == 0 ||
                            strcasecmp(pSetting,"X_RDKCENTRAL-COM_FirmwareFilename") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"X_COMCAST-COM_Reset") == 0 ||
                            strcasecmp(pSetting,"X_RDKCENTRAL-COM_Reset") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"VendorConfigFileNumberOfEntries") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_VendorConfigFileNumberOfEntries(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"SupportedDataModelNumberOfEntries") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_SupportedDataModelNumberOfEntries(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"VendorLogFileNumberOfEntries") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_VendorLogFileNumberOfEntries(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"VendorLogFileNumberOfEntries") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_DeviceInfo_VendorLogFileNumberOfEntries(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                }
            }
        }
    }

#endif /*HAVE_VALUE_CHANGE_EVENT*/
    hostIf_DeviceInfo::releaseLock();
}


void DeviceClientReqHandler::notifyIARM(IARM_Bus_tr69HostIfMgr_EventId_t event_type, const char* paramName, const char* paramValue, HostIf_ParamType_t paramtype) {
    IARM_Bus_tr69HostIfMgr_EventData_t eventData;
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] DeviceClientReqHandler Notify for Parameter :- %s..\n", __FILE__, __FUNCTION__,paramName);
    strncpy(eventData.paramName,paramName, sizeof(eventData.paramName) -1);  //CID:136301 - Buffer size warning
    eventData.paramName[sizeof(eventData.paramName) -1]  = '\0';

    if(paramValue)
    {
        strncpy(eventData.paramValue, paramValue, sizeof(eventData.paramValue) -1);
        eventData.paramValue[sizeof(eventData.paramValue) -1] = '\0';
    }

    eventData.paramtype = paramtype;
    IARM_Bus_BroadcastEvent(IARM_BUS_TR69HOSTIFMGR_NAME, (IARM_EventId_t) event_type, (void *)&eventData, sizeof(eventData));
}






/** @} */
/** @} */
