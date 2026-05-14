# Automatics Thunder Plugin Test Gap Analysis — tr69hostif

> Generated: May 14, 2026
> Source: `test/functional-tests/automatics/tr69_thunder_auto.xlsx` (TC-THUNDER_PLUGIN-1000)
> Converted: `test/functional-tests/automatics/tr69_thunder_auto.md` (via `xls_to_markdown.py`)
> Reference: `test/docs/L2_Test_Coverage.md`, source analysis of `src/hostif/profiles/`

---

## Executive Summary

The **TC-THUNDER_PLUGIN-1000** Automatics test case (37 steps, P1 priority) validates
Thunder JSON-RPC plugin interactions in the tr69hostif module. Source code analysis
identifies **20 Thunder-backed TR-181 parameter handlers** across 5 Thunder plugins.
The Automatics test covers **12 of 20 parameters (~60%)**, leaving **8 parameters
untested**. Additionally, the test is purely **GET-focused** — no SET+GET roundtrip
or negative/error scenarios are included.

### Key Findings

| Metric | Value |
|--------|-------|
| Thunder-backed TR-181 Parameters (source) | **20** |
| Parameters tested by TC-THUNDER_PLUGIN-1000 | **12** |
| Parameters NOT tested (gaps) | **8** |
| Thunder Plugins exercised | 3 of 5 |
| SET+GET roundtrip tests | **2** (partial — no GET verification after SET) |
| Negative/error tests | **0** |
| Log verification steps | **16** (50% of all steps) |
| **Parameter Coverage** | **~60%** |
| **Scenario Coverage** (incl. negative/SET) | **~35%** |

---

## Test Case Structure Analysis

### TC-THUNDER_PLUGIN-1000 — Step-by-Step Mapping

The 37 steps follow a pattern: odd steps execute a TR-181 GET/SET, even steps verify
the curl response appears in `tr69hostif.log`. Mapped to source handlers:

