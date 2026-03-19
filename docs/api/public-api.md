# Public API

## Overview

The core public contract for `tr69hostif` is the shared request/response envelope declared in `src/hostif/include/hostIf_tr69ReqHandler.h`. IPC front ends such as IARM and WebPA populate this structure, invoke the appropriate dispatcher path, and inspect the returned `faultCode` and value fields.

## Core Types

### `HOSTIF_MsgData_t`

```c
typedef struct _HostIf_MsgData_t {
    char paramName[TR69HOSTIFMGR_MAX_PARAM_LEN];
    char paramValue[TR69HOSTIFMGR_MAX_PARAM_LEN];
    char* paramValueLong;
    char transactionID[_BUF_LEN_256];
    short paramLen;
    short instanceNum;
    HostIf_ParamType_t paramtype;
    HostIf_ReqType_t reqType;
    faultCode_t faultCode;
    HostIf_Source_Type_t requestor;
    HostIf_Source_Type_t bsUpdate;
    bool isLengthyParam;
} HOSTIF_MsgData_t;
```

### Field semantics

| Field | Meaning |
|-------|---------|
| `paramName` | Fully qualified TR-181 parameter path |
| `paramValue` | Inline string buffer for normal-length values |
| `paramValueLong` | Heap buffer for long values when `isLengthyParam` is true |
| `transactionID` | Correlation token for remote callers |
| `paramLen` | Returned value length |
| `instanceNum` | Object instance identifier when applicable |
| `paramtype` | Value type such as string, int, bool, or unsigned long |
| `reqType` | GET, SET, GETATTRIB, or SETATTRIB |
| `faultCode` | TR-069 fault code returned to caller |
| `requestor` | Request source classification |
| `bsUpdate` | Bootstrap-update source level |

## Enums

### `HostIf_ParamType_t`

- `hostIf_StringType`
- `hostIf_IntegerType`
- `hostIf_UnsignedIntType`
- `hostIf_BooleanType`
- `hostIf_DateTimeType`
- `hostIf_UnsignedLongType`

### `HostIf_ReqType_t`

- `HOSTIF_GET`
- `HOSTIF_SET`
- `HOSTIF_GETATTRIB`
- `HOSTIF_SETATTRIB`

### `faultCode_t`

| Value | Meaning |
|-------|---------|
| `fcNoFault` | Success |
| `fcMethodNotSupported` | Unsupported RPC or operation |
| `fcRequestDenied` | Access denied |
| `fcInternalError` | Internal processing failure |
| `fcInvalidArguments` | Invalid request arguments |
| `fcResourcesExceeded` | Resource exhaustion |
| `fcInvalidParameterName` | Unknown or unmapped parameter |
| `fcInvalidParameterType` | Type mismatch |
| `fcInvalidParameterValue` | Value outside accepted range |
| `fcAttemptToSetaNonWritableParameter` | Attempt to write read-only parameter |

## Dispatcher Entry Points

The primary C/C++ entry points are declared in `src/hostif/handlers/include/hostIf_msgHandler.h`.

### `hostIf_GetMsgHandler()`

```c
int hostIf_GetMsgHandler(HOSTIF_MsgData_t *stMsgData);
```

Routes a GET request to the appropriate manager. The function serializes top-level GET handling with `get_handler_mutex`, resolves the manager from `paramName`, and invokes `handleGetMsg()` on the selected handler.

### `hostIf_SetMsgHandler()`

```c
int hostIf_SetMsgHandler(HOSTIF_MsgData_t *stMsgData);
```

Routes a SET request to the appropriate manager. The function serializes top-level SET handling with `set_handler_mutex` and delegates to `handleSetMsg()`.

### Attribute operations

```c
int hostIf_GetAttributesMsgHandler(HOSTIF_MsgData_t *stMsgData);
int hostIf_SetAttributesMsgHandler(HOSTIF_MsgData_t *stMsgData);
```

These use the same manager resolution model for attribute-specific flows.

### Utility helpers

```c
void hostIf_Init_Dummy_stMsgData(HOSTIF_MsgData_t **stMsgData);
void hostIf_Print_msgData(HOSTIF_MsgData_t *stMsgData);
void hostIf_Free_stMsgData(HOSTIF_MsgData_t *stMsgData);
void paramValueToString(const HOSTIF_MsgData_t *stMsgData, char *paramValueStr, size_t strSize);
```

These helpers are used by internal adapters and tests to initialize, print, free, or stringify the shared envelope.

## IARM Interface

The IARM-facing contract is declared in `src/hostif/include/hostIf_tr69ReqHandler.h`.

### Lifecycle

```c
bool hostIf_IARM_IF_Start(void);
void hostIf_IARM_IF_Stop(void);
```

These functions initialize and tear down the daemon's IARM registration.

### RPC names

| Macro | RPC name |
|-------|----------|
| `IARM_BUS_TR69HOSTIFMGR_API_SetParams` | `tr69HostIfSetParams` |
| `IARM_BUS_TR69HOSTIFMGR_API_GetParams` | `tr69HostIfGetParams` |
| `IARM_BUS_TR69HOSTIFMGR_API_SetAttributes` | `tr69HostIfGetAttributes` |
| `IARM_BUS_TR69HOSTIFMGR_API_GetAttributes` | `tr69HostIfSetAttributes` |
| `IARM_BUS_TR69HOSTIFMGR_API_RegisterForEvents` | `tr69HostIfRegisterForEvents` |

### Events

| Event | Meaning |
|-------|---------|
| `IARM_BUS_TR69HOSTIFMGR_EVENT_ADD` | Dynamic object instance added |
| `IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE` | Dynamic object instance removed |
| `IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED` | Existing parameter value changed |

Event payloads use:

```c
typedef struct _tr69HostIfMgr_EventData_t {
    char paramName[TR69HOSTIFMGR_MAX_PARAM_LEN];
    char paramValue[TR69HOSTIFMGR_MAX_PARAM_LEN];
    HostIf_ParamType_t paramtype;
} IARM_Bus_tr69HostIfMgr_EventData_t;
```

## Thread Safety

- Top-level GET dispatch is serialized.
- Top-level SET dispatch is serialized.
- The API does not guarantee that individual profile handlers are reentrant beyond the dispatcher-level locking shown above.
- Callers that allocate `paramValueLong` must preserve a matching cleanup path.

## Example: Internal GET Request

```c
#include <string.h>

#include "hostIf_msgHandler.h"
#include "hostIf_tr69ReqHandler.h"

int query_manufacturer(void)
{
    HOSTIF_MsgData_t request;
    memset(&request, 0, sizeof(request));

    strncpy(request.paramName,
            "Device.DeviceInfo.Manufacturer",
            sizeof(request.paramName) - 1);
    request.reqType = HOSTIF_GET;
    request.paramtype = hostIf_StringType;
    request.requestor = HOSTIF_SRC_IARM;

    if (hostIf_GetMsgHandler(&request) != 0) {
        return -1;
    }

    if (request.faultCode != fcNoFault) {
        return -1;
    }

    return 0;
}
```

## See Also

- [System Overview](../architecture/overview.md)
- [Data Flow](../architecture/data-flow.md)
- [Testing](../integration/testing.md)