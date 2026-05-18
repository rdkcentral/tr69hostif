# tr69hostif Boot-Up Latency & Dependency Analysis

## Executive Summary

This document analyzes boot-up latency for the `tr69hostif` service on **RDKE** (`#ifndef RDKV_TR69`) and **RDKV** (`#ifdef RDKV_TR69`) build paths, with focus on service dependencies, blocking points, and decoupling opportunities.

**Key findings (RDKE):**
- tr69hostif's own initialization is fast (~266ms from process start to `sd_notify`).
- The dominant boot delay is the **systemd dependency chain**: `iarmbusd → nxserver → mfrmgr → tr69hostif`.
- DS Manager init is already decoupled (background thread) and does NOT block `sd_notify`.
- The `init_rbus_dml_provider()` retry loop (potential 30s) is the only significant **internal** risk — but is typically 0s when `rbusd` is already running.
- A hardcoded `sleep(10)` in the background `pwrThread` delays PowerController readiness unnecessarily.

**Key findings (RDKV):**
- `DSClientReqHandler::init()` blocks the main thread with an **infinite retry loop** — this is the primary bottleneck.
- The non-RDKV path already solves this with a detached thread pattern.

---

## RDKE Systemd Dependency Chain

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  ACTUAL RDKE SYSTEMD SERVICE ORDERING                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Layer 0 (SoC/platform):                                                    │
│     ┌───────────┐     ┌──────────────┐     ┌───────────────┐               │
│     │ iarmbusd   │     │  nxserver     │     │  mfrlibapp    │               │
│     │            │     │  (Nexus SoC)  │     │               │               │
│     └─────┬──────┘     └──────┬────────┘     └───────┬───────┘               │
│           │                   │                      │                       │
│           │  Requires+After   │  Requires+After      │  After                │
│           └───────────┬───────┴──────────────────────┘                       │
│                       ▼                                                      │
│  Layer 1 (HAL managers):                                                     │
│     ┌─────────────────────────────────────────────────────────┐             │
│     │  mfrmgr.service  (Type=notify)                           │             │
│     │  - Requires=iarmbusd.service                              │             │
│     │  - After=iarmbusd.service mfrlibapp.service               │             │
│     │  - Vendor overlay adds: After=nxserver.service            │             │
│     │                          Requires=nxserver.service        │             │
│     └────────────────────────────┬────────────────────────────┘             │
│                                  │                                           │
│                    Requires + After                                           │
│                                  │                                           │
│  Layer 2:                        ▼                                           │
│     ┌─────────────────────────────────────────────────────────┐             │
│     │  tr69hostif.service  (Type=notify)                       │             │
│     │  - After=lighttpd.service securemount.service             │             │
│     │  - After=mfrmgr.service (+ resolves iarmbusd via chain)  │             │
│     │  - Requires=mfrmgr.service                                │             │
│     │  - @DSMGR_DEPENDENCY@ = iarmbusd.service (on RDKE)       │             │
│     └─────────────────────────────────────────────────────────┘             │
│                                                                             │
│  NOT ORDERED (parallel start, accessed at runtime):                          │
│     ┌──────────┐     ┌────────────────────┐                                 │
│     │  rbusd   │     │   WPEFramework     │                                 │
│     │          │     │   (port 9998)      │                                 │
│     └──────────┘     └────────────────────┘                                 │
│       ↑ accessed by       ↑ accessed by                                     │
│       init_rbus_dml       pwrThread (after sleep(10))                       │
│       (0-30s retry)       Thunder/NetworkManager (lazy GET)                 │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Effective serialized boot order:**
```
nxserver ready → mfrmgr ready → tr69hostif starts → sd_notify(READY=1)
   T₁ (SoC)       T₂ (HAL)       T₃ (~266ms)
```

---