| Step | Description | TR-181 Parameter | Thunder Plugin.Method | Covered? |
|:---:|---|---|---|:---:|
| 1 | GET wifi BSSID | `Device.WiFi.SSID.1.BSSID` | `org.rdk.NetworkManager.GetConnectedSSID` | ✅ |
| 2 | Verify curl in log | *(log check)* | — | — |
| 3 | GET wifi SSID | `Device.WiFi.SSID.1.SSID` | `org.rdk.NetworkManager.GetConnectedSSID` | ✅ |
| 4 | Verify curl in log | *(log check)* | — | — |
| 5 | GET Device.WiFi.SSID.1.Name | `Device.WiFi.SSID.1.Name` | `org.rdk.NetworkManager.GetConnectedSSID` | ✅ |
| 6 | Verify curl in log | *(log check)* | — | — |
| 7 | GET Device.WiFi.SSID.1.Enable | `Device.WiFi.SSID.1.Enable` | `org.rdk.NetworkManager.GetAvailableInterfaces` | ✅ |
| 8 | Verify curl in log | *(log check)* | — | — |
| 9 | GET Device.WiFi.SSID.1.MACAddress | `Device.WiFi.SSID.1.MACAddress` | `org.rdk.NetworkManager.GetAvailableInterfaces` | ✅ |
| 10 | Verify curl in log | *(log check)* | — | — |
| 11 | GET Device.WiFi.SSID.1.Status | `Device.WiFi.SSID.1.Status` | `org.rdk.NetworkManager.GetWifiState` | ✅ |
| 12 | Verify curl in log | *(log check)* | — | — |
| 13 | GET Device.WiFi.EndPoint.1.Enable | `Device.WiFi.EndPoint.1.Enable` | `org.rdk.NetworkManager.GetAvailableInterfaces` | ✅ |
| 14 | Verify curl in log | *(log check)* | — | — |
| 15 | GET Device.WiFi.EndPoint.1.Stats.SignalStrength | `Device.WiFi.EndPoint.1.Stats.SignalStrength` | `org.rdk.NetworkManager.GetConnectedSSID` | ✅ |
| 16 | Verify curl in log | *(log check)* | — | — |
| 17 | GET Device.WiFi.EndPoint.1.Security.ModesEnabled | `Device.WiFi.EndPoint.1.Security.ModesEnabled` | `org.rdk.NetworkManager.GetConnectedSSID` | ✅ |
| 18 | Verify curl in log | *(log check)* | — | — |
| 19 | GET Device.DeviceInfo.X_RDKCENTRAL-COM_Experience | `Device.DeviceInfo.X_RDKCENTRAL-COM_Experience` | `org.rdk.AuthService.getExperience` | ✅ |
| 20 | Verify curl in log | *(log check)* | — | — |
| 21 | SET invalid partner ID | `Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId` | `org.rdk.AuthService.setPartnerId` | ⚠️ SET only |
| 22 | Verify partner ID changed in log | *(log check)* | — | — |
| 23 | GET Device.DeviceInfo.X_COMCAST-COM_STB_IP | `Device.DeviceInfo.X_COMCAST-COM_STB_IP` | `org.rdk.NetworkManager.GetPrimaryInterface` → `GetIPSettings` | ✅ |
| 24 | Verify curl in log | *(log check)* | — | — |
| 25 | SET account ID to unknown | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID` | `org.rdk.AuthService.getServiceAccountId` (GET handler) | ⚠️ SET mismatch |
| 26 | GET the account ID | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID` | `org.rdk.AuthService.getServiceAccountId` | ⚠️ partial |
| 27 | Verify curl in log | *(log check)* | — | — |
| 28 | GET HotelCheckout.LastResetTime | `Device.DeviceInfo.X_RDKCENTRAL-COM_xAccount.HotelCheckout.LastResetTime` | `org.rdk.Account.getLastCheckoutResetTime` | ✅ |
| 29 | Verify curl in log | *(log check)* | — | — |
| 30 | GET HotelCheckout.Status | `Device.DeviceInfo.X_RDKCENTRAL-COM_xAccount.HotelCheckout.Status` | `org.rdk.Account.getLastCheckoutResetTime` | ✅ |
| 31 | Verify curl in log | *(log check)* | — | — |
| 32 | GET Device.WiFi.X_RDKCENTRAL-COM_WiFiEnable | `Device.WiFi.X_RDKCENTRAL-COM_WiFiEnable` | `org.rdk.NetworkManager.GetAvailableInterfaces` | ✅ |
| 33 | Verify curl in log | *(log check)* | — | — |
| 34 | GET ReverseSSH.xOpsReverseSshArgs | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs` | *(not Thunder-backed — file/IARM)* | N/A |
| 35 | GET ReverseSSH.xOpsReverseSshTrigger | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger` | *(SET handler uses Thunder guard)* | N/A |
| 36 | GET Device.WiFi.EndPoint.1.Status | `Device.WiFi.EndPoint.1.Status` | `org.rdk.NetworkManager.GetAvailableInterfaces` | ✅ |
| 37 | Verify curl in log | *(log check)* | — | — |

---

## Thunder Plugin Coverage Matrix

### Plugin: `org.rdk.NetworkManager` (6 methods)

