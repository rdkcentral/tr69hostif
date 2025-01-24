#include "libIBus.h"
#include "webpa_parameter.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_main.h"
#include "XrdkCentralComRFCStore.h"
#include <mutex>
#include "dsError.h"
#include "dsAVDTypes.h"
using namespace std;

namespace edid_parser {

enum edid_status_e {
    EDID_STATUS_OK,
    EDID_STATUS_INVALID_PARAMETER,
    EDID_STATUS_NOT_SUPPORTED,
    EDID_STATUS_INVALID_HEADER,
    EDID_STATUS_INVALID_CHECKSUM
};
}
class HdmiInput  
{

public:
    static HdmiInput & getInstance();

    bool    isPortConnected          (int8_t Port) const;
    
private:
    HdmiInput ();           /* default constructor */
    virtual ~HdmiInput ();  /* destructor */
};



int main()
{
}

int hostIf_SetMsgHandler(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    return ret;
}

void getValues (const char *paramName[], const unsigned int paramCount, param_t ***paramValArr,size_t **retValCount, WDMP_STATUS **retStatus)
{

}

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
IARM_Result_t IARM_Bus_RegisterCall(const char *methodName, IARM_BusCall_t handler)
{
return IARM_RESULT_SUCCESS;
}
IARM_Result_t IARM_Bus_Term(void)
{
return IARM_RESULT_SUCCESS;
}
IARM_Result_t IARM_Bus_Disconnect(void)
{
return IARM_RESULT_SUCCESS;
}
IARM_Result_t IARM_Bus_RegisterEvent(IARM_EventId_t maxEventId)
{
return IARM_RESULT_SUCCESS;
}
IARM_Result_t IARM_Bus_Call(const char* ownerName, const char* methodName, void* arg, size_t argLen)
{
    return IARM_RESULT_SUCCESS;
}
extern dsError_t dsGetStereoMode(intptr_t handle, dsAudioStereoMode_t *stereoMode, bool isPersist)
{
}
extern dsError_t dsEnableAudioPort(intptr_t handle, bool enabled, const char* portName)
{
}
extern dsError_t dsSetStereoMode(intptr_t handle, dsAudioStereoMode_t mode, bool isPersist)
{
}
extern dsError_t dsSetEnablePersist(intptr_t handle, const char* portName, bool enabled)
{
}
extern dsError_t dsSetStereoAuto(intptr_t handle, int autoMode, bool isPersist)
{
	
}
extern dsError_t dsGetEnablePersist(intptr_t handle, const char* portName, bool* enabled)
{

}
namespace device 
{
bool HdmiInput::isPortConnected(int8_t Port) const
{

}
HdmiInput & HdmiInput::getInstance()
{

}
}
namespace edid_parser 
{
edid_status_e EDID_Verify(unsigned char* bytes, size_t count)
{
	
}
}
