#include <gmock/gmock.h>
#include <iostream>
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "XrdkCentralComRFCStore.h"
#include "XrdkCentralComBSStore.h"
#include "XrdkCentralComBSStoreJournal.h"
#include "Device_DeviceInfo_Processor.h"
#include "Device_DeviceInfo_ProcessStatus.h"
#include "XrdkCentralComRFCVar.h"
#include "request_handler.h"
#include "IniFile.h"
#include "hostIf_utils.h"
#include "hostIf_main.h"
#include "webpa_notification.h"
#include "webpa_parameter.h"
#include "rbus_value.h"

#include "hostIf_msgHandler.h"
#include "http_server.h"
#include "request_handler.h"
#include "rdk_debug.h"
#include "waldb.h"
#include "file_writer.h"


#ifdef __cplusplus
extern "C"
{
#endif
#include <wdmp-c.h>
#include <wdmp_internal.h>
#ifdef __cplusplus
}
#endif

#include "Device_DeviceInfo.h"

#include <curl/curl.h>
#include "cJSON.h"

#include <mutex>
#include <condition_variable>

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "hostif_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 128

using namespace std;

#ifdef GTEST_ENABLE
extern size_t (*getWriteCurlResponse(void))(void *ptr, size_t size, size_t nmemb, std::string stream);
#endif

XRFCStore* m_rfcStore;
XBSStore* m_bsStore;
XBSStoreJournal* m_bsStoreJournal;
XRFCVarStore* m_varStore;

std::mutex mtx_httpServerThreadDone;
std::condition_variable cv_httpServerThreadDone;
bool httpServerThreadDone = false;
GThread *HTTPServerThread = NULL;
char *HTTPServerName = (char *)"HTTPServerThread";
GError *httpError = NULL;
T_ARGLIST argList = {{'\0'}, 0};

TEST(srcTest, load) {
    IniFile* m_ini = new IniFile();
    const string filename = "/opt/secure/RFC/bootstrap.ini";
    bool result = m_ini->load(filename);
    EXPECT_EQ(result, true);
    delete m_ini;
}

TEST(srcTest, inValidFile) {
    IniFile* m_ini = new IniFile();
    const string filename = "/opt/secure/RFC/bootstrap_test.ini";
    bool result = m_ini->load(filename);
    EXPECT_EQ(result, false);
    delete m_ini;
}

TEST(srcTest, value) {
    IniFile* m_ini = new IniFile();
    const string key = "Device.Time.NTPServer5";
    const string defaultValue = "time";

    bool ret = m_ini->load("/opt/secure/RFC/bootstrap.ini");
    string result = m_ini->value(key, defaultValue);
    EXPECT_EQ(result, "override_time4.com");
    delete m_ini;
}

TEST(srcTest, srcsetValue) {
    IniFile* m_ini = new IniFile();
    const string key = "Device.Time.NTPServer4";
    const string value = "overidetime3.com";

    bool ret = m_ini->load("/opt/secure/RFC/bootstrap.ini");
    bool result = m_ini->setValue(key, value);
    EXPECT_EQ(result, true);
    delete m_ini;
}

TEST(srcTest, srcsetDefaultValue) {
    IniFile* m_ini = new IniFile();
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId";
    const string value = "sky";

    bool result = m_ini->setValue(key, value);
    EXPECT_EQ(result, false);
    delete m_ini;
}

TEST(srcTest, flush) {
    IniFile* m_ini = new IniFile();
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId";
    const string value = "sky";

    m_ini->m_filename = "/opt/secure/RFC_test/bootstrap_temp.ini";
    bool result = m_ini->flush();
    EXPECT_EQ(result, false);
    delete m_ini;
}

/* TEST(srcTest, clear) {
    IniFile *inFile = new IniFile();
    inFile->load("/opt/secure/RFC/bootstrap.ini");
    bool result = inFile->clear();
    EXPECT_EQ(result, true);
} */

TEST(srcTest, getStringFromEnum) {
    EnumStringMapper myEnumMap[] = {
        {0, "ZERO"},
        {1, "ONE"},
        {2, "TWO"},
        {3, "THREE"}
    };

    int size = sizeof(myEnumMap) / sizeof(myEnumMap[0]);
    int inputCode = 2;

    const char *result = getStringFromEnum(myEnumMap, size, inputCode);
    EXPECT_EQ(result, "TWO");
    
    const char *ret = getStringFromEnum(myEnumMap, 0, inputCode);
}

