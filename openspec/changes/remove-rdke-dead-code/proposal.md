## Why

The tr69hostif codebase carries ~2,520 lines of dead code behind build flags that are never enabled on RDKE platforms. This includes the XRE/Xcalibur client handler (~1,600 lines), HW Self-Test profile (~200 lines), obsolete service files, stale webconfig flags, and `#if 0` remnants. Architect review has confirmed all 9 items are dead for RDKE. Removing them reduces maintenance burden, shrinks the binary, eliminates misleading code paths, and simplifies onboarding for new developers.

## What Changes

- **BREAKING** Remove XRE/Xcalibur client handler (`USE_XRESRC` / `XRELIB_FLAG`): delete `hostIf_XREClient_ReqHandler.cpp/.h`, remove `#ifdef USE_XRESRC` blocks from ~10 source files, remove Xcalibur data-model objects from XML schemas, remove XRE entries from `mgrlist.conf`, `tr69hostIf.conf`, and `typemap.json` (~59 entries).
- **BREAKING** Remove HW Self-Test profile (`USE_HWSELFTEST_PROFILE` / `HWSELFTEST_PROFILE_FLAG`): remove `#ifdef USE_HWSELFTEST_PROFILE` blocks from ~7 files, remove ~11 `hwHealthTest` entries from `typemap.json`.
- Remove `NEW_HTTP_SERVER_DISABLE` conditional compilation and `legacyRFCEnabled()`: flatten ~12 `#ifndef NEW_HTTP_SERVER_DISABLE` blocks in `hostIf_main.cpp` (keeping the "new" code path), remove `legacyRFCEnabled()` from `hostIf_utils.cpp/.h`, delete `tr69hostif_no_new_http_server.service`.
- Remove `WEBCONFIG_LITE_FLAG` / `WEB_CONFIG_ENABLE` dead flags from `configure.ac` and `hostIf_main.cpp`.
- Delete `ip-iface-monitor.service` (unused systemd unit).
- Remove `#if 0` commented-out remnants (~100 lines) in `hostIf_updateHandler.cpp` and `hostIf_XREClient_ReqHandler.cpp`.
- Remove stale XRE config lines from `mgrlist.conf` and `tr69hostIf.conf`.
- Clean Autotools build system (`configure.ac`, `Makefile.am` files) of dead feature macros.
- Update documentation (`dead-code-deprecation-plan.md`, `build-setup.md`, `README.md`) and unit test scaffolding.

## Capabilities

### New Capabilities
- `xre-removal`: Removal of XRE/Xcalibur client handler and all associated data-model entries, build flags, config references, and typemap entries.
- `new-http-server-consolidation`: Removal of `NEW_HTTP_SERVER_DISABLE` conditional compilation, `legacyRFCEnabled()` utility, and the `no_new_http_server` service file. Flattens hostIf_main to a single code path.
- `hwselftest-removal`: Removal of HW Self-Test profile behind `USE_HWSELFTEST_PROFILE` including build flags and typemap entries.
- `misc-dead-code-cleanup`: Removal of stale webconfig flags, `ip-iface-monitor.service`, `#if 0` blocks, and documentation updates.

### Modified Capabilities
<!-- No existing specs to modify -->

## Impact

- **Files affected**: ~27 files across source, build system, configs, data models, tests, and docs.
- **Lines removed**: ~2,520 (net deletion).
- **Files deleted**: 4 (`hostIf_XREClient_ReqHandler.cpp`, `hostIf_XREClient_ReqHandler.h`, `ip-iface-monitor.service`, `tr69hostif_no_new_http_server.service`).
- **Build system**: `configure.ac` loses 5 feature macros; 3 `Makefile.am` files simplified.
- **Data model**: `data-model-generic.xml` loses ~300 lines of Xcalibur objects; `data-model-tv.xml` loses ~10 lines.
- **Config**: `typemap.json` loses ~70 entries.
- **API surface**: No public TR-181 parameter changes (removed parameters were never registered on RDKE).
- **Out-of-tree dependency**: Yocto recipe `tr69hostif_git.bb` in `meta-rdk-video` must be updated in sync (remove `ip-iface-monitor.service` install, `NEW_HTTP_SERVER_DISABLE` conditional, `PACKAGECONFIG[xre]` definition).
- **Risk hotspot**: `Device_DeviceInfo.cpp` is touched by 3 of the 4 removal scopes; careful merge sequencing needed.
- **Open questions for architect**: (a) Are `xre-receiver` RFC params in `data-model-generic.xml` L182-248 also dead? (b) Should `hwHealthTest` typemap entries be removed given they serve the RFC store layer too?
