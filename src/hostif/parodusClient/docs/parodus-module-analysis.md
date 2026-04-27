# Parodus Module Analysis

## Overview

`tr69hostif` currently embeds the Parodus client directly as a compiled-in subsystem. This document explains why the integration is structured the way it is today, what problems that creates operationally, and what a separation into an independent module would look like.

---

## 1. Why It Is the Way It Is Today

### 1.1 Historical Context

Parodus is the WebPA client gateway that connects RDK devices to the cloud-side management infrastructure over a persistent WebSocket. When WebPA was introduced into the RDK ecosystem, `tr69hostif` was already the resident TR-181 parameter manager. The shortest path to expose TR-181 parameters over WebPA was to embed the Parodus client library connection directly into `tr69hostif` so it could call `hostIf_GetMsgHandler()` and `hostIf_SetMsgHandler()` without any IPC.

### 1.2 Current Architecture

```mermaid
flowchart TB
    subgraph tr69hostif process
        MAIN[hostIf_main.cpp\nMain thread + GLib loop]
        PARODUS_THREAD[libpd_client_mgr thread\nPARODUS_ENABLE compile flag]
        WALDB[waldb\nXML data-model validator]
        PAL[PAL layer\nwebpa_adapter\nwebpa_parameter\nwebpa_attribute\nwebpa_notification]
        START_PARODUS[startParodus binary\nbuilt separately\nlaunched by system]

        MAIN -->|pthread_create| PARODUS_THREAD
        PARODUS_THREAD --> PAL
        PAL --> WALDB
        PAL -->|hostIf_GetMsgHandler\nhostIf_SetMsgHandler| MAIN
    end

    subgraph External
        LIBPARODUS[libparodus.so\nexternal dependency]
        PARODUS_DAEMON[parodus binary\nWebSocket gateway to cloud]
        CLOUD[Cloud Management\nWebPA / XMiDT]
    end

    PARODUS_THREAD -->|libparodus_init\nlibparodus_receive\nlibparodus_send| LIBPARODUS
    LIBPARODUS -->|ZeroMQ tcp://127.0.0.1:6666| PARODUS_DAEMON
    PARDOUS_DAEMON --- CLOUD

    START_PARODUS -->|v_secure_system backgroundrun| PARODUS_DAEMON
```

### 1.3 Component Inventory

| Component | Where it lives | Role |
|---|---|---|
| `libpd.cpp` | `parodusClient/pal/` | Entry point for the Parodus IPC thread. Owns `libparodus_init`, receive loop, and `sendNotification`. |
| `webpa_adapter.cpp` | `parodusClient/pal/` | WRP message router. Calls `wdmp_parse_request` and dispatches GET/SET/GETATTR/SETATTR to `webpa_parameter` and `webpa_attribute`. |
| `webpa_parameter.cpp` | `parodusClient/pal/` | Translates WDMP GET/SET into `HOSTIF_MsgData_t` and calls `hostIf_GetMsgHandler()` / `hostIf_SetMsgHandler()`. |
| `webpa_attribute.cpp` | `parodusClient/pal/` | Translates WDMP GETATTR/SETATTR into notify-flag operations via `hostIf_GetAttributesMsgHandler`. |
| `webpa_notification.cpp` | `parodusClient/pal/` | Builds notification WRP events and calls `sendNotification()`. Reads notify config from `notify_webpa_cfg.json`. |
| `waldb.cpp` | `parodusClient/waldb/` | Parses the XML data model (`/tmp/data-model.xml`) to validate parameter names and resolve wildcard paths. Linked as `libwaldb.la` into the main binary. |
| `startParodus.cpp` | `parodusClient/startParodus/` | Separate binary. Collects device identity (MAC, serial, partner ID, firmware version) and launches the `parodus` daemon via `v_secure_system`. |

### 1.4 Thread Lifecycle

The Parodus subsystem is spawned from `hostIf_main.cpp` behind the `PARODUS_ENABLE` compile-time flag:

```c
// hostIf_main.cpp:482
if(0 == pthread_create(&parodus_init_tid, NULL, libpd_client_mgr, NULL))
```

