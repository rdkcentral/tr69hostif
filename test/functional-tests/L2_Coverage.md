# tr69hostif L2 Coverage Report

**Generated:** 2026-11-25  
**Component:** `tr69hostif` (`src/hostif (C++ runtime coverage is in LCOV and HTML artifacts)`)  
**Test suite:** `test/functional-tests/tests (run_l2.sh)`  
**Coverage tool:** lcov (source-level instrumentation via `--coverage`)

---

## 1. Executive Summary

**L2 Test Coverage**

| Metric | Value |
|---|:---:|
| Source functions found | 25 |
| Functions hit | 0 |
| Functions not hit | 25 |
| Current functional coverage | 0.0% |
| Target L2 functional coverage | No threshold configured |

> Function coverage = functions called at least once during the L2 run (lcov `FNDA` > 0),
> divided by all function definitions in `src/hostif (C++ runtime coverage is in LCOV and HTML artifacts)/**/*.c`, including those compiled out by `#if`.

**L2 Test Summary**

| Metric | Value |
|---|:---:|
| Feature files | 36 |
| Feature scenarios | 479 |
| Pytest files | 2 |
| Test functions (`test_*`) | 28 |
| Mapped feature/test pairs | 0 / 36 |
| Unmatched test files | 2 |
| Line coverage (lcov) | N/A (0 of 0 lines) |

> Function definitions are discovered with a heuristic C source scan.
> Disabled test functions (skip markers or commented definitions): 0;
> files with no enabled test functions: 0.
> Test counts include skipped definitions and are not pass/fail results. Skip counts are
> estimates; conditional, class-level and module-level skips are not evaluated.

---

## 2. Feature ↔ Test Mapping

### 2.1 Mapped Pairs

| # | Feature File | Scenarios | Test File | Tests | Status |
|:---:|---|:---:|---|:---:|:---:|
| | **Totals** | **0** | | **0** | |

> PASS means the test-definition count meets the scenario count, not that tests passed.
> Matching uses the same relative directory and feature stem, with an optional
> test_ prefix. Scenario Outline rows and parametrized cases are not expanded.

### 2.2 Tests Exceeding Scenario Count

> Extra functions can include setup/teardown split more granularly than feature scenarios.

| Feature File | Scenarios | Test File | Tests | Extra |
|---|:---:|---|:---:|:---:|

### 2.3 Orphan Tests (test exists, no feature file)

| Test File | Tests | Note |
|---|:---:|---|
| `test_bootup_sequence.py` | 18 | **Missing `.feature` file** |
| `test_handlers_communications.py` | 10 | **Missing `.feature` file** |

### 2.4 Orphan Features (feature exists, no test file)

| Feature File | Scenarios | Note |
|---|:---:|---|
| `tr69hostif_account_thunder_plugin.feature` | 2 | **Missing test file** |
| `tr69hostif_authservice_thunder_plugin.feature` | 3 | **Missing test file** |
| `tr69hostif_bluetooth.feature` | 7 | **Missing test file** |
| `tr69hostif_bootup_sequence.feature` | 18 | **Missing test file** |
| `tr69hostif_custom.feature` | 16 | **Missing test file** |
| `tr69hostif_device_info.feature` | 30 | **Missing test file** |
| `tr69hostif_deviceip.feature` | 9 | **Missing test file** |
| `tr69hostif_devicetime.feature` | 14 | **Missing test file** |
| `tr69hostif_dhcpv4.feature` | 4 | **Missing test file** |
| `tr69hostif_ethernet.feature` | 13 | **Missing test file** |
| `tr69hostif_ethernet_handlers.feature` | 30 | **Missing test file** |
| `tr69hostif_handlers_communications.feature` | 21 | **Missing test file** |
| `tr69hostif_http_server.feature` | 14 | **Missing test file** |
| `tr69hostif_interfacestack.feature` | 3 | **Missing test file** |
| `tr69hostif_ip.feature` | 33 | **Missing test file** |
| `tr69hostif_ipremotesupport.feature` | 5 | **Missing test file** |
| `tr69hostif_moca.feature` | 59 | **Missing test file** |
| `tr69hostif_negative_edge_cases.feature` | 4 | **Missing test file** |
| `tr69hostif_negative_tests.feature` | 26 | **Missing test file** |
| `tr69hostif_networkmanager_endpoint_thunder_plugin.feature` | 7 | **Missing test file** |
| `tr69hostif_networkmanager_ssid_thunder_plugin.feature` | 8 | **Missing test file** |
| `tr69hostif_opsdevicemgmt_logging.feature` | 7 | **Missing test file** |
| `tr69hostif_opsdevicemgmt_rpc.feature` | 10 | **Missing test file** |
| `tr69hostif_processor_processstatus.feature` | 9 | **Missing test file** |
| `tr69hostif_rfc_store.feature` | 4 | **Missing test file** |
| `tr69hostif_rfc_store_params.feature` | 20 | **Missing test file** |
| `tr69hostif_secure_debug_state.feature` | 6 | **Missing test file** |
| `tr69hostif_std_params.feature` | 9 | **Missing test file** |
| `tr69hostif_storageservice.feature` | 15 | **Missing test file** |
| `tr69hostif_system_thunder_plugin.feature` | 1 | **Missing test file** |
| `tr69hostif_thunder_negative_edge_cases.feature` | 3 | **Missing test file** |
| `tr69hostif_thunder_plugins.feature` | 9 | **Missing test file** |
| `tr69hostif_time_chrony.feature` | 29 | **Missing test file** |
| `tr69hostif_webpa.feature` | 16 | **Missing test file** |
| `tr69hostif_webpa_negative_edge_cases.feature` | 6 | **Missing test file** |
| `tr69hostif_webpa_rdkdlmgr.feature` | 9 | **Missing test file** |