| TR-181 Parameter | Handler | Thunder Method | TC Step | Status |
|---|---|---|:---:|:---:|
| `Device.DeviceInfo.X_COMCAST-COM_STB_IP` | `get_Device_DeviceInfo_X_COMCAST_COM_STB_IP` | `GetPrimaryInterface` → `GetIPSettings` | 23 | ✅ Covered |
| `Device.WiFi.Enable` | `get_Device_WiFi_EnableWiFi` | `GetAvailableInterfaces` | 32 | ✅ Covered |
| `Device.WiFi.Enable` | `set_Device_WiFi_EnableWiFi` | `SetInterfaceState` | — | ❌ **GAP** |
| `Device.WiFi.SSID.{i}.BSSID` | `get_Device_WiFi_SSID_BSSID` | `GetConnectedSSID` | 1 | ✅ Covered |
| `Device.WiFi.SSID.{i}.SSID` | `get_Device_WiFi_SSID_SSID` | `GetConnectedSSID` | 3 | ✅ Covered |
| `Device.WiFi.SSID.{i}.Name` | `get_Device_WiFi_SSID_Name` | `GetConnectedSSID` | 5 | ✅ Covered |
| `Device.WiFi.SSID.{i}.MACAddress` | `get_Device_WiFi_SSID_MACAddress` | `GetAvailableInterfaces` | 9 | ✅ Covered |
| `Device.WiFi.SSID.{i}.Enable` | `get_Device_WiFi_SSID_Enable` | `GetAvailableInterfaces` | 7 | ✅ Covered |
| `Device.WiFi.SSID.{i}.Status` | `get_Device_WiFi_SSID_Status` | `GetWifiState` | 11 | ✅ Covered |
| `Device.WiFi.EndPoint.{i}.Enable` | `get_Device_WiFi_EndPoint_Enable` | `GetAvailableInterfaces` | 13 | ✅ Covered |
| `Device.WiFi.EndPoint.{i}.Status` | `get_Device_WiFi_EndPoint_Status` | `GetAvailableInterfaces` | 36 | ✅ Covered |
| `Device.WiFi.EndPoint.{i}.Stats.SignalStrength` | `get_Device_WiFi_EndPoint_Stats_SignalStrength` | `GetConnectedSSID` | 15 | ✅ Covered |

### Plugin: `org.rdk.AuthService` (3 methods)

| TR-181 Parameter | Handler | Thunder Method | TC Step | Status |
|---|---|---|:---:|:---:|
| `Device.DeviceInfo.X_RDKCENTRAL-COM_Experience` | `get_X_RDKCENTRAL_COM_experience` | `getExperience` | 19 | ✅ Covered |
| `Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId` | `set_..._Syndication_PartnerId` | `setPartnerId` | 21 | ⚠️ SET only, no GET verify |
| `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID` | `get_xRDKCentralComRFCAccountId` | `getServiceAccountId` | 26 | ⚠️ Partial |

### Plugin: `org.rdk.Account` (1 method)

| TR-181 Parameter | Handler | Thunder Method | TC Step | Status |
|---|---|---|:---:|:---:|
| `Device.DeviceInfo.X_RDKCENTRAL-COM_xAccount.HotelCheckout.LastResetTime` | `get_HotelCheckoutLastResetTime` | `getLastCheckoutResetTime` | 28 | ✅ Covered |
| `Device.DeviceInfo.X_RDKCENTRAL-COM_xAccount.HotelCheckout.Status` | `get_HotelCheckoutStatus` | `getLastCheckoutResetTime` | 30 | ✅ Covered |

### Plugin: `org.rdk.MigrationPreparer` (1 method) — **NOT TESTED**

| TR-181 Parameter | Handler | Thunder Method | TC Step | Status |
|---|---|---|:---:|:---:|
| `Device.DeviceInfo.X_RDKCENTRAL-COM_Migration.MigrationReady` | `get_..._MigrationReady` | `getComponentReadiness` | — | ❌ **GAP** |

### Plugin: `org.rdk.UserSettings` (1 method) — **NOT TESTED**

| TR-181 Parameter | Handler | Thunder Method | TC Step | Status |
|---|---|---|:---:|:---:|
| `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger` | `set_xOpsReverseSshTrigger` | `getPrivacyMode` (guard) | — | ❌ **GAP** |

---

## Identified Test Gaps

### GAP-001: Missing Thunder Plugin Coverage

