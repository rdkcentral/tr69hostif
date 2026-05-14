---
name: bdd-feature-generator
description: Generate BDD (Behavior Driven Development) feature files from source code analysis. Use for creating Gherkin-format documentation of implemented functionality, test specifications, and requirements traceability.
---

# BDD Feature Generator for Source Code

## Purpose

Automatically generate BDD feature files in Gherkin format by analyzing source code. This skill creates comprehensive behavioral documentation that can serve as:
- **Functional documentation** of implemented features
- **Test specifications** for automated testing
- **Requirements traceability** linking code to behavior
- **Gap analysis baseline** for comparing tests vs implementation

## Usage

Invoke this skill when:
- Documenting existing source code functionality in BDD format
- Creating test specifications from implementation
- Generating feature files for a new or existing project
- Performing gap analysis between tests and implementation
- Onboarding new team members with behavioral documentation

## Prerequisites

Before running this skill:

1. **Identify the build system** - Locate `Makefile.am`, `CMakeLists.txt`, or equivalent
2. **Determine compiled components** - Only document code that is actually built
3. **Locate existing feature files** - Match the format if they exist
4. **Identify output location** - Typically `docs/features/` or similar

## Process

### Step 1: Analyze Build Configuration

First, identify which source directories are actually compiled:

```bash
# For Autotools projects
cat source/Makefile.am | grep "SUBDIRS"

# For CMake projects
grep "add_subdirectory" CMakeLists.txt

# For Make projects
grep -E "^[a-zA-Z_]+:" Makefile | head -20
```

**Example Makefile.am analysis:**
```makefile
SUBDIRS = utils ccspinterface t2parser xconf-client protocol reportgen scheduler dcautil commonlib bulkdata

# Conditional compilation
if ENABLE_CCSP_SUPPORT
SUBDIRS += t2dm
endif
```

**Result:** Document only: `utils`, `ccspinterface`, `t2parser`, `xconf-client`, `protocol`, `reportgen`, `scheduler`, `dcautil`, `commonlib`, `bulkdata`, and conditionally `t2dm`.

**Exclude:** Directories not in SUBDIRS (e.g., `docs/`, `testApp/`, `nativeProtocolSimulator/`)

### Step 2: Analyze Source Code Structure

For each compiled component:

1. **Read header files** (`.h`) - Identify public APIs and data structures
2. **Read implementation files** (`.c`, `.cpp`) - Understand behavior
3. **Identify key functions** - Focus on public/exported functions
4. **Map dependencies** - Understand component relationships
5. **Note error handling** - Document failure scenarios

**Key elements to extract:**
- Function signatures and purposes
- Data structures and their fields
- Initialization/cleanup sequences
- Error codes and conditions
- Thread safety characteristics
- Configuration options

### Step 3: Create Feature File Structure

Create output directory:
```bash
mkdir -p docs/features
```

**Naming convention:** `{component_name}.feature`

**Examples:**
- `source/scheduler/` → `docs/features/scheduler.feature`
- `source/protocol/http/` → `docs/features/protocol_http.feature`
- `source/bulkdata/profile.c` → `docs/features/profile_management.feature`

### Step 4: Generate Feature Files

Use this template for each feature file:

```gherkin
####################################################################################
# If not stated otherwise in this file or this component's Licenses
# following copyright and licenses apply:
#
# Copyright [YEAR] RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
####################################################################################

# Source: source/{component_path}

Feature: {Feature Name - Descriptive Title}

  Background:
    Given {common preconditions for all scenarios}
    And {additional setup if needed}

  Scenario: {Descriptive scenario name}
    Given {initial context/state}
    When {action is performed}
    Then {expected outcome}
    And {additional verifications}

  Scenario: {Another scenario}
    Given {context}
    When {action}
    Then {outcome}
```

### Step 5: Map Code to Scenarios

**For each public function, create scenarios covering:**

1. **Happy path** - Normal successful operation
2. **Error conditions** - Invalid inputs, failures
3. **Edge cases** - Boundary conditions
4. **State transitions** - Initialization, cleanup

**Example mapping:**

```c
// Source code
T2ERROR initScheduler(TimeoutNotificationCB notificationCb, 
                      ActivationTimeoutCB activationCB,
                      NotifySchedulerstartCB notifyschedulerCB);
```

**Generated scenarios:**

```gherkin
Scenario: Scheduler initialization
  Given the scheduler is not initialized
  When initScheduler is called with callback functions
  Then the timeout notification callback should be registered
  And the activation timeout callback should be registered
  And the scheduler start notification callback should be registered
  And the scheduler should be ready to accept profile registrations

Scenario: Scheduler initialization with NULL callbacks
  Given the scheduler is not initialized
  When initScheduler is called with NULL callback functions
  Then an error should be returned
  And the scheduler should remain uninitialized
```

### Step 6: Document Data Structures

For important structures, create table-based scenarios:

```gherkin
Scenario: Profile structure contains all required fields
  Given a profile is being created
  Then the profile should support the following fields
    | Field                  | Type          | Description                        |
    | enable                 | bool          | Profile enabled status             |
    | hash                   | char*         | Unique profile hash                |
    | name                   | char*         | Profile name                       |
    | protocol               | char*         | Communication protocol             |
    | reportingInterval      | unsigned int  | Reporting interval in seconds      |
```

### Step 7: Create README Index

Create `docs/features/README.md`:

