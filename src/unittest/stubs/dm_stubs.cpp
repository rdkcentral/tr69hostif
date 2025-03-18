/*
 * Copyright 2024 Comcast Cable Communications Management, LLC
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
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libIBus.h"
#include "webpa_parameter.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_main.h"
#include "XrdkCentralComRFCStore.h"
#include "rbus.h"
#include "power_controller.h"

rbusHandle_t rbusHandle = NULL;
int hostIf_GetMsgHandler(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    XRFCStore* m_rfcStore;
    m_rfcStore = XRFCStore::getInstance();
    ret = m_rfcStore->getValue(stMsgData);
    return ret;
}


int hostIf_SetMsgHandler(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    return ret;
}

/**
 * @brief getValues interface returns the parameter values.
 *
 * getValues supports an option to pass wildward parameters. This can be achieved by passing an object name followed by '.'
 * instead of parameter name.
 *
 * @param[in] paramName List of Parameters.
 * @param[in] paramCount Number of parameters.
 * @param[out] paramValArr Two dimentional array of parameter name/value pairs.
 * @param[out] retValCount List of "number of parameters" for each input paramName. Usually retValCount will be 1 except
 * for wildcards request where it represents the number of param/value pairs retrieved for the particular wildcard parameter.
 * @param[out] retStatus List of Return status.
 */
void getValues (const char *paramName[], const unsigned int paramCount, param_t ***paramValArr,size_t **retValCount, WDMP_STATUS **retStatus)
{

}
/**
 * @brief setValues interface sets the parameter value.
 *
 * @param[in] paramVal List of Parameter name/value pairs.
 * @param[in] paramCount Number of parameters.
 * @param[out] retStatus List of Return status.
 */
void setValues(const ParamVal paramVal[], const unsigned int paramCount, const WEBPA_SET_TYPE setType, money_trace_spans *timeSpan, WDMP_STATUS **retStatus,char * transaction_id)
{

}