TEST(srcTest, getEnumFromString) {
     EnumStringMapper myEnumMap[] = {
        {0, "ZERO"},
        {1, "ONE"},
        {2, "TWO"},
        {3, "THREE"}
    };

    int size = sizeof(myEnumMap) / sizeof(myEnumMap[0]);

    int ret = getEnumFromString(myEnumMap, size, "THREE");
    EXPECT_EQ(ret, 3);

    int result = getEnumFromString(myEnumMap, 0, "THREE");
    EXPECT_EQ(result, -1);
}

TEST(srcTest, type_conversions) {
    int d = 100;
    string itosret = int_to_string(d);
    EXPECT_EQ(itosret, "100");

    uint unum = 143;
    string uitosret = uint_to_string(unum);
    EXPECT_EQ(uitosret, "143");

    unsigned long ulnum = 123456789UL;
    string ultosret = ulong_to_string(ulnum);
    EXPECT_EQ(ultosret, "123456789");

    int num = 42;
    const char* ptr = (const char*)&num;
    int iret = get_int(ptr);
    EXPECT_EQ(iret, 42);

    uint number = 123456;
    char *uptr = (char *)&number;
    uint uiret = get_uint(uptr);
    EXPECT_EQ(uiret, number);

    int stoiret = string_to_int("123");
    EXPECT_EQ(stoiret, 123);

    uint stouret = string_to_uint("42");
    EXPECT_EQ(stouret, 42);

    unsigned long stoulret = string_to_ulong("1234");
    EXPECT_EQ(stoulret, 1234);

    bool stobret = string_to_bool("false");
    EXPECT_EQ(stobret, false);
}

TEST(srcTest, bool_to_string) {
    std::string value = bool_to_string(true);
    EXPECT_EQ(value, "true");

    string ret = bool_to_string(false);
    EXPECT_EQ(ret, "false");
}

TEST(srcTest, string_to_bool) {
    bool ret = string_to_bool("false");
    EXPECT_EQ(ret, false);

    bool value = string_to_bool("1");
    EXPECT_EQ(value, true);
}

TEST(srcTest, getBSUpdateEnum) {
    HostIf_Source_Type_t type;
    type = getBSUpdateEnum("allUpdate");
    EXPECT_EQ(type, HOSTIF_SRC_ALL);

    type = getBSUpdateEnum("rfcUpdate");
    EXPECT_EQ(type, HOSTIF_SRC_RFC);

    type = getBSUpdateEnum("default");
    EXPECT_EQ(type, HOSTIF_SRC_DEFAULT);

    type = getBSUpdateEnum("none");
    EXPECT_EQ(type, HOSTIF_NONE);

    type = getBSUpdateEnum(NULL);
    EXPECT_EQ(type, HOSTIF_NONE);
}


TEST(srcTest, isWebpaReady) {
    bool ret = isWebpaReady();
    EXPECT_EQ(ret, true);
}

TEST(srcTest, isNtpTimeFilePresent) {
    bool ret = isNtpTimeFilePresent();
    EXPECT_EQ(ret, true);
}

TEST(srcTest, get_system_manageble_ntp_time) {
    write_on_file("/tmp/timeReceivedNTP", "Mon Aug 11 14:22:30 UTC 2025");
    unsigned long ret = get_system_manageble_ntp_time();
    EXPECT_EQ(ret, 1754922150);
}

TEST(srcTest, get_device_manageble_time) {
    write_on_file("/tmp/webpa/start_time", "1754835750");
    unsigned long ret = get_device_manageble_time();
    EXPECT_EQ(ret, 1754835750);
}


TEST(srcTest, set_get_GatewayConnStatus) {
    set_GatewayConnStatus(true);
    bool status = get_GatewayConnStatus();
    EXPECT_EQ(status, true);
}

TEST(srcTest, set_get_LegacyRFCEnabled) {
    setLegacyRFCEnabled(true);
    bool status = legacyRFCEnabled();
    EXPECT_EQ(status, true);
}

TEST(srcTest, matchComponent) {
    const char* param = "Device.WiFi.SSID.3.SSID";
    const char* key = "Device.WiFi.SSID";
    const char* setting = nullptr;
    int instance = 0;
    bool matched = matchComponent(param, key, &setting, instance);
    EXPECT_EQ(matched, true);
}

TEST(srcTest, getJsonRPCData) {
    std::string jsonRequest = "{\"jsonrpc\":\"2.0\",\"method\":\"getTime\",\"params\":{},\"id\":1}";
    string result = getJsonRPCData(jsonRequest);
    EXPECT_EQ(0, 0);
}