Inside `libpd_client_mgr`:

1. `checkDataModelStatus()` validates the XML data model via `waldb`.
2. `connect_parodus()` calls `pthread_detach(pthread_self())` — **the thread detaches itself** — then retries `libparodus_init()` with exponential backoff until the parodus daemon is reachable over ZeroMQ.
3. Once connected, `registerNotifyCallback()` and `setInitialNotify()` configure change notifications.
4. `parodus_receive_wait()` runs a blocking receive loop, dispatching every `WRP_MSG_TYPE__REQ` to `processRequest()`.

Because `connect_parodus()` detaches the thread, the main thread cannot `pthread_join(parodus_init_tid)` during shutdown. Any attempt to do so causes a crash (see the `pthread_detach_crash_fix` user memory note).

### 1.5 startParodus Coupling

`startParodus` is a separate compiled binary but lives inside the same source tree and build system. Its job is to read device identity parameters — many of which are TR-181 parameters — and launch the `parodus` daemon with them as command-line arguments. It reads several values via `getRFCParameter()` (serial number, boot time, server URL, token server URL) and reads partner ID directly from `/opt/www/authService/partnerId3.dat`, replicating the same PartnerId resolution logic that already exists in `XBSStore`.

<<<<<<< HEAD
---

## 2. Drawbacks and Issues with the Current Design
=======
### 1.6 Startup Sequence

Two independent activities bring the full WebPA channel online. They are not directly coordinated; `libpd_client_mgr` retries until the parodus daemon is reachable regardless of how it was launched.

```mermaid
sequenceDiagram
    participant SYS as System / systemd
    participant SP as startParodus binary
    participant PD as parodus daemon
    participant MAIN as hostIf_main.cpp
    participant LPD as libpd_client_mgr thread
    participant LIBP as libparodus.so

    SYS->>SP: launch startParodus
    SP->>SP: read HW MAC from /tmp/.macAddress
    SP->>SP: read PartnerId from partnerId3.dat
    SP->>SP: read SerialNumber / BootTime via getRFCParameter()
    SP->>SP: read WebPA URL / JWT / network interface from webpa_cfg.json
    SP->>PD: v_secure_system backgroundrun /usr/bin/parodus --hw-mac=... --partner-id=...
    PD-->>SP: daemon launched (background)

    MAIN->>MAIN: init profile handlers, HTTP server, RFC store
    MAIN->>LPD: pthread_create(libpd_client_mgr)
    LPD->>LPD: checkDataModelStatus() load /tmp/data-model.xml via waldb
    LPD->>LPD: connect_parodus() → pthread_detach(self)
    LPD->>LPD: get_parodus_url() from /etc/webpa_cfg.json

    loop exponential backoff until parodus responds
        LPD->>LIBP: libparodus_init(&cfg)
        LIBP->>PD: ZeroMQ connect tcp://127.0.0.1:6666
        PD-->>LIBP: accept or reject
        LIBP-->>LPD: ret == 0 (success) or error
    end

    LPD->>LPD: registerNotifyCallback()
    LPD->>LPD: setInitialNotify()
    LPD->>LPD: write /tmp/webpa/start_time
    LPD->>LPD: enter parodus_receive_wait() loop
```

### 1.7 Inbound WRP Request Flow (Cloud GET / SET)

This is the complete path from a cloud-originated WebPA GET or SET request to the TR-181 profile handler and back to the cloud.

