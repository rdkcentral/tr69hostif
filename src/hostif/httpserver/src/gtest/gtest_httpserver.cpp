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

TEST(httpserverTest,  getStringValue) {
    EXPECT_STREQ(getStringValueFunc()(hostIf_StringType, "global"), "global");
    EXPECT_STREQ(getStringValueFunc()(hostIf_IntegerType, "100"), "3158065");
    EXPECT_STREQ(getStringValueFunc()(hostIf_BooleanType, "true"), "false");
    EXPECT_STREQ(getStringValueFunc()(hostIf_BooleanType, "false"), "false");
    EXPECT_STREQ(getStringValueFunc()(hostIf_UnsignedLongType, "123456789"), "4050765991979987505");
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

TEST(httpserverTest, HTTPRequestHandler_GET) { 

    // Create a SoupMessage for POST
    SoupServer *server = soup_server_new(nullptr, nullptr);
    SoupMessage *msg = soup_message_new("POST", "http://localhost/api/status");
    const char *json_body = "{\"action\":\"ping\"}";
    GBytes *body_bytes = g_bytes_new_static(json_body, strlen(json_body));
    soup_message_set_request_body_from_bytes(msg, "application/json", body_bytes);
    SoupMessageHeaders *headers = soup_message_get_request_headers(msg);
    soup_message_headers_append(headers, "CallerID", "unittest");

    HTTPRequestHandlerFunc()(server, reinterpret_cast<SoupServerMessage *>(msg), "/api/status", nullptr, nullptr); 
    EXPECT_EQ(0, 0);
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
