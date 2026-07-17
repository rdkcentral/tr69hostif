/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2024 RDK Management
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
 * @file thunder_plugin_stub.cpp
 *
 * Stub implementations of invokeThunderPlugin* helper functions used by the
 * STBService Thunder components.  Tests configure canned responses via the
 * ThunderStub namespace before exercising the production code under test.
 *
 * Also provides stub implementations of:
 *   - put_boolean / get_boolean   (normally in hostIf_utils.cpp)
 *   - getStringValue              (normally in hostIf_utils.cpp)
 *   - IARM_Bus_BroadcastEvent     (no-op, only needed for doUpdates path)
 */

#include <string>
#include <map>
#include <cstring>
#include "hostIf_utils.h"
#include "hostIf_main.h"

/* ------------------------------------------------------------------ */
/* Per-test response store                                              */
/* ------------------------------------------------------------------ */

namespace ThunderStub {

struct BoolResp   { bool success; bool value; };
struct StringResp { bool success; std::string value; };
struct IntResp    { bool success; int value; };
struct ULongResp  { bool success; unsigned long value; };

static std::map<std::string, BoolResp>   g_bool;
static std::map<std::string, StringResp> g_string;
static std::map<std::string, IntResp>    g_int;
static std::map<std::string, ULongResp>  g_ulong;

/** Set the bool result returned when the given Thunder method is invoked. */
void setBool(const std::string& method, bool success, bool value = false)
{
    g_bool[method] = {success, value};
}

/** Set the string result returned when the given Thunder method is invoked. */
void setString(const std::string& method, bool success, const std::string& value = "")
{
    g_string[method] = {success, value};
}

/** Set the int result returned when the given Thunder method is invoked. */
void setInt(const std::string& method, bool success, int value = 0)
{
    g_int[method] = {success, value};
}

/** Set the unsigned long result returned when the given Thunder method is invoked. */
void setULong(const std::string& method, bool success, unsigned long value = 0UL)
{
    g_ulong[method] = {success, value};
}

/** Clear all configured responses (call from test SetUp). */
void clear()
{
    g_bool.clear();
    g_string.clear();
    g_int.clear();
    g_ulong.clear();
}

} /* namespace ThunderStub */

/* ------------------------------------------------------------------ */
/* invokeThunderPlugin* stub implementations                           */
/* ------------------------------------------------------------------ */

bool invokeThunderPluginMethod(const std::string& /*method*/,
                               const std::string& /*paramsJson*/,
                               std::string& /*response*/)
{
    /* Not used directly by STBService components — they call the typed helpers. */
    return false;
}

bool invokeThunderPluginMethodAndExtractBoolField(const std::string& method,
    const std::string& /*paramsJson*/, const std::string& /*fieldName*/, bool& value)
{
    auto it = ThunderStub::g_bool.find(method);
    if (it == ThunderStub::g_bool.end())
        return false;
    value = it->second.value;
    return it->second.success;
}

bool invokeThunderPluginMethodAndExtractStringField(const std::string& method,
    const std::string& /*paramsJson*/, const std::string& /*fieldName*/, std::string& value)
{
    auto it = ThunderStub::g_string.find(method);
    if (it == ThunderStub::g_string.end())
        return false;
    value = it->second.value;
    return it->second.success;
}

bool invokeThunderPluginMethodAndExtractNumberField(const std::string& method,
    const std::string& /*paramsJson*/, const std::string& /*fieldName*/, int& value)
{
    auto it = ThunderStub::g_int.find(method);
    if (it == ThunderStub::g_int.end())
        return false;
    value = it->second.value;
    return it->second.success;
}

bool invokeThunderPluginMethodAndExtractULongField(const std::string& method,
    const std::string& /*paramsJson*/, const std::string& /*fieldName*/, unsigned long& value)
{
    auto it = ThunderStub::g_ulong.find(method);
    if (it == ThunderStub::g_ulong.end())
        return false;
    value = it->second.value;
    return it->second.success;
}

bool invokeThunderPluginMethodAndExtractDelimitedStringArrayField(
    const std::string& method, const std::string& /*paramsJson*/,
    const std::string& /*fieldName*/, const std::string& /*delimiter*/,
    std::string& value)
{
    auto it = ThunderStub::g_string.find(method);
    if (it == ThunderStub::g_string.end())
        return false;
    value = it->second.value;
    return it->second.success;
}

bool invokeThunderPluginMethodAndExtractScalarStringResult(const std::string& method,
    const std::string& /*paramsJson*/, std::string& value)
{
    auto it = ThunderStub::g_string.find(method);
    if (it == ThunderStub::g_string.end())
        return false;
    value = it->second.value;
    return it->second.success;
}

bool invokeThunderPluginMethodAndExtractScalarBoolResult(const std::string& method,
    const std::string& /*paramsJson*/, bool& value)
{
    auto it = ThunderStub::g_bool.find(method);
    if (it == ThunderStub::g_bool.end())
        return false;
    value = it->second.value;
    return it->second.success;
}

/* ------------------------------------------------------------------ */
/* hostIf_utils helpers (normally in hostIf_utils.cpp)                 */
/* ------------------------------------------------------------------ */

bool get_boolean(const char *ptr)
{
    return ptr && (*ptr != '\0') && (*ptr != '0');
}

void put_boolean(char *ptr, bool val)
{
    if (ptr)
        *ptr = val ? '1' : '0';
}

int get_int(const char *ptr)
{
    if (!ptr) return 0;
    return *reinterpret_cast<const int *>(ptr);
}

void put_int(char *ptr, int val)
{
    if (ptr)
        *reinterpret_cast<int *>(ptr) = val;
}

uint get_uint(char *ptr)
{
    if (!ptr) return 0u;
    return *reinterpret_cast<const unsigned int *>(ptr);
}

void put_uint(char *ptr, uint val)
{
    if (ptr)
        *reinterpret_cast<unsigned int *>(ptr) = val;
}

int get_ulong(const char *ptr)
{
    if (!ptr) return 0;
    return static_cast<int>(*reinterpret_cast<const unsigned long *>(ptr));
}

void put_ulong(char *ptr, unsigned long val)
{
    if (ptr)
        *reinterpret_cast<unsigned long *>(ptr) = val;
}

std::string getStringValue(HOSTIF_MsgData_t *stMsgData)
{
    if (!stMsgData)
        return "";
    return std::string(stMsgData->paramValue,
                       static_cast<size_t>(stMsgData->paramLen));
}

void putValue(HOSTIF_MsgData_t *stMsgData, const std::string &value)
{
    if (!stMsgData)
        return;
    strncpy(stMsgData->paramValue, value.c_str(), TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    stMsgData->paramValue[TR69HOSTIFMGR_MAX_PARAM_LEN - 1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen  = static_cast<int>(value.size());
}

/* ------------------------------------------------------------------ */
/* IARM stub (no-op)                                                    */
/* ------------------------------------------------------------------ */

#include "libIBus.h"

IARM_Result_t IARM_Bus_BroadcastEvent(const char * /*ownerName*/,
                                       IARM_EventId_t /*eventId*/,
                                       void * /*arg*/,
                                       size_t /*argLen*/)
{
    return IARM_RESULT_SUCCESS;
}