## RDKE Boot Sequence: Critical Path to sd_notify(READY=1)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  systemd starts tr69hostif.service                                          │
│  After=lighttpd.service securemount.service mfrmgr.service                  │
│  Requires=mfrmgr.service                                                    │
└─────────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│  1. Option parsing, t2_init(), signal handler setup           [< 1 ms]      │
│  2. WiFiDevice::init()  (if USE_WIFI_PROFILE)                 [< 10 ms]     │
│  3. hostIf_initalize_ConfigManger() - parse mgrlist.conf      [< 10 ms]     │
│  4. legacyRFCEnabled() check                                  [< 1 ms]      │
└─────────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│  5. hostIf_IARM_IF_Start()                                    [BLOCKING]    │
│     ├── TR69_HostIf_Mgr_Init()                                              │
│     │   ├── IARM_Bus_Init("tr69hostIfMgr")                   [< 100 ms]    │
│     │   └── [non-RDKV only] spawn detached pwrThread          [background]  │
│     ├── TR69_HostIf_Mgr_Connect()                                           │
│     │   └── IARM_Bus_Connect()                                [< 100 ms]    │
│     ├── TR69_HostIf_Mgr_Get_RegisterCall()                                  │
│     │   ├── Register IARM RPC handlers                        [< 10 ms]     │
│     │   └── Register pwrMgr event handler (RDKV)             [< 10 ms]     │
│     │                                                                       │
│     ├── ★ DSClientReqHandler::init()  [RDKV PATH ONLY]       [UNBOUNDED]   │
│     │   └── while(true) {                                                   │
│     │         device::Manager::Initialize();  ← BLOCKS HERE                 │
│     │         catch → sleep(3) → retry                                      │
│     │       }                                                               │
│     │                                                                       │
│     └── DeviceClientReqHandler::init()                        [< 1 ms]      │
│         └── memset() only - trivial                                         │
└─────────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│  6. mergeDataModel() - merge XML data models                  [~50-200 ms]  │
│  7. loadDataModel() - parse XML into memory                   [~50-200 ms]  │
└─────────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│  8. Start JSON handler thread                                 [< 10 ms]     │
│  9. Start HTTP server thread (if !legacyRFC)                  [< 100 ms]    │
│ 10. updateHandler::Init() - polling thread                    [< 10 ms]     │
│ 11. Parodus/WebPA init thread (if PARODUS_ENABLE)             [background]  │
└─────────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│  12. ★ init_rbus_dml_provider()                               [UP TO 30s]  │
│      └── while(retry < 6) {                                                 │
│            rbus_checkStatus();                                               │
│            if not enabled → sleep(5) → retry                                │
│          }                                                                  │
│      └── rbus_open("tr69hostif")                                            │
│      └── Register all TR-181 data elements                                  │
└─────────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│  13. Wait for httpServerThreadDone (max 10s timeout)          [0-10s]       │
└─────────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│  ✓ sd_notify(0, "READY=1")                                                 │
│  14. send_DeviceManageableNotification() → detached thread    [post-ready]  │
│  15. g_main_loop_run() → event loop                           [runs forever]│
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Real Device Boot Log Analysis (RDKE)

Captured from an RDKE STB device (May 2026):