```markdown
# Source Code Feature Documentation

This folder contains BDD feature files documenting the functionality 
implemented in the `source/` directory.

## Feature Files Overview

| Feature File | Source Components | Description |
|--------------|-------------------|-------------|
| `component.feature` | `source/component/` | Brief description |

## Source Directory Mapping

Based on `source/Makefile.am`, the following directories are compiled:

### Always Compiled
- `component1/` - Description
- `component2/` - Description

### Conditionally Compiled
- `optional/` - Condition description

### Not Documented (Not Compiled)
- `testApp/` - Development tool only
- `docs/` - Documentation only

## BDD Format

All feature files follow Gherkin syntax...

## Generation Date

Generated: {DATE}
```

## Scenario Patterns

### Initialization Pattern

```gherkin
Scenario: {Component} initialization
  Given the {component} is not initialized
  When init{Component} is called
  Then {resources should be allocated}
  And {state should be set to initialized}
  And {component should be ready for use}

Scenario: {Component} initialization failure
  Given {failure condition}
  When init{Component} is called
  Then an error should be returned
  And {cleanup should occur}
```

### CRUD Pattern

```gherkin
Scenario: Create {entity}
  Given {preconditions}
  When {entity} is created with {parameters}
  Then the {entity} should be added to {collection}
  And the {entity} count should be incremented

Scenario: Read {entity}
  Given {entity} exists with {identifier}
  When {entity} is retrieved by {identifier}
  Then the correct {entity} should be returned

Scenario: Update {entity}
  Given {entity} exists
  When {entity} is updated with {new values}
  Then the {entity} should reflect the changes

Scenario: Delete {entity}
  Given {entity} exists
  When {entity} is deleted
  Then the {entity} should be removed
  And resources should be freed
```

### Signal/Event Pattern

```gherkin
Scenario: Handle {signal/event}
  Given the {component} is running
  When {signal/event} is received
  Then {handler should be invoked}
  And {expected action should occur}
  And {state should be updated}
```

### Protocol Pattern

```gherkin
Scenario: Send {data} via {protocol}
  Given {protocol} is configured
  And {data} is ready
  When send is called
  Then {data} should be transmitted
  And success status should be returned

Scenario: Handle {protocol} failure
  Given {protocol} is configured
  When transmission fails
  Then an error should be returned
  And {retry/cache behavior}
```

### Thread Safety Pattern

```gherkin
Scenario: {Component} thread safety
  Given multiple threads access {component} concurrently
  Then the following lock hierarchy should be maintained
    | Level | Lock Name | Purpose |
    | L0    | globalLock | Protects global state |
    | L1    | itemLock   | Protects individual items |
```

## Quality Checklist

Before completing feature generation:

- [ ] All compiled source directories analyzed
- [ ] Non-compiled directories excluded
- [ ] Each public function has at least one scenario
- [ ] Error conditions documented
- [ ] Data structures documented with tables
- [ ] Thread safety documented where applicable
- [ ] License headers included
- [ ] Source file references included
- [ ] README index created
- [ ] Consistent naming conventions used
- [ ] Scenarios are atomic (one behavior per scenario)
- [ ] Given/When/Then structure followed

## Output Structure

```
docs/
└── features/
    ├── README.md                    # Index and overview
    ├── {main_component}.feature     # Main entry point
    ├── {component1}.feature         # Component features
    ├── {component2}.feature
    ├── {protocol_type}.feature      # Protocol-specific
    └── {utility}.feature            # Utility functions
```

## Example: Complete Feature File

```gherkin
####################################################################################
# If not stated otherwise in this file or this component's Licenses
# following copyright and licenses apply:
#
# Copyright 2024 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
####################################################################################

# Source: source/scheduler/scheduler.c, scheduler.h

Feature: Report Scheduling and Timing

  Background:
    Given the telemetry daemon is running
    And the scheduler module is initialized

  Scenario: Scheduler initialization
    Given the scheduler is not initialized
    When initScheduler is called with callback functions
    Then the timeout notification callback should be registered
    And the activation timeout callback should be registered
    And the scheduler should be ready to accept profile registrations

  Scenario: Register profile with scheduler
    Given the scheduler is initialized
    When registerProfileWithScheduler is called with profile parameters
    Then a scheduler profile should be created
    And a scheduler thread should be created for the profile

  Scenario: Scheduler triggers timeout notification
    Given a profile is registered with a reporting interval
    When the reporting interval expires
    Then the timeout notification callback should be invoked
    And the profile name should be passed to the callback

  Scenario: Unregister profile from scheduler
    Given a profile is registered with the scheduler
    When unregisterProfileFromScheduler is called with the profile name
    Then the scheduler profile should be terminated
    And the scheduler thread should be stopped

  Scenario: Scheduler profile structure
    Given a scheduler profile is being created
    Then the scheduler profile should contain
      | Field           | Type            | Description                    |
      | name            | char*           | Profile name                   |
      | timeOutDuration | unsigned int    | Timeout duration in seconds    |
      | repeat          | bool            | Repeat flag                    |
      | terminated      | bool            | Termination flag               |
```

## Integration with Gap Analysis

After generating feature files, use them for gap analysis:

1. **Compare with test files** - Map features to existing tests
2. **Identify missing tests** - Features without test coverage
3. **Identify undocumented tests** - Tests without feature documentation
4. **Generate gap report** - Summary of coverage

## Maintenance

When source code changes:

1. **Re-analyze modified components** - Update affected feature files
2. **Add new scenarios** - For new functions/features
3. **Remove obsolete scenarios** - For deleted functionality
4. **Update README** - Reflect structural changes
5. **Version tag** - Include generation date

## Related Skills

- `technical-documentation-writer` - For detailed technical docs
- `memory-safety-analyzer` - For safety-critical code analysis
- `test-gap-analyzer` - For comparing features to tests
