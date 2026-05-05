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


#ifndef HOSTIF_UTILS_H_
#define HOSTIF_UTILS_H_

#include <unistd.h>
#include <math.h>
#include <string>
#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include <curl/curl.h>
#include "cJSON.h"

#define REBOOT_SCR "backgroundrun sh /rebootNow.sh -s hostIf_utils"
#define SCR_PATH "/lib/rdk"
#define NTP_FILE_NAME "/opt/persistent/firstNtpTime"
#define FW_DWN_FILE_PATH "/opt/fwdnldstatus.txt"
#define TEMP_FW_DWN_FILE_PATH "/opt/fwdnldstatus_tmp.txt"
#define JSONRPC_URL    "http://127.0.0.1:9998/jsonrpc"

#define BUFF_MAC 18
#define BUFF_MIN_16 16
#define BUFF_LENGTH_16 16
#define BUFF_LENGTH_32 32
#define BUFF_LENGTH_64 64
#define BUFF_LENGTH_128 128
#define BUFF_LENGTH_256 256
#define BUFF_LENGTH_1024 1024
#define BUFF_LENGTH BUFF_LENGTH_1024

#ifdef RDKV_TR69
static const char* NOT_IMPLEMENTED = "Not Implemented";

static const char* STATE_UP = "Up";
static const char* STATE_DOWN = "Down";

static const char* TIME_UNKNOWN = "0001-01-01T00:00:00Z";
static const char* TIME_INFINITY = "9999-12-31T23:59:59Z";
#endif

typedef enum __eSTBResetState
{
    NoReset = 0,
    ColdReset,
    FactoryReset,
    WarehouseReset,
    CustomerReset
} eSTBResetState;

typedef struct
{
    int		enumCode;
    const char	*enumString;
} EnumStringMapper;

class EntryExitLogger
{
    const char* func;
    const char* file;
public:
    EntryExitLogger (const char* func, const char* file);
    ~EntryExitLogger ();
};

#define LOG_ENTRY_EXIT EntryExitLogger logger (__FUNCTION__, __FILE__)

static volatile eSTBResetState gResetState = NoReset;

void setResetState( eSTBResetState );

eSTBResetState getResetState( void );

void triggerResetScript(void);

const char * getStringFromEnum( EnumStringMapper *, int, int );

int getEnumFromString( EnumStringMapper *, int, const char * );

bool matchComponent(const char* pParam, const char *pKey, const char **pSetting, int &instanceNo);

int get_int(const char *ptr);

void put_int(char *ptr, int val);

uint get_uint(char *ptr);

void put_uint(char *ptr, uint val);

bool get_boolean(const char *ptr);

void put_boolean(char *ptr, bool val);

int get_ulong(const char* ptr);

void put_ulong(char *ptr, unsigned long val);

std::string int_to_string(int d);

std::string uint_to_string(uint d);

std::string ulong_to_string(unsigned long d);

int string_to_int(const char *value);

uint string_to_uint(const char *value);

unsigned long string_to_ulong(const char *value);

bool string_to_bool(const char *value);

std::string bool_to_string(bool value);

std::string getStringValue(HOSTIF_MsgData_t *stMsgData);

void putValue(HOSTIF_MsgData_t *stMsgData, const std::string &value);

void set_GatewayConnStatus( bool enabled);
bool get_GatewayConnStatus();

/**
 * Returns:
 * the specified environment variable's value if it is not NULL.
 * the specified default value otherwise.
 */
char* getenvOrDefault (const char* name, const char* defaultValue);

//bool write_config_file(const std::string &filename, const std::string &key, const std::string &value);
//std::string read_config_file(const std::string &filename, const std::string &key, const std::string &defaultValue);

#ifndef NEW_HTTP_SERVER_DISABLE
void getCurrentTime(struct timespec *timer);
long timeValDiff(struct timespec *starttime, struct timespec *finishtime);

void setLegacyRFCEnabled(bool value);
bool legacyRFCEnabled();
#endif
#ifdef T2_EVENT_ENABLED
void t2CountNotify(const char *marker, int val);
void t2ValNotify(const char *marker, const char *val);
#endif
HostIf_Source_Type_t getBSUpdateEnum(const char *bsUpdate);

bool isWebpaReady();
bool isNtpTimeFilePresent();
unsigned long get_system_manageble_ntp_time();
unsigned long get_device_manageble_time();

/**
* This function makes jsonrpc call and returns the result.
* @return A string contiaining the jsonrpc call result.
*/
string getJsonRPCData(std::string postData);