```
Timestamp (UTC)          Δ from start  Event
───────────────────────  ────────────  ──────────────────────────────────────────
13:48:49.809Z            +0ms          Starting tr69HostIf Service
13:48:49.826Z            +17ms         IARM_Init (bus name: tr69HostIfMgr)
13:48:49.845Z            +36ms         IARM setting init done
13:48:49.845Z            +36ms         Created getPwrContInterface thread (detached)
13:48:49.845Z            +36ms         Registering tr69HostIfMgr + dispatchThread
13:48:49.847Z            +38ms         Entering data model merge process
13:48:49.848Z            +39ms         RDKE: Merging XML for profile: STB
13:48:49.994Z            +185ms        Merged XML successfully → /tmp/data-model.xml
13:48:50.013Z            +204ms        Successfully initialized Data Model (loaded)
13:48:50.013Z            +204ms        Parodus config set + connection initiated
13:48:50.025Z            +216ms        rbus_checkStatus() = Enabled (NO RETRY)
13:48:50.026Z            +217ms        rbus_open("tr69hostif") success
13:48:50.050Z            +241ms        RBUS DML provider: registered 488 parameters
13:48:50.075Z            +266ms        HTTP Server started successfully
──────────────────────────────────────────────────────────────────────────────────
★ sd_notify(READY=1) fires here                     TOTAL: ~266ms
──────────────────────────────────────────────────────────────────────────────────
13:48:50.075Z            +266ms        libparodus_init (background thread)
13:48:50.857Z            +1048ms       Bootstrap Properties File loaded
13:48:50.899Z            +1090ms       sendDeviceMgtNotification broadcast
13:48:50.900Z            +1091ms       Device Management Notification file updated
```

### Timing Breakdown (RDKE — Observed)

| Phase | Duration | What Happens |
|-------|----------|-------------|
| IARM Bus Init + Connect | 36ms | `IARM_Bus_Init()` + `IARM_Bus_Connect()` + RPC registration |
| XML Data Model Merge | 149ms | `mergeDataModel()` — file I/O merge of generic + STB XML |
| XML Data Model Load | 19ms | `loadDataModel()` — parse merged XML into memory |
| RBUS Init | 37ms | `rbus_checkStatus()` + `rbus_open()` + register 488 elements |
| HTTP Server Start | 25ms | libsoup server listen on port |
| **Total to sd_notify** | **~266ms** | |

### Key Observations from Logs

1. **RBUS was immediately available** — zero retries needed. The 30s worst-case never materialized because `rbusd` was already running when tr69hostif started.
2. **DS Manager did NOT block** — the RDKE path spawns `getPwrContInterface` as a detached thread. Main thread proceeds immediately.
3. **XML merge is the slowest single operation** at 149ms (file I/O on flash).
4. **No mfrmgr call on critical path** — all MFR data is fetched lazily on parameter GET.
5. **The real latency is BEFORE tr69hostif starts** — waiting for `nxserver → mfrmgr` in the systemd dependency chain.

---

## Blocking Points Summary

### RDKE Path (`#ifndef RDKV_TR69`)

| # | Potential Blocking Call | Location | Max Delay | Observed | On Critical Path? |
|---|---|---|---|---|---|
| 1 | `init_rbus_dml_provider()` → `rbus_checkStatus()` retry | `hostIf_rbus_Dml_Provider.cpp:440` | **30s** (sleep 5s × 6) | **0s** (rbus ready) | YES (if rbusd is late) |
| 2 | HTTP server thread wait | `hostIf_main.cpp:510` | **10s** (timeout) | **<25ms** | YES (but typically instant) |
| 3 | `hostIf_getPwrContInterface()` → `sleep(10)` + retries | `hostIf_IARM_ReqHandler.cpp:113` | **Unbounded** | ~10-15s | NO (detached thread) |
| 4 | systemd ordering: nxserver → mfrmgr → tr69hostif | External | **Platform-dependent** | T₁ + T₂ | YES (before process starts) |

### RDKV Path (`#ifdef RDKV_TR69`)

| # | Blocking Call | Location | Max Delay | On Critical Path? |
|---|---|---|---|---|
| 1 | `DSClientReqHandler::init()` → `device::Manager::Initialize()` infinite retry | `hostIf_IARM_ReqHandler.cpp:89` | **Unbounded** (sleep 3s × ∞) | **YES** |
| 2 | `init_rbus_dml_provider()` → `rbus_checkStatus()` retry loop | `hostIf_rbus_Dml_Provider.cpp:440` | **30s** (sleep 5s × 6) | YES |
| 3 | HTTP server thread wait | `hostIf_main.cpp:510` | **10s** (timeout) | YES |

