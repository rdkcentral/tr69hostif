# L2 Functional Test — BDD Feature Analysis Report

> Generated: May 14, 2026
> Source: `test/functional-tests/` — 4 test files, **47 ordered pytest functions**
> Feature files: `test/functional-tests/features/` — 4 BDD feature files documenting all implemented tests
> Detailed coverage data: [`test/docs/L2_Test_Coverage.md`](../../test/docs/L2_Test_Coverage.md)

---

## Feature Files Overview

| Feature File | Source Test File | Scenarios | Description |
|---|---|:---:|---|
| [`tr69hostif_bootup_sequence.feature`](../../test/functional-tests/features/tr69hostif_bootup_sequence.feature) | `tests/test_bootup_sequence.py` | 18 | Daemon startup, thread init, rbus registration, data model, bootstrap, power controller |
| [`tr69hostif_handlers_communications.feature`](../../test/functional-tests/features/tr69hostif_handlers_communications.feature) | `tests/test_handlers_communications.py` | 22 | RFC, Non-RFC, Bootstrap, Time, Chrony SET/GET via rbus DML |
| [`tr69hostif_deviceip.feature`](../../test/functional-tests/features/tr69hostif_deviceip.feature) | `tests/tr69hostif_deviceip.py` | 17 | DeviceInfo defaults, Device.IP, Services, ReverseSSH GET/SET |
| [`tr69hostif_webpa.feature`](../../test/functional-tests/features/tr69hostif_webpa.feature) | `tests/tr69hostif_webpa.py` | 16 | WebPA SET/GET via mock parodus binary |

**Total documented scenarios: 73** (expanded from the 47 pytest functions to individual parameter-level scenarios)

---

## Source Directory Mapping

Based on `src/Makefile.am`, the following directories are compiled:

### Always Compiled
| Directory | Description |
|---|---|
| `src/hostif/handlers/` | TR-069 request handlers and HTTP server |
| `src/hostif/profiles/` | All TR-181 data model profile implementations |
| `src/hostif/parodusClient/` | Parodus/WebPA client |
| `src/hostif/parodusClient/startParodus/` | Parodus launcher |

### Conditionally Compiled
| Directory | Condition | Description |
|---|---|---|
| `src/hostif/snmpAdapter/` | `WITH_SNMP_ADAPTER` | SNMP adapter |
| `src/hostif/httpserver/` | `!WITH_NEW_HTTP_SERVER_DISABLE` | libsoup HTTP server |

### Not Documented (Not Compiled as Independent Units)
| Directory | Reason |
|---|---|
| `src/hostif/include/` | Headers only |
| `src/hostif/docs/` | Documentation only |
| `test/` | Test infrastructure |
| `scripts/` | Build/validation scripts |

---

## Gap Analysis: Feature Files vs Test Implementations

### Gap 1 — Original Feature Files vs Actual Tests

The original `.feature` files in `test/functional-tests/features/` were **documentation only** (not wired to `pytest-bdd`). They have now been replaced with comprehensive BDD scenarios that fully document all 47 implemented pytest functions.

Prior state comparison:

| Original Feature File | Scenarios in Feature | Tests Actually Implemented | Discrepancy |
|---|:---:|:---:|---|
| `tr69hostif_bootup_sequence.feature` | 17 | 18 | Missing: IARM init (order 7), critical errors sweep (order 17), RFC defaults file check (order 18) |
| `tr69hostif_handlers_communications.feature` | 2 | 9 | Missing: Time handlers (order 20), RFC multi-param (order 21), Non-RFC (order 22), Bootstrap persistence (order 24), all 3 Chrony tests (orders 25–27) |
| `tr69hostif_deviceip.feature` | 1 | 4 | Missing: DeviceDefault params (order 25), 11 IP params (order 26), Services (order 27), ReverseSSH (order 28) |
| `tr69hostif_webpa.feature` | 2 | 16 | Missing: 14 of 16 WebPA tests (only generic SET/GET example present; missing XconfUrl, LogUrl, firmware upgrade sequence, wildcard, etc.) |
| **TOTAL** | **22** | **47** | **25 tests have no feature documentation** |

### Gap 2 — Implemented Tests vs Module Surface

Based on the [`test/docs/L2_Test_Coverage.md`](../../test/docs/L2_Test_Coverage.md) analysis:

| Category | Total Testable | Currently Tested | Gap | Coverage |
|---|:---:|:---:|:---:|:---:|
| TR-181 Parameter Handlers (GET+SET) | 707 | ~34 | ~673 | ~5% |
| Behavioral Scenarios (HTTP, WebPA, RFC, lifecycle) | 38 | ~18 | ~20 | ~47% |
| Negative / Edge Case Tests | ~16 | 0 | ~16 | 0% |
| **TOTAL** | **~761** | **~52** | **~709** | **~6.8%** |

---

## Missing Coverage — Priority Breakdown

### P1: Thunder Plugin Calls (0% coverage — 21 parameters)

All 5 Thunder plugins (`org.rdk.NetworkManager`, `org.rdk.AuthService`, `org.rdk.System`, `org.rdk.MigrationPreparer`, `org.rdk.Account`) and their 21 mapped TR-181 parameters have **zero test coverage**. These are synchronous blocking calls with 10-second timeouts — any regression silently returns empty/NOK.

