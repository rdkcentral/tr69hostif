# Dead Code Deprecation Plan — tr69hostif (RDKE Stack)

## Overview

This document identifies dead, unused, and deprecated code in the tr69hostif module to support the RDKE stack deprecation initiative. The analysis categorizes findings by severity and recommends a phased removal approach.

> **Last updated:** 2026-05-27 — Revised after Yocto recipe cross-referencing and architect review.
> Two items originally flagged as dead (#2 RF4CE, #5 HAVE_VALUE_CHANGE_EVENT) were found to be
> **live on RDKE** builds. Nine items are confirmed dead and tracked in
> [`openspec/changes/remove-rdke-dead-code/`](../../openspec/changes/remove-rdke-dead-code/proposal.md).

---

## Summary of Findings

| # | Category | Component | Severity | Status | Recommendation |
|---|----------|-----------|----------|--------|----------------|
| 1 | **Ghost External Dependency** | XRE/Xcalibur Client (USE_XRESRC) | High | ✅ Confirmed Dead | Remove |
| 2 | **~~Ghost External Dependency~~** | RF4CE Profile (USE_XRDK_RF4CE_PROFILE) | ~~High~~ | ❌ **LIVE on RDKE** | ~~Remove~~ Keep |
| 3 | **Ghost External Dependency** | HW Self-Test (USE_HWSELFTEST_PROFILE) | High | ✅ Confirmed Dead | Remove |
| 4 | **Dead Source File** | `hostIf_sysScriptHandler.cpp` | High | ✅ Confirmed Dead | Remove |
| 5 | **~~Dormant Legacy Mechanism~~** | HAVE_VALUE_CHANGE_EVENT polling | ~~Medium~~ | ❌ **LIVE on RDKE** | ~~Deprecate → Remove~~ Keep |
| 6 | **Legacy Protocol** | SNMP Adapter (`snmpAdapter/`) | Medium | ✅ Confirmed Dead | Remove |
| 7 | **Dual Code Path** | `NEW_HTTP_SERVER_DISABLE` ifdef guards + `legacyRFCEnabled()` | Medium | ✅ Confirmed Dead | Remove (immediate) |
| 8 | **Orphan Service File** | `ip-iface-monitor.service` | Low | ✅ Confirmed Dead | Remove |
| 9 | **Unused Build Artifact** | `snmp-data-model.xml` | Low | ✅ Confirmed Dead | Remove |
| 10 | **Dead Build Flag** | `WEBCONFIG_LITE_FLAG` / `WEB_CONFIG_ENABLE` | Low | ✅ Confirmed Dead | Remove |
| 11 | **Dead Build Flag** | `WEBPA_RFC_ENABLED` (broken function) | Medium | ✅ Confirmed Dead | Remove |
| 12 | **Stale Data-Model Entries** | Xcalibur objects in `data-model-generic.xml` | Medium | ✅ Confirmed Dead | Remove with XRE |
| 13 | **Dead Config Entries** | `xreMgr` in `mgrlist.conf` / `tr69hostIf.conf` | Medium | ✅ Confirmed Dead | Remove with XRE |
| 14 | **Unreachable Code Blocks** | `#if 0` blocks, missing headers, commented-out calls | Low | ✅ Confirmed Dead | Remove with parent items |

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

### 2. RF4CE Profile (`USE_XRDK_RF4CE_PROFILE`) — ❌ CONFIRMED LIVE

> **Status update (2026-05-27):** Yocto recipe cross-referencing revealed that
> `meta-rdk-video` bbappend files enable `PACKAGECONFIG:append = " xre rf4ce"`
> for RDKE builds. The base recipe defines `PACKAGECONFIG[rf4ce]` which passes
> `--enable-rf4ce` to configure. **This is NOT dead code.** The original analysis
> examined only the in-tree source without checking Yocto recipes.

**Files Affected:**
- `src/hostif/handlers/src/hostIf_dsClient_ReqHandler.cpp` (lines 55–57, 392–405)
- `configure.ac` (lines 240–250)
- `src/hostif/handlers/Makefile.am` (lines 46, 72–73)

**Original (incorrect) Evidence:**
- The include `Components_XrdkRf4ce.h` does not exist in this repository — but it is provided by an out-of-tree RF4CE library that IS linked in RDKE builds.
- RF4CE is still actively used for remote control pairing on current RDKE devices.

**Impact:** None — this code is live. **Do not remove.**

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

### 5. HAVE_VALUE_CHANGE_EVENT Polling Code — ❌ CONFIRMED LIVE

> **Status update (2026-05-27):** Yocto recipe cross-referencing revealed that
> the **base recipe** `tr69hostif_git.bb` includes `--enable-notification` in
> `EXTRA_OECONF`. This means `HAVE_VALUE_CHANGE_EVENT` is **always defined** on
> RDKE builds. The original analysis was incorrect — the commented-out
> `//#define HAVE_VALUE_CHANGE_EVENT` in source files is irrelevant because the
> flag is injected via compiler flags from the build system.
> **These ~2,877 lines are live, compiled code. Do not remove.**

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

**Original (incorrect) Evidence:**
- The define is commented out at the top of every file: `//#define HAVE_VALUE_CHANGE_EVENT`
- The original analysis concluded "Standard RDKE builds do not enable `--enable-notification`" — **this was wrong**. The base Yocto recipe enables it.

**Corrected Understanding:**
- `--enable-notification` IS enabled in the base recipe, which passes `-DHAVE_VALUE_CHANGE_EVENT` via CXXFLAGS.
- The `checkForUpdates()` polling code IS compiled and may be active on production devices.
- The newer RBUS `NotificationHandler` coexists with this mechanism; they are not mutually exclusive.

**Impact:** None — this code is live. **Do not remove.**

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

> **Status update (2026-05-27):** Architect review confirmed `NEW_HTTP_SERVER_DISABLE`
> is **never defined on RDKE**. The flag and `legacyRFCEnabled()` are confirmed dead.
> Removal reclassified from Phase 3 (Q4 2026) to **immediate removal**.

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

> were dead and placed NEW_HTTP_SERVER_DISABLE in Phase 3. After Yocto recipe
> cross-referencing and architect review, the phasing has been corrected.
> Items #2 (RF4CE) and #5 (HAVE_VALUE_CHANGE_EVENT) are **removed from all phases**
> as they are confirmed live on RDKE.
> RF4CE (~30 lines) and HAVE_VALUE_CHANGE_EVENT (~2,877 lines) which are live,
> plus SNMP/WEBPA items (~400 lines) still pending review.

## Deprecation Phasing (2026-05-29)

> **Revision note:** 2026-05-29: Updated phasing and assignments per architect review and codebase analysis.

### Deprecation Phasing Table

| Phase   | Items                                                                 |
|---------|-----------------------------------------------------------------------|
| Phase 1 | SNMP Adapter, snmp-data-model.xml, HW Self-Test, hwHealthTest typemap entries |
| Phase 2 | XRE/Xcalibur code, xre-receiver RFC params, Xcalibur data-model, XRE config entries |
| Phase 3 | WEBConfig flags, WEBPA_RFC_ENABLED, WebPA/Parodus legacy code         |
| Phase 4 | NEW_HTTP_SERVER_DISABLE, hostIf_sysScriptHandler.cpp, ip-iface-monitor.service, #if 0 blocks, legacy bbappend |

---

### Phase Details

#### Phase 1: SNMP and HW Self-Test
- Remove SNMP Adapter and snmp-data-model.xml
- Remove HW Self-Test code and hwHealthTest typemap entries
- Validate: Ensure no build or runtime dependencies remain; run L2/functional tests

#### Phase 2: XRE/Xcalibur and Dependencies
- Remove all XRE/Xcalibur code, xre-receiver RFC params, Xcalibur data-model, and XRE config entries
- Validate: Confirm no references in code, config, or recipes; run regression tests

#### Phase 3: WEBConfig and WebPA
- Remove WEBConfig flags, WEBPA_RFC_ENABLED, and WebPA/Parodus legacy code
- Validate: Ensure notification/event-driven code is unaffected; run notification tests

#### Phase 4: Miscellaneous and Final Sweep
- Remove NEW_HTTP_SERVER_DISABLE, hostIf_sysScriptHandler.cpp, ip-iface-monitor.service, #if 0 blocks, and legacy bbappend
- Validate: Final build and runtime validation; sweep for any remaining dead code

---

## Risk Assessment

| Phase | Risk | Mitigation |
|-------|------|------------|
| Phase 1 | Low — SNMP and HW Self-Test are unused and not referenced in current builds | Confirm with L2/functional tests; check for out-of-tree references |
| Phase 2 | Low — XRE/Xcalibur and dependencies are not present in current builds | Confirm with code/config/recipe search; coordinate with platform teams |
| Phase 3 | Low — WEBConfig and WebPA code is not active in current builds | Validate with notification/event-driven tests |
| Phase 4 | Low — Miscellaneous items are not referenced | Final validation and code sweep |

---

## Verification Steps

Before removing any item:
1. Confirm the build flag is not enabled in any Yocto recipe/bbappend (`meta-rdk-*`)
2. Run full L2 test suite to ensure no regressions
3. Verify no active field devices depend on the removed parameter paths
4. Update `data-model-*.xml` files to remove corresponding objects
5. Coordinate Yocto recipe changes in `meta-rdk-video` (out-of-tree dependency)

---

## Architect Review Log

| Date | Decision | Items Affected |
|------|----------|----------------|
| 2026-05-27 | Confirmed 9 items dead for RDKE | #1, #3, #7, #8, #10, #12, #13, #14 |
| 2026-05-27 | Corrected #2 RF4CE as LIVE | #2 — bbappend enables rf4ce |
| 2026-05-27 | Corrected #5 HAVE_VALUE_CHANGE_EVENT as LIVE | #5 — base recipe enables --enable-notification |
| 2026-05-27 | Reclassified #7 from Q4 2026 to immediate | #7 — NEW_HTTP_SERVER_DISABLE never defined |
| 2026-05-27 | Confirmed #4, #6, #9 dead for RDKE | #4 — dead source file; #6 — SNMP adapter unused; #9 — orphaned XML |
| 2026-05-27 | Confirmed #11 dead for RDKE | #11 — WEBPA_RFC_ENABLED has compile bug; never enabled in any recipe |


---

## See Also

- [Architecture Overview](overview.md)
- [Threading Model](threading-model.md)
- [Build Setup](../integration/build-setup.md)
- [L2 Test Coverage](../../test/docs/L2_Test_Coverage.md)
- [OpenSpec Change: remove-rdke-dead-code](../../openspec/changes/remove-rdke-dead-code/proposal.md)