---

## 3. Source Module Coverage (lcov)

| Module | Lines | Functions | Coverage Bar |
|---|:---:|:---:|---|
| `backgroundrun.c` | — | — | no data |
| `unittest/stubs/paramMgr.c` | — | — | no data |
| `unittest/stubs/secure_wrapper.c` | — | — | no data |
| `unittest/stubs/telemetry_msgsender_stub.c` | — | — | no data |
| `unittest/stubs/wdmp-c.c` | — | — | no data |
| `unittest/stubs/wdmp_internal.c` | — | — | no data |

> Only trace records matched to source files in this module contribute to line coverage.
> Ambiguous source paths are excluded; keep source-relative paths in the tracefile.

---

## 4. Per-Behavior Coverage Detail

> Module-supplied assessment; not inferred from lcov or test counts.

The aggregate runtime trace is not attributable to individual tests or behaviors.

---

## 5. Scenario-to-Test Gap Analysis

All mapped feature files have sufficient test function coverage.

**Orphan tests** (no feature file):

- `test_bootup_sequence.py`
- `test_handlers_communications.py`

> Unmatched features are listed in section 2.4; counts alone do not prove behavior coverage.

---

## 6. Gap Recommendations

> Module-supplied recommendations; review when the implementation or tests change.

No module-specific recommendations supplied.

---

## 7. Test Infrastructure Notes

LCOV and HTML include instrumented C++ production sources under src/hostif.
The shared Markdown generator inventories C source files only. Since the exercised tr69hostif implementation is C++, its source coverage tables may show N/A and are not an aggregate component coverage percentage.
The shared Markdown generator inventories test_*.py files only; most functional suites here are named tr69hostif_*.py, so its feature/test mapping is partial.
Feature-to-test mapping is definition-based; execution outcomes are reported separately in coverage-status.md and the pytest JSON reports.
The coverage build is opt-in for L2 CI and uses GCC --coverage flags; unit-test counters are cleared before the L2 run.

---

## 8. Appendix: File Inventory

### Feature Files

| # | File | Scenarios |
|:---:|---|:---:|
| 1 | `tr69hostif_account_thunder_plugin.feature` | 2 |
| 2 | `tr69hostif_authservice_thunder_plugin.feature` | 3 |
| 3 | `tr69hostif_bluetooth.feature` | 7 |
| 4 | `tr69hostif_bootup_sequence.feature` | 18 |
| 5 | `tr69hostif_custom.feature` | 16 |
| 6 | `tr69hostif_device_info.feature` | 30 |
| 7 | `tr69hostif_deviceip.feature` | 9 |
| 8 | `tr69hostif_devicetime.feature` | 14 |
| 9 | `tr69hostif_dhcpv4.feature` | 4 |
| 10 | `tr69hostif_ethernet.feature` | 13 |
| 11 | `tr69hostif_ethernet_handlers.feature` | 30 |
| 12 | `tr69hostif_handlers_communications.feature` | 21 |
| 13 | `tr69hostif_http_server.feature` | 14 |
| 14 | `tr69hostif_interfacestack.feature` | 3 |
| 15 | `tr69hostif_ip.feature` | 33 |
| 16 | `tr69hostif_ipremotesupport.feature` | 5 |
| 17 | `tr69hostif_moca.feature` | 59 |
| 18 | `tr69hostif_negative_edge_cases.feature` | 4 |
| 19 | `tr69hostif_negative_tests.feature` | 26 |
| 20 | `tr69hostif_networkmanager_endpoint_thunder_plugin.feature` | 7 |
| 21 | `tr69hostif_networkmanager_ssid_thunder_plugin.feature` | 8 |
| 22 | `tr69hostif_opsdevicemgmt_logging.feature` | 7 |
| 23 | `tr69hostif_opsdevicemgmt_rpc.feature` | 10 |
| 24 | `tr69hostif_processor_processstatus.feature` | 9 |
| 25 | `tr69hostif_rfc_store.feature` | 4 |
| 26 | `tr69hostif_rfc_store_params.feature` | 20 |
| 27 | `tr69hostif_secure_debug_state.feature` | 6 |
| 28 | `tr69hostif_std_params.feature` | 9 |
| 29 | `tr69hostif_storageservice.feature` | 15 |
| 30 | `tr69hostif_system_thunder_plugin.feature` | 1 |
| 31 | `tr69hostif_thunder_negative_edge_cases.feature` | 3 |
| 32 | `tr69hostif_thunder_plugins.feature` | 9 |
| 33 | `tr69hostif_time_chrony.feature` | 29 |
| 34 | `tr69hostif_webpa.feature` | 16 |
| 35 | `tr69hostif_webpa_negative_edge_cases.feature` | 6 |
| 36 | `tr69hostif_webpa_rdkdlmgr.feature` | 9 |
| | **Total** | **479** |

### Test Files

| # | File | Tests |
|:---:|---|:---:|
| 1 | `test_bootup_sequence.py` | 18 |
| 2 | `test_handlers_communications.py` | 10 |
| | **Total** | **28** |