| Plugin | Parameters Affected | Example |
|---|:---:|---|
| `org.rdk.NetworkManager` | 12 | `Device.WiFi.SSID.{i}.SSID`, `Device.DeviceInfo.X_COMCAST-COM_STB_IP` |
| `org.rdk.AuthService` | 3 | `Device.DeviceInfo.X_RDKCENTRAL-COM_Experience`, `…AccountID` |
| `org.rdk.System` | 1 | `…ReverseSSH.xOpsReverseSshTrigger` privacy gate |
| `org.rdk.MigrationPreparer` | 1 | `…MigrationReady` |
| `org.rdk.Account` | 1 | `…HotelCheckout.LastResetTime` |

**Recommended:** Deploy mock Thunder JSON-RPC server on `127.0.0.1:9998`.

### P2: HTTP Server Protocol (0% coverage — 8 tests needed)

The libsoup HTTP server accepting WDMP-C JSON on port `11999` is completely untested. Dead code `profile_init_run_command()` in `test_bootup_sequence.py` was never wired.

| Missing Test | Expected |
|---|---|
| GET single parameter | `{"statusCode":200,...}` |
| GET multiple parameters | Multi-value response |
| GET wildcard | All matching params |
| SET with CallerID | `{"statusCode":200}` |
| SET without CallerID | `500 POST Not Allowed without CallerID` |
| Malformed JSON body | `400 Bad Request` |
| Unknown parameter | Non-zero statusCode |
| Empty body | `400 No request data.` |

### P3: WiFi TR-181 Subtree (0% coverage — 153 parameters)

Entire `Device.WiFi.*` is untested: Radio (36), AccessPoint (41), SSID (22), EndPoint (32), ClientRoaming (13).

### P4: RFC Variable Store (partial — 4 tests needed)

| Scenario | Status |
|---|---|
| `rfcdefaults.ini` file read + rbus GET | **Covered** (order 18) |
| `bootstrap.ini` persistence + `.journal` file | **Covered** (order 24) |
| `rfcVariable.ini` read-back | **Not covered** |
| RFC override precedence (`rfcVariable` overrides `rfcdefaults`) | **Not covered** |
| `XRFCVarStore` consistency after daemon restart | **Not covered** |
| `RFC_CONTROL_RELOADCACHE` trigger | **Not covered** |

### P5: Negative / Edge Case Tests (0% coverage — 16 tests needed)

| Missing Test | Description |
|---|---|
| SET wrong data type | SET a string param with integer value |
| SET out-of-range value | SET integer param beyond valid range |
| GET nonexistent parameter | GET param not in data model |
| Malformed WebPA JSON | Malformed JSON via parodus mock |
| Thunder timeout simulation | Kill mock Thunder mid-request |
| Thunder empty response | Return `{}` from mock |
| HTTP POST without CallerID | Expect `500` response |
| WebPA REPLACE command | Currently only GET/SET tested |

### P6: Untested Module Profiles (0% coverage)

| Profile | Parameters | Status |
|---|:---:|---|
| `Device.Ethernet.*` | 30 | Thread start logged only — no param GET/SET |
| `Device.DHCPv4.*` | 4 | Zero coverage |
| `Device.InterfaceStack.*` | 2 | Zero coverage |
| `Device.MoCA.*` | 99 | Zero coverage |
| `Device.StorageService.*` | 15 | Zero coverage |
| `Device.Services.STBService.*` | 84 | Only `STBServiceNumberOfEntries` GET (order 27) |
| `Device.Time.*` (beyond Chrony) | 36 | `NTPServer1` + 3 Chrony tests only |

---

## Infrastructure Issues Affecting Test Reliability

| Issue | Impact | Recommendation |
|---|---|---|
| No `conftest.py` / fixtures | No setup/teardown; SET values persist between tests | Add `conftest.py` with parameter rollback |
| BDD feature files not wired | `.feature` files are docs-only — no `@given/@when/@then` | Wire with `pytest-bdd` or keep as documentation |
| `profile_helper_functions.py` broken | `GREP_STRING` undefined → `NameError` at runtime | Fix or remove |
| Dead code `profile_init_run_command()` | HTTP server test never invoked | Move into actual test functions |
| Hardcoded expected values | Tests tied to specific container image | Extract to `basic_constants.py` |
| Log isolation absent | Logs not cleared between tests; grep spans full boot log | Call `clear_tr69hostiflogs()` per test |
| Order conflicts | `tr69hostif_deviceip.py` and `test_handlers_communications.py` share orders 25–27 | Renumber to avoid pytest-ordering conflicts |

---

## Summary

| Metric | Value |
|---|---|
| Implemented L2 pytest functions | **47** |
| Scenarios documented in new BDD features | **73** |
| Original feature file scenarios | **22** |
| Feature-to-test documentation gap (original) | **25 undocumented tests (54%)** |
| Total module surface (testable items) | **~761** |
| Current effective coverage | **~52 tests (~6.8%)** |
| Tests still required for 100% | **~709** |
| Top priority gaps | Thunder plugins (21), HTTP server (8), WiFi (153), Negative tests (16) |

---

## BDD Format

All feature files follow Gherkin syntax with:
- Apache 2.0 license header
- Source file reference comment
- `Background:` for common preconditions
- `@order-N` tags mapping to pytest execution order
- `Scenario Outline:` with `Examples:` tables for parameterized tests
- Consistent `Given/When/Then` step vocabulary

## Related Files

- [`test/docs/L2_Test_Coverage.md`](../../test/docs/L2_Test_Coverage.md) — Detailed per-parameter coverage analysis
- [`test/functional-tests/features/`](../../test/functional-tests/features/) — BDD feature files (updated)
- [`test/functional-tests/tests/`](../../test/functional-tests/tests/) — Runnable pytest implementations
- [`test/functional-tests/automatics/`](../../test/functional-tests/automatics/) — Automatics gap analysis tooling
