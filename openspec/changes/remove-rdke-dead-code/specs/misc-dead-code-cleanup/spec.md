## REMOVED Requirements

### Requirement: WEBCONFIG dead flags
The system SHALL NOT include `WEBCONFIG_LITE_FLAG` or `WEB_CONFIG_ENABLE` build flags. Their definitions in `configure.ac` and any usage in source files SHALL be removed.

**Reason**: These flags are never enabled on RDKE. They add dead conditional paths in `configure.ac` and `hostIf_main.cpp`. Confirmed by architect review.
**Migration**: No migration needed.

#### Scenario: WEBCONFIG flag references eliminated
- **WHEN** the codebase is searched for `WEBCONFIG_LITE_FLAG` or `WEB_CONFIG_ENABLE`
- **THEN** zero matches SHALL be found in any source, header, or build file

### Requirement: ip-iface-monitor.service removal
The system SHALL NOT include the `ip-iface-monitor.service` systemd unit file.

**Reason**: This service file is unused on RDKE. No recipe installs it. Confirmed by architect review.
**Migration**: No migration needed.

#### Scenario: ip-iface-monitor.service deleted
- **WHEN** the source tree is inspected
- **THEN** `ip-iface-monitor.service` SHALL NOT exist at the repository root

### Requirement: Commented-out code remnants removal
The system SHALL NOT contain `#if 0` blocks that were identified as dead remnants. Specifically, the `#if 0` blocks in `hostIf_updateHandler.cpp` and `hostIf_XREClient_ReqHandler.cpp` (~100 lines total) SHALL be removed.

**Reason**: `#if 0` blocks are dead code that adds confusion and maintenance burden. Confirmed by architect review.
**Migration**: No migration needed. Note: `hostIf_XREClient_ReqHandler.cpp` is deleted entirely as part of the xre-removal capability, which also satisfies this requirement for that file.

#### Scenario: No #if 0 dead remnants in updated files
- **WHEN** `src/hostif/handlers/src/hostIf_updateHandler.cpp` is inspected
- **THEN** no `#if 0` blocks containing dead code remnants SHALL be present

### Requirement: Documentation reflects cleanup
The system SHALL update `docs/architecture/dead-code-deprecation-plan.md` to mark all 9 items as completed, and update `docs/integration/build-setup.md` and `README.md` to remove references to removed features.

**Reason**: Documentation must stay in sync with code changes.
**Migration**: No migration needed.

#### Scenario: Dead-code plan updated
- **WHEN** `docs/architecture/dead-code-deprecation-plan.md` is inspected
- **THEN** all 9 dead code items SHALL be marked as completed/removed

#### Scenario: Build docs updated
- **WHEN** `docs/integration/build-setup.md` is inspected
- **THEN** references to `USE_XRESRC`, `USE_HWSELFTEST_PROFILE`, `NEW_HTTP_SERVER_DISABLE`, `WEBCONFIG_LITE_FLAG`, and `WEB_CONFIG_ENABLE` build flags SHALL NOT be present as active options
