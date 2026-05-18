# Dead Code Deprecation Plan — tr69hostif (RDKE Stack)

## Overview

This document identifies dead, unused, and deprecated code in the tr69hostif module to support the RDKE stack deprecation initiative. The analysis categorizes findings by severity and recommends a phased removal approach.

---

## Summary of Findings

| # | Category | Component | Severity | Recommendation |
|---|----------|-----------|----------|----------------|
| 1 | **Ghost External Dependency** | XRE/Xcalibur Client (USE_XRESRC) | High | Remove |
| 2 | **Ghost External Dependency** | RF4CE Profile (USE_XRDK_RF4CE_PROFILE) | High | Remove |
| 3 | **Ghost External Dependency** | HW Self-Test (USE_HWSELFTEST_PROFILE) | High | Remove |
| 4 | **Dead Source File** | `hostIf_sysScriptHandler.cpp` | High | Remove |
| 5 | **Dormant Legacy Mechanism** | HAVE_VALUE_CHANGE_EVENT polling | Medium | Deprecate → Remove |
| 6 | **Legacy Protocol** | SNMP Adapter (`snmpAdapter/`) | Medium | Deprecate → Remove |
| 7 | **Dual Code Path** | `NEW_HTTP_SERVER_DISABLE` ifdef guards + `legacyRFCEnabled()` | Medium | Consolidate |
| 8 | **Orphan Service File** | `ip-iface-monitor.service` | Low | Remove |
| 9 | **Unused Build Artifact** | `snmp-data-model.xml` | Low | Remove |
| 10 | **Dead Build Flag** | `WEBCONFIG_LITE_FLAG` / `WEB_CONFIG_ENABLE` | Low | Remove |
| 11 | **Dead Build Flag** | `WEBPA_RFC_ENABLED` (broken function) | Medium | Remove |
| 12 | **Stale Data-Model Entries** | Xcalibur objects in `data-model-generic.xml` | Medium | Remove with XRE |
| 13 | **Dead Config Entries** | `xreMgr` in `mgrlist.conf` / `tr69hostIf.conf` | Medium | Remove with XRE |
| 14 | **Unreachable Code Blocks** | `#if 0` blocks, missing headers, commented-out calls | Low | Remove with parent items |

---

## Detailed Analysis

### 1. XRE/Xcalibur Client (`USE_XRESRC`)

**Files Affected:**
- `src/hostif/handlers/src/hostIf_XREClient_ReqHandler.cpp` (entire file body guarded by `#ifdef USE_XRESRC`)
- `src/hostif/handlers/include/hostIf_XREClient_ReqHandler.h`
- `src/hostif/handlers/src/hostIf_updateHandler.cpp` (lines 58, 98)
- `src/hostif/profiles/DeviceInfo/Device_DeviceInfo.cpp` (line 97, 2068)
- `src/hostif/parodusClient/waldb/waldb.cpp` (lines 43, 170)
- `src/hostif/parodusClient/waldb/data-model/data-model-generic.xml` (lines 1706–1870+)
- `conf/mgrlist.conf` (line 5: `Device.X_COMCAST-COM_Xcalibur xreMgr`)
- `conf/tr69hostIf.conf` (lines 6–7)

**Evidence:**
- The required headers (`Device_XComcast_Xcalibur_Client_XRE.h`, `Device_XComcast_Xcalibur_Client_XRE_ConnectionTable.h`, `Device_XComcast_Xcalibur_TRM.h`) **do not exist** in this repository.
- The external library `libtr69ProfileXcaliber` is referenced in the build but is never shipped with this repo.
- The `X_COMCAST-COM_Xcalibur` include directory path (`src/hostif/profiles/X_COMCAST-COM/X_COMCAST-COM_Xcalibur/include`) does not exist.
- The `xreMgr` handler relies on IPC to a separate XRE daemon process that is being retired from the RDKE stack.

**Impact:** ~1000+ lines of dead conditional code across multiple files. Requires external library that doesn't ship.

---

