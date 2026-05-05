---
name: tr69hostif-issue-triage
description: >
  Triage any tr69hostif behavioral issue on RDK devices by correlating device
  log bundles with source code. Covers daemon hangs, TR-069/CWMP RPC failures,
  TR-181 parameter get/set errors, WebPA/parodus handler faults, RFC parameter
  override issues, CPU/memory spikes, and test gap analysis. The user states the
  issue; this skill guides systematic root-cause analysis regardless of issue type.
---

# tr69hostif Issue Triage Skill

## Purpose

Systematically correlate device log bundles with tr69hostif source code to
identify root causes, characterize impact, and propose unit-test and
functional-test reproduction scenarios — for **any** behavioral anomaly reported
by the user.

---

## Usage

Invoke this skill when:
- A device log bundle is available under `logs/` (or attached separately)
- The user describes a behavioral anomaly (examples: daemon stuck or crashing,
  TR-069 RPC not executing, parameter get/set silently failing, WebPA/parodus
  requests timing out, RFC overrides not applying, high CPU, high memory)
- You need to write a reproduction scenario for an existing or proposed fix

**The user's stated issue drives the investigation.** Do not assume a specific
failure mode — read the issue description first, then follow the steps below.

---

## Step 1: Orient to the Log Bundle

**Log bundle layout** (typical RDK device):
```
logs/<MAC>/<SESSION_TIMESTAMP>/logs/
    tr69hostIf.log.0          ← Primary tr69hostif daemon log (start here)
    PAMlog.txt.0              ← Platform/parameter management
    WPEFramework*.txt.0       ← Component framework messages
    SelfHeal*.txt.0           ← Watchdog and recovery events
    top_log.txt.0             ← CPU/memory snapshots (useful for perf issues)
    messages.txt.0            ← Kernel and system messages
```

Include any log files surfaced by the user's issue description (e.g., `parodus*.txt.0`
for WebPA connectivity issues, `syslog` for OOM events).

**Log timestamp prefix format**: `YYMMDD-HH:MM:SS.uuuuuu`
- Session folder names are **local-time snapshots** (format: `MM-DD-YY-HH:MMxM`)
- Log lines use device local time

**Session ordering**: Sort session folders chronologically. Multiple sessions may
represent reboots. Alphabetical sort does NOT equal chronological order.

---

## Step 2: Map Daemon Startup and Components

Read the startup section of `tr69hostIf.log.0` (first ~50 lines) to identify:

| What to find | Log pattern |
|---|---|
| Daemon start | `tr69HostIf starting up` |
| mgrlist loaded | `mgrlist.conf` path and profile count |
| Handler registration | `Registered handler for <module>` |
| WebPA/parodus connection | `Connected to parodus` or `parodus_connect` |
| RFC defaults loaded | `RFC defaults loaded` |
| IARM bus ready | `IARMBUS_Init` success |

**Key threads in tr69hostif**:
- Main thread — initializes handlers, listens for CWMP or parodus requests
- IARM event listener thread — receives platform events (network up/down, etc.)
- Handler worker threads — service individual TR-181 parameter get/set requests

---

## Step 3: Identify the Anomaly Window

Based on the **user's stated issue**, search for the relevant evidence pattern:

### Daemon Hang / Stuck
A hang manifests as a **timestamp gap** in `tr69hostIf.log.0` or no response to
get/set requests from the CWMP ACS or WebPA:
```
grep -n "GetParamValue\|SetParamValue\|RPC\|request" tr69hostIf.log.0 | tail -40
```
Gap > expected response time = anomaly. During the gap, check:
- Is the IARM bus thread still logging? (no → IARM bus deadlock or crash)
- Is there a mutex hold log before the gap? (yes → lock contention)

### TR-181 Parameter GET / SET Failure
Look for handler errors or missing responses:
```
grep -n "GetParamValue\|SetParamValue\|Error\|Failed\|NULL" tr69hostIf.log.0
```
- Identify which parameter path failed (`Device.X_RDKCENTRAL-COM_RFC.Feature.*`, etc.)
- Check if the handler is registered in `mgrlist.conf`
- Check if the backing store (`tr181store.ini`, `rfcdefaults.ini`) has the key

### RFC Parameter Override Not Applied
Look for RFC processing logs:
```
grep -n "RFC\|rfc\|override\|feature" tr69hostIf.log.0
```
- Confirm `rfcdefaults.ini` and `rfcVariable.ini` are loaded at startup
- Check for handler-specific RFC processing in `src/hostif/handlers/`
- Verify parameter name matches between RFC file and handler registration

