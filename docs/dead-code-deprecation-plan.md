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
| 5 | **Commented-Out Feature** | HAVE_VALUE_CHANGE_EVENT polling | Medium | Remove |
| 6 | **Legacy Protocol** | SNMP Adapter (`snmpAdapter/`) | Medium | Deprecate → Remove |
| 7 | **Legacy HTTP Server** | `httpserver/` (NEW_HTTP_SERVER_DISABLE path) | Medium | Consolidate |
| 8 | **Orphan Service File** | `ip-iface-monitor.service` | Low | Remove |
| 9 | **Unused Build Artifact** | `snmp-data-model.xml` | Low | Remove |
| 10 | **Dead Build Flag** | `WEBCONFIG_LITE_FLAG` / `WEB_CONFIG_ENABLE` | Low | Remove |
| 11 | **Dead Build Flag** | `WEBPA_RFC_ENABLED` (broken function) | Medium | Remove |
| 12 | **Stale Data-Model Entries** | Xcalibur objects in `data-model-generic.xml` | Medium | Remove with XRE |
| 13 | **Dead Config Entries** | `xreMgr` in `mgrlist.conf` / `tr69hostIf.conf` | Medium | Remove with XRE |

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

### 5. HAVE_VALUE_CHANGE_EVENT Polling Code

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
- While it can be enabled via `--enable-notification` build flag (`-DHAVE_VALUE_CHANGE_EVENT`), the code inside these blocks implements a legacy TR-069 Active Notification polling mechanism that has been superseded by RBUS event-driven notifications.
- **Estimated ~2,877+ lines** of dead conditional code across 8+ handler files.

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

### 7. Legacy HTTP Server (`NEW_HTTP_SERVER_DISABLE` path)

**Files Affected:**
- `src/hostif/httpserver/` (entire directory)
- `src/hostif/src/hostIf_main.cpp` (12 `#ifndef NEW_HTTP_SERVER_DISABLE` blocks)
- `tr69hostif_no_new_http_server.service` (systemd service variant)
- `src/Makefile.am` (lines 33–35: conditional subdirectory)

**Evidence:**
- The existence of `tr69hostif_no_new_http_server.service` indicates a deployment mode where the HTTP server is disabled.
- The `NEW_HTTP_SERVER_DISABLE` flag creates two code paths that must be maintained.
- Modern RDKE deployments use the JSON/RBUS interface exclusively; the soup-based HTTP server serves legacy TR-069 ACS communication.
- The entire HTTP server code path is guarded behind `legacyRFCEnabled()` even when compiled in.

**Impact:** Significant maintenance burden. The httpserver/ directory contains its own Makefile.am, src/, include/, and gtest/. Consolidation recommended.

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
- `WEBCONFIG_LITE_FLAG` is declared and substituted in `configure.ac` but there is **no `AC_ARG_ENABLE`** to set it.
- `WEB_CONFIG_ENABLE` is declared (line 60) and substituted (line 504) but **never assigned a value** in any `AC_ARG_ENABLE` block.
- These flags appear in documentation but the actual configuration entry is missing, making them permanently empty.

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
| XRE/Xcalibur (`USE_XRESRC`) | Remove all `#ifdef USE_XRESRC` blocks, XRE handler, config entries, data-model objects, build flags |
| RF4CE (`USE_XRDK_RF4CE_PROFILE`) | Remove all `#ifdef USE_XRDK_RF4CE_PROFILE` blocks, build flags |
| HW Self-Test (`USE_HWSELFTEST_PROFILE`) | Remove all `#ifdef USE_HWSELFTEST_PROFILE` blocks, build flags |
| XRE config entries | Remove from `mgrlist.conf`, `tr69hostIf.conf` |
| Xcalibur data-model objects | Remove from `data-model-generic.xml`, `data-model-tv.xml` |

**Estimated removal:** ~1,100+ lines + build system entries

---

### Phase 3: Deprecate Legacy Subsystems

| Item | Action | Timeline |
|------|--------|----------|
| SNMP Adapter | Mark deprecated, remove in next major release | Q3 2026 |
| HAVE_VALUE_CHANGE_EVENT polling code | Remove (superseded by RBUS notifications) | Q3 2026 |
| Legacy HTTP Server (httpserver/) | Consolidate to JSON/RBUS-only path | Q4 2026 |
| `tr69hostif_no_new_http_server.service` | Remove after httpserver consolidation | Q4 2026 |

**Estimated removal:** ~3,500+ lines

---

## Total Dead Code Estimate

| Phase | Lines | Files |
|-------|-------|-------|
| Phase 1 | ~100 | 3 files deleted, 2 edited |
| Phase 2 | ~1,100 | 10+ files edited, configs cleaned |
| Phase 3 | ~3,500 | Entire subsystems removed |
| **Total** | **~4,700+** | |

---

## Risk Assessment

| Phase | Risk | Mitigation |
|-------|------|------------|
| Phase 1 | None | Code is provably unreachable |
| Phase 2 | Low — headers don't exist, code never compiles | Verify no downstream recipes enable these flags |
| Phase 3 | Medium — SNMP/HTTP may be used by legacy field devices | Feature-flag deprecation notices, 1 release cycle grace period |

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