| Gap ID | TR-181 Parameter | Thunder Plugin | Gap Type | Priority |
|--------|---|---|---|:---:|
| GAP-001 | `Device.WiFi.Enable` (SET) | `org.rdk.NetworkManager.SetInterfaceState` | Missing SET+GET roundtrip | **P1** |
| GAP-002 | `Device.DeviceInfo.X_RDKCENTRAL-COM_Migration.MigrationReady` | `org.rdk.MigrationPreparer.getComponentReadiness` | Entire parameter untested | **P2** |
| GAP-003 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger` SET | `org.rdk.UserSettings.getPrivacyMode` (guard) | SET with Thunder guard untested | **P2** |
| GAP-004 | `Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId` GET after SET | `org.rdk.AuthService.setPartnerId` | No GET verification after SET (step 21) | **P1** |
| GAP-005 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID` | `org.rdk.AuthService.getServiceAccountId` | SET in step 25 targets GET-only param | **P2** |

### GAP-002: Missing Test Scenario Types

| Gap ID | Scenario Type | Description | Priority |
|--------|---|---|:---:|
| GAP-006 | Negative: Thunder plugin unavailable | GET when Thunder plugin is not activated/crashed | **P1** |
| GAP-007 | Negative: Thunder returns error | Plugin returns error response, verify graceful handling | **P1** |
| GAP-008 | Negative: Thunder timeout | Plugin call times out, verify daemon doesn't hang | **P2** |
| GAP-009 | SET+GET roundtrip: PartnerId | SET valid partnerId → GET → verify value matches | **P1** |
| GAP-010 | SET+GET roundtrip: WiFi Enable | SET WiFi.Enable false/true → GET → verify toggle | **P1** |
| GAP-011 | Boundary: Invalid partner ID response | Verify daemon handles malformed setPartnerId response | **P2** |
| GAP-012 | Boundary: Empty STB_IP | When no interface is up, verify empty/error response | **P2** |

### GAP-003: Non-Thunder WiFi Parameters (153 total handlers, 0 in Automatics)

The WiFi profile contains **153 GET handlers** and **~22 SET handlers** across Radio, SSID, AccessPoint, EndPoint, Security, WPS, and ClientRoaming sub-objects. The TC-THUNDER_PLUGIN-1000 test covers only the **10 Thunder-backed WiFi parameters**. The remaining WiFi handlers are not Thunder-backed but are still untested in Automatics:

| WiFi Sub-Object | GET Handlers | SET Handlers | Automatics Coverage |
|---|:---:|:---:|:---:|
| `Device.WiFi.*` (top-level) | 5 | 1 | 1 GET, 0 SET |
| `Device.WiFi.Radio.{i}.*` | 23 | 13 | **0** |
| `Device.WiFi.Radio.{i}.Stats.*` | 9 | 0 | **0** |
| `Device.WiFi.SSID.{i}.*` | 8 | 4 | 5 GET, 0 SET |
| `Device.WiFi.SSID.{i}.Stats.*` | 15 | 0 | **0** |
| `Device.WiFi.AccessPoint.{i}.*` | 11 | 0 | **0** |
| `Device.WiFi.AccessPoint.{i}.AssociatedDevice.*` | 7 | 0 | **0** |
| `Device.WiFi.AccessPoint.{i}.Security.*` | 9 | 0 | **0** |
| `Device.WiFi.AccessPoint.{i}.WPS.*` | 3 | 0 | **0** |
| `Device.WiFi.EndPoint.{i}.*` | 7 | 3 | 3 GET, 0 SET |
| `Device.WiFi.EndPoint.{i}.Profile.*` | 6 | 0 | **0** |
| `Device.WiFi.EndPoint.{i}.Profile.Security.*` | 4 | 0 | **0** |
| `Device.WiFi.EndPoint.{i}.Security.*` | 2 | 0 | 1 GET |
| `Device.WiFi.EndPoint.{i}.WPS.*` | 3 | 0 | **0** |
| `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.*` | 9 | 8 | **0** |
| **WiFi Totals** | **~121** | **~29** | **10 GET / 0 SET** |

