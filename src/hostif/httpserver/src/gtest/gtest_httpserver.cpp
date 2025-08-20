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
#include "hostIf_utils.h"
#include "hostIf_main.h"
#include "webpa_notification.h"
#include "webpa_parameter.h"
#include "rbus_value.h"

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

#ifdef GTEST_ENABLE
extern DATA_TYPE (*getWdmpDataTypeFunc())(char * );
extern HostIf_ParamType_t (*getHostIfParamTypeFunc())(DATA_TYPE wdmpDataType);
bool (*validateParamValueFunc())(const string &paramValue, HostIf_ParamType_t dataType);
WDMP_STATUS (*handleRFCRequestFunc())(REQ_TYPE reqType, param_t *param);
WDMP_STATUS (*invokeHostIfAPIFunc())(REQ_TYPE reqType, param_t *param, HostIf_Source_Type_t bsUpdate, const char *pcCallerID);
WDMP_STATUS (*validateAgainstDataModelFunc())(REQ_TYPE reqType, char* paramName, const char* paramValue, DATA_TYPE *dataType, char **defaultValue, HostIf_Source_Type_t *bsUpdate);
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


TEST(httpserverTest, getWdmpDataType) {
    EXPECT_EQ(getWdmpDataTypeFunc()("string"), WDMP_STRING);
    EXPECT_EQ(getWdmpDataTypeFunc()("boolean"), WDMP_BOOLEAN);
    EXPECT_EQ(getWdmpDataTypeFunc()("unsignedInt"), WDMP_UINT);
    EXPECT_EQ(getWdmpDataTypeFunc()("int"),  WDMP_INT);
    EXPECT_EQ(getWdmpDataTypeFunc()("unsignedLong"), WDMP_ULONG);
    EXPECT_EQ(getWdmpDataTypeFunc()("dataTime"), WDMP_DATETIME);
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
}

TEST(httpserverTest, handleRFCRequest_GET) {
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

TEST(httpserverTest, invokeHostIfAPI) {
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
    EXPECT_EQ(respSt->retStatus[0], WDMP_ERR_DEFAULT_VALUE);
}

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