### 2. RF4CE Profile (`USE_XRDK_RF4CE_PROFILE`)

**Files Affected:**
- `src/hostif/handlers/src/hostIf_dsClient_ReqHandler.cpp` (lines 55–57, 392–405)
- `configure.ac` (lines 240–250)
- `src/hostif/handlers/Makefile.am` (lines 46, 72–73)

**Evidence:**
- The include `Components_XrdkRf4ce.h` **does not exist** in this repository.
- The class `hostIf_STBServiceXrdkRf4ce` is referenced but has no implementation in-tree.
- RF4CE is a legacy remote control protocol replaced by Bluetooth LE in modern RDK devices.

**Impact:** Small (~30 lines of guarded code), but carries build configuration overhead.

---

### 3. HW Self-Test Profile (`USE_HWSELFTEST_PROFILE`)

**Files Affected:**
- `src/hostif/handlers/src/hostIf_DeviceClient_ReqHandler.cpp` (lines 271–284, 698–707)
- `configure.ac` (lines 312–321)
- `src/Makefile.am` (line 153–154: links `-ltr69ProfileHwSelfTest`)
- `src/hostif/handlers/Makefile.am` (lines 84–85)

**Evidence:**
- The external library `libtr69ProfileHwSelfTest` is not in this repository.
- No source implementation exists in-tree; it's an out-of-tree profile library.
- The include `DeviceInfo_hwHealthTest.h` (referenced in `Device_DeviceInfo.cpp` line 100 under `#if USE_HWSELFTEST_PROFILE`) also does not exist in this repository.
- HW self-test is a legacy diagnostics feature not used in modern RDKE deployments.

**Impact:** ~20 lines of guarded code + build system entries. Requires phantom library.

---

### 4. Dead Source File: `hostIf_sysScriptHandler.cpp`

**File:** `src/hostif/handlers/src/hostIf_sysScriptHandler.cpp`

**Evidence:**
- The file contains **only includes and empty comment blocks** — no function definitions.
- It is **NOT listed** in `src/hostif/handlers/Makefile.am` (`libMsgHandlers_la_SOURCES`).
- Confirmed dead by existing docs: "effectively a placeholder in the current tree."

**Impact:** Zero functional code; file is 46 lines of boilerplate.

---

### 5. HAVE_VALUE_CHANGE_EVENT Polling Code (Dormant Legacy Mechanism)

**Files Affected (large code blocks):**
- `hostIf_DeviceClient_ReqHandler.cpp` (lines 895–1321: ~426 lines)
- `hostIf_EthernetClient_ReqHandler.cpp` (lines 442–802: ~360 lines)
- `hostIf_IPClient_ReqHandler.cpp` (lines 533–1165: ~632 lines)
- `hostIf_MoCAClient_ReqHandler.cpp` (lines 788–1753: ~965 lines)
- `hostIf_TimeClient_ReqHandler.cpp` (lines 427–477: ~50 lines)
- `hostIf_XREClient_ReqHandler.cpp` (lines 563–1007: ~444 lines)
- `hostIf_DHCPv4Client_ReqHandler.cpp` (line 302+)
- `hostIf_InterfaceStackClient_ReqHandler.cpp` (line 296+)
- `x_rdk_req_handler.cpp` (commented out)

**Evidence:**
- The define is **commented out** at the top of every file: `//#define HAVE_VALUE_CHANGE_EVENT`
- It **can** be enabled via `--enable-notification` build flag (`-DHAVE_VALUE_CHANGE_EVENT`) in `configure.ac`, and the Makefile.am wires `$(HAVE_VALUE_CHANGE_EVENT_FLAG)` into CXXFLAGS when `WITH_NOTIFICATION_SUPPORT` is true.
- However, the code implements a **legacy TR-069 Active Notification polling mechanism** (`checkForUpdates()` loop) that has been superseded by the modern RBUS event-driven `NotificationHandler` (always compiled, uses `hostIf_NotificationHandler.cpp`).
- Standard RDKE builds do **not** enable `--enable-notification`; the newer `NotificationHandler` dispatches `VALUE_CHANGE_NOTIFICATION` events via RBUS/JSON without polling.
- **Estimated ~2,877+ lines** of dormant conditional code across 8+ handler files.