**RDKE worst-case:** systemd chain + rbus retry (30s) + HTTP wait (10s) = **chain + 40s**
**RDKE typical (observed):** systemd chain + **266ms**

**RDKV worst-case:** DS Manager (unbounded) + rbus (30s) + HTTP (10s) = **unbounded**
**RDKV typical:** DS Manager (3-15s) + rbus (0-5s) + HTTP (<1s) = **~5-20s**

---

## MFR Manager Dependency Analysis (RDKE)

### What `mfrmgr.service` Provides

`mfrmgr` is the **Manufacturer Library Manager** — it exposes device identity data via IARM RPC (`IARM_BUS_MFRLIB_NAME = "MFRLib"`):

| Serialized Type | TR-181 Parameter | Purpose |
|-----------------|------------------|---------|
| `mfrSERIALIZED_TYPE_SERIALNUMBER` | `Device.DeviceInfo.SerialNumber` | Device identity, Parodus/WebPA registration |
| `mfrSERIALIZED_TYPE_MANUFACTURER` | `Device.DeviceInfo.Manufacturer` | Device classification |
| `mfrSERIALIZED_TYPE_MANUFACTUREROUI` | `Device.DeviceInfo.ManufacturerOUI` | ACS authentication |
| `mfrSERIALIZED_TYPE_MODELNAME` | `Device.DeviceInfo.ModelName` | Device classification |
| `mfrSERIALIZED_TYPE_PRODUCTCLASS` | `Device.DeviceInfo.ProductClass` | ACS grouping |
| `mfrSERIALIZED_TYPE_HARDWAREVERSION` | `Device.DeviceInfo.HardwareVersion` | Firmware targeting |
| `mfrSERIALIZED_TYPE_SOFTWAREVERSION` | `Device.DeviceInfo.SoftwareVersion` | Update checks |
| `mfrSERIALIZED_TYPE_DEVICEMAC` | `Device.DeviceInfo.X_COMCAST-COM_STB_MAC` | Network identity |

### Access Pattern: All Calls Are Lazy

All 9 `IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, ...)` invocations in the codebase are inside **parameter GET handlers** in `Device_DeviceInfo.cpp`. None are called during the init path before `sd_notify`.

### Why `Requires=mfrmgr.service` Exists

The `Requires=` (hard dependency) means if mfrmgr crashes, systemd will **stop tr69hostif** too. Reasons:
1. Parodus (background thread) needs `SerialNumber` for cloud registration
2. Historical RDKV requirement for CWMP session setup (OUI, SerialNumber, ProductClass)
3. Defense-in-depth: if mfrmgr is dead, device management is fundamentally broken

### mfrmgr Service Definition

```ini
# /lib/systemd/system/mfrmgr.service
[Unit]
Description=IARM Mgr Daemon MFR MGR
Requires=iarmbusd.service
After=iarmbusd.service mfrlibapp.service

[Service]
Type=notify
ExecStart=/bin/sh -c '/usr/bin/mfrMgrMain'
Restart=always

# /lib/systemd/system/mfrmgr.service.d/vendor-mfrmgr.conf
[Unit]
After=mfrmgr.service nxserver.service
Requires=nxserver.service
```

### Decoupling Opportunity

Since all MFR calls are lazy (parameter GET only), the `Requires=` could potentially be downgraded to `Wants=` + `After=`. The IARM call handlers already check `iarm_ret == IARM_RESULT_SUCCESS` before using data.

---

## DS Manager Dependency Deep Dive

### What DS Manager Provides

`device::Manager::Initialize()` initializes the DeviceSettings HAL layer providing:
- Display resolution/HDMI status (`Device.Services.STBService.Components.FrontEnd.*`)
- Audio output configuration
- Video output port information
- Hardware component inventory

### Where DS Manager Is Actually Used (Post-Init)

