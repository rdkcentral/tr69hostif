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
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#ifndef DEVICE_IP_DIAGNOSTICS_IPPING_H_
#define DEVICE_IP_DIAGNOSTICS_IPPING_H_

#include "hostIf_tr69ReqHandler.h"
#include "IniFile.h"

class hostIf_IP_Diagnostics_IPPing
{

public:

    static const char* PROFILE_NAME;
    static hostIf_IP_Diagnostics_IPPing& getInstance ();

    hostIf_IP_Diagnostics_IPPing (hostIf_IP_Diagnostics_IPPing const&) = delete;
    void operator= (hostIf_IP_Diagnostics_IPPing const&) = delete;

    int handleGetMsg (HOSTIF_MsgData_t* stMsgData);
    int handleSetMsg (HOSTIF_MsgData_t* stMsgData);

private:

    IniFile store;
    hostIf_IP_Diagnostics_IPPing ();
};

#endif /* DEVICE_IP_DIAGNOSTICS_IPPING_H_ */


/** @} */
/** @} */