/**
 * Invoke a Thunder JSON-RPC method and return the complete response payload.
 *
 * @param[in] method Thunder method name, for example org.rdk.AuthService.getExperience.
 * @param[in] paramsJson JSON object string for params, pass empty string when params are not required.
 * @param[out] response Full JSON response payload from Thunder.
 *
 * @return true on transport success with non-empty payload, false otherwise.
 */
bool invokeThunderPluginMethod(const std::string& method, const std::string& paramsJson, std::string& response);

/**
 * Invoke a Thunder JSON-RPC method and extract a string field from result object.
 */
bool invokeThunderPluginMethodAndExtractStringField(const std::string& method,
    const std::string& paramsJson, const std::string& fieldName, std::string& value);

/**
 * Invoke a Thunder JSON-RPC method and extract a numeric field from result object.
 */
bool invokeThunderPluginMethodAndExtractNumberField(const std::string& method,
    const std::string& paramsJson, const std::string& fieldName, int& value);

/**
 * Invoke a Thunder JSON-RPC method and extract a boolean field from result object.
 * Accepts both JSON booleans and numeric 0/1 values.
 */
bool invokeThunderPluginMethodAndExtractBoolField(const std::string& method,
    const std::string& paramsJson, const std::string& fieldName, bool& value);

/**
 * Parse and validate a JSON-RPC response and extract a string field from result object.
 * Also handles arrays and objects by serializing them to a JSON string.
 */
bool thunderExtractResultStringField(const std::string& response, const char* fieldName, std::string& value);

/**
 * Parse and validate a JSON-RPC response and extract a numeric field from result object.
 */
bool thunderExtractResultNumberField(const std::string& response, const char* fieldName, int& value);

/**
 * Parse and validate a JSON-RPC response and extract a boolean field from result object.
 * Accepts both JSON booleans and numeric 0/1 values.
 */
bool thunderExtractResultBoolField(const std::string& response, const char* fieldName, bool& value);

/**
 * Intended for non-negative numeric values (for example, Unix timestamps) that fit in
 * unsigned long. Since JSON numbers may be represented internally as floating-point values,
 * very large integers can lose precision and values outside the target range are not safe.
 */
bool invokeThunderPluginMethodAndExtractULongField(const std::string& method,
    const std::string& paramsJson, const std::string& fieldName, unsigned long& value);

/**
 * Parse and validate a JSON-RPC response and extract an unsigned long numeric field from result object.
 */
bool thunderExtractResultULongField(const std::string& response, const char* fieldName, unsigned long& value);

/**
 * Parse Thunder response, locate an object in result[arrayFieldName] where matchKey == matchValue,
 * and read a string field from that object.
 */
bool readThunderArrayItemByKey(const std::string& response,
    const char* arrayFieldName,
    const char* matchKey,
    const char* matchValue,
    const char* fieldName,
    std::string& value);

/**
 * Parse Thunder response, locate an object in result[arrayFieldName] where matchKey == matchValue,
 * and read a boolean field from that object. Accepts both JSON booleans and numeric 0/1 values.
 */
bool readThunderArrayItemByKey(const std::string& response,
    const char* arrayFieldName,
    const char* matchKey,
    const char* matchValue,
    const char* fieldName,
    bool& value);

/**
 * Extract string values from a cJSON array and concatenate them with a delimiter.
 * @param[in] arrayObj The cJSON array object to process.
 * @param[in] delimiter The string to use as separator between array elements.
 * @param[out] value The resulting concatenated string.
 * @return true if successful, false if arrayObj is not a valid array.
 */
bool extractThunderStringArrayAsDelimitedString(cJSON* arrayObj, const std::string& delimiter, std::string& value);

/**
 * Invoke a Thunder JSON-RPC method and extract a result array field as a delimited string.
 * Example: ["A","B"] with delimiter "_" becomes "A_B".
 */
bool invokeThunderPluginMethodAndExtractDelimitedStringArrayField(const std::string& method,
    const std::string& paramsJson, const std::string& fieldName,
    const std::string& delimiter, std::string& value);

/**
 * Invoke a Thunder JSON-RPC method and extract the top-level "result" field as a plain string.
 * Use this when the response shape is {"result": "VALUE"} rather than {"result": {"field": "VALUE"}}.
 */
bool invokeThunderPluginMethodAndExtractScalarStringResult(const std::string& method,
    const std::string& paramsJson, std::string& value);

#endif /* HOSTIF_UTILS_H_*/


/** @} */
/** @} */