```mermaid
sequenceDiagram
    participant CLOUD as Cloud / XMiDT
    participant PD as parodus daemon
    participant LIBP as libparodus.so
    participant RW as parodus_receive_wait()
    participant WA as webpa_adapter processRequest()
    participant WP as webpa_parameter getValues / setValues
    participant WALDB as waldb checkDataModelStatus
    participant MSG as hostIf_GetMsgHandler / hostIf_SetMsgHandler
    participant PROF as TR-181 Profile Handler

    CLOUD->>PD: WebSocket WRP REQ (GET or SET)
    PD->>LIBP: deliver via ZeroMQ
    LIBP-->>RW: libparodus_receive() returns wrp_msg

    RW->>WA: processRequest(payload, transaction_uuid)
    WA->>WA: wdmp_parse_request() parse WDMP JSON

    alt GET request
        WA->>WP: getValues(paramNames, count)
        WP->>WALDB: checkDataModelStatus / wildcard expand
        WALDB-->>WP: validated param list
        WP->>MSG: hostIf_GetMsgHandler(HOSTIF_MsgData_t)
        MSG->>PROF: profile handler get()
        PROF-->>MSG: value in paramValue
        MSG-->>WP: return OK / NOK
        WP-->>WA: param_t array
    else SET request
        WA->>WP: setValues(paramVal, count)
        WP->>MSG: hostIf_SetMsgHandler(HOSTIF_MsgData_t) with requestor=HOSTIF_SRC_WEBPA
        MSG->>PROF: profile handler set()
        PROF-->>MSG: faultCode
        MSG-->>WP: return OK / NOK
        WP-->>WA: WDMP_STATUS
    end

    WA->>WA: wdmp_form_response() build JSON response
    WA-->>RW: resPayload string
    RW->>LIBP: libparodus_send(res_wrp_msg)
    LIBP->>PD: ZeroMQ reply
    PD->>CLOUD: WebSocket WRP response
```

**Key timing note:** `processRequest()` runs synchronously on the same thread as the receive loop. While `hostIf_GetMsgHandler()` is executing, no further WRP messages can be received from the parodus daemon. HAL calls inside profile handlers can take 100–500 ms on some device platforms, which causes visible WebPA latency.

### 1.8 Notification Push Flow (Parameter Change to Cloud)

When a TR-181 parameter that has been marked for WebPA notification changes, the change is pushed proactively to the cloud without waiting for a GET from the cloud.

```mermaid
sequenceDiagram
    participant PROF as TR-181 Profile Handler
    participant NH as NotificationHandler
    participant WN as webpa_notification.cpp
    participant LPD as libpd.cpp sendNotification()
    participant LIBP as libparodus.so
    participant PD as parodus daemon
    participant CLOUD as Cloud / XMiDT

    PROF->>NH: notifyCallback() on param change
    NH->>WN: build notification JSON payload
    WN->>WN: getNotifySource() → mac:device_mac
    WN->>LPD: sendNotification(payload, source, destination)

    LPD->>LPD: build WRP_MSG_TYPE__EVENT struct
    loop retry up to 3 times with exponential backoff
        LPD->>LIBP: libparodus_send(notif_wrp_msg)
        LIBP->>PD: ZeroMQ event message
        PD-->>LIBP: send status
        LIBP-->>LPD: sendStatus == 0 (success) or error
    end

    PD->>CLOUD: WebSocket WRP EVENT push
```

Initial notification state is configured at startup by `setInitialNotify()`, which reads `notify_webpa_cfg.json` (checked from `/opt/` first, falling back to `/etc/`) to determine which parameters should have notifications enabled on connect.

### 1.9 Threading Model

#### Thread Inventory

| Thread | Created by | Entry point | Detached? | Purpose |
|---|---|---|---|---|
| Main (GLib loop) | OS | `hostIf_main::main()` | No | Init, signal handling, profile router |
| Parodus IPC | `pthread_create` in `hostIf_main` | `libpd_client_mgr()` | Yes (self-detaches in `connect_parodus`) | libparodus connect, receive loop, notification send |

#### Synchronization Primitives Used by the Parodus Subsystem

| Primitive | Declared in | Purpose |
|---|---|---|
| `pthread_cond_t parodus_cond` | `libpd.cpp` (global) | Wakes the receive loop retry wait when `stop_parodus_recv_wait()` is called |
| `pthread_mutex_t parodus_lock` | `libpd.cpp` (global) | Guards `parodus_cond` during `pthread_cond_timedwait` in the error retry path |
| `std::mutex g_db_mutex` | `waldb.cpp` | Serializes access to the XML data model handle during `loadDataModel()` |

#### Thread State Diagram

