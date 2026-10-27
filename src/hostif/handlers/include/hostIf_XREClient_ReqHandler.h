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
 * @file hostIf_XREClient_ReqHandler.h
 * @brief Minimal handler for Device.X_COMCAST-COM_Xcalibur.Client.xconfCheckNow parameter.
 * @note All XRE/Xcalibur profile features except xconfCheckNow have been removed.
 */

/**
 * @defgroup TR-069HOSTIF_XRECLIENT_REQHANDLER_CLASSES XREClient RequestHandler Public Classes
 * Describe the minimal classes used in TR-069 xconfCheckNow request handler.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_HANDLER
 */

/**
*  HOST-IF is a platform agnostic Inter-process communication (IPC) interface. It allows
*  applications to communicate with each other by sending Events or invoking Remote
*  Procedure Calls. The common programming APIs offered by the RDK IARM-Bus interface is
*  independent of the operating system or the underlying IPC mechanism.
*
*/


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIF_XRECLIENT_REQHANDLER_H_
#define HOSTIF_XRECLIENT_REQHANDLER_H_

#include "hostIf_msgHandler.h"
#include "hostIf_main.h"
#include "secure_wrapper.h"

/**
 * @brief Minimal handler for xconfCheckNow parameter only.
 * @ingroup TR-069HOSTIF_XRECLIENT_REQHANDLER_CLASSES
 */
class XREClientReqHandler : public msgHandler
{
    XREClientReqHandler() {};
    ~XREClientReqHandler() {};
    static class XREClientReqHandler *pInstance;
    static GMutex m_mutex;
    static void getLock();
public:
    virtual bool init();
    virtual bool unInit();
    virtual int handleSetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    static void releaseLock();
    static void checkForUpdates();
    static void reset();
    static msgHandler* getInstance();
};

int set_Device_X_COMCAST_COM_Xcalibur_Client_xconfCheckNow(HOSTIF_MsgData_t *stMsgData);
int get_Device_X_COMCAST_COM_Xcalibur_Client_xconfCheckNow(HOSTIF_MsgData_t *stMsgData);
#endif /* HOSTIF_XRECLIENT_REQHANDLER_H_ */
/* End of HOSTIF_XRECLIENT_REQHANDLER_H_ doxygen group */
/**
 * @}
 */


/** @} */
/** @} */
