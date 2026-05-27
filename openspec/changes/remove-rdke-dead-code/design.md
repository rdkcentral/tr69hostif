## Context

The tr69hostif daemon is the TR-069/TR-181 host interface for RDK devices. Over time, several features were added behind compile-time flags for specific platforms that are no longer active on RDKE (RDK Entertainment). An architect review confirmed 9 items as dead code for RDKE, totaling ~2,520 lines across ~27 files.

The codebase uses Autotools (`configure.ac` + `Makefile.am`) with feature flags passed via `--enable-*` options. The Yocto recipe `tr69hostif_git.bb` in `meta-rdk-video` controls which flags are active per platform. For RDKE, none of the following flags are enabled: `USE_XRESRC`, `USE_HWSELFTEST_PROFILE`, `NEW_HTTP_SERVER_DISABLE`, `WEBCONFIG_LITE_FLAG`, `WEB_CONFIG_ENABLE`, `XRELIB_FLAG`, `HWSELFTEST_PROFILE_FLAG`.

Key stakeholders: tr69hostif maintainers, RDKE platform team, Yocto recipe maintainers (meta-rdk-video).

## Goals / Non-Goals

**Goals:**
- Remove all confirmed dead code for RDKE across source, build system, configs, data models, tests, and docs.
- Ensure the build still succeeds after removal (no dangling references).
- Maintain backward compatibility for all TR-181 parameters that ARE active on RDKE.
- Update documentation to reflect the cleanup.

**Non-Goals:**
- Removing code that is live on non-RDKE platforms (e.g., the bbappend that enables `xre` for legacy platforms is out of scope).
- Refactoring or improving code beyond the dead-code boundaries.
- Adding new features or test coverage beyond what's needed to verify the removal.
- Modifying the Yocto recipe directly (out-of-tree; documented as a sync requirement).

## Decisions

### D1: Three-phase implementation approach

**Decision**: Split the removal into 3 phases rather than a single monolithic change.

- **Phase 1 — XRE Removal**: Largest scope (~1,600 lines). Self-contained: own handler files, own data-model objects, own config entries. Can be tested in isolation.
- **Phase 2 — NEW_HTTP_SERVER consolidation**: Moderate complexity. Touches `hostIf_main.cpp` heavily (12 conditional blocks). Requires careful flattening to keep only the "new" code path.
- **Phase 3 — HW Self-Test + Miscellaneous**: Smaller items grouped together: `USE_HWSELFTEST_PROFILE`, `WEBCONFIG` flags, `ip-iface-monitor.service`, `#if 0` blocks, doc updates.

**Rationale**: `Device_DeviceInfo.cpp` is a hotspot touched by 3 phases. Sequential phasing reduces merge conflicts and makes bisection easier if a regression is introduced.

**Alternative considered**: Single-commit removal. Rejected because the 27-file scope makes review and bisection difficult.

### D2: Keep the "new HTTP server" code path, remove the "legacy" path

**Decision**: When flattening `#ifndef NEW_HTTP_SERVER_DISABLE` blocks, keep the code inside the `#else` branch (the "new" path) and remove the `#ifndef` branch (the "legacy" path).

**Rationale**: `NEW_HTTP_SERVER_DISABLE` is never defined on RDKE, meaning the `#ifndef` evaluates to true (legacy path is compiled). However, the actual *active* path is the new HTTP server code since the flag is absent — wait, let me correct: `#ifndef NEW_HTTP_SERVER_DISABLE` means when the flag is NOT defined, the code inside executes. So the legacy path IS what runs. But the consolidation goal is to remove the toggle entirely. Since the flag is never defined, the `#ifndef` branch is the one that compiles. We keep that branch's code and remove the `#else` (dead) branch and the preprocessor guards.

### D3: File deletion over gutting

**Decision**: Delete entire files (`hostIf_XREClient_ReqHandler.cpp/.h`, `ip-iface-monitor.service`, `tr69hostif_no_new_http_server.service`) rather than leaving empty stubs.

**Rationale**: Empty stubs add confusion. Clean deletion with build-system updates is the standard approach.

### D4: typemap.json cleanup scope

**Decision**: Remove XRE-related typemap entries (~59 entries). Flag `hwHealthTest` entries (~11) as needing architect confirmation before removal since they may serve the RFC store layer independently of the handler.

**Rationale**: typemap.json drives parameter type resolution at runtime. Removing entries for parameters that are never registered is safe. But hwHealthTest entries may have a dual purpose.

## Risks / Trade-offs

- **[Device_DeviceInfo.cpp merge conflicts]** → Mitigation: Phase the removal so each phase's changes to this file are reviewed and merged before the next begins.
- **[Yocto recipe out of sync]** → Mitigation: Document exact recipe changes needed. Coordinate with meta-rdk-video maintainer for same-sprint landing.
- **[Non-RDKE platform regression]** → Mitigation: The bbappend that enables `xre`/`rf4ce` for legacy platforms will fail to build after Phase 1. This is acceptable if those platforms are EOL; otherwise, the bbappend owner must be notified.
- **[data-model-generic.xml xre-receiver params]** → Mitigation: Flag as open question; do not remove until architect confirms they are dead.
- **[legacyRFCEnabled() removal side effects]** → Mitigation: Audit all 4 call sites in `hostIf_main.cpp` and the definition in `hostIf_utils.cpp` to ensure no runtime path depends on its return value when the new HTTP server is active.

## Migration Plan

1. Land Phase 1 (XRE removal), run L2 integration tests, verify clean build.
2. Land Phase 2 (NEW_HTTP_SERVER consolidation), run L2 tests.
3. Land Phase 3 (HW Self-Test + misc), run L2 tests.
4. Coordinate with meta-rdk-video to land recipe changes after Phase 2 (which removes the NEW_HTTP_SERVER_DISABLE conditional the recipe sets).
5. Update `dead-code-deprecation-plan.md` to mark all 9 items as completed.

**Rollback**: Each phase is a separate commit/PR. Revert the specific phase if regression is found.

## Open Questions

1. Are `xre-receiver` RFC parameters in `data-model-generic.xml` L182-248 also dead for RDKE? (Blocks complete XRE cleanup.)
2. Should `hwHealthTest` typemap entries be removed given they may serve the RFC store layer independently of the HW Self-Test handler?
3. Should the legacy platform bbappend (that enables `xre`/`rf4ce`) be updated or removed as part of this change?