---

## Test Quality Issues

### Issue 1: No Value Validation

All test steps use "should be return successfully" or "should be fetched successfully" — there is **no validation of the actual returned value** (format, range, type). For example:

- Step 1 (GET BSSID): Should verify MAC address format (`XX:XX:XX:XX:XX:XX`)
- Step 11 (GET Status): Should verify value is one of `Up`, `Down`, `Error`, `Disabled`
- Step 19 (GET Experience): Should verify non-empty string

### Issue 2: Duplicate Log Verification Pattern

Every other step (2, 4, 6, ..., 38) is "Verify the curl response in tr69hostif.log" with identical action text about SSH commands. This is a **generic log-scrape check** that does not verify the specific curl request/response content. 50% of all steps are this duplicate pattern.

### Issue 3: SET Without GET Verification

- **Step 21** (SET invalid partner ID): Sets an invalid value but only checks the log — does not GET the parameter afterward to verify the daemon handled it correctly.
- **Step 25** (SET account ID to unknown): The `AccountID` parameter is **GET-only** in the source code — there is no SET handler. This step may silently fail or exercise a code path that returns an error.

### Issue 4: Missing Test Metadata

- `WEIGHTAGE` is 0 for all steps (no priority differentiation)
- No `ALM ID` or `ICOLLECT TCs` mapped
- `ENABLED / DISABLED` is set to `DISABLED`
- No `FUNCTION MAPPING` populated

---

## Remediation Plan

### Priority 1 — High Impact Gaps (New test steps for existing TC)

| Action | New Steps | Effort |
|---|:---:|---|
| Add GET after SET for PartnerId (step 21) | +1 | Low — append GET + value check |
| Add SET+GET roundtrip for WiFi.Enable | +2 | Low — SET false → GET → verify → SET true → GET |
| Add value format validation to all GET steps | 0 (modify existing) | Medium — update expected output patterns |
| Remove or enhance duplicate log-check steps | 0 (modify existing) | Low — add specific log content patterns |

### Priority 2 — New Test Cases for Missing Thunder Plugins

| Action | New TC | Steps |
|---|---|:---:|
| GET MigrationReady via `org.rdk.MigrationPreparer` | TC-THUNDER_PLUGIN-1001 | ~4 |
| SET ReverseSSH.xOpsReverseSshTrigger (Thunder guard path) | TC-THUNDER_PLUGIN-1002 | ~4 |
| Negative: Thunder plugin unavailable during GET | TC-THUNDER_PLUGIN-1003 | ~6 |
| Negative: Thunder plugin returns error response | TC-THUNDER_PLUGIN-1004 | ~6 |

### Priority 3 — Non-Thunder WiFi Profile Coverage

| Action | New TC | Estimated Steps |
|---|---|:---:|
| WiFi.Radio parameters (23 GET + 13 SET) | TC-WIFI_RADIO-1000 | ~72 |
| WiFi.SSID.Stats (15 GET) | TC-WIFI_SSID_STATS-1000 | ~30 |
| WiFi.AccessPoint + Security + WPS (30 GET) | TC-WIFI_AP-1000 | ~60 |
| WiFi.EndPoint.Profile + Security + WPS (15 GET) | TC-WIFI_EP_PROFILE-1000 | ~30 |
| WiFi.X_RDKCENTRAL-COM_ClientRoaming (9 GET + 8 SET) | TC-WIFI_ROAMING-1000 | ~34 |

---

## Coverage Summary

```
Thunder-backed Parameters:     12 / 20  covered  (~60%)
  - org.rdk.NetworkManager:    12 / 12  covered  (100% GET, 0% SET)
  - org.rdk.AuthService:        2 /  3  covered  (~67%)
  - org.rdk.Account:            2 /  2  covered  (100%)
  - org.rdk.MigrationPreparer:  0 /  1  covered  (  0%)
  - org.rdk.UserSettings:       0 /  1  covered  (  0%)

Full WiFi Profile:             10 / 150  covered  (~6.7%)
SET+GET Roundtrips:             0 / ~29  covered  (  0%)
Negative/Error Scenarios:       0 /  4   minimum  (  0%)
```