DS Manager data is consumed **lazily** — only when TR-181 parameters under `Device.Services.STBService.*` are queried via:
- IARM RPC get/set handlers
- HTTP REST API requests
- RBUS data element callbacks

**Critical insight:** No DS Manager data is needed during the boot sequence itself. It is only needed when external systems (ACS, WebPA, RFC) query STB-specific parameters.

### RDKV vs RDKE (Non-RDKV) Initialization Comparison

| Aspect | RDKV Path (`#ifdef RDKV_TR69`) | RDKE Path (`#ifndef RDKV_TR69`) |
|--------|-------------------------------|--------------------------------------|
| DS Manager init | **Synchronous on main thread** | **Detached background thread** |
| Power Manager | IARM pwrMgr event handler | PowerController Thunder plugin |
| Blocks sd_notify? | **YES** | **NO** |
| Code location | `hostIf_IARM_ReqHandler.cpp:89` | `hostIf_IARM_ReqHandler.cpp:108` |
| Thunder plugins | Not used | NetworkManager, PowerController |
| MFR access | Via IARM (lazy) | Via IARM (lazy) |

```cpp
// RDKV path - BLOCKS main thread:
#ifdef RDKV_TR69
pMsgHandler = DSClientReqHandler::getInstance();
pMsgHandler->init();   // ← infinite retry loop in here
#endif

// RDKE path - runs in background:
#ifndef RDKV_TR69
std::thread pwrThread(hostIf_getPwrContInterface);
pwrThread.detach();  // ← DS Manager init happens asynchronously
#endif
```

### RDKE Background Thread Detail (`hostIf_getPwrContInterface`)

```cpp
void hostIf_getPwrContInterface() {
    sleep(10);  // ← HARDCODED: blind wait for WPEFramework
    DSClientReqHandler::getInstance()->init();   // infinite retry, sleep(3)
    PowerController_Init();
    PowerController_Connect();  // infinite retry, 300ms intervals
    PowerController_RegisterPowerModeChangedCallback(...);
}
```

This thread does NOT block `sd_notify`, but delays:
- STBService parameter availability (~13-30s after process start)
- Power state change notification registration

### Historical Context (from PRs)

- **PR #168**: Removed `dsmgr.service` dependency from systemd unit → caused regression (DS calls failed before dsmgr was ready)
- **PR #288**: Changed init logic → insufficient (still blocks on main thread)
- **PR #250**: Added back dsmgr/iarm dependency → confirmed the coupling is structural

---

## Decoupling Recommendations

### RDKE-Specific Recommendations

#### Strategy E1: Add `rbusd` to systemd `After=` ordering (HIGH PRIORITY)

**Problem:** `init_rbus_dml_provider()` has a retry loop (sleep 5s × 6 = 30s max) that fires when rbusd isn't ready. Currently no systemd ordering guarantees rbusd is running.

**Approach:** Add `After=rbus.service` (or platform-specific unit name) to the tr69hostif service dependencies.

**Risk:** Zero — pure ordering fix. Observed logs show rbus IS typically ready, but there's no guarantee without explicit ordering.

**Estimated improvement:** Eliminates the 0-30s variable latency risk entirely.

#### Strategy E2: Remove `sleep(10)` in pwrThread (MEDIUM PRIORITY)

**Problem:** `hostIf_getPwrContInterface()` starts with a blind `sleep(10)` to wait for WPEFramework. The TODO comment references METROL-1045.

**Approach:** Remove the sleep, rely on the existing `PowerController_Connect()` retry loop (300ms intervals) which already handles WPEFramework not being ready.

```cpp
void hostIf_getPwrContInterface() {
    // REMOVE: sleep(10);  // TODO: remove after METROL-1045
    DSClientReqHandler::getInstance()->init();
    PowerController_Init();
    while (true) {
        if (POWER_CONTROLLER_ERROR_NONE == PowerController_Connect()) {
            break;
        }
        usleep(RETRYSLEEP);  // 300ms - already handles WPE not ready
    }
    PowerController_RegisterPowerModeChangedCallback(...);
}
```

