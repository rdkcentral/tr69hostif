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
 * @file hostIf_XREClient_ReqHandler.cpp
 * @brief Minimal handler for Device.X_COMCAST-COM_Xcalibur.Client.xconfCheckNow parameter.
 * @note All XRE/Xcalibur profile features except xconfCheckNow have been removed.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_XREClient_ReqHandler.h"

XREClientReqHandler* XREClientReqHandler::pInstance = NULL;
GMutex XREClientReqHandler::m_mutex;

#define XCONF_CHECKNOW_SCRIPT_CMD "backgroundrun /usr/bin/rdkvfwupgrader 0 3 >> /opt/logs/swupdate.log"

msgHandler* XREClientReqHandler::getInstance()
{

    if(!pInstance)
        pInstance = new XREClientReqHandler();
    return pInstance;
}

/**
 * @brief This function is use to initialize. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successfully .
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_XRECLIENT_REQHANDLER_CLASSES
 */
bool XREClientReqHandler::init()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

/**
 * @brief This function is use to uninitialized. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successfully .
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_XRECLIENT_REQHANDLER_CLASSES
 */
bool XREClientReqHandler::unInit()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}


void XREClientReqHandler::reset()
{
    getLock();
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    releaseLock();
}

void XREClientReqHandler::getLock()
{
    g_mutex_init(&XREClientReqHandler::m_mutex);
    g_mutex_lock(&XREClientReqHandler::m_mutex);

}

void XREClientReqHandler::releaseLock()
{
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s] Unlocking mutex..\n", __FUNCTION__, __FILE__);
    g_mutex_unlock(&XREClientReqHandler::m_mutex);
}

/**
 * @brief Handle set message requests for xconfCheckNow parameter.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to set data to the device.
 * @ingroup TR-069HOSTIF_XRECLIENT_REQHANDLER_CLASSES
 */
int XREClientReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;

    stMsgData->instanceNum = 0;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);

    getLock();

    if (strcasecmp (stMsgData->paramName, "Device.X_COMCAST-COM_Xcalibur.Client.xconfCheckNow") == 0)
    {
        ret = set_Device_X_COMCAST_COM_Xcalibur_Client_xconfCheckNow (stMsgData);
    }
    else
    {
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOT_HANDLED;
    }
    releaseLock();

    return ret;
}

/**
 * @brief Handle get message requests for xconfCheckNow parameter.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to get data from the device.
 * @ingroup TR-069HOSTIF_XRECLIENT_REQHANDLER_CLASSES
 */
int XREClientReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;

    getLock();

    stMsgData->instanceNum = 0;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);

    if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.xconfCheckNow") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_xconfCheckNow(stMsgData);
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOK;
    }

    releaseLock();

    return ret;
}

int XREClientReqHandler::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    getLock();
    // TODO: Retrieve notification value from DeviceInfo structure for given parameter
    releaseLock();
    return ret; 
}       
     
int XREClientReqHandler::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{           
    int ret = NOT_HANDLED;
    getLock();
    // TODO: Set notification value from DeviceInfo structure for given parameter
    releaseLock();
    return ret;
}

/**
 * @brief Set xconfCheckNow parameter to trigger firmware update check.
 * @param[in] stMsgData Message data containing the parameter value.
 * @return OK on success, NOK on failure.
 */
int set_Device_X_COMCAST_COM_Xcalibur_Client_xconfCheckNow(HOSTIF_MsgData_t *stMsgData)
{
    FILE *file = fopen("/tmp/xconfchecknow_val", "w");
    if (file == NULL) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF, "[%s:%s:%d]Device_X_COMCAST_COM_Xcalibur_Client_xconfCheckNow: Error opening file for write.\n",__FILE__,__FUNCTION__,__LINE__);
        return NOK;
    }
    fprintf(file, "%s", stMsgData->paramValue);
    fclose(file);
    if(0 == strncasecmp("TRUE",stMsgData->paramValue ,strlen("TRUE")) || 0 == strncasecmp("CANARY",stMsgData->paramValue ,strlen("CANARY")))
    {
        /*On setting xconfCheckNow,results the device to connect with the XCONF server
            for the purpose of firmware and configuration checks.*/
        if(-1 == v_secure_system(XCONF_CHECKNOW_SCRIPT_CMD))
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF, "[%s:%s:%d]Device_X_COMCAST_COM_Xcalibur_Client_xconfCheckNow: Running checkNow script failed.\n",__FILE__,__FUNCTION__,__LINE__);

            return NOK;
        }

        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF, "[%s:%s:%d]Device_X_COMCAST_COM_Xcalibur_Client_xconfCheckNow: CheckNow Running... \n",__FILE__,__FUNCTION__,__LINE__);
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF, "[%s:%s:%d]Device_X_COMCAST_COM_Xcalibur_Client_xconfCheckNow: \"%s\" Invalid Input. Valid Input is \"TRUE or CANARY\" \n",__FILE__,__FUNCTION__,__LINE__,stMsgData->paramValue);
        return NOK;	    
    }
    return OK;
}

/**
 * @brief Get xconfCheckNow parameter value.
 * @param[out] stMsgData Message data to store the parameter value.
 * @return OK on success, NOK on failure.
 */
int get_Device_X_COMCAST_COM_Xcalibur_Client_xconfCheckNow(HOSTIF_MsgData_t *stMsgData)
{
    FILE *file = fopen("/tmp/xconfchecknow_val", "r");
    if (file == NULL) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF, "[%s:%s:%d]Device_X_COMCAST_COM_Xcalibur_Client_xconfCheckNow: Error opening file for read.\n",__FILE__,__FUNCTION__,__LINE__);
        return NOK;
    }
    if (fscanf(file, "%9s", stMsgData->paramValue) != 1) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF, "[%s:%s:%d]Device_X_COMCAST_COM_Xcalibur_Client_xconfCheckNow: Error reading file.\n",__FILE__,__FUNCTION__,__LINE__);
        fclose(file);
        return NOK;
    }
    fclose(file);
    return OK;
}

/**
 * @brief Check for parameter updates. Minimal implementation.
 * @note XRE-specific parameter update checking has been removed.
 */
void XREClientReqHandler::checkForUpdates()
{
    getLock();
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d] Minimal xconfCheckNow handler.\n", __FUNCTION__, __LINE__);
    releaseLock();
}

/** @} */
/** @} */