```mermaid
stateDiagram-v2
    [*] --> Created : pthread_create in hostIf_main
    Created --> DataModelLoad : libpd_client_mgr entry
    DataModelLoad --> DataModelFail : checkDataModelStatus != 0
    DataModelFail --> [*] : return NULL
    DataModelLoad --> Connecting : checkDataModelStatus == 0
    Connecting --> Connecting : libparodus_init failed → sleep backoff
    Connecting --> Detached : pthread_detach(self) called inside connect_parodus
    Detached --> Connected : libparodus_init ret == 0
    Connected --> ReceiveLoop : parodus_receive_wait entered
    ReceiveLoop --> ReceiveLoop : receive and dispatch WRP messages
    ReceiveLoop --> ErrorRetry : libparodus_receive returned non-zero
    ErrorRetry --> ReceiveLoop : pthread_cond_timedwait timeout or signal
    ReceiveLoop --> Shutdown : exit_parodus_recv == true
    Shutdown --> [*] : libparodus_close_receiver + libparodus_shutdown
```

**Note on the Detached state:** once `connect_parodus()` calls `pthread_detach(pthread_self())`, the thread handle `parodus_init_tid` in `hostIf_main.cpp` becomes invalid for any join operation. The thread will run until `exit_parodus_recv` is set true via `stop_parodus_recv_wait()`.

---
>>>>>>> 22b609abda3573678a880df3716a5f25e968b7f6

### 2.1 Thread Detachment Causes Crash-Risk on Shutdown

`connect_parodus()` calls `pthread_detach(pthread_self())` at the start of its retry loop. The thread is therefore detached for its entire lifetime. The `parodus_init_tid` handle stored in `hostIf_main.cpp` is invalid for joining after that point. Any future refactor that adds `pthread_join(parodus_init_tid, NULL)` to the shutdown path will crash in `__pthread_clockjoin_ex`.

### 2.2 Blocking Receive Loop Holds an Entire Thread Permanently

`parodus_receive_wait()` is a `while(1)` loop that calls `libparodus_receive(libparodus_instance, &wrp_msg, 2000)`. If the parodus daemon becomes unreachable (network failure, daemon restart), the loop falls into a 5-second `pthread_cond_timedwait` retry cycle. The thread never exits cleanly and never participates in structured shutdown.

The exit flag `exit_parodus_recv` is set by `stop_parodus_recv_wait()`, but that function is only called during normal daemon shutdown. If the receive call itself hangs (latent ZeroMQ socket behavior), the flag check is never reached.

### 2.3 processRequest Is Synchronous and Blocks the Receive Thread

`processRequest()` is called directly from the receive loop on the same Parodus thread. It calls through `webpa_parameter.cpp` → `hostIf_GetMsgHandler()` → concrete profile handlers, some of which make HAL calls that can take hundreds of milliseconds. During that time, no new WRP messages can be received. A slow HAL or a hung IARM call causes the entire WebPA channel to back up.

### 2.4 waldb Is Linked Into the Main Binary

`libwaldb.la` is linked directly into `tr69hostif`. This means the XML data-model XML is loaded into the same process address space and parsed at startup. The data model XML is large; loading it costs RSS memory even when the Parodus channel is not in use. If Parodus is not needed on a platform, the memory is still consumed because the library is unconditionally linked (even if `PARODUS_ENABLE` guards the thread creation, the library symbols are always resolved).

### 2.5 Duplicate PartnerId Resolution in startParodus

`startParodus.cpp` re-implements PartnerId reading from `/opt/www/authService/partnerId3.dat` and falls back to `getRFCParameter()` for `Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId`. This duplicates the same logic that already exists in `Device_DeviceInfo.cpp::get_PartnerId_From_Script()` and `XBSStore`. There are now three independent code paths that each read PartnerId independently, each with their own fallback handling and file-open retry logic.

### 2.6 Configuration Is Fragmented Across Multiple Files

The Parodus subsystem reads configuration from at least four separate sources:

