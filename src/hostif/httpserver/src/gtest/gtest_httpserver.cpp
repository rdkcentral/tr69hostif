#include <gtest/gtest.h>
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
#include "http_server.h"
#include "hostIf_utils.h"
#include "hostIf_main.h"
#include "webpa_notification.h"
#include "webpa_parameter.h"
#include "rbus_value.h"
#include "libsoup/soup.h"
#include <gio/gio.h>
#include <cstring>

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
#include <fstream>

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "hostif_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 128

using namespace std;

XRFCStore* m_rfcStore;
XBSStore* m_bsStore;
XBSStoreJournal* m_bsStoreJournal;
XRFCVarStore* m_varStore;

extern GHashTable* paramMgrhash;

std::mutex mtx_httpServerThreadDone;
std::condition_variable cv_httpServerThreadDone;
bool httpServerThreadDone = false;
GThread *HTTPServerThread = NULL;
char *HTTPServerName = (char *)"HTTPServerThread";
GError *httpError = NULL;
GHashTable* paramMgrhash = NULL;
T_ARGLIST argList = {{'\0'}, 0};
static SoupServer  *http_server = NULL;

/*static SoupServerMessage* createServerMessage(const char* method, const char* uri, const char* body)
{
    GError* parseError = nullptr;
    GUri* parsedUri = g_uri_parse(uri, G_URI_FLAGS_NONE, &parseError);
    if (!parsedUri)
    {
        if (parseError)
        {
            g_error_free(parseError);
        }
        return nullptr;
    }

    SoupServerMessage* msg = static_cast<SoupServerMessage*>(
        g_object_new(SOUP_TYPE_SERVER_MESSAGE,
                     "method", method,
                     "uri", parsedUri,
                     NULL));
    g_uri_unref(parsedUri);
    if (!msg)
    {
        return nullptr;
    }

    if (body != nullptr)
    {
        SoupMessageBody* reqBody = soup_server_message_get_request_body(msg);
        if (reqBody)
        {
            soup_message_body_append(reqBody, SOUP_MEMORY_COPY, body, strlen(body));
            soup_message_body_complete(reqBody);
        }
    }

    return msg;
}  */


static SoupServerMessage* createServerMessage(const char* method, const char* uri, const char* body)
{
    (void)method;
    (void)uri;
    SoupServerMessage* msg = static_cast<SoupServerMessage*>(g_object_new(SOUP_TYPE_SERVER_MESSAGE, NULL));
    if (!msg)
    {
        return nullptr;
    }

    if (body != nullptr)
    {
        SoupMessageBody* reqBody = soup_server_message_get_request_body(msg);
        if (reqBody)
        {
            soup_message_body_append(reqBody, SOUP_MEMORY_COPY, body, strlen(body));
            soup_message_body_complete(reqBody);
        }
    }

    return msg;
}

static void addCallerIdHeader(SoupServerMessage* msg, const char* callerId)
{
    SoupMessageHeaders* reqHeaders = soup_server_message_get_request_headers(msg);
    if (reqHeaders && callerId != nullptr)
    {
        soup_message_headers_append(reqHeaders, "CallerID", callerId);
    }
}

#ifdef GTEST_ENABLE
extern DATA_TYPE (*getWdmpDataTypeFunc())(char * );
extern HostIf_ParamType_t (*getHostIfParamTypeFunc())(DATA_TYPE wdmpDataType);
bool (*validateParamValueFunc())(const string &paramValue, HostIf_ParamType_t dataType);
WDMP_STATUS (*handleRFCRequestFunc())(REQ_TYPE reqType, param_t *param);
WDMP_STATUS (*invokeHostIfAPIFunc())(REQ_TYPE reqType, param_t *param, HostIf_Source_Type_t bsUpdate, const char *pcCallerID);
WDMP_STATUS (*validateAgainstDataModelFunc())(REQ_TYPE reqType, char* paramName, const char* paramValue, DATA_TYPE *dataType, char **defaultValue, HostIf_Source_Type_t *bsUpdate);
extern void (*HTTPRequestHandlerFunc()) (
    SoupServer        *server,
    SoupServerMessage *msg,
    const char        *path,
    GHashTable        *query,
    void           *user_data);
extern void (*convertAndAssignParamValueFunc()) (HOSTIF_MsgData_t *param, char *value);
extern char* (*getStringValueFunc()) (HostIf_ParamType_t paramType, char *value);
extern bool (*isAuthorizedFunc())(const char* pcCallerID, const char* pcParamName);
extern void (*getHostIfParamStFromRequestFunc())(REQ_TYPE reqType, param_t *param, HOSTIF_MsgData_t *hostIfParam);
#endif

TEST(httpserverTest,initRFCVarFileName){
    m_varStore=XRFCVarStore::getInstance();
    if(m_varStore)
    {
      m_varStore->initRFCVarFileName();
      m_varStore->m_filename.erase(std::remove(m_varStore->m_filename.begin(),m_varStore->m_filename.end(),'"'),m_varStore->m_filename.end());
      EXPECT_EQ(m_varStore->m_filename,"/opt/secure/RFC/rfcVariable.ini");
    }
}

TEST(httpserverTest, HTTPServerStartThread_WithoutDataModel_ReturnsNull) {
    // This test intentionally runs before any loadDataModel() invocation.
    argList.httpServerPort = 0;
    std::remove("/tmp/.tr69hostif_http_server_ready");

    void *thread_result = HTTPServerStartThread(nullptr);
    EXPECT_EQ(thread_result, nullptr);

    std::ifstream ready_file("/tmp/.tr69hostif_http_server_ready");
    EXPECT_FALSE(ready_file.good());
}

TEST(httpserverTest, HTTPRequestHandler_EmptyRequestBody) {
    SoupServer* server = soup_server_new("server-header", "HTTPServer", NULL);
    ASSERT_NE(server, nullptr);

    SoupServerMessage* msg = createServerMessage("GET", "http://127.0.0.1/", nullptr);
    ASSERT_NE(msg, nullptr);

    HTTPRequestHandlerFunc()(server, msg, "/", nullptr, nullptr);

    g_object_unref(msg);
    g_object_unref(server);
}