### WebPA / Parodus Request Timeout
Look for parodus connection and request-handling logs:
```
grep -n "parodus\|webpa\|WEBPA\|connect\|timeout" tr69hostIf.log.0
```
- Confirm parodus connected at startup: `Connected to parodus`
- Identify which parameter request timed out (GET/SET/ADD/DELETE)
- Check waldb data-model XML for parameter registration

### CPU / Memory Spike
Correlate `top_log.txt.0` timestamps with tr69hostif activity:
```
grep -n "tr69hostif" top_log.txt.0
```
- Identify what tr69hostif was doing (bulk GET, data-model scan, IARM callback) at spike time
- Check for large iterative operations over Device.IP or Device.Ethernet tables
- Check for memory growth from uncleaned handler context objects

### Handler Registration / Module Load Failure
Look for initialization errors:
```
grep -n "ERROR\|WARN\|Failed\|register\|load" tr69hostIf.log.0 | head -60
```
- Identify which handler module failed to load
- Check shared library availability (`ldd /usr/local/bin/tr69hostif`)
- Confirm mgrlist.conf lists the module correctly

---

## Step 4: Correlate with Other Component Logs

Based on the anomaly window identified in Step 3, cross-reference with other logs:

| Issue Type | Companion Log | What to Look For |
|---|---|---|
| Daemon hang | `PAMlog.txt.0` | PAM parameter lock contention within hang window |
| Parameter GET/SET fail | `PAMlog.txt.0` | Underlying parameter store errors |
| WebPA timeout | `parodus*.txt.0` | Connection drops or message queue overflow |
| RFC override missing | `PAMlog.txt.0` | RFC feature flag not propagated |
| CPU spike | `top_log.txt.0` | CPU% at anomaly timestamps |
| Memory growth | `messages.txt.0` | OOM killer events |
| Crash / segfault | `messages.txt.0` | Kernel segfault or signal 11 for tr69hostif PID |
| IARM event miss | Any IARM bus log | Event dispatch errors |

---

## Step 5: Locate the Code Path

Navigate to the relevant source based on the anomaly type. Key modules:

### Daemon Main (`src/hostif/src/hostIf_main.cpp`)
- Initializes all subsystems: IARM, parodus, handler managers
- Starts the main request loop
- Processes CWMP ACS connections and dispatches RPCs

### Handler Framework (`src/hostif/handlers/`)
- Per-profile handler implementations (IP, Ethernet, DeviceInfo, Time, wifi, etc.)
- Each handler registers `GetParamValue` / `SetParamValue` callbacks
- Handlers use `IniFile` / `hostIf_utils` to access backing stores

### WebPA / Parodus Client (`src/hostif/parodusClient/pal/`)
- Bridges parodus/WebPA GET/SET/ADD/DELETE to TR-181 handler calls
- `waldb` data-model XML controls which parameters are registered with parodus
- Connection management and retry logic

### RFC Parameter Management (`src/hostif/handlers/src/` — rfcapi wrappers)
- Reads `rfcdefaults.ini` at startup for default values
- Reads `rfcVariable.ini` for operator overrides
- `tr181store.ini` / `bootstrap.ini` in `/opt/secure/RFC/` for runtime state

### Profile Modules (`src/hostif/profiles/`)
- Subdirectories per TR-181 subtree: `Device/`, `DeviceInfo/`, `IP/`, `Ethernet/`, `Time/`, `wifi/`, `moca/`, `STBService/`, `StorageService/`
- Each profile implements data-model object instances and their parameters
- Integer table indices can cause off-by-one issues in bulk GET operations

### SNMP Adapter (`src/hostif/snmpAdapter/`)
- Translates SNMP OID requests to TR-181 parameter paths
- Uses `tr181_snmpOID.conf` for OID-to-parameter mapping

---

## Step 6: Characterize Root Cause

Use this matrix to classify the issue based on observed evidence:

