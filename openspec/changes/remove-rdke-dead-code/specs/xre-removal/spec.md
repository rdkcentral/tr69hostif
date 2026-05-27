## REMOVED Requirements

### Requirement: XRE/Xcalibur client request handler
The system SHALL NOT include the XRE/Xcalibur client request handler (`hostIf_XREClient_ReqHandler`). All source files, build-system references, data-model objects, config entries, and typemap entries associated with `USE_XRESRC` and `XRELIB_FLAG` SHALL be removed.

**Reason**: XRE/Xcalibur client support is dead code on RDKE — the `USE_XRESRC` build flag is never enabled. Confirmed by architect review.
**Migration**: No migration needed. These parameters were never registered on RDKE devices.

#### Scenario: XRE handler files deleted
- **WHEN** the source tree is inspected after this change
- **THEN** `src/hostif/handlers/src/hostIf_XREClient_ReqHandler.cpp` and `src/hostif/handlers/include/hostIf_XREClient_ReqHandler.h` SHALL NOT exist

#### Scenario: USE_XRESRC preprocessor blocks removed
- **WHEN** the codebase is searched for `USE_XRESRC` or `XRELIB_FLAG`
- **THEN** zero matches SHALL be found in any source, header, or build file

#### Scenario: XRE config entries removed
- **WHEN** `conf/mgrlist.conf` is inspected
- **THEN** the XRE client manager entry SHALL NOT be present
- **WHEN** `conf/tr69hostIf.conf` is inspected
- **THEN** XRE-related configuration lines SHALL NOT be present

#### Scenario: Xcalibur data-model objects removed
- **WHEN** `data-model-generic.xml` is inspected
- **THEN** the Xcalibur/XRE object definitions (approximately lines 1706-2005) SHALL NOT be present
- **WHEN** `data-model-tv.xml` is inspected
- **THEN** Xcalibur-related object definitions SHALL NOT be present

#### Scenario: XRE typemap entries removed
- **WHEN** `typemap.json` is inspected
- **THEN** all entries with XRE/Xcalibur parameter paths SHALL NOT be present (approximately 59 entries)

#### Scenario: Build system cleaned of XRE references
- **WHEN** `configure.ac`, `src/hostif/handlers/Makefile.am`, and `src/hostif/profiles/DeviceInfo/Makefile.am` are inspected
- **THEN** all `USE_XRESRC`, `XRELIB_FLAG`, and XRE-related conditionals SHALL NOT be present

#### Scenario: Build succeeds without XRE
- **WHEN** the project is built with the default RDKE configuration
- **THEN** the build SHALL succeed with no errors or warnings related to missing XRE symbols