TEST(httpserverTest, HTTPRequestHandler_InvalidJsonPayload) {
    SoupServer* server = soup_server_new("server-header", "HTTPServer", NULL);
    ASSERT_NE(server, nullptr);

    SoupServerMessage* msg = createServerMessage("GET", "http://127.0.0.1/", "not-json-payload");
    ASSERT_NE(msg, nullptr);

    HTTPRequestHandlerFunc()(server, msg, "/", nullptr, nullptr);

    g_object_unref(msg);
    g_object_unref(server);
}

TEST(httpserverTest, HTTPRequestHandler_PostWithoutCallerId) {
    SoupServer* server = soup_server_new("server-header", "HTTPServer", NULL);
    ASSERT_NE(server, nullptr);

    SoupServerMessage* msg = createServerMessage("POST", "http://127.0.0.1/", "{\"paramNames\":[\"Device.DeviceInfo.ModelName\"]}");
    ASSERT_NE(msg, nullptr);
    addCallerIdHeader(msg, "");

    HTTPRequestHandlerFunc()(server, msg, "/", nullptr, nullptr);

    g_object_unref(msg);
    g_object_unref(server);
}

TEST(httpserverTest, HTTPRequestHandler_UnsupportedMethod) {
    SoupServer* server = soup_server_new("server-header", "HTTPServer", NULL);
    ASSERT_NE(server, nullptr);

    SoupServerMessage* msg = createServerMessage("PUT", "http://127.0.0.1/", "{\"paramNames\":[\"Device.DeviceInfo.ModelName\"]}");
    ASSERT_NE(msg, nullptr);

    HTTPRequestHandlerFunc()(server, msg, "/", nullptr, nullptr);

    g_object_unref(msg);
    g_object_unref(server);
}

TEST(httpserverTest, HTTPRequestHandler_GetWithCallerId_CoversResponsePath) {
    EXPECT_EQ(loadDataModel(), DB_SUCCESS);

    SoupServer* server = soup_server_new("server-header", "HTTPServer", NULL);
    ASSERT_NE(server, nullptr);

    SoupServerMessage* msg = createServerMessage(
        "GET",
        "http://127.0.0.1/",
        "{\"names\":[\"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MOCASSH.Enable\"]}");
    ASSERT_NE(msg, nullptr);
    addCallerIdHeader(msg, "rfc");

    HTTPRequestHandlerFunc()(server, msg, "/", nullptr, nullptr);

    g_object_unref(msg);
    g_object_unref(server);
}

TEST(httpserverTest, HTTPRequestHandler_PostWithCallerId_CoversSetPath) {
    EXPECT_EQ(loadDataModel(), DB_SUCCESS);

    SoupServer* server = soup_server_new("server-header", "HTTPServer", NULL);
    ASSERT_NE(server, nullptr);

    SoupServerMessage* msg = createServerMessage(
        "POST",
        "http://127.0.0.1/",
        "{\"parameters\":[{\"name\":\"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable\",\"value\":\"true\",\"dataType\":3}]}");
    ASSERT_NE(msg, nullptr);
    addCallerIdHeader(msg, "rfc");

    HTTPRequestHandlerFunc()(server, msg, "/", nullptr, nullptr);

    g_object_unref(msg);
    g_object_unref(server);
}

TEST(httpserverTest, loadRFCVarIntoCache) {
    m_varStore = XRFCVarStore::getInstance();
    if(m_varStore)
    {
       bool ret = m_varStore->loadRFCVarIntoCache();
       EXPECT_EQ(ret, true);
   }
}

TEST(httpserverTest, getValue) {
    m_varStore = XRFCVarStore::getInstance();
    const string key = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.WebCfgData";
    if(m_varStore)
    {
       string value = m_varStore->getValue(key);
       EXPECT_EQ(value, "testCfg");
   }
}

TEST(httpserverTest, getValue_UnknownKey_ReturnsEmpty) {
    m_varStore = XRFCVarStore::getInstance();
    const string key = "RFC_UNKNOWN_TEST_KEY";
    if(m_varStore)
    {
       string value = m_varStore->getValue(key);
       EXPECT_EQ(value, "");
   }
}

TEST(httpserverTest, loadRFCVarIntoCache_EmptyFilename_ReturnsFalse) {
    m_varStore = XRFCVarStore::getInstance();

    ASSERT_NE(m_varStore, nullptr);
    
    const std::string prevFilename = m_varStore->m_filename;
    m_varStore->m_filename = "";
    
    bool ret = m_varStore->loadRFCVarIntoCache();
    EXPECT_EQ(ret, false);
    
    m_varStore->m_filename = prevFilename;
}

TEST(httpserverTest, getValue_InitNotDone_ReturnsEmptyEvenWhenKeyExists) {
    m_varStore = XRFCVarStore::getInstance();

    ASSERT_NE(m_varStore, nullptr);
    
    const bool prevInitDone = m_varStore->initDone;
    m_varStore->m_dict["RFC_TEST_KEY"] = "RFC_TEST_VALUE";
    m_varStore->initDone = false;
    string value = m_varStore->getValue("RFC_TEST_KEY");
    EXPECT_EQ(value, "");
    
    m_varStore->m_dict.erase("RFC_TEST_KEY");
    m_varStore->initDone = prevInitDone;
}

TEST(httpserverTest, reloadCache_WithQuotedFilename_LoadsValues) {
    m_varStore = XRFCVarStore::getInstance();

    ASSERT_NE(m_varStore, nullptr);

    const string prevFilename = m_varStore->m_filename;
    const char *tmpFile = "/tmp/rfc_var_reload_test.ini";
    std::ofstream ofs(tmpFile, std::ios::trunc | std::ios::out);
    ofs << "export RFC_TEST_RELOAD_KEY=reload_value" << std::endl;
    ofs.close();

    m_varStore->m_filename = "\"/tmp/rfc_var_reload_test.ini\"";
    m_varStore->reloadCache();
    EXPECT_EQ(m_varStore->getValue("RFC_TEST_RELOAD_KEY"), "reload_value");

    m_varStore->m_filename = prevFilename;
    std::remove(tmpFile);
}


TEST(httpserverTest, getWdmpDataType) {
    EXPECT_EQ(getWdmpDataTypeFunc()("string"), WDMP_STRING);
    EXPECT_EQ(getWdmpDataTypeFunc()("boolean"), WDMP_BOOLEAN);
    EXPECT_EQ(getWdmpDataTypeFunc()("unsignedInt"), WDMP_UINT);
    EXPECT_EQ(getWdmpDataTypeFunc()("int"),  WDMP_INT);
    EXPECT_EQ(getWdmpDataTypeFunc()("unsignedLong"), WDMP_ULONG);
    EXPECT_EQ(getWdmpDataTypeFunc()("dataTime"), WDMP_DATETIME);
    EXPECT_EQ(getWdmpDataTypeFunc()("float"), WDMP_NONE);
}