---

### 6. SNMP Adapter (`SNMP_ADAPTER_ENABLED`)

**Files Affected:**
- `src/hostif/snmpAdapter/snmpAdapter.cpp`
- `src/hostif/snmpAdapter/snmpAdapter.h`
- `src/hostif/snmpAdapter/Makefile.am`
- `src/hostif/handlers/src/hostIf_SNMPClient_ReqHandler.cpp`
- `src/hostif/handlers/include/hostIf_SNMPClient_ReqHandler.h`
- `src/hostif/parodusClient/waldb/snmp-data-model.xml`
- `conf/tr181_snmpOID.conf`
- `conf/mgrlist.conf` (lines referencing `snmpAdapterMgr`)

**Evidence:**
- Default is `--enable-snmpAdapter=no` (disabled by default in `configure.ac`).
- SNMP is a legacy management protocol being phased out of the RDKE stack in favor of TR-181/RBUS.
- The adapter shells out to `snmpget`/`snmpset` commands via `v_secure_system()` — no in-process SNMP library.
- Hardcoded community string `"hDaFHJG7"` in source (security concern).

**Impact:** Entire subsystem (~300 lines impl + handler + data-model). Candidate for full removal.

---

### 7. Dual Code Path: `NEW_HTTP_SERVER_DISABLE` + `legacyRFCEnabled()`

**Clarification of naming:**
- `src/hostif/httpserver/` is the **new** libsoup-based HTTP/JSON REST server (introduced 2018).
- When `NEW_HTTP_SERVER_DISABLE` is defined → the new server is **NOT** compiled → this is the "legacy mode" deployment.
- `tr69hostif_no_new_http_server.service` is the systemd unit for the **legacy mode** (no HTTP server).
- When `legacyRFCEnabled()` returns true at runtime (file `/opt/RFC/.RFC_LegacyRFCEnabled.ini` exists) → the HTTP server thread is **not started** even if compiled in.

**Files Affected:**
- `src/hostif/src/hostIf_main.cpp` (12 `#ifndef NEW_HTTP_SERVER_DISABLE` blocks creating dual code paths)
- `src/Makefile.am` (lines 33–35: conditional `hostif/httpserver` subdirectory)
- `tr69hostif_no_new_http_server.service` (legacy variant systemd unit)
- `src/hostif/include/hostIf_utils.h` (lines 161–162: `setLegacyRFCEnabled()` / `legacyRFCEnabled()`)

**Evidence:**
- The `NEW_HTTP_SERVER_DISABLE` flag creates **two parallel code paths** in `hostIf_main.cpp` that must both be maintained (option parsing, thread management, shutdown logic).
- The `legacyRFCEnabled()` runtime check adds a **third behavioral variant** (compiled but not started).
- Modern RDKE deployments always run with the new HTTP server enabled; the dual path exists only for backward compatibility with older field devices.
- Once legacy mode is fully retired, both the `#ifndef NEW_HTTP_SERVER_DISABLE` guards and the `legacyRFCEnabled()` runtime path can be removed, leaving only the new HTTP server as the single path.

**Impact:** ~200 lines of duplicated/guarded code in `hostIf_main.cpp` + orphan service file. Consolidation will simplify the initialization flow.

---

### 8. Orphan Service File: `ip-iface-monitor.service`

**File:** `ip-iface-monitor.service` (project root)

**Evidence:**
- No source code in this repository implements an "ip-iface-monitor" binary or script.
- Not referenced in any Makefile.am or build script.
- Not referenced in any other configuration file.

**Impact:** Orphaned deployment artifact.

---

### 9. Unused Data-Model XML: `snmp-data-model.xml`

**File:** `src/hostif/parodusClient/waldb/snmp-data-model.xml`