IARM_Result_t IARM_Bus_BroadcastEvent(const char *ownerName, IARM_EventId_t eventId, void *arg, size_t argLen)
{
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t IARM_Bus_Init(const char* name)
{
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t IARM_Bus_Connect()
{
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t IARM_Bus_IsConnected(const char* memberName, int* isRegistered)
{
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t IARM_Bus_RegisterEventHandler(const char* ownerName, IARM_EventId_t eventId, IARM_EventHandler_t handler)
{
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t IARM_Bus_UnRegisterEventHandler(const char* ownerName, IARM_EventId_t eventId)
{
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t IARM_Bus_RemoveEventHandler(const char* ownerName, IARM_EventId_t eventId, IARM_EventHandler_t handler)
{
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t IARM_Bus_Call(const char* ownerName, const char* methodName, void* arg, size_t argLen)
{
    return IARM_RESULT_SUCCESS;
}


void PowerController_Init()
{

}

void PowerController_Term()
{

}

uint32_t PowerController_Connect()
{
    return POWER_CONTROLLER_ERROR_NONE;
}

uint32_t PowerController_RegisterPowerModeChangedCallback(PowerController_PowerModeChangedCb callback, void* userdata)
{
    return POWER_CONTROLLER_ERROR_NONE;
}

uint32_t PowerController_UnRegisterPowerModeChangedCallback(PowerController_PowerModeChangedCb callback)
{
    return POWER_CONTROLLER_ERROR_NONE;
}

uint32_t PowerController_GetPowerState(PowerController_PowerState_t* currentState, PowerController_PowerState_t* previousState)
{
    return POWER_CONTROLLER_ERROR_NONE;
}

rbusValue_t rbusValue_Init(rbusValue_t* value)
{
    return NULL;
}

void rbusValue_SetString(rbusValue_t value, char const* var)
{
    return;
}

void rbusObject_SetValue(rbusObject_t object, char const* var, rbusValue_t value)
{
    return;
}

void rbusValue_Release(rbusValue_t value)
{
    return;
}

void rbusObject_Release(rbusObject_t object)
{
    return;
}

void rbusValue_SetInt32(rbusValue_t value, int32_t i32)
{
    return;
}

rbusError_t rbusMethod_Invoke(rbusHandle_t handle, char const* value, rbusObject_t object, rbusObject_t* objectpt)
{
    return RBUS_ERROR_SUCCESS;
}

const char* rbusError_ToString(rbusError_t e)
{
    #define rbusError_String(E, S) case E: s = S; break;

  char const * s = NULL;
  switch (e)
  {
    rbusError_String(RBUS_ERROR_SUCCESS, "ok");
    rbusError_String(RBUS_ERROR_BUS_ERROR, "generic error");
    rbusError_String(RBUS_ERROR_INVALID_INPUT, "invalid input");
    rbusError_String(RBUS_ERROR_NOT_INITIALIZED, "not initialized");
    rbusError_String(RBUS_ERROR_OUT_OF_RESOURCES, "out of resources");
    rbusError_String(RBUS_ERROR_DESTINATION_NOT_FOUND, "destination not found");
    rbusError_String(RBUS_ERROR_DESTINATION_NOT_REACHABLE, "destination not reachable");
    rbusError_String(RBUS_ERROR_DESTINATION_RESPONSE_FAILURE, "destination response failure");
    rbusError_String(RBUS_ERROR_INVALID_RESPONSE_FROM_DESTINATION, "invalid response from destination");
    rbusError_String(RBUS_ERROR_INVALID_OPERATION, "invalid operation");
    rbusError_String(RBUS_ERROR_INVALID_EVENT, "invalid event");
    rbusError_String(RBUS_ERROR_INVALID_HANDLE, "invalid handle");
    rbusError_String(RBUS_ERROR_SESSION_ALREADY_EXIST, "session already exists");
    rbusError_String(RBUS_ERROR_COMPONENT_NAME_DUPLICATE, "duplicate component name");
    rbusError_String(RBUS_ERROR_ELEMENT_NAME_DUPLICATE, "duplicate element name");
    rbusError_String(RBUS_ERROR_ELEMENT_NAME_MISSING, "name missing");
    rbusError_String(RBUS_ERROR_COMPONENT_DOES_NOT_EXIST, "component does not exist");
    rbusError_String(RBUS_ERROR_ELEMENT_DOES_NOT_EXIST, "element name does not exist");
    rbusError_String(RBUS_ERROR_ACCESS_NOT_ALLOWED, "access denied");
    rbusError_String(RBUS_ERROR_INVALID_CONTEXT, "invalid context");
    rbusError_String(RBUS_ERROR_TIMEOUT, "timeout");
    rbusError_String(RBUS_ERROR_ASYNC_RESPONSE, "async operation in progress");
    default:
      s = "unknown error";
  }
  return s;
}

rbusError_t  rbusEvent_Publish(
    rbusHandle_t handle,
    rbusEvent_t* eventData)
{
    return RBUS_ERROR_SUCCESS;
}

rbusObject_t rbusObject_Init(rbusObject_t* object, char const* value)
{
    return NULL;
}

void rbusValue_SetBoolean(rbusValue_t value, bool b)
{
    return;
}
rbusValue_t rbusProperty_GetValue(rbusProperty_t property)
{
    rbusValue_t value;
    rbusValue_Init(&value);
    return value;
}

rbusValueType_t rbusValue_GetType(rbusValue_t value)
{
    return RBUS_NONE;
}

char* rbusValue_ToString(rbusValue_t value, char* buf, size_t buflen)
{
    return (char *)NULL;
}

char const* rbusProperty_GetName(rbusProperty_t property)
{
    return (char *)NULL;
}

void rbusValue_SetUInt32(rbusValue_t value, uint32_t u32)
{
    return;
}
void rbusValue_SetUInt64(rbusValue_t value, uint64_t u64)
{
    return;
}

void rbusProperty_SetValue(rbusProperty_t property, rbusValue_t value)
{
    return;
}

rbusStatus_t rbus_checkStatus(void)
{
    return RBUS_ENABLED;
}

rbusError_t rbus_open(rbusHandle_t* handle, char const* value)
{
    return RBUS_ERROR_SUCCESS;
}

rbusError_t rbus_close(rbusHandle_t handle)
{
    return RBUS_ERROR_SUCCESS;
}

rbusError_t rbus_get( rbusHandle_t handle, char const* name, rbusValue_t* value)
{
    return RBUS_ERROR_SUCCESS;
}

rbusError_t rbus_set(rbusHandle_t handle, char const* name, rbusValue_t value, rbusSetOptions_t* opts)
{
    return RBUS_ERROR_SUCCESS;
}

rbusError_t rbus_regDataElements(rbusHandle_t handle,int numDataElements,rbusDataElement_t *elements)
{
    return RBUS_ERROR_SUCCESS;
}