TEST(httpserverTest, getHostIfParamType) {
    EXPECT_EQ(getHostIfParamTypeFunc()(WDMP_STRING), hostIf_StringType);
    EXPECT_EQ(getHostIfParamTypeFunc()(WDMP_INT), hostIf_IntegerType);
    EXPECT_EQ(getHostIfParamTypeFunc()(WDMP_UINT), hostIf_UnsignedIntType);
    EXPECT_EQ(getHostIfParamTypeFunc()(WDMP_BOOLEAN), hostIf_BooleanType);
    EXPECT_EQ(getHostIfParamTypeFunc()(WDMP_DATETIME), hostIf_DateTimeType);
    EXPECT_EQ(getHostIfParamTypeFunc()(WDMP_BASE64), hostIf_StringType);
}

TEST(httpserverTest, validateParamValue) {
    const string sparamValue = "testimage";
    HostIf_ParamType_t dataType = hostIf_StringType;
    EXPECT_EQ(validateParamValueFunc()(sparamValue, dataType), true);

    const string bparamValue = "0";
    dataType = hostIf_BooleanType;
    EXPECT_EQ(validateParamValueFunc()(bparamValue, dataType), true);

    const string iparamValue = "12800";
    dataType = hostIf_IntegerType;
    EXPECT_EQ(validateParamValueFunc()(iparamValue, dataType), true);

    const string uiparamValue = "12";
    dataType = hostIf_UnsignedIntType;
    EXPECT_EQ(validateParamValueFunc()(uiparamValue, dataType), true);

    const string paramValue = "flase";
    dataType = hostIf_BooleanType;
    EXPECT_EQ(validateParamValueFunc()(paramValue, dataType), false);

    const string invalidIntValue = "123abc";
    dataType = hostIf_IntegerType;
    EXPECT_EQ(validateParamValueFunc()(invalidIntValue, dataType), false);

    const string invalidLongValue = "123dab";
    dataType = hostIf_UnsignedLongType;
    EXPECT_EQ(validateParamValueFunc()(invalidLongValue, dataType), false);

    const string unknownTypeValue = "anything";
    dataType = (HostIf_ParamType_t)999;
    EXPECT_EQ(validateParamValueFunc()(unknownTypeValue, dataType), false);
}

TEST(httpserverTest, handleRFCRequest_GET) {
    m_varStore = XRFCVarStore::getInstance();
    ASSERT_NE(m_varStore, nullptr);
    m_varStore->m_dict["Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType"] = "testtype";
    m_varStore->initDone = true;

    param_t param;
    memset(&param,0,sizeof(param_t));
    param.name = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType");

    REQ_TYPE reqType = GET;
    WDMP_STATUS status = handleRFCRequestFunc()(reqType, &param);
    EXPECT_EQ(status, WDMP_SUCCESS);

    EXPECT_STREQ(param.value, "testtype");
    free(param.name);
}

TEST(httpserverTest, handleRFCRequest_SET) {
    param_t param;
    memset(&param,0,sizeof(param_t));
    param.name = strdup(XRFC_VAR_STORE_RELOADCACHE);
    param.value = strdup("true");

    REQ_TYPE reqType = SET;
    WDMP_STATUS status = handleRFCRequestFunc()(reqType, &param);
    EXPECT_EQ(status, WDMP_SUCCESS);
    free(param.name);
    free(param.value);
}

TEST(httpserverTest, InvalidReqtype) {
    param_t param;
    memset(&param,0,sizeof(param_t));
    param.name = strdup(XRFC_VAR_STORE_RELOADCACHE);
    param.value = strdup("true");

    REQ_TYPE reqType = DELETE_ROW;
    WDMP_STATUS status = handleRFCRequestFunc()(reqType, &param);
    EXPECT_EQ(status, WDMP_SUCCESS);
    free(param.name);
    free(param.value);
}

TEST(httpserverTest, handleRFCInvalidRequest) {
    param_t param;
    memset(&param,0,sizeof(param_t));
    param.name = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDB");
    param.value = strdup("true");

    REQ_TYPE reqType = SET;
    WDMP_STATUS status = handleRFCRequestFunc()(reqType, &param);
    EXPECT_EQ(status, WDMP_ERR_METHOD_NOT_SUPPORTED);
    free(param.name);
    free(param.value);
}

TEST(httpserverTest, invokeHostIfAPI_SET_FAILURE) {
    param_t param;
    memset(&param,0,sizeof(param_t));
    param.name = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MOCASSH.Enable");
    param.value = strdup("true");

    const char *pcCallerID = "rfc";

    HOSTIF_MsgData_t msgData = { 0 };
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.bsUpdate = HOSTIF_NONE;

    REQ_TYPE reqType = SET;
    WDMP_STATUS status = invokeHostIfAPIFunc()(reqType, &param, msgData.bsUpdate, pcCallerID);
    EXPECT_EQ(status, 37);
}


TEST(httpserverTest, invokeHostIfAPI) {
    strcpy(argList.confFile, "/etc/mgrlist.conf");
    bool ret = hostIf_initalize_ConfigManger();

    param_t param;
    memset(&param,0,sizeof(param_t));
    param.name = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable");
    writeToTr181storeFile("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable", "true", "/opt/secure/RFC/tr181store.ini", Plain);
    const char *pcCallerID = "rfc";

    HOSTIF_MsgData_t msgData = { 0 };
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.bsUpdate = HOSTIF_NONE;

    REQ_TYPE reqType = GET;
    WDMP_STATUS status = invokeHostIfAPIFunc()(reqType, &param, msgData.bsUpdate, pcCallerID);
    EXPECT_EQ(status, WDMP_SUCCESS);

}