| Parameter | Source |
|---|---|
| Parodus ZeroMQ URL | `/etc/webpa_cfg.json` or `/opt/webpa_cfg.json` |
| Client ZeroMQ URL | `/etc/webpa_cfg.json` or `/opt/webpa_cfg.json` |
| WebPA server IP | `getRFCParameter("Device.X_RDK_WebPA_Server.URL")` |
| Notification config | `/etc/notify_webpa_cfg.json` or `/opt/notify_webpa_cfg.json` |
| JWT key path | Hardcoded `/etc/ssl/certs/webpa-rs256.pem` |
| CRUD config | Hardcoded `/opt/secure/parodus_cfg.json` |
| Partner ID | File → RFC parameter fallback (duplicated) |

There is no single configuration object that a test or integration environment can substitute cleanly.

### 2.7 No Independent Testability

Because the PAL layer calls `hostIf_GetMsgHandler()` / `hostIf_SetMsgHandler()` directly (function calls into the same process), it is impossible to unit-test the Parodus request dispatch path without bringing up the full `tr69hostif` subsystem. The unit tests in `parodusClient/gtest/` stub out waldb and the IPC layer but cannot test a real parameter GET through the Parodus path without the entire profile manager being initialized.

### 2.8 Build Flag Inconsistency

Some platform builds define `PARODUS_ENABLE` to include the receive thread but do not define `WEB_CONFIG_ENABLED` or `WEBCONFIG_LITE_ENABLE`, leaving the startParodus binary unused but still compiled. The compile-time flag guards only the thread creation, not the library linkage, which means object files and their static data are always included.

---

## 3. Potential Solution: Separating Parodus as an Independent Module

### 3.1 Target Architecture

The core idea is to decouple the Parodus client from the `tr69hostif` process entirely. Instead of calling `hostIf_GetMsgHandler()` directly, the Parodus module communicates with `tr69hostif` through the existing RBUS or HTTP server interface.

```mermaid
flowchart TB
    subgraph tr69hostif process
        MAIN[hostIf_main\nTR-181 parameter store]
        RBUS[RBUS provider\nor HTTP server]
        MAIN --> RBUS
    end

    subgraph parodus-client process NEW
        PARODUSMGR[Parodus client manager]
        WALDB_STANDALONE[waldb\nstandalone library]
        PAL_STANDALONE[PAL layer\nwebpa_adapter\nwebpa_parameter\nwebpa_attribute]
        START_P[startParodus\ndevice identity collector]

        PARODUSMGR --> PAL_STANDALONE
        PAL_STANDALONE --> WALDB_STANDALONE
        START_P --> PARODUSMGR
    end

    subgraph External
        LIBPARODUS[libparodus.so]
        PARODUS_DAEMON[parodus binary]
        CLOUD[Cloud Management]
    end

    PAL_STANDALONE -->|RBUS get/set\nor HTTP REST| RBUS
    PARODUSMGR -->|libparodus IPC| LIBPARODUS
    LIBPARODUS -->|ZeroMQ| PARODUS_DAEMON
    PARODUS_DAEMON --- CLOUD
```

### 3.2 What Changes

**Replace direct function calls with IPC:**

`webpa_parameter.cpp` currently calls `hostIf_GetMsgHandler()` and `hostIf_SetMsgHandler()` directly. In the separated design, those calls are replaced with RBUS `rbusValue_Get` / `rbusValue_Set` calls, or with HTTP REST calls to `tr69hostif`'s HTTP server. The PAL layer becomes protocol-agnostic.

**Move waldb out of tr69hostif linkage:**

`libwaldb.la` should be a dependency of the parodus-client process only. It is removed from `tr69hostif_LDADD`. This reduces the resident memory of `tr69hostif` on platforms that do not use WebPA.

**Consolidate PartnerId reading:**

`startParodus.cpp` should query PartnerId via a single TR-181 GET (`Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId`) through the IPC interface instead of reading the file directly. `tr69hostif` already owns the canonical resolution path.

**Give the thread a clean shutdown path:**

Remove `pthread_detach(pthread_self())` from `connect_parodus()`. The calling context — now the parodus-client process's own `main()` — can manage the thread lifecycle and call `pthread_join` cleanly.

**Unify configuration loading:**