**Evidence:**
- Not referenced by any source file, Makefile, or build script (grep returns zero matches).
- Contains SNMP-specific parameters (`Device.DeviceInfo.X_RDK_SNMP.*`) that overlap with the SNMP adapter marked for removal.

**Impact:** Orphaned XML file.

---

### 10. Dead Build Flags: `WEBCONFIG_LITE_FLAG` / `WEB_CONFIG_ENABLE`

**Evidence:**
- `WEBCONFIG_LITE_FLAG` is declared and substituted in `configure.ac` but there is **no `AC_ARG_ENABLE`** to set it within this repository.
- `WEB_CONFIG_ENABLE` is declared (line 60) and substituted (line 504) but **never assigned a value** in any `AC_ARG_ENABLE` block within this repository.
- These flags cannot be enabled from this repo's `configure.ac` alone, making them permanently empty strings (`" "`) in standalone builds.

**Caveat:** Yocto recipes (`.bbappend` files in `meta-rdk-*` layers outside this repo) may pass `-DWEBCONFIG_LITE_ENABLE` or `-DWEB_CONFIG_ENABLED` directly via `CXXFLAGS`, bypassing `configure.ac`. The corresponding `#ifdef` blocks in `hostIf_main.cpp` (lines 66–77, 492–501) may be active on certain platform builds even though this repo's build system cannot enable them.

**Recommendation:** Verify with platform team whether any Yocto recipe sets these defines before removing the guarded code. The `configure.ac` variables themselves are safe to remove.

---

### 11. Broken `GetFeatureEnabled()` (`WEBPA_RFC_ENABLED`)

**File:** `src/hostif/src/hostIf_main.cpp` (lines 187–195)

```c
#ifdef WEBPA_RFC_ENABLED
bool GetFeatureEnabled(char *cmd)
{
   struct stat buffer;
   string fileName = "/opt/secure/RFC/" + string(".RFC_") + feature + ".ini";
   return (stat(fileName.c_str(), &buffer) == 0);
}
#endif
```

**Evidence:**
- References undefined variable `feature` (should be `cmd`).
- Parameter `cmd` is unused.
- This code would fail to compile if `WEBPA_RFC_ENABLED` were actually enabled — indicating it is never built.
- The `--enable-webpaRFC` flag exists in `configure.ac` but this code path is dead/broken.

---

### 12. Stale Data-Model Entries for Xcalibur/XRE

**File:** `src/hostif/parodusClient/waldb/data-model/data-model-generic.xml` (lines 1706–1870+)

**Evidence:**
- Contains extensive `Device.X_COMCAST-COM_Xcalibur.*` object definitions that map to the non-existent XRE handler.
- These data-model entries will never resolve to a functioning handler after XRE removal.

---

### 13. Stale Configuration Entries

**Files:**
- `conf/mgrlist.conf` — `Device.X_COMCAST-COM_Xcalibur xreMgr`
- `conf/tr69hostIf.conf` — `Device.X_COMCAST-COM_Xcalibur.Client.XRE=xreMgr` and `Device.X_COMCAST-COM_Xcalibur.TRM=xreMgr`

---

### 14. Additional Dead Code Blocks

**`#if 0` blocks in XRE handler:**
- `src/hostif/handlers/src/hostIf_XREClient_ReqHandler.cpp` contains two `#if 0`...`#endif` blocks (~130 lines total) with fully unreachable code for XRE connection table polling. These are dead regardless of any build flag.

**Missing header `DeviceInfo_hwHealthTest.h`:**
- `src/hostif/profiles/DeviceInfo/Device_DeviceInfo.cpp` (line 100) includes `DeviceInfo_hwHealthTest.h` under `#if USE_HWSELFTEST_PROFILE` — this header does not exist in-tree, same as `Components_XrdkRf4ce.h`. Confirms HW Self-Test is an out-of-tree dependency.