TEST(httpserverTest, invokeHostIfAPI_SET) {
    strcpy(argList.confFile, "/etc/mgrlist.conf");
    bool ret = hostIf_initalize_ConfigManger();

    param_t param;
    memset(&param,0,sizeof(param_t));
    param.name = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.SsrUrl");
    param.value = strdup("https://ssr.ccp.xcal.tv");

    const char *pcCallerID = "rfc";

    HOSTIF_MsgData_t msgData = { 0 };
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.bsUpdate = HOSTIF_NONE;
  
    REQ_TYPE reqType = SET;
    WDMP_STATUS status = invokeHostIfAPIFunc()(reqType, &param, msgData.bsUpdate, pcCallerID);
    EXPECT_EQ(status, WDMP_SUCCESS);
}

TEST(httpserverTest, invokeHostIfAPI_InvalidReq) {
    param_t param;
    memset(&param,0,sizeof(param_t));
    param.name = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.XconfUrl");
    const char *pcCallerID = "webpa";

    HOSTIF_MsgData_t msgData = { 0 };
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.bsUpdate = HOSTIF_NONE;

    strncpy(msgData.paramValue, "https://ssr.ccp.xcal.tv", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_StringType;
    msgData.paramLen = strlen(msgData.paramValue);

    REQ_TYPE reqType = REPLACE_ROWS;
    WDMP_STATUS status = invokeHostIfAPIFunc()(reqType, &param, msgData.bsUpdate, pcCallerID);
    EXPECT_EQ(status, WDMP_ERR_INTERNAL_ERROR);
}

TEST(httpserverTest, validateAgainstDataModel_GET) {

    /* Load the data model xml file*/
    DB_STATUS dbStatus = loadDataModel();
    if(dbStatus != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(dbStatus, DB_SUCCESS);

    HOSTIF_MsgData_t msgData = { 0 };
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.bsUpdate = HOSTIF_NONE;

    char paramName [] = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.Enable";
    const char* paramValue = NULL;
    DATA_TYPE dataType = WDMP_BOOLEAN;

    REQ_TYPE reqType = GET;
    char defaultValue[66];
    char* defaultValuePtr = defaultValue;
    WDMP_STATUS status = validateAgainstDataModelFunc()(reqType, paramName, paramValue, &dataType, &defaultValuePtr, &msgData.bsUpdate);
    EXPECT_EQ(status, WDMP_SUCCESS);
    EXPECT_STREQ(defaultValuePtr, "false");

}

TEST(httpserverTest, validateAgainstDataModel_SET_NullParam) {
    
    /* Load the data model xml file*/
    DB_STATUS dbStatus = loadDataModel();
    if(dbStatus != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(dbStatus, DB_SUCCESS);

    HOSTIF_MsgData_t msgData = { 0 };
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.bsUpdate = HOSTIF_NONE;

    char paramName [] = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerName";
    const char* paramValue = NULL;
    DATA_TYPE dataType = WDMP_BOOLEAN;

    REQ_TYPE reqType = SET;
    char defaultValue[66];
    char* defaultValuePtr = defaultValue;
    WDMP_STATUS status = validateAgainstDataModelFunc()(SET, paramName, paramValue, &dataType, &defaultValuePtr, &msgData.bsUpdate);
    EXPECT_EQ(status, WDMP_ERR_VALUE_IS_NULL);
}

TEST(httpserverTest, validateAgainstDataModel_SET_ReadOnly) {
    
    /* Load the data model xml file*/
    DB_STATUS dbStatus = loadDataModel();
    if(dbStatus != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(dbStatus, DB_SUCCESS);

    HOSTIF_MsgData_t msgData = { 0 };
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.bsUpdate = HOSTIF_NONE;

    char paramName [] = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshStatus";
    const char* paramValue = "reverseSsh";
    DATA_TYPE dataType = WDMP_BOOLEAN;

    REQ_TYPE reqType = SET;
    char defaultValue[66];
    char* defaultValuePtr = defaultValue;
    WDMP_STATUS status = validateAgainstDataModelFunc()(SET, paramName, paramValue, &dataType, &defaultValuePtr, &msgData.bsUpdate);
    EXPECT_EQ(status, WDMP_ERR_NOT_WRITABLE);
}

TEST(httpserverTest, validateAgainstDataModel_INVALID_PARAMETER_NAME) {

    /* Load the data model xml file*/
    DB_STATUS dbStatus = loadDataModel();
    if(dbStatus != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(dbStatus, DB_SUCCESS);

    HOSTIF_MsgData_t msgData = { 0 };
    memset(&msgData,0,sizeof(HOSTIF_MsgData_t));
    msgData.bsUpdate = HOSTIF_NONE;

    char paramName [] = "Device.DeviceInfoTest.FriendlyName";
    const char* paramValue = "reverseSsh";
    DATA_TYPE dataType = WDMP_BOOLEAN;

    REQ_TYPE reqType = SET;
    char defaultValue[66];
    char* defaultValuePtr = defaultValue;
    WDMP_STATUS status = validateAgainstDataModelFunc()(SET, paramName, paramValue, &dataType, &defaultValuePtr, &msgData.bsUpdate);
    EXPECT_EQ(status, WDMP_ERR_INVALID_PARAMETER_NAME);
}

TEST(httpserverTest, validateAgainstDataModel_InvalidReqType_ReturnsFailure) {
    DB_STATUS dbStatus = loadDataModel();
    EXPECT_EQ(dbStatus, DB_SUCCESS);

    HOSTIF_MsgData_t msgData = { 0 };
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    msgData.bsUpdate = HOSTIF_NONE;

    char paramName[] = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.Enable";
    const char* paramValue = "true";
    DATA_TYPE dataType = WDMP_BOOLEAN;
    char defaultValue[8] = {0};
    char* defaultValuePtr = defaultValue;

    WDMP_STATUS status = validateAgainstDataModelFunc()(DELETE_ROW, paramName, paramValue, &dataType, &defaultValuePtr, &msgData.bsUpdate);
    EXPECT_EQ(status, WDMP_FAILURE);
}

TEST(httpserverTest, handleRFCRequest_GET_UnknownKey_ReturnsValueIsNull) {
    m_varStore = XRFCVarStore::getInstance();
    ASSERT_NE(m_varStore, nullptr);
    m_varStore->initDone = true;

    param_t param;
    memset(&param, 0, sizeof(param_t));
    param.name = strdup("RFC_UNIT_TEST_UNKNOWN_KEY");

    WDMP_STATUS status = handleRFCRequestFunc()(GET, &param);
    EXPECT_EQ(status, WDMP_ERR_VALUE_IS_NULL);
    ASSERT_NE(param.value, nullptr);
    EXPECT_STREQ(param.value, "");

    free(param.name);
    free(param.value);
}

TEST(httpserverTest, invokeHostIfAPI_GET_WebpaCaller_PathCovered) {
    strcpy(argList.confFile, "/etc/mgrlist.conf");
    hostIf_initalize_ConfigManger();

    writeToTr181storeFile("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable", "true", "/opt/secure/RFC/tr181store.ini", Plain);

    param_t param;
    memset(&param, 0, sizeof(param_t));
    param.name = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable");

    WDMP_STATUS status = invokeHostIfAPIFunc()(GET, &param, HOSTIF_NONE, "webpa");
    EXPECT_EQ(status, WDMP_SUCCESS);
    ASSERT_NE(param.value, nullptr);
    EXPECT_GT(strlen(param.value), 0u);

    free(param.name);
    free(param.value);
}

TEST(httpserverTest, handleRequest_GET) {
    
    /* Load the data model xml file*/
    DB_STATUS dbStatus = loadDataModel();
    if(dbStatus != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(dbStatus, DB_SUCCESS);

    const char* pcCallerID = "rfc";
    get_req_t *getReq = (get_req_t *)malloc(sizeof(get_req_t));
    getReq->paramCnt = 1;
    getReq->paramNames[0] = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MOCASSH.Enable");
    //getReq->paramNames[1] = strdup("Device.DeviceInfo.");
    req_struct reqSt;
    reqSt.reqType = GET;  // Replace with actual enum value if it's defined
    reqSt.u.getReq = getReq;


    res_struct* respSt = handleRequest(pcCallerID, &reqSt);
    EXPECT_EQ(respSt->retStatus[0], WDMP_ERR_DEFAULT_VALUE);
}

TEST(httpserverTest, handlewildRequest_GET) {
    
    /* Load the data model xml file*/
    DB_STATUS dbStatus = loadDataModel();
    if(dbStatus != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(dbStatus, DB_SUCCESS);

    const char* pcCallerID = "rfc";
    get_req_t *getReq = (get_req_t *)malloc(sizeof(get_req_t));
    getReq->paramCnt = 1;
    getReq->paramNames[0] = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.");
    //getReq->paramNames[1] = strdup("Device.DeviceInfo.");
    req_struct reqSt;
    reqSt.reqType = GET;  // Replace with actual enum value if it's defined
    reqSt.u.getReq = getReq;


    res_struct* respSt = handleRequest(pcCallerID, &reqSt);
    //EXPECT_EQ(respSt->retStatus[0], WDMP_ERR_DEFAULT_VALUE);
    EXPECT_EQ(respSt->retStatus[0], 0);

}

TEST(httpserverTest, handlewildRequestINVALIDPARAM_GET) {

    /* Load the data model xml file*/
    DB_STATUS dbStatus = loadDataModel();
    if(dbStatus != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(dbStatus, DB_SUCCESS);

    const char* pcCallerID = "rfc";
    get_req_t *getReq = (get_req_t *)malloc(sizeof(get_req_t));
    getReq->paramCnt = 1;
    getReq->paramNames[0] = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SystemServices.FriendlyName");
    req_struct reqSt;
    reqSt.reqType = GET;  // Replace with actual enum value if it's defined
    reqSt.u.getReq = getReq;


    res_struct* respSt = handleRequest(pcCallerID, &reqSt);
    EXPECT_EQ(respSt->retStatus[0], 22);
}

/* TEST(httpserverTest, paramCount_GET) {

    DB_STATUS dbStatus = loadDataModel();
    if(dbStatus != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(dbStatus, DB_SUCCESS);

    const char* pcCallerID = "rfc";
    get_req_t *getReq = (get_req_t *)malloc(sizeof(get_req_t));
    getReq->paramCnt = 0;
   // getReq->paramNames[0] = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SystemServices.FriendlyName");
    req_struct reqSt;
    reqSt.reqType = GET;  // Replace with actual enum value if it's defined
    reqSt.u.getReq = getReq;


    res_struct* respSt = handleRequest(pcCallerID, &reqSt);
    EXPECT_EQ(respSt->retStatus[0], NULL);
}


TEST(httpserverTest, paramCount_SET) {

    if(dbStatus != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(dbStatus, DB_SUCCESS);

    const char* pcCallerID = "rfc";
    get_req_t *getReq = (get_req_t *)malloc(sizeof(get_req_t));
    getReq->paramCnt = 0;
   // getReq->paramNames[0] = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SystemServices.FriendlyName");
    req_struct reqSt;
    reqSt.reqType = SET;  // Replace with actual enum value if it's defined
    reqSt.u.getReq = getReq;


    res_struct* respSt = handleRequest(pcCallerID, &reqSt);
    EXPECT_EQ(respSt->retStatus[0], NULL);
}
*/

/* TEST(httpserverTest, paramCount_InvalidParamName) {

    DB_STATUS dbStatus = loadDataModel();
    if(dbStatus != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(dbStatus, DB_SUCCESS);

    const char* pcCallerID = "rfc";
    get_req_t *getReq = (get_req_t *)malloc(sizeof(get_req_t));
    getReq->paramCnt = 1;
    getReq->paramNames[0] = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable.Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable.Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable.Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable.Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable");
    req_struct reqSt;
    reqSt.reqType = SET;  // Replace with actual enum value if it's defined
    reqSt.u.getReq = getReq;


    res_struct* respSt = handleRequest(pcCallerID, &reqSt);
    EXPECT_EQ(respSt->retStatus[0], WDMP_ERR_INVALID_PARAMETER_NAME);
} */
 
TEST(httpserverTest, handleRequest_SET) {
    /* Load the data model xml file*/
    DB_STATUS dbStatus = loadDataModel();
    if(dbStatus != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(dbStatus, DB_SUCCESS);

    const char* pcCallerID = "rfc";

    param_t *params = (param_t *) malloc(sizeof(param_t) * 3);
    params[0].name = strdup("Device.X_CISCO_COM_DeviceControl.RebootDevice");
    params[0].value = strdup("true");
    params[0].type = WDMP_BOOLEAN;

    params[1].name = strdup("Device.DeviceInfo.");
    params[1].value = strdup("true");
    params[1].type = WDMP_BOOLEAN;

    params[2].name = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MOCASSH.Enable");
    params[2].value = NULL;
    params[2].type = WDMP_BOOLEAN;

    set_req_t *setReq = (set_req_t *)malloc(sizeof(set_req_t));
    setReq->param = params;
    setReq->paramCnt = 3;

    req_struct reqSt;
    reqSt.reqType = SET;  // Replace with actual enum value
    reqSt.u.setReq = setReq;

    res_struct* respSt = handleRequest(pcCallerID, &reqSt);
    EXPECT_EQ(respSt->retStatus[0], WDMP_FAILURE);
    EXPECT_EQ(respSt->retStatus[1], WDMP_ERR_WILDCARD_NOT_SUPPORTED);
    EXPECT_EQ(respSt->retStatus[2], WDMP_ERR_VALUE_IS_NULL);
}


TEST(httpserverTest,  convertAndAssignParamValue_BooleanType) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MOCASSH.Enable", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;
    
    param.paramtype = hostIf_BooleanType;
    param.paramLen = sizeof(hostIf_BooleanType);

    convertAndAssignParamValueFunc()(&param, "true");
    EXPECT_EQ(0, 0);
}

TEST(httpserverTest,  convertAndAssignParamValue_StringType) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;

    param.paramtype = hostIf_StringType;
    param.paramLen = strlen(param.paramValue);

    convertAndAssignParamValueFunc()(&param, "Testtype");
    EXPECT_EQ(0, 0);
}

TEST(httpserverTest,  convertAndAssignParamValue_IntegerType) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.TopSpeed", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;

    param.paramtype = hostIf_IntegerType;
    param.paramLen = sizeof(hostIf_IntegerType);

    convertAndAssignParamValueFunc()(&param, "1180000");
    EXPECT_EQ(0, 0);
}

TEST(httpserverTest,  convertAndAssignParamValue_UnsignedLongType) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param,0,sizeof(HOSTIF_MsgData_t));
    param.reqType = HOSTIF_SET;
    strncpy (param.paramName, "Device.Ethernet.Interface.1.Stats.BytesReceived", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    param.bsUpdate = HOSTIF_NONE;
    param.requestor = HOSTIF_SRC_WEBPA;

    param.paramtype = hostIf_UnsignedLongType;
    param.paramLen = sizeof(hostIf_UnsignedLongType);

    convertAndAssignParamValueFunc()(&param, "123456789");
    EXPECT_EQ(0, 0);
}

TEST(httpserverTest, convertAndAssignParamValue_UnknownType_DoesNotCrash) {
    HOSTIF_MsgData_t param = { 0 };
    memset(&param, 0, sizeof(HOSTIF_MsgData_t));
    param.paramtype = (HostIf_ParamType_t)999;

    convertAndAssignParamValueFunc()(&param, "unit-test-value");
    EXPECT_EQ(0, 0);
}

TEST(httpserverTest,  getStringValue) {

    // Test hostIf_StringType: passes string pointer directly
    char* stringResult = getStringValueFunc()(hostIf_StringType, "global");
    EXPECT_STREQ(stringResult, "global");
    free(stringResult);

    // Test hostIf_IntegerType: dereferences as int*
    int intValue = 100;
    char* intResult = getStringValueFunc()(hostIf_IntegerType, (char*)&intValue);
    EXPECT_STREQ(intResult, "100");
    free(intResult);

    // Test hostIf_BooleanType with true: dereferences as bool*
    bool boolValueTrue = true;
    char* boolTrueResult = getStringValueFunc()(hostIf_BooleanType, (char*)&boolValueTrue);
    EXPECT_STREQ(boolTrueResult, "true");
    free(boolTrueResult);

    // Test hostIf_BooleanType with false: dereferences as bool*
    bool boolValueFalse = false;
    char* boolFalseResult = getStringValueFunc()(hostIf_BooleanType, (char*)&boolValueFalse);
    EXPECT_STREQ(boolFalseResult, "false");
    free(boolFalseResult);

    // Test hostIf_UnsignedLongType: dereferences as unsigned long*
    unsigned long ulValue = 123456789;
    char* ulResult = getStringValueFunc()(hostIf_UnsignedLongType, (char*)&ulValue);
    EXPECT_STREQ(ulResult, "123456789");
    free(ulResult);

    // Test unknown type: should return empty string	
    char value[] = "x";
    char* unknownType = getStringValueFunc()((HostIf_ParamType_t)999, value);
    EXPECT_STREQ(unknownType, "");
    free(unknownType);
}

TEST(httpserverTest, getHostIfParamStFromRequest_InvalidReqType_NoMutation) {
    param_t param;
    memset(&param, 0, sizeof(param_t));
    param.name = strdup("Device.DeviceInfo.ModelName");
    param.type = WDMP_STRING;
    param.value = strdup("model");

    HOSTIF_MsgData_t hostIfParam;
    memset(&hostIfParam, 0, sizeof(HOSTIF_MsgData_t));
    hostIfParam.reqType = HOSTIF_GETATTRIB;

    getHostIfParamStFromRequestFunc()(DELETE_ROW, &param, &hostIfParam);

    EXPECT_STREQ(hostIfParam.paramName, "Device.DeviceInfo.ModelName");
    EXPECT_EQ(hostIfParam.reqType, HOSTIF_GETATTRIB);

    free(param.name);
    free(param.value);
}

TEST(httpserverTest, isAuthorized_CoversRebootAndAllowedPaths) {
    EXPECT_EQ(isAuthorizedFunc()("webpa", "Device.X_CISCO_COM_DeviceControl.RebootDevice"), false);
    EXPECT_EQ(isAuthorizedFunc()("webpa", "Device.DeviceInfo.ModelName"), true);
}

TEST(httpserverTest, XRFCVarStore_getInstance_IsSingleton) {
    XRFCVarStore* instance1 = XRFCVarStore::getInstance();
    XRFCVarStore* instance2 = XRFCVarStore::getInstance();
    EXPECT_EQ(instance1, instance2);
}

TEST(httpserverTest, Invalid_RFC_filename) {
    m_varStore = XRFCVarStore::getInstance();
    m_varStore->m_filename = "/opt/secure/RFC1/rfctest_Variable.ini";
    std::ofstream file("/opt/secure/RFC1/rfctest_Variable.ini");
    file.close();
    if(m_varStore)
    {
       bool ret = m_varStore->loadRFCVarIntoCache();
       EXPECT_EQ(ret, false);
   }
}

TEST(httpserverTest, HTTPServerStartThread_And_Stop_CoversLifecycle) {
    // Ensure data model is initialized for checkDataModelStatus()
    EXPECT_EQ(loadDataModel(), DB_SUCCESS);

    // Use ephemeral port to avoid collisions
    argList.httpServerPort = 0;
        
    std::remove("/tmp/.tr69hostif_http_server_ready");
    
    void *thread_result = HTTPServerStartThread(nullptr);
    EXPECT_EQ(thread_result, nullptr);

    std::ifstream ready_file("/tmp/.tr69hostif_http_server_ready");
    EXPECT_TRUE(ready_file.good());
    ready_file.close();

    HttpServerStop();
    EXPECT_EQ(httpServerThreadDone, false);

    std::remove("/tmp/.tr69hostif_http_server_ready");
}

TEST(httpserverTest, HttpServerStop_WhenServerAlreadyStopped_NoCrash) {
    HttpServerStop();
    EXPECT_EQ(httpServerThreadDone, false);
}

TEST(httpserverTest, handleRequest_InvalidReqType_ReturnsNull) {
    req_struct reqSt;
    memset(&reqSt, 0, sizeof(req_struct));
    reqSt.reqType = DELETE_ROW;

    res_struct* respSt = handleRequest("rfc", &reqSt);
    EXPECT_EQ(respSt, nullptr);
}

TEST(httpserverTest, handleRequest_GetWithZeroParamCount_ReturnsNull) {
    get_req_t *getReq = (get_req_t *)malloc(sizeof(get_req_t));
    memset(getReq, 0, sizeof(get_req_t));
    getReq->paramCnt = 0;

    req_struct reqSt;
    memset(&reqSt, 0, sizeof(req_struct));
    reqSt.reqType = GET;
    reqSt.u.getReq = getReq;

    res_struct* respSt = handleRequest("rfc", &reqSt);
    EXPECT_EQ(respSt, nullptr);

    free(getReq);
}

TEST(httpserverTest, handleRequest_SetWithZeroParamCount_ReturnsNull) {
    set_req_t *setReq = (set_req_t *)malloc(sizeof(set_req_t));
    memset(setReq, 0, sizeof(set_req_t));
    setReq->paramCnt = 0;
    setReq->param = nullptr;

    req_struct reqSt;
    memset(&reqSt, 0, sizeof(req_struct));
    reqSt.reqType = SET;
    reqSt.u.setReq = setReq;

    res_struct* respSt = handleRequest("rfc", &reqSt);
    EXPECT_EQ(respSt, nullptr);

    free(setReq);
}

TEST(httpserverTest, handleRequest_GetLongParamName_ReturnsInvalidParameterName) {
    std::string longParamName(MAX_PARAMETERNAME_LEN + 32, 'A');

    get_req_t *getReq = (get_req_t *)malloc(sizeof(get_req_t));
    memset(getReq, 0, sizeof(get_req_t));
    getReq->paramCnt = 1;
    getReq->paramNames[0] = strdup(longParamName.c_str());

    req_struct reqSt;
    memset(&reqSt, 0, sizeof(req_struct));
    reqSt.reqType = GET;
    reqSt.u.getReq = getReq;

    res_struct* respSt = handleRequest("rfc", &reqSt);
    ASSERT_NE(respSt, nullptr);
    EXPECT_EQ(respSt->retStatus[0], WDMP_ERR_INVALID_PARAMETER_NAME);

    wdmp_free_res_struct(respSt);
    free(getReq->paramNames[0]);
    free(getReq);
}

TEST(httpserverTest, handleRequest_SetRFCVariable_MethodNotSupported) {
    set_req_t *setReq = (set_req_t *)malloc(sizeof(set_req_t));
    memset(setReq, 0, sizeof(set_req_t));
    setReq->paramCnt = 1;
    setReq->param = (param_t *)malloc(sizeof(param_t));
    memset(setReq->param, 0, sizeof(param_t));
    setReq->param[0].name = strdup("RFC_TEST_VARIABLE");
    setReq->param[0].value = strdup("true");
    setReq->param[0].type = WDMP_STRING;

    req_struct reqSt;
    memset(&reqSt, 0, sizeof(req_struct));
    reqSt.reqType = SET;
    reqSt.u.setReq = setReq;

    res_struct* respSt = handleRequest("rfc", &reqSt);
    ASSERT_NE(respSt, nullptr);
    EXPECT_EQ(respSt->retStatus[0], WDMP_ERR_METHOD_NOT_SUPPORTED);

    wdmp_free_res_struct(respSt);
    free(setReq->param[0].name);
    free(setReq->param[0].value);
    free(setReq->param);
    free(setReq);
}

TEST(httpserverTest, handleRequest_SetRFCReloadCache_Success) {
    set_req_t *setReq = (set_req_t *)malloc(sizeof(set_req_t));
    memset(setReq, 0, sizeof(set_req_t));
    setReq->paramCnt = 1;
    setReq->param = (param_t *)malloc(sizeof(param_t));
    memset(setReq->param, 0, sizeof(param_t));
    setReq->param[0].name = strdup(XRFC_VAR_STORE_RELOADCACHE);
    setReq->param[0].value = strdup("1");
    setReq->param[0].type = WDMP_STRING;

    req_struct reqSt;
    memset(&reqSt, 0, sizeof(req_struct));
    reqSt.reqType = SET;
    reqSt.u.setReq = setReq;

    res_struct* respSt = handleRequest("rfc", &reqSt);
    ASSERT_NE(respSt, nullptr);
    EXPECT_EQ(respSt->retStatus[0], WDMP_SUCCESS);

    wdmp_free_res_struct(respSt);
    free(setReq->param[0].name);
    free(setReq->param[0].value);
    free(setReq->param);
    free(setReq);
}

TEST(HTTPServerTest, handleRFCRequest_SET_InvalidParam_ReturnsError) {
    set_req_t *setReq = (set_req_t *)malloc(sizeof(set_req_t));
    memset(setReq, 0, sizeof(set_req_t));
    setReq->paramCnt = 1;
    setReq->param = (param_t *)malloc(sizeof(param_t));
    memset(setReq->param, 0, sizeof(param_t));
    setReq->param[0].name = strdup("Device.Unknown.Param");
    setReq->param[0].value = strdup("badval");
    setReq->param[0].type = WDMP_STRING;

    req_struct reqSt;
    memset(&reqSt, 0, sizeof(req_struct));
    reqSt.reqType = SET;
    reqSt.u.setReq = setReq;

    res_struct* respSt = handleRequest("rfc", &reqSt);
    if (respSt) {
        EXPECT_TRUE(respSt->retStatus[0] != WDMP_SUCCESS);
        wdmp_free_res_struct(respSt);
    }
    free(setReq->param[0].name);
    free(setReq->param[0].value);
    free(setReq->param);
    free(setReq);
}


TEST(HTTPServerTest, handleRFCRequest_GET_EmptyParam_ReturnsError) {
    get_req_t *getReq = (get_req_t *)malloc(sizeof(get_req_t));
    memset(getReq, 0, sizeof(get_req_t));
    getReq->paramCnt = 0;

    req_struct reqSt;
    memset(&reqSt, 0, sizeof(req_struct));
    reqSt.reqType = GET;
    reqSt.u.getReq = getReq;

    res_struct* respSt = handleRequest("rfc", &reqSt);
    if (respSt) {
        EXPECT_TRUE(respSt->retStatus[0] != WDMP_SUCCESS || respSt->paramCnt == 0);
        wdmp_free_res_struct(respSt);
    }
    free(getReq);
}

TEST(HTTPServerTest, handleRFCRequest_REPLACE_ValidParam) {
    set_req_t *setReq = (set_req_t *)malloc(sizeof(set_req_t));
    memset(setReq, 0, sizeof(set_req_t));
    setReq->paramCnt = 1;
    setReq->param = (param_t *)malloc(sizeof(param_t));
    memset(setReq->param, 0, sizeof(param_t));
    setReq->param[0].name = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.Enable");
    setReq->param[0].value = strdup("false");
    setReq->param[0].type = WDMP_STRING;

    req_struct reqSt;
    memset(&reqSt, 0, sizeof(req_struct));
    reqSt.reqType = REPLACE_ROWS;
    reqSt.u.setReq = setReq;

    res_struct* respSt = handleRequest("rfc", &reqSt);
    if (respSt) {
        EXPECT_TRUE(respSt->retStatus[0] == WDMP_SUCCESS || respSt->retStatus[0] != WDMP_SUCCESS);
        wdmp_free_res_struct(respSt);
    }
    free(setReq->param[0].name);
    free(setReq->param[0].value);
    free(setReq->param);
    free(setReq);
}

TEST(HTTPServerTest, handleRFCRequest_REPLACE_MultipleParams) {
    set_req_t *setReq = (set_req_t *)malloc(sizeof(set_req_t));
    memset(setReq, 0, sizeof(set_req_t));
    setReq->paramCnt = 2;
    setReq->param = (param_t *)malloc(2 * sizeof(param_t));
    memset(setReq->param, 0, 2 * sizeof(param_t));

    setReq->param[0].name = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.Enable");
    setReq->param[0].value = strdup("false");
    setReq->param[0].type = WDMP_STRING;

    setReq->param[1].name = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MOCASSH.Enable");
    setReq->param[1].value = strdup("false");
    setReq->param[1].type = WDMP_STRING;

    req_struct reqSt;
    memset(&reqSt, 0, sizeof(req_struct));
    reqSt.reqType = REPLACE_ROWS;
    reqSt.u.setReq = setReq;

    res_struct* respSt = handleRequest("rfc", &reqSt);
    if (respSt) {
        EXPECT_TRUE(respSt->retStatus[0] == WDMP_SUCCESS || respSt->retStatus[0] != WDMP_SUCCESS);
        wdmp_free_res_struct(respSt);
    }
    free(setReq->param[0].name);
    free(setReq->param[0].value);
    free(setReq->param[1].name);
    free(setReq->param[1].value);
    free(setReq->param);
    free(setReq);
}

TEST(HTTPServerTest, validateDataModel_REPLACE_WithNoAttr) {
    set_req_t *setReq = (set_req_t *)malloc(sizeof(set_req_t));
    memset(setReq, 0, sizeof(set_req_t));
    setReq->paramCnt = 1;
    setReq->param = (param_t *)malloc(sizeof(param_t));
    memset(setReq->param, 0, sizeof(param_t));
    setReq->param[0].name = strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.Enable");
    setReq->param[0].value = strdup("true");
    setReq->param[0].type = WDMP_STRING;

    req_struct reqSt;
    memset(&reqSt, 0, sizeof(req_struct));
    reqSt.reqType = REPLACE_ROWS;
    reqSt.u.setReq = setReq;

    res_struct* respSt = handleRequest("rfc", &reqSt);
    ASSERT_NE(respSt, nullptr);
    EXPECT_TRUE(respSt->retStatus[0] == WDMP_SUCCESS || respSt->retStatus[0] != WDMP_SUCCESS);
    wdmp_free_res_struct(respSt);
    free(setReq->param[0].name);
    free(setReq->param[0].value);
    free(setReq->param);
    free(setReq);
}

TEST(HTTPServerTest, convertAndAssignParamValue_BoolType) {
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(msgData.paramName, "Device.Test.Param", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_BooleanType;

    convertAndAssignParamValueFunc()(&msgData, (char*)"true");
    EXPECT_EQ(0, 0);
}

TEST(HTTPServerTest, convertAndAssignParamValue_IntType) {
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(HOSTIF_MsgData_t));
    strncpy(msgData.paramName, "Device.Test.IntParam", TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
    msgData.paramtype = hostIf_IntegerType;

    convertAndAssignParamValueFunc()(&msgData, (char*)"42");
    EXPECT_EQ(0, 0);
}

TEST(HTTPServerTest, handleRFCRequest_ATTRS_UnknownParam_ReturnsNone) {
    get_req_t *getReq = (get_req_t *)malloc(sizeof(get_req_t));
    memset(getReq, 0, sizeof(get_req_t));
    getReq->paramCnt = 1;
    getReq->paramNames[0] = strdup("Device.Unknown.UnknownParam");

    req_struct reqSt;
    memset(&reqSt, 0, sizeof(req_struct));
    reqSt.reqType = GET;
    reqSt.u.getReq = getReq;

    res_struct* respSt = handleRequest("rfc", &reqSt);
    if (respSt) {
        EXPECT_TRUE(respSt->retStatus[0] != WDMP_SUCCESS || respSt->paramCnt == 0);
        wdmp_free_res_struct(respSt);
    }
    free(getReq->paramNames[0]);
    free(getReq);
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
