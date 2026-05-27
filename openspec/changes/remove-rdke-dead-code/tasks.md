## 1. Phase 1 — XRE/Xcalibur Removal

- [ ] 1.1 Delete `src/hostif/handlers/src/hostIf_XREClient_ReqHandler.cpp` and `src/hostif/handlers/include/hostIf_XREClient_ReqHandler.h`
- [ ] 1.2 Remove `USE_XRESRC` / `XRELIB_FLAG` definitions from `configure.ac`
- [ ] 1.3 Remove XRE conditional blocks from `src/hostif/handlers/Makefile.am`
- [ ] 1.4 Remove XRE conditional blocks from `src/hostif/profiles/DeviceInfo/Makefile.am`
- [ ] 1.5 Remove `#ifdef USE_XRESRC` blocks from `src/hostif/handlers/src/hostIf_msgHandler.cpp`
- [ ] 1.6 Remove `#ifdef USE_XRESRC` blocks from `src/hostif/handlers/src/hostIf_updateHandler.cpp`
- [ ] 1.7 Remove `#ifdef USE_XRESRC` blocks from `src/hostif/profiles/DeviceInfo/Device_DeviceInfo.cpp` and `Device_DeviceInfo.h`
- [ ] 1.8 Remove XRE include/reference from `src/hostif/include/hostIf_main.h`
- [ ] 1.9 Remove XRE entries from `conf/mgrlist.conf` (line 5) and `src/integrationtest/conf/mgrlist.conf`
- [ ] 1.10 Remove XRE config lines from `conf/tr69hostIf.conf` (lines 6-7)
- [ ] 1.11 Remove Xcalibur data-model objects from `data-model-generic.xml` (~lines 1706-2005)
- [ ] 1.12 Remove Xcalibur entries from `data-model-tv.xml` (~lines 269-279)
- [ ] 1.13 Remove ~59 XRE typemap entries from `typemap.json`
- [ ] 1.14 Remove XRE references from `src/hostif/parodusClient/waldb/waldb.cpp`
- [ ] 1.15 Remove XRE references from unit test files (`run_ut.sh`, `paramMgr.h`, `gtest_src.cpp`)
- [ ] 1.16 Verify clean build after Phase 1

## 2. Phase 2 — NEW_HTTP_SERVER Consolidation

- [ ] 2.1 Flatten ~12 `#ifndef NEW_HTTP_SERVER_DISABLE` blocks in `src/hostif/src/hostIf_main.cpp` (keep the `#ifndef` branch, remove `#else` branch and guards)
- [ ] 2.2 Remove `NEW_HTTP_SERVER_DISABLE` definition from `configure.ac`
- [ ] 2.3 Remove `legacyRFCEnabled()` declaration from `src/hostif/include/hostIf_utils.h`
- [ ] 2.4 Remove `legacyRFCEnabled()` implementation from `src/hostif/src/hostIf_utils.cpp`
- [ ] 2.5 Remove `legacyRFCEnabled()` call sites from `src/hostif/profiles/DeviceInfo/Device_DeviceInfo.cpp`
- [ ] 2.6 Delete `tr69hostif_no_new_http_server.service` from repository root
- [ ] 2.7 Remove `no_new_http_server` service references from `src/Makefile.am` (install targets)
- [ ] 2.8 Verify clean build after Phase 2

## 3. Phase 3 — HW Self-Test + Miscellaneous Cleanup

- [ ] 3.1 Remove `USE_HWSELFTEST_PROFILE` / `HWSELFTEST_PROFILE_FLAG` definitions from `configure.ac`
- [ ] 3.2 Remove HW Self-Test conditional blocks from `src/hostif/profiles/DeviceInfo/Makefile.am`
- [ ] 3.3 Remove `#ifdef USE_HWSELFTEST_PROFILE` blocks from `src/hostif/profiles/DeviceInfo/Device_DeviceInfo.cpp` and `Device_DeviceInfo.h`
- [ ] 3.4 Remove HW Self-Test references from remaining source files (~4 additional files)
- [ ] 3.5 Remove `WEBCONFIG_LITE_FLAG` / `WEB_CONFIG_ENABLE` from `configure.ac`
- [ ] 3.6 Remove `WEBCONFIG_LITE_FLAG` / `WEB_CONFIG_ENABLE` usage from `src/hostif/src/hostIf_main.cpp`
- [ ] 3.7 Delete `ip-iface-monitor.service` from repository root
- [ ] 3.8 Remove `#if 0` dead remnants from `src/hostif/handlers/src/hostIf_updateHandler.cpp`
- [ ] 3.9 Verify clean build after Phase 3

## 4. Documentation and Test Updates

- [ ] 4.1 Update `docs/architecture/dead-code-deprecation-plan.md` to mark all 9 items as completed
- [ ] 4.2 Update `docs/integration/build-setup.md` to remove references to removed build flags
- [ ] 4.3 Update `README.md` if it references removed features
- [ ] 4.4 Document required Yocto recipe changes for `meta-rdk-video` (out-of-tree sync note)

## 5. Validation

- [ ] 5.1 Run L2 integration tests to verify no regressions
- [ ] 5.2 Grep full codebase for any residual references to removed flags/symbols
- [ ] 5.3 Confirm no dangling `#include` or forward declarations remain