TEST(srcTest, timeValDiff) {
    struct timespec starttime = {
        .tv_sec = 100,
        .tv_nsec = 500000000   // 0.5 seconds
    };

    struct timespec endtime = {
        .tv_sec = 102,
        .tv_nsec = 200000000   // 0.2 seconds
    };

    long msec = timeValDiff(&starttime, &endtime);
    EXPECT_EQ(msec, 1700);
}

TEST(srcTest, writeCurlResponse) {
    const char* input = "MockCurlData";
    size_t size = 1;
    size_t nmemb = strlen(input);
    std::string response;
    size_t written = getWriteCurlResponse()((void*)input, size, nmemb, response);
    EXPECT_EQ(written, nmemb);
}

TEST(srcTest, getCurrentTime) {
    struct timespec ts;
    getCurrentTime(&ts);
    EXPECT_GT(ts.tv_sec, 0);

    EXPECT_GE(ts.tv_nsec, 0);
    EXPECT_LT(ts.tv_nsec, 1000000000L);

}

TEST(srcTest, ReturnsEnvValueIfSet)
{
    const char* envName = "TEST_ENV_VAR";
    const char* envValue = "expected_value";
    const char* defaultValue = "default_value";

    // Set environment variable
    setenv(envName, envValue, 1);

    // Call function
    char* result = getenvOrDefault(envName, defaultValue);

    // Validate
    EXPECT_EQ(std::string(result), std::string(envValue));
}


TEST(srcTest, triggerResetScript)
{
    setResetState(ColdReset);
    triggerResetScript();

    setResetState(FactoryReset);
    triggerResetScript();

    setResetState(WarehouseReset);
    triggerResetScript();

    setResetState(CustomerReset);
    triggerResetScript();

    EXPECT_EQ(0, 0);
}

TEST(srcTest, getResetState)
{
    setResetState(FactoryReset);
    eSTBResetState state = getResetState();
    EXPECT_EQ(state, FactoryReset);
}

TEST(srcTest, get_security_token)
{
    std::string token = get_security_token();
    EXPECT_EQ(token, "");
}

TEST(srcTest, putBoolValue)
{
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    putValue(&param, "true");
    cout << "msgData.paramValue = " <<  get_boolean(param.paramValue) << endl;
    EXPECT_EQ(get_boolean(param.paramValue), true);
}

TEST(srcTest, putIntValue)
{
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.TopSpeed", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_IntegerType;
    param.paramLen = sizeof(hostIf_IntegerType);

    putValue(&param, "14200");
    cout << "msgData.paramValue = " <<  get_int(param.paramValue) << endl;
    EXPECT_EQ(get_int(param.paramValue), 14200);
}

TEST(srcTest, putStringValue)
{
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerProductName", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    putValue(&param, "testName");
    cout << "msgData.paramValue = " << param.paramValue << endl;
    EXPECT_STREQ(param.paramValue, "testName");
}

TEST(srcTest, putUnsignedValue)
{
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.IP.Interface.1.Stats.BytesReceived", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;
    param.paramtype = hostIf_UnsignedLongType;
    param.paramLen = sizeof(hostIf_UnsignedLongType);

    putValue(&param, "1048576");
    cout << "msgData.paramValue = " << get_ulong(param.paramValue) << endl;
    EXPECT_EQ(get_ulong(param.paramValue), 1048576);
}

TEST(srcTest, ReturnsTrueWhenInputIsTrue)
{
    bool value = true;
    EXPECT_EQ(get_boolean((const char *)&value), true);
}


TEST(srcTest, getStringValue)
{
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_GET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_RFC;

    put_boolean(param.paramValue, true);
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    std::string value = getStringValue(&param);
    cout << "param.paramValue = " << param.paramValue << endl;
    EXPECT_EQ(value, "true");
}

GTEST_API_ int main(int argc, char *argv[]){
    char testresults_fullfilepath[GTEST_REPORT_FILEPATH_SIZE];
    char buffer[GTEST_REPORT_FILEPATH_SIZE];

    memset( testresults_fullfilepath, 0, GTEST_REPORT_FILEPATH_SIZE );
    memset( buffer, 0, GTEST_REPORT_FILEPATH_SIZE );
    snprintf( testresults_fullfilepath, GTEST_REPORT_FILEPATH_SIZE, "json:%s%s" , GTEST_DEFAULT_RESULT_FILEPATH , GTEST_DEFAULT_RESULT_FILENAME);

    ::testing::GTEST_FLAG(output) = testresults_fullfilepath;
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
