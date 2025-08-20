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

TEST(bsStoreTest, load) {
    IniFile* m_ini = new IniFile();
    const string filename = "/opt/secure/RFC/bootstrap.ini";
    bool result = m_ini->load(filename);
    EXPECT_EQ(result, true);
    delete m_ini;
}

TEST(bsStoreTest, value) {
    IniFile* m_ini = new IniFile();
    const string key = "Device.Time.NTPServer5";
    const string defaultValue = "time";

    bool ret = m_ini->load("/opt/secure/RFC/bootstrap.ini");
    string result = m_ini->value(key, defaultValue);
    EXPECT_EQ(result, "time4.com");
    delete m_ini;
}

TEST(bsStoreTest, srcsetValue) {
    IniFile* m_ini = new IniFile();
    const string key = "Device.Time.NTPServer4";
    const string value = "overidetime3.com";

    bool ret = m_ini->load("/opt/secure/RFC/bootstrap.ini");
    bool result = m_ini->setValue(key, value);
    EXPECT_EQ(result, true);
    delete m_ini;
}


/* TEST(bsStoreTest, clear) {
    IniFile *inFile = new IniFile();
    inFile->load("/opt/secure/RFC/bootstrap.ini");
    bool result = inFile->clear();
    EXPECT_EQ(result, true);
} */

TEST(bsStoreTest, getStringFromEnum) {
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
}

TEST(bsStoreTest, getEnumFromString) {
     EnumStringMapper myEnumMap[] = {
        {0, "ZERO"},
        {1, "ONE"},
        {2, "TWO"},
        {3, "THREE"}
    };

    int size = sizeof(myEnumMap) / sizeof(myEnumMap[0]);

    int ret = getEnumFromString(myEnumMap, size, "THREE");
    EXPECT_EQ(ret, 3);
}

TEST(bsStoreTest, type_conversions) {
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

    const char* input = "true";
    bool ret = get_boolean("true");
    EXPECT_EQ(ret, true);

    string btosret = bool_to_string(false);
    EXPECT_EQ(btosret, "false");

    int stoiret = string_to_int("123");
    EXPECT_EQ(stoiret, 123);

    uint stouret = string_to_uint("42");
    EXPECT_EQ(stouret, 42);

    unsigned long stoulret = string_to_ulong("1234");
    EXPECT_EQ(stoulret, 1234);

    bool stobret = string_to_bool("false");
    EXPECT_EQ(stobret, false);
}

TEST(bsStoreTest, getBSUpdateEnum) {
    HostIf_Source_Type_t type;
    type = getBSUpdateEnum("allUpdate");
    EXPECT_EQ(type, HOSTIF_SRC_ALL);

    type = getBSUpdateEnum("rfcUpdate");
    EXPECT_EQ(type, HOSTIF_SRC_RFC);

    type = getBSUpdateEnum("default");
    EXPECT_EQ(type, HOSTIF_SRC_DEFAULT);
}


TEST(bsStoreTest, isWebpaReady) {
    bool ret = isWebpaReady();
    EXPECT_EQ(ret, true);
}

TEST(bsStoreTest, isNtpTimeFilePresent) {
    bool ret = isNtpTimeFilePresent();
    EXPECT_EQ(ret, true);
}

TEST(bsStoreTest, get_system_manageble_ntp_time) {
    write_on_file("/tmp/timeReceivedNTP", "Mon Aug 11 14:22:30 UTC 2025");
    unsigned long ret = get_system_manageble_ntp_time();
    EXPECT_EQ(ret, 1754922150);
}

TEST(bsStoreTest, get_device_manageble_time) {
    write_on_file("/tmp/webpa/start_time", "1754835750");
    unsigned long ret = get_device_manageble_time();
    EXPECT_EQ(ret, 1754835750);
}


TEST(bsStoreTest, set_get_GatewayConnStatus) {
    set_GatewayConnStatus(true);
    bool status = get_GatewayConnStatus();
    EXPECT_EQ(status, true);
}

TEST(bsStoreTest, set_get_LegacyRFCEnabled) {
    setLegacyRFCEnabled(true);
    bool status = legacyRFCEnabled();
    EXPECT_EQ(status, true);
}

TEST(bsStoreTest, matchComponent) {
    const char* param = "Device.WiFi.SSID.3.SSID";
    const char* key = "Device.WiFi.SSID";
    const char* setting = nullptr;
    int instance = 0;
    bool matched = matchComponent(param, key, &setting, instance);
    EXPECT_EQ(matched, true);
}

TEST(bsStoreTest, getJsonRPCData) {
    std::string jsonRequest = "{\"jsonrpc\":\"2.0\",\"method\":\"getTime\",\"params\":{},\"id\":1}";
    string result = getJsonRPCData(jsonRequest);
    EXPECT_EQ(result,"test");
}

TEST(bsStoreTest, timeValDiff) {
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