**Commented-out `XREClientReqHandler::registerUpdateCallback`:**
- `src/hostif/handlers/src/hostIf_updateHandler.cpp` (line 79) has a commented-out call: `/* XREClientReqHandler::registerUpdateCallback(notifyCallback); */` — while the `#ifdef USE_XRESRC` version at line 98 is the active path. The commented line is a dead remnant.

---

## Recommended Deprecation Phases

### Phase 1: Immediate Removal (No functional impact)

| Item | Action |
|------|--------|
| `hostIf_sysScriptHandler.cpp` | Delete file |
| `ip-iface-monitor.service` | Delete file |
| `snmp-data-model.xml` | Delete file |
| Broken `GetFeatureEnabled()` | Remove `WEBPA_RFC_ENABLED` blocks |
| Dead `WEBCONFIG_LITE_FLAG`/`WEB_CONFIG_ENABLE` build vars | Remove from `configure.ac` |

**Estimated removal:** ~100 lines + 3 files

---

### Phase 2: Remove Ghost Dependencies (Build system cleanup)

| Item | Action |
|------|--------|
| XRE/Xcalibur (`USE_XRESRC`) | Remove all `#ifdef USE_XRESRC` blocks, XRE handler, config entries, data-model objects, build flags, `#if 0` blocks within |
| RF4CE (`USE_XRDK_RF4CE_PROFILE`) | Remove all `#ifdef USE_XRDK_RF4CE_PROFILE` blocks, build flags |
| HW Self-Test (`USE_HWSELFTEST_PROFILE`) | Remove all `#ifdef USE_HWSELFTEST_PROFILE` blocks, build flags |
| XRE config entries | Remove from `mgrlist.conf`, `tr69hostIf.conf` |
| Xcalibur data-model objects | Remove from `data-model-generic.xml`, `data-model-tv.xml` |
| Commented-out remnants | Remove dead `/* XREClientReqHandler::... */` in `hostIf_updateHandler.cpp` |

**Estimated removal:** ~1,100+ lines + build system entries

---

### Phase 3: Deprecate Legacy Subsystems

| Item | Action | Timeline |
|------|--------|----------|
| SNMP Adapter | Mark deprecated, remove in next major release | Q3 2026 |
| HAVE_VALUE_CHANGE_EVENT polling code | Remove once confirmed no Yocto recipe enables `--enable-notification` | Q3 2026 |
| `NEW_HTTP_SERVER_DISABLE` dual code path | Consolidate to new-HTTP-server-only (remove `#ifndef` guards and `legacyRFCEnabled()` logic) | Q4 2026 |
| `tr69hostif_no_new_http_server.service` | Remove after legacy mode retirement | Q4 2026 |

**Estimated removal:** ~3,500+ lines

---

## Total Dead Code Estimate

| Phase | Lines | Files |
|-------|-------|-------|
| Phase 1 | ~100 | 3 files deleted, 2 edited |
| Phase 2 | ~1,230 | 10+ files edited, configs cleaned (includes `#if 0` blocks) |
| Phase 3 | ~3,500 | Entire subsystems removed |
| **Total** | **~4,830+** | |

---

## Risk Assessment

| Phase | Risk | Mitigation |
|-------|------|------------|
| Phase 1 | None | Code is provably unreachable |
| Phase 2 | Low — headers don't exist, code never compiles | Verify no downstream Yocto recipes enable these flags |
| Phase 3 | Medium — SNMP may be used by legacy field devices; `--enable-notification` may be enabled in some recipes; legacy HTTP mode may still serve older deployments | Feature-flag deprecation notices, 1 release cycle grace period, verify with platform team |

---

## Verification Steps

Before removing any item:
1. Confirm the build flag is not enabled in any Yocto recipe/bbappend (`meta-rdk-*`)
2. Run full L2 test suite to ensure no regressions
3. Verify no active field devices depend on the removed parameter paths
4. Update `data-model-*.xml` files to remove corresponding objects

---

## See Also

- [Architecture Overview](overview.md)
- [Threading Model](threading-model.md)
- [Build Setup](../integration/build-setup.md)
- [L2 Test Coverage](../../test/docs/L2_Test_Coverage.md)
