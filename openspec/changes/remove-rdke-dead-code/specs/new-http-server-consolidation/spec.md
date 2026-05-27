## REMOVED Requirements

### Requirement: NEW_HTTP_SERVER_DISABLE conditional compilation
The system SHALL NOT use `NEW_HTTP_SERVER_DISABLE` as a compile-time toggle. All `#ifndef NEW_HTTP_SERVER_DISABLE` / `#ifdef NEW_HTTP_SERVER_DISABLE` preprocessor blocks in `hostIf_main.cpp` and related files SHALL be flattened, keeping the code path that was active when the flag was NOT defined (the default RDKE path).

**Reason**: `NEW_HTTP_SERVER_DISABLE` is never defined on RDKE. The toggle adds ~12 conditional blocks of dead alternate code paths. Confirmed by architect review.
**Migration**: No migration needed. The active code path is preserved; only the dead alternate branch and preprocessor guards are removed.

#### Scenario: NEW_HTTP_SERVER_DISABLE references eliminated
- **WHEN** the codebase is searched for `NEW_HTTP_SERVER_DISABLE`
- **THEN** zero matches SHALL be found in any source, header, or build file

#### Scenario: hostIf_main.cpp conditional blocks flattened
- **WHEN** `src/hostif/src/hostIf_main.cpp` is inspected
- **THEN** the ~12 `#ifndef NEW_HTTP_SERVER_DISABLE` blocks SHALL be replaced with the unconditional code that was inside the `#ifndef` branch

#### Scenario: No new HTTP server service file deleted
- **WHEN** the source tree is inspected
- **THEN** `tr69hostif_no_new_http_server.service` SHALL NOT exist at the repository root

### Requirement: legacyRFCEnabled utility removal
The system SHALL NOT include the `legacyRFCEnabled()` function. Its declaration in `hostIf_utils.h`, implementation in `hostIf_utils.cpp`, and all call sites SHALL be removed.

**Reason**: `legacyRFCEnabled()` is only meaningful when `NEW_HTTP_SERVER_DISABLE` is defined. With that flag removed, this function is dead code.
**Migration**: No migration needed. All call sites are inside `#ifndef NEW_HTTP_SERVER_DISABLE` blocks that are also being removed.

#### Scenario: legacyRFCEnabled function removed
- **WHEN** the codebase is searched for `legacyRFCEnabled`
- **THEN** zero matches SHALL be found in any source or header file

#### Scenario: Build succeeds after consolidation
- **WHEN** the project is built with the default RDKE configuration
- **THEN** the build SHALL succeed with no errors related to missing `legacyRFCEnabled` or `NEW_HTTP_SERVER_DISABLE` symbols