| Observed Pattern | Issue Class | Primary Code Location |
|---|---|---|
| No response to GET/SET, timestamp gap in log | Daemon hang or deadlock | `hostIf_main.cpp`, handler mutex |
| `ERROR` on specific `Device.X.*` parameter | Handler not registered or NULL callback | `handlers/src/`, `mgrlist.conf` |
| RFC feature enabled but behaviour unchanged | RFC parameter path mismatch or wrong store file | `rfcapi` wrappers, `rfcdefaults.ini` |
| Parodus GET returns stale/wrong value | waldb data-model out of sync, handler not updating cache | `parodusClient/pal/`, `waldb/data-model/` |
| Crash (SIGSEGV) on specific parameter | NULL pointer dereference in handler | handler `GetParamValue` / `SetParamValue` |
| High CPU during bulk GET operation | Iterating large object table without bounds | profile handler loop logic |
| Memory growth over uptime | Handler context never freed on module unload | handler `init` / `free` lifecycle |
| SNMP OID returns wrong value | OID mapping incorrect or TR-181 path stale | `tr181_snmpOID.conf`, `snmpAdapter.cpp` |
| Bootstrap parameters not persisted | `bootstrap.ini` write path wrong or permissions | RFC store path configuration |
| Parameter visible via CWMP but not WebPA | waldb data-model XML missing the parameter | `waldb/data-model/data-model-*.xml` |

---

## Step 7: Assess Unit Test Coverage

**Location**: `src/unittest/`, `src/hostif/src/gtest/`, `src/hostif/parodusClient/gtest/`

**Identify gaps relevant to the issue**. For each gap, write a test template:

```
Test Name: <what it verifies>
Setup:     <initial conditions — stubs, config files, handler registration>
Action:    <what triggers the behavior — GetParamValue/SetParamValue call>
Assert:    <expected return value and output parameter content>
File:      src/unittest/ or src/hostif/*/gtest/
```

**Common gap areas** (match to the issue class):
- Handler returns wrong value when backing store key is missing
- RFC override applies correctly when `rfcVariable.ini` has a matching entry
- WebPA SET propagates to the correct handler and persists in `tr181store.ini`
- NULL handler callback registered for a parameter path — graceful error, no crash
- Object table GET with index out of range — returns error, no buffer overflow

---

## Step 8: Assess L2 (Functional) Test Coverage

**Location**: `test/functional-tests/tests/`

**Existing test modules**:
- `test_bootup_sequence.py` — daemon startup, handler registration, initial parameter values
- `test_handlers_communications.py` — parameter GET/SET via handler protocol
- `tr69hostif_deviceip.py` — Device.IP subtree parameter reads
- `tr69hostif_webpa.py` — WebPA/parodus GET/SET round-trip

**Identify the missing scenario** that would catch the reported issue. Write a
Python pytest outline covering:
1. The precondition (daemon running, config files in place, specific parameter value)
2. The triggering action (GET/SET request, RFC reload, IARM event injection)
3. The correct observable outcome (expected parameter value, return code, log message)
4. The failure observable outcome (what the bug produces vs. what is expected)

```python
def test_<feature>_<scenario>(tr69hostif_daemon):
    """
    Verify <expected behavior> when <condition>.
    """
    # Arrange
    # ...set preconditions...

    # Act
    result = tr69hostif_daemon.get_param("Device.X.<path>")

    # Assert
    assert result == expected_value
```

---

## Step 9: Document Findings

Produce a triage report with:
1. **Issue restatement**: confirm back the user's stated problem in one sentence
2. **Device context**: MAC, firmware version, session timestamp(s) examined
3. **Anomaly timeline**: exact timestamps, relevant thread IDs, duration or frequency
4. **Root cause chain**: numbered steps, each with log evidence + source code reference
5. **Unit test gap**: which test file, test name, and what assertion it needs
6. **L2 test gap**: Python pytest outline
7. **Proposed fix**: minimum-scope change — file, function, and what to change

---

## Common Pitfalls

- **mgrlist.conf not loaded**: If a handler module is not listed in `/etc/mgrlist.conf`,
  its parameters will silently return empty — check mgrlist first for any missing GET/SET
- **waldb data-model mismatch**: Parameters included in one `data-model-*.xml` but absent
  in another are invisible to parodus/WebPA — always check all three XML files
- **RFC store path confusion**: `rfcdefaults.ini` is in `/tmp/`, `rfcVariable.ini` in
  `/opt/secure/RFC/` — a path mismatch causes overrides to be silently ignored
- **tr181store.ini vs bootstrap.ini precedence**: `bootstrap.ini` values take precedence
  over `tr181store.ini`; write to the wrong file and the value appears to not persist
- **IARM bus init order**: If tr69hostif starts before the IARM bus is ready, event
  subscriptions may be missed — look for `IARMBUS_Init` failure in the log
- **Index base**: TR-181 table indices start at **1**, not 0 — off-by-one in handler
  loops produces wrong data for the last or first instance
- **Thread-safety of handler context**: Some handlers cache state in a global struct
  that is not mutex-protected — concurrent GET and SET can corrupt the cache