**Rationale:** Since nxserver is already in the dependency chain (via mfrmgr vendor overlay), WPEFramework (which depends on nxserver) is likely already active by the time tr69hostif starts + DS Manager completes its own retry.

**Risk:** Low — the retry loop below already handles the case.

**Estimated improvement:** 10s faster to PowerController readiness (background, doesn't affect sd_notify).

#### Strategy E3: Downgrade mfrmgr `Requires=` to `Wants=` (LOW PRIORITY)

**Problem:** `Requires=mfrmgr.service` means if mfrmgr crashes or restarts, systemd kills tr69hostif — even though all MFR calls are lazy and handle failures.

**Approach:** Change to `Wants=mfrmgr.service` + keep `After=mfrmgr.service`.

**Risk:** Low — all IARM MFR calls already check `iarm_ret == IARM_RESULT_SUCCESS`. mfrmgr restart would cause temporary parameter GET failures (returned as error codes, not crashes).

#### Strategy E4: Move `init_rbus_dml_provider()` after sd_notify (EXPLORE)

**Problem:** If rbusd is slow to start, the 30s retry blocks sd_notify.

**Approach:** Send sd_notify after HTTP server is ready, then init RBUS:

```
HTTP server ready → sd_notify(READY=1) → init_rbus_dml_provider()
```

**Risk:** RBUS data elements won't be registered until rbus init completes. External RBUS callers would get "element not found" during the window. Need to confirm RBUS callers (e.g., rdkservice, webconfig) retry on failure.

**Estimated improvement:** Moves entire rbus latency off the critical path.

---

### RDKV-Specific Recommendations

#### Strategy V1: Deferred DS Manager Initialization (CRITICAL)

**Approach:** Move `DSClientReqHandler::init()` to a background thread in the RDKV path, matching the existing RDKE pattern.

```cpp
// Proposed change in hostIf_IARM_IF_Start():
bool hostIf_IARM_IF_Start()
{
    if (TR69_HostIf_Mgr_Init() &&
        TR69_HostIf_Mgr_Connect() &&
        TR69_HostIf_Mgr_Get_RegisterCall())
    {
        ret = true;
        
#ifdef RDKV_TR69
        // Move DS Manager init to background thread (like RDKE path)
        std::thread dsInitThread([]() {
            msgHandler *pMsgHandler = DSClientReqHandler::getInstance();
            pMsgHandler->init();  // Retries in background, doesn't block sd_notify
        });
        dsInitThread.detach();
#endif
        pMsgHandler = DeviceClientReqHandler::getInstance();
        pMsgHandler->init();
    }
}
```

**Risk mitigation:** Add a `ds_ready` atomic flag. DS-dependent parameter handlers check this flag and return a "not ready" error code until DS Manager initializes (same pattern as `IsPwrCtlInt` in RDKE path).

**Estimated boot time reduction:** 3-15+ seconds.

#### Strategy V2: Bounded Retry with Graceful Degradation

**Approach:** Replace the infinite retry loop with bounded retry + background continuation.

**Estimated boot time reduction:** Caps worst-case at 15s instead of unbounded.

---

## Recommended Implementation Order

### RDKE Stack

| Priority | Change | Risk | Impact |
|----------|--------|------|--------|
| **HIGH** | E1: Add rbusd to `After=` ordering | Zero | Eliminates 0-30s rbus retry gamble |
| **MEDIUM** | E2: Remove `sleep(10)` in pwrThread | Low | 10s faster PowerController readiness |
| **LOW** | E3: Downgrade mfrmgr `Requires=` to `Wants=` | Low | Prevents cascade kill on mfrmgr crash |
| **EXPLORE** | E4: Move rbus init after sd_notify | Medium | Removes rbus from critical path entirely |

### RDKV Stack

| Priority | Change | Risk | Impact |
|----------|--------|------|--------|
| **CRITICAL** | V1: Background DS Manager init + readiness flag | Medium | 3-15s (eliminates primary blocker) |
| **HIGH** | Parallel RBUS init | Low | 0-30s |
| **MEDIUM** | Remove systemd dsmgr ordering (after V1 validated) | Low | Eliminates systemd chain delay |

---

## RDKE Interaction & Dependency Map

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  RDKE BOOT: SYSTEMD → tr69hostif → sd_notify                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌──────────────────── systemd dependency resolution ─────────────────────┐ │
│  │                                                                         │ │
│  │    ┌───────────┐                                                        │ │
│  │    │ iarmbusd   │                                                        │ │
│  │    └─────┬──────┘                                                        │ │
│  │          │ Requires+After                                                │ │
│  │          ▼                                                               │ │
│  │    ┌───────────┐    ┌───────────────┐                                   │ │
│  │    │ mfrlibapp  │    │  nxserver      │                                   │ │
│  │    └─────┬──────┘    └───────┬────────┘                                   │ │
│  │          │ After              │ Requires+After (vendor)                   │ │
│  │          └──────────┬─────────┘                                           │ │
│  │                     ▼                                                     │ │
│  │    ┌────────────────────────────────────┐                                │ │
│  │    │  mfrmgr.service (Type=notify)       │                                │ │
│  │    │  ExecStart=/usr/bin/mfrMgrMain       │                                │ │
│  │    └──────────────────┬─────────────────┘                                │ │
│  │                       │ Requires+After                                    │ │
│  │                       ▼                                                   │ │
│  │    ┌──────────┐  ┌──────────┐                                           │ │
│  │    │lighttpd  │  │securemount│                                           │ │
│  │    └────┬─────┘  └─────┬────┘                                           │ │
│  │         │ After         │ After                                           │ │
│  │         └───────┬───────┘                                                │ │
│  │                 ▼                                                         │ │
│  │    ┌────────────────────────────────────────────────────────────────┐    │ │
│  │    │  tr69hostif.service (Type=notify)                               │    │ │
│  │    │  After=lighttpd securemount mfrmgr iarmbusd                     │    │ │
│  │    │  Requires=mfrmgr                                                 │    │ │
│  │    └────────────────────────────────────────────────────────────────┘    │ │
│  └─────────────────────────────────────────────────────────────────────────┘ │
│                                                                             │
│  ┌───────────────── tr69hostif internal init (Main Thread) ───────────────┐ │
│  │                                                                         │ │
│  │  IARM_Bus_Init ──┐                                                      │ │
│  │                  └──spawn──────► pwrThread (DETACHED)                    │ │
│  │  IARM_Bus_Connect                 │                                      │ │
│  │  Register IARM RPCs               ▼                                      │ │
│  │  DeviceClient::init (memset)     sleep(10) → DS Manager init            │ │
│  │  mergeDataModel (XML I/O)        → PowerController_Init/Connect          │ │
│  │  loadDataModel (XML parse)       → RegisterPowerModeCallback             │ │
│  │  Start JSON thread                                                       │ │
│  │  Start HTTP server thread                                                │ │
│  │  updateHandler::Init                                                     │ │
│  │  Parodus thread (background)                                             │ │
│  │  init_rbus_dml_provider()  ←── ONLY RISK (if rbusd not ready: 0-30s)    │ │
│  │  Wait HTTP server done (typically instant)                               │ │
│  │                                                                         │ │
│  │  ★ sd_notify(READY=1) ★  [~266ms from process start]                    │ │
│  │                                                                         │ │
│  └─────────────────────────────────────────────────────────────────────────┘ │
│                                                                             │
│  ┌───────────────── Post-READY (non-blocking) ────────────────────────────┐ │
│  │  send_DeviceManageableNotification() → detached thread                  │ │
│  │  updateHandler::run() → 60s polling loop                                │ │
│  │  Parodus cloud connection (needs SerialNumber from mfrmgr via IARM)     │ │
│  │  g_main_loop_run() → event loop                                         │ │
│  └─────────────────────────────────────────────────────────────────────────┘ │
│                                                                             │
│  ┌───────────────── Runtime (lazy, on-demand) ────────────────────────────┐ │
│  │  MFR calls (SerialNumber, Manufacturer, etc.) → IARM → mfrmgr          │ │
│  │  DS Manager calls (HDMI, Audio, Video) → device::Manager HAL            │ │
│  │  Thunder calls (NetworkManager, PowerController) → curl → port 9998     │ │
│  │  RBUS callbacks → registered data elements                              │ │
│  └─────────────────────────────────────────────────────────────────────────┘ │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Summary of All sleep() Calls

| File | Line | Sleep | Purpose | Critical Path (RDKE)? | Critical Path (RDKV)? |
|------|------|-------|---------|----------------------|----------------------|
| `hostIf_dsClient_ReqHandler.cpp` | 94 | `sleep(3)` in loop | Wait for DS Manager | No (detached thread) | **YES** |
| `hostIf_rbus_Dml_Provider.cpp` | 454 | `sleep(5)` × 6 | Wait for rbus daemon | **YES** (if rbusd late) | **YES** |
| `hostIf_IARM_ReqHandler.cpp` | 113 | `sleep(10)` | Wait for WPEFramework | No (detached thread) | N/A |
| `hostIf_main.cpp` | 510 | `wait_for(10s)` | Wait for HTTP server | **YES** (usually <25ms) | **YES** (usually fast) |
| `hostIf_updateHandler.cpp` | 181 | `sleep(60)` | Polling interval | No (post-ready loop) | No (post-ready loop) |
| `Device_DeviceInfo.cpp` | 5221 | `sleep(10)` | Manageable notification | No (detached thread) | No (detached thread) |

---

## Thunder Plugin Dependencies (RDKE Runtime)

RDKE uses Thunder plugins via HTTP JSON-RPC to `http://127.0.0.1:9998/jsonrpc`:

| Plugin | Used For | When Called | Timeout |
|--------|----------|-------------|--------|
| `org.rdk.NetworkManager` | IP address, WiFi state, interfaces | On parameter GET (lazy) | 5s connect, 10s request |
| `PowerController` (C API) | Power state management | Background thread at boot | Infinite retry (300ms) |

Thunder calls use curl with connect/request timeouts and **fail gracefully** — they are not on the critical path to sd_notify.

---

## Conclusion

### RDKE

The tr69hostif internal initialization on RDKE is **already fast (~266ms observed)**. The boot-up latency problem is dominated by:

1. **Systemd dependency chain** (`nxserver → mfrmgr → tr69hostif`) — platform/HAL startup time outside tr69hostif's control
2. **`init_rbus_dml_provider()` retry risk** — potential 0-30s if rbusd isn't ordered in systemd (currently unordered, but observed to be ready in practice)
3. **Background thread `sleep(10)`** — doesn't block sd_notify but delays PowerController/DS Manager availability by 10s unnecessarily

**Recommended fix for RDKE:** Add `After=rbus.service` to systemd unit (zero-risk, eliminates the only significant variable delay on the critical path).

### RDKV

The RDKV path has a fundamental design flaw:

1. **Primary bottleneck:** `device::Manager::Initialize()` infinite retry on the main thread — blocks everything until dsmgr is ready
2. **Secondary bottleneck:** `rbus_checkStatus()` retry loop — adds up to 30s
3. **Pattern exists but unused:** The RDKE path already solves this with a detached thread, but RDKV was never updated

**Recommended fix for RDKV:** Adopt the RDKE pattern — move DS Manager initialization to a background thread with a readiness flag, allowing `sd_notify(READY=1)` to fire as soon as HTTP server and data model are ready (~1 second).