All Parodus configuration is consolidated into a single structure loaded from `/etc/parodus_client.json` (or the existing `webpa_cfg.json` extended with the missing keys). No more fragmented reads across four files.

### 3.3 Interface Contract

The separated parodus-client module needs two capabilities from `tr69hostif`:

| Need | Provided by |
|---|---|
| GET any TR-181 parameter | RBUS `rbusValue_Get` or HTTP GET `/tr181?param=...` |
| SET any TR-181 parameter | RBUS `rbusValue_Set` or HTTP POST `/tr181` |
| Change notifications (push to cloud) | RBUS event subscription or `tr69hostif` notification callback |

No startup ordering dependency is required: the parodus-client retries until `tr69hostif` is reachable, which is the same behavior `connect_parodus()` already implements for the parodus daemon.

### 3.4 Transition Path

A clean incremental migration is possible without rewriting everything at once:

1. **Phase 1 — Isolate the PAL layer from tr69hostif linkage.** Introduce an abstract `IParamAccessor` interface in `webpa_parameter.cpp`. The current implementation calls `hostIf_GetMsgHandler()` directly. Compile-time injection (or link-time injection) can swap in an RBUS or HTTP implementation without changing the PAL API. Unit tests can use a mock implementation.

2. **Phase 2 — Remove waldb from tr69hostif link dependencies.** Move `libwaldb.la` out of `tr69hostif_LDADD`. Update the waldb `Makefile.am` to produce a shared or static library installable independently.

3. **Phase 3 — Split the startParodus binary into its own deliverable.** Give it its own `configure.ac` or package. The binary reads device identity from `tr69hostif` via RBUS at startup rather than duplicating file reads.

4. **Phase 4 — Move the parodus thread into the standalone process.** The thread entry point `libpd_client_mgr` becomes `main()`. The `PARODUS_ENABLE` guard in `hostIf_main.cpp` is removed entirely.

<<<<<<< HEAD
### 3.5 Benefits of Separation
=======
### 3.5 Target Startup Sequence After Separation

```mermaid
sequenceDiagram
    participant SYS as systemd
    participant TR69 as tr69hostif process
    participant PCC as parodus-client process
    participant PD as parodus daemon
    participant CLOUD as Cloud / XMiDT

    SYS->>TR69: start (existing service)
    TR69->>TR69: init profile handlers, RFC store, RBUS provider
    TR69-->>SYS: READY (sd_notify)

    SYS->>PCC: start parodus-client service (new)
    PCC->>PCC: read device identity via RBUS from tr69hostif
    PCC->>PD: launch parodus daemon with collected args
    PCC->>PCC: libparodus_init retry loop
    PD-->>PCC: ZeroMQ ready

    PCC->>PCC: enter receive loop
    CLOUD->>PD: WebSocket WRP REQ
    PD->>PCC: ZeroMQ deliver
    PCC->>TR69: RBUS get/set TR-181 parameter
    TR69-->>PCC: value or fault
    PCC->>PD: ZeroMQ reply
    PD->>CLOUD: WebSocket WRP response
```

### 3.6 Benefits of Separation
>>>>>>> 22b609abda3573678a880df3716a5f25e968b7f6

| Concern | Current | After Separation |
|---|---|---|
| Memory footprint of tr69hostif | waldb + PAL always linked | Only TR-181 profile code |
| Parodus crash impact | Crash in parodus thread crashes tr69hostif (same process) | Isolated; tr69hostif continues |
| Independent restartability | Impossible; requires full tr69hostif restart | parodus-client can restart independently |
| Unit testability | Requires full tr69hostif profile manager | PAL layer testable with mock IParamAccessor |
| PartnerId reading | Three independent implementations | Single source: tr69hostif TR-181 parameter |
| Thread shutdown | Detached; join not possible | Joinable; clean shutdown |
| Build reduction for non-WebPA platforms | PAL object files always compiled | Separate package; omit from build entirely |

---

## See Also

- [System Overview](overview.md)
- [Threading Model](threading-model.md)
- [Data Flow](data-flow.md)
- [Partner Defaults Workflow](partner-defaults-workflow.md)
- [Public API](../api/public-api.md)