---

## BDD Feature Scenarios for Missing Coverage

The following Gherkin scenarios document the gaps identified above, suitable for
creating new Automatics test cases or L2 pytest implementations.

### Missing Thunder GET Tests

```gherkin
Feature: Thunder Plugin GET Parameter Gaps

  Background:
    Given the tr69hostif daemon is running and initialized
    And Thunder plugins are activated

  Scenario: GET MigrationReady via org.rdk.MigrationPreparer
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_Migration.MigrationReady" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain a boolean value
    # Source: src/hostif/profiles/DeviceInfo/Device_DeviceInfo.cpp
    # Thunder: org.rdk.MigrationPreparer.getComponentReadiness
```

### Missing SET+GET Roundtrip Tests

```gherkin
Feature: Thunder Plugin SET+GET Roundtrip Gaps

  Background:
    Given the tr69hostif daemon is running and initialized
    And Thunder plugins are activated

  Scenario: SET and GET WiFi Enable via org.rdk.NetworkManager
    When I SET "Device.WiFi.Enable" to "false" as boolean via rbus
    And I GET "Device.WiFi.Enable" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "false"
    # Source: src/hostif/profiles/wifi/Device_WiFi.cpp
    # Thunder: org.rdk.NetworkManager.SetInterfaceState

  Scenario: SET and GET PartnerId via org.rdk.AuthService
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId" to "comcast" as string via rbus
    And I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId" via rbus
    Then the rbus response should not contain an error
    And the rbus response should contain "comcast"
    # Source: src/hostif/profiles/DeviceInfo/Device_DeviceInfo.cpp
    # Thunder: org.rdk.AuthService.setPartnerId

  Scenario: SET ReverseSSH Trigger with Thunder privacy guard
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger" to "start" as string via rbus
    Then the log should contain evidence of org.rdk.UserSettings.getPrivacyMode call
    # Source: src/hostif/profiles/DeviceInfo/Device_DeviceInfo.cpp
    # Thunder: org.rdk.UserSettings.getPrivacyMode (guard check)
```

### Missing Negative Tests

```gherkin
Feature: Thunder Plugin Negative Scenarios

  Background:
    Given the tr69hostif daemon is running and initialized

  Scenario: GET Thunder-backed parameter when plugin is unavailable
    Given the org.rdk.NetworkManager Thunder plugin is deactivated
    When I GET "Device.DeviceInfo.X_COMCAST-COM_STB_IP" via rbus
    Then the rbus response should contain an error
    And the log should contain a Thunder connection failure message

  Scenario: GET Thunder-backed parameter when plugin returns error
    Given the org.rdk.AuthService Thunder plugin returns an error response
    When I GET "Device.DeviceInfo.X_RDKCENTRAL-COM_Experience" via rbus
    Then the rbus response should contain an error or empty value
    And the daemon should not crash

  Scenario: SET AccountID (GET-only parameter)
    When I SET "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID" to "unknown" as string via rbus
    Then the rbus response should contain an error
    # Note: AccountID has no SET handler in source — SET should fail gracefully
```

---

## Related Files

- [`test/docs/L2_Test_Coverage.md`](../../docs/L2_Test_Coverage.md) — Full per-parameter L2 coverage analysis
- [`test/functional-tests/features/`](../features/) — BDD feature files for existing L2 tests
- [`docs/features/L2_Analysis_Report.md`](../../../docs/features/L2_Analysis_Report.md) — BDD feature gap analysis report
- [`test/functional-tests/automatics/tr69_thunder_auto.md`](tr69_thunder_auto.md) — Markdown conversion of xlsx test data
