## REMOVED Requirements

### Requirement: HW Self-Test profile support
The system SHALL NOT include the HW Self-Test profile handler. All `#ifdef USE_HWSELFTEST_PROFILE` / `HWSELFTEST_PROFILE_FLAG` preprocessor blocks, build-system references, and associated code SHALL be removed.

**Reason**: `USE_HWSELFTEST_PROFILE` is never enabled on RDKE. The HW Self-Test profile adds ~200 lines of dead code across ~7 files. Confirmed by architect review.
**Migration**: No migration needed. HW Self-Test parameters were never registered on RDKE devices.

#### Scenario: USE_HWSELFTEST_PROFILE references eliminated
- **WHEN** the codebase is searched for `USE_HWSELFTEST_PROFILE` or `HWSELFTEST_PROFILE_FLAG`
- **THEN** zero matches SHALL be found in any source, header, or build file

#### Scenario: HW Self-Test conditional blocks removed
- **WHEN** `src/hostif/profiles/DeviceInfo/Device_DeviceInfo.cpp` and related files are inspected
- **THEN** no `#ifdef USE_HWSELFTEST_PROFILE` blocks SHALL be present

#### Scenario: Build system cleaned of HW Self-Test references
- **WHEN** `configure.ac` and `src/hostif/profiles/DeviceInfo/Makefile.am` are inspected
- **THEN** all `USE_HWSELFTEST_PROFILE` and `HWSELFTEST_PROFILE_FLAG` conditionals SHALL NOT be present

#### Scenario: Build succeeds without HW Self-Test
- **WHEN** the project is built with the default RDKE configuration
- **THEN** the build SHALL succeed with no errors related to missing HW Self-Test symbols
