# tr69hostif Automation Test Suite

> **Reference**: Derived from `Test Case for Tr69Hostif` (PDF, April 2026) and tr69hostif source code analysis.
> **Tool**: `rbuscli` (on-device) or equivalent test harness using `HOSTIF_MsgData_t` API.
**Total Test Cases**: 243

---
## Table of Contents

1. [Initialization & Lifecycle](#1-initialization--lifecycle)
2. [Data Model Loading](#2-data-model-loading)
3. [RBus DML Provider](#3-rbus-dml-provider)
4. [RFC Store — Get/Set/Clear](#4-rfc-store--getsetclear)
5. [Bootstrap Store (BSStore)](#5-bootstrap-store-bsstore)
6. [HTTP Server](#6-http-server)
7. [DeviceInfo — Read-Only Parameters](#7-deviceinfo--read-only-parameters)
8. [DeviceInfo — Read-Write Parameters](#8-deviceinfo--read-write-parameters)
9. [Firmware Download Parameters](#9-firmware-download-parameters)
10. [RFC Feature Flags](#10-rfc-feature-flags)
11. [ManagementServer Parameters](#11-managementserver-parameters)
12. [Time Parameters](#12-time-parameters)
13. [Ethernet Parameters](#13-ethernet-parameters)
14. [IP Interface Parameters](#14-ip-interface-parameters)
15. [IP Diagnostics](#15-ip-diagnostics)
16. [WiFi Parameters](#16-wifi-parameters)
17. [STBService / Services Parameters](#17-stbservice--services-parameters)
18. [Notification & Event Subscription](#18-notification--event-subscription)
19. [WebPA / Parodus Client](#19-webpa--parodus-client)
20. [xOpsDeviceMgmt Parameters](#20-xopsdevicemgmt-parameters)
21. [T2 Telemetry Parameters](#21-t2-telemetry-parameters)
22. [Negative / Boundary Tests](#22-negative--boundary-tests)
23. [DHCPv4 Client Parameters](#23-dhcpv4-client-parameters)
24. [MoCA Interface Parameters](#24-moca-interface-parameters)
25. [StorageService Parameters](#25-storageservice-parameters)
26. [SNMP Adapter](#26-snmp-adapter)
27. [XRFCVarStore — RFC Variable Cache](#27-xrfcvarstore--rfc-variable-cache)
28. [WebConfig Parameters](#28-webconfig-parameters)
29. [WebPA Attributes API](#29-webpa-attributes-api)
30. [Message Handler Diagnostics](#30-message-handler-diagnostics)
31. [Update Handler Lifecycle](#31-update-handler-lifecycle)
32. [STBService Extended (SPDIF / VideoOutput / SDCard)](#32-stbservice-extended-spdif--videooutput--sdcard)

---

## Legend

| Column | Meaning |
|--------|---------|
| **TC-ID** | Unique test case identifier |
| **Priority** | P1=critical, P2=high, P3=medium |
| **Type** | FUNC=functional, NEG=negative, BND=boundary |
| **Automation** | CLI=rbuscli, API=C API, L2=L2 integration |

---

## 1. Initialization & Lifecycle

> **Existing coverage (PDF):** Threads verified, init functions verified.

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-INIT-001 | Daemon startup — thread creation | Start `tr69hostif` binary; query `/proc/<pid>/task` or log file | Threads `jsonIfHandlerThread`, `HTTPServerStartThread`, `shutdown_thread`, `libpd_client_mgr` all created | P1 | FUNC | L2 |
| TC-INIT-002 | Initialization functions success | Start daemon; check log for each init call | `init_rbus_dml_provider()`, `hostIf_initalize_ConfigManger()`, `loadDataModel()`, `updateHandler::Init()` all return success | P1 | FUNC | L2 |
| TC-INIT-003 | Daemon restart recovery | Stop and restart `tr69hostif`; repeat TC-INIT-001 | All threads re-created; no crash or zombie threads | P1 | FUNC | L2 |
| TC-INIT-004 | IARM bus registration | Start daemon; verify IARM registration in logs | IARM bus connected without error | P2 | FUNC | L2 |
| TC-INIT-005 | RFC defaults loaded on first boot | On fresh start verify RFC defaults file is read | Default values from `rfcdefaults/tr69hostif.ini` are applied | P2 | FUNC | L2 |
| TC-INIT-006 | Graceful shutdown via `shutdown_thread` | Send SIGTERM; verify orderly teardown | All threads exit cleanly; no file descriptor leaks | P1 | FUNC | L2 |

---

## 2. Data Model Loading

> **Existing coverage (PDF):** DM initialization success verified.

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-DM-001 | `loadDataModel()` success | Start daemon; check `DB_SUCCESS` return and log | Data model XML parsed without error | P1 | FUNC | API |
| TC-DM-002 | Parameter count after load | Query total registered parameters via waldb | Matches expected count (904 in test DML) | P1 | FUNC | API |
| TC-DM-003 | Invalid DML file path | Point config to non-existent DML; start daemon | Daemon logs error; does not crash | P2 | NEG | L2 |
| TC-DM-004 | Malformed DML file | Replace DML with malformed XML; start daemon | Daemon logs parse error; fallback or clean exit | P2 | NEG | L2 |
| TC-DM-005 | Re-load without restart | Trigger `loadDataModel()` a second time | Idempotent — no duplicate registration, no crash | P2 | FUNC | API |
| TC-DM-006 | T2 exclusion list respected | Confirm `Device.X_RDKCENTRAL-COM_T2.ReportProfiles` not registered via rbus | Parameter absent from rbus provider list | P1 | FUNC | CLI |

---

## 3. RBus DML Provider

> **Existing coverage (PDF):** RFC and non-RFC get/set referenced.

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-RBUS-001 | `init_rbus_dml_provider()` succeeds | Check rbus handle non-NULL in logs after init | `rbusHandle` initialized without `RBUS_ERROR` | P1 | FUNC | API |
| TC-RBUS-002 | GET handler returns value for read-only param | `rbuscli get Device.DeviceInfo.Manufacturer` | Returns non-empty string | P1 | FUNC | CLI |
| TC-RBUS-003 | GET handler returns value for read-write param | `rbuscli get Device.DeviceInfo.ProvisioningCode` | Returns string value (may be empty but no error) | P1 | FUNC | CLI |
| TC-RBUS-004 | SET handler for read-write param | `rbuscli set Device.DeviceInfo.ProvisioningCode string NewCode` | Returns success; subsequent GET returns `NewCode` | P1 | FUNC | CLI |
| TC-RBUS-005 | SET rejected for read-only param | `rbuscli set Device.DeviceInfo.Manufacturer string X` | Returns error code; value unchanged | P1 | NEG | CLI |
| TC-RBUS-006 | GET on non-existent parameter | `rbuscli get Device.DeviceInfo.NonExistent` | Returns `RBUS_ERROR_INVALID_INPUT` or equivalent | P2 | NEG | CLI |
| TC-RBUS-007 | Data type coercion — boolean GET | `rbuscli get Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadNow` | Returns `true` or `false` (boolean) | P2 | FUNC | CLI |
| TC-RBUS-008 | Data type coercion — unsignedInt GET | `rbuscli get Device.DeviceInfo.UpTime` | Returns numeric value ≥ 0 | P2 | FUNC | CLI |
| TC-RBUS-009 | Event subscription — autopublish default | Subscribe to `Device.DeviceInfo.SoftwareVersion` | Subscription accepted; `autoPublish=true` | P2 | FUNC | API |
| TC-RBUS-010 | Event subscription — autopublish disabled for RDKRemoteDebugger.IssueType | Subscribe; check `autoPublish` flag | `autoPublish=false` returned | P2 | FUNC | CLI |
| TC-RBUS-011 | Concurrent GET requests | Send 20 parallel `rbuscli get` calls for different params | All return valid results; no deadlock | P1 | FUNC | CLI |

---

## 4. RFC Store — Get/Set/Clear

> **Existing unit tests**: `rfcStoreTest.setValue`, `rfcStoreTest.getValue`, `rfcStoreTest.clearLocalValue`.

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-RFC-001 | Set boolean RFC value | Set `RFC.Feature.Airplay.Enable = true` via `XRFCStore::setValue` | Returns 0 (success) | P1 | FUNC | API |
| TC-RFC-002 | Get boolean RFC value after set | Get `RFC.Feature.Airplay.Enable` | Returns `true` | P1 | FUNC | API |
| TC-RFC-003 | Set string RFC value (local/WebPA source) | Set `RFC.SystemServices.FriendlyName = "TestName"` | Returns 0 | P1 | FUNC | API |
| TC-RFC-004 | Get string RFC value after set | Get `RFC.SystemServices.FriendlyName` | Returns `"TestName"` | P1 | FUNC | API |
| TC-RFC-005 | Clear RFC local value via `RFC.ClearParam` | Set `RFC.ClearParam = RFC.SystemServices.FriendlyName` | Returns `fcNoFault` | P1 | FUNC | API |
| TC-RFC-006 | Get RFC value after clear returns error | Get `RFC.SystemServices.FriendlyName` after clear | Returns `fcInternalError` | P1 | FUNC | API |
| TC-RFC-007 | RFC requestor source — HOSTIF_SRC_RFC vs WEBPA | Set same param with both sources; verify precedence | RFC source takes precedence over WEBPA | P2 | FUNC | API |
| TC-RFC-008 | RFC value persists across store re-init | Set value; re-initialize store; get value | Value still present in backing store | P2 | FUNC | API |
| TC-RFC-009 | Set RFC value with maximum length name | Set param with 511-char name | Returns error or truncation handled gracefully | P2 | BND | API |
| TC-RFC-010 | `rbuscli` GET for RFC namespace param | `rbuscli get Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DAB.Enable` | Returns boolean value with no error | P1 | FUNC | CLI |
| TC-RFC-011 | `rbuscli` SET for RFC namespace param | `rbuscli set Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DAB.Enable bool true` | Returns success | P1 | FUNC | CLI |

---

## 5. Bootstrap Store (BSStore)

> **Existing unit tests**: `bsStoreTest.getValueFactoryFresh`, `bsStoreTest.setValue`, `bsStoreTest.verifySetValue`.

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-BS-001 | Get factory default BS value | Call `XBSStore::getValue` for `Device.Time.NTPServer1` before partner ID | Returns default value, ret = 0 | P1 | FUNC | API |
| TC-BS-002 | Partner ID lookup applies correct BS values | Wait for partner ID; get `Device.Time.NTPServer1` | Returns partner-specific value | P1 | FUNC | API |
| TC-BS-003 | Override BS value | Call `XBSStore::overrideValue` for `Device.Time.NTPServer1` with test URL | Returns 0 | P1 | FUNC | API |
| TC-BS-004 | Verify overridden BS value persists | Get `Device.Time.NTPServer1` after override | Returns override URL | P1 | FUNC | API |
| TC-BS-005 | BS journal entry created on override | Check `XBSStoreJournal` after override | Journal entry exists for overridden parameter | P2 | FUNC | API |
| TC-BS-006 | BS value with `bsUpdate=HOSTIF_NONE` | Set BS param with `bsUpdate=HOSTIF_NONE` | Stored without triggering an upstream update | P2 | FUNC | API |

---

## 6. HTTP Server

> **Existing unit tests**: `hostifMainTest.initHostIf` (HTTPServerStartThread tested).

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-HTTP-001 | HTTP server thread starts | Call `HTTPServerStartThread`; verify non-NULL thread handle | Thread handle non-NULL | P1 | FUNC | API |
| TC-HTTP-002 | HTTP GET request for a parameter | `curl http://127.0.0.1:11999` with JSON body `{"names":["Device.Time.NTPServer1"]}` | HTTP 200 response; JSON contains parameter value | P1 | FUNC | CLI |
| TC-HTTP-003 | HTTP SET request for a read-write parameter | POST SET request for `Device.DeviceInfo.ProvisioningCode` | HTTP 200; subsequent GET reflects new value | P1 | FUNC | CLI |
| TC-HTTP-004 | HTTP server handles malformed JSON | POST malformed JSON to server | HTTP 400 or appropriate error; no crash | P2 | NEG | CLI |
| TC-HTTP-005 | HTTP server stop and clean exit | Call `HttpServerStop()`; join thread | Thread exits cleanly within 5 seconds | P1 | FUNC | API |

---

## 7. DeviceInfo — Read-Only Parameters

> GET only. Assert value is non-empty string or valid numeric.

| TC-ID | Parameter | Expected Assertion | Priority | Automation |
|-------|-----------|--------------------|----------|------------|
| TC-DI-RO-001 | `Device.DeviceInfo.Manufacturer` | Non-empty string | P1 | CLI |
| TC-DI-RO-002 | `Device.DeviceInfo.ModelName` | Non-empty string | P1 | CLI |
| TC-DI-RO-003 | `Device.DeviceInfo.SerialNumber` | Non-empty string, alphanumeric format | P1 | CLI |
| TC-DI-RO-004 | `Device.DeviceInfo.HardwareVersion` | Non-empty string | P1 | CLI |
| TC-DI-RO-005 | `Device.DeviceInfo.SoftwareVersion` | Non-empty string, version format (e.g. x.y.z) | P1 | CLI |
| TC-DI-RO-006 | `Device.DeviceInfo.UpTime` | unsignedInt > 0 | P1 | CLI |
| TC-DI-RO-007 | `Device.DeviceInfo.X_COMCAST-COM_STB_MAC` | Match MAC address regex `[0-9A-Fa-f:]{17}` | P1 | CLI |
| TC-DI-RO-008 | `Device.DeviceInfo.X_COMCAST-COM_STB_IP` | Valid IPv4 or IPv6 address | P1 | CLI |
| TC-DI-RO-009 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RDKVersion` | Non-empty string | P1 | CLI |
| TC-DI-RO-010 | `Device.DeviceInfo.X_RDKCENTRAL-COM.BootStatus` | One of: `"active"`, `"starting"` | P1 | CLI |
| TC-DI-RO-011 | `Device.DeviceInfo.X_RDKCENTRAL-COM_BootTime` | unsignedInt; epoch timestamp > 0 | P2 | CLI |
| TC-DI-RO-012 | `Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareFilename` | Non-empty string | P2 | CLI |
| TC-DI-RO-013 | `Device.DeviceInfo.MemoryStatus.Total` | unsignedInt > 0 | P1 | CLI |
| TC-DI-RO-014 | `Device.DeviceInfo.MemoryStatus.Free` | unsignedInt ≥ 0 and ≤ Total | P1 | CLI |
| TC-DI-RO-015 | `Device.DeviceInfo.ProcessStatus.CPUUsage` | unsignedInt in range 0–100 | P1 | CLI |
| TC-DI-RO-016 | `Device.DeviceInfo.ProcessorNumberOfEntries` | unsignedInt ≥ 1 | P2 | CLI |
| TC-DI-RO-017 | `Device.DeviceInfo.X_RDK_FirmwareName` | Non-empty string | P2 | CLI |

---

## 8. DeviceInfo — Read-Write Parameters

> GET + SET. Verify set/get round-trip. Restore original value after test.

| TC-ID | Parameter | Set Value | GET Assert | Priority | Automation |
|-------|-----------|-----------|------------|----------|------------|
| TC-DI-RW-001 | `Device.DeviceInfo.ProvisioningCode` | `"AUTOTEST001"` | Returns `"AUTOTEST001"` | P1 | CLI |
| TC-DI-RW-002 | `Device.DeviceInfo.X_RDKCENTRAL-COM_PreferredGatewayType` | `"ipv4"` | Returns `"ipv4"` | P2 | CLI |
| TC-DI-RW-003 | `Device.DeviceInfo.X_RDKCENTRAL-COM.CPUTemp` | `50` (int) | Returns `50` | P2 | CLI |
| TC-DI-RW-004 | `Device.DeviceInfo.X_RDKCENTRAL-COM_LastRebootReason` | `"AutoTest"` | Returns `"AutoTest"` | P2 | CLI |
| TC-DI-RW-005 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.LimitBeaconDetection` | `true` | Returns `true` | P2 | CLI |
| TC-DI-RW-006 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable` | `false` | Returns `false` | P2 | CLI |
| TC-DI-RW-007 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs` | `"-R 2222:localhost:22 user@host"` | Returns same string | P2 | CLI |
| TC-DI-RW-008 | `Device.DeviceInfo.X_RDK_RDKProfileName` | `"RDKV"` | Returns `"RDKV"` | P2 | CLI |

---

## 9. Firmware Download Parameters

| TC-ID | Test Case | Steps | Expected Result | Priority | Automation |
|-------|-----------|-------|-----------------|----------|------------|
| TC-FW-001 | GET firmware download status | `rbuscli get Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus` | Non-empty string (e.g. `"Idle"`, `"Downloading"`) | P1 | CLI |
| TC-FW-002 | GET firmware download percent | `rbuscli get Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadPercent` | int in range 0–100 | P1 | CLI |
| TC-FW-003 | SET firmware download URL | Set `X_RDKCENTRAL-COM_FirmwareDownloadURL` to test URL | GET returns same URL | P1 | CLI |
| TC-FW-004 | SET firmware download protocol | Set `X_RDKCENTRAL-COM_FirmwareDownloadProtocol` to `"https"` | GET returns `"https"` | P2 | CLI |
| TC-FW-005 | SET firmware `UseCodebig` flag | Set `X_RDKCENTRAL-COM_FirmwareDownloadUseCodebig` to `false` | GET returns `false` | P2 | CLI |
| TC-FW-006 | GET firmware update state | `rbuscli get Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareUpdateState` | Non-empty string | P2 | CLI |
| TC-FW-007 | SET `DeferReboot` flag | Set `X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot` to `true` | GET returns `true` | P2 | CLI |

---

## 10. RFC Feature Flags

> Representative subset covering common feature flag patterns. All follow SET bool / GET bool verification pattern.

| TC-ID | Parameter | Set Value | GET Assert | Priority | Automation |
|-------|-----------|-----------|------------|----------|------------|
| TC-RFCF-001 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.TR069support.Enable` | `true` | Returns `true` | P1 | CLI |
| TC-RFCF-002 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DAB.Enable` | `false` | Returns `false` | P1 | CLI |
| TC-RFCF-003 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LnFUseXPKI.Enable` | `true` | Returns `true` | P2 | CLI |
| TC-RFCF-004 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UNII3.Enable` | `true` | Returns `true` | P2 | CLI |
| TC-RFCF-005 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LoudnessEquivalence.Enable` | `false` | Returns `false` | P2 | CLI |
| TC-RFCF-006 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.Enable` | `true` | Returns `true` | P2 | CLI |
| TC-RFCF-007 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NTP.failoverServer` | `"ntp.test.com"` | Returns `"ntp.test.com"` | P2 | CLI |
| TC-RFCF-008 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.XconfUrl` | `"https://xconf.test.com"` | Returns same URL | P1 | CLI |
| TC-RFCF-009 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.RetrieveNow` | `1` (unsignedInt) | Returns `1` | P1 | CLI |
| TC-RFCF-010 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDB` | `true` | Returns `true` | P2 | CLI |

---

## 11. ManagementServer Parameters

| TC-ID | Test Case | Steps | Expected Result | Priority | Automation |
|-------|-----------|-------|-----------------|----------|------------|
| TC-MS-001 | GET `ConnectionRequestURL` | `rbuscli get Device.ManagementServer.ConnectionRequestURL` | Non-empty URL string | P1 | CLI |
| TC-MS-002 | GET `ParameterKey` | `rbuscli get Device.ManagementServer.ParameterKey` | Returns string (may be empty) | P1 | CLI |
| TC-MS-003 | SET/GET `EnableCWMP` | Set `true`, GET | Returns `true` | P1 | CLI |
| TC-MS-004 | SET/GET `PeriodicInformEnable` | Set `true` | Returns `true` | P1 | CLI |
| TC-MS-005 | SET/GET `PeriodicInformInterval` | Set `600` | Returns `600` | P1 | CLI |
| TC-MS-006 | SET/GET `URL` (ACS URL) | Set `"https://acs.test.com"` | Returns same URL | P1 | CLI |
| TC-MS-007 | SET/GET `Username` | Set `"testuser"` | Returns `"testuser"` | P2 | CLI |
| TC-MS-008 | SET/GET `STUNEnable` | Set `false` | Returns `false` | P2 | CLI |
| TC-MS-009 | SET/GET `STUNServerAddress` | Set `"stun.test.com"` | Returns `"stun.test.com"` | P2 | CLI |
| TC-MS-010 | GET `AliasBasedAddressing` | GET only | Returns boolean value | P2 | CLI |

---

## 12. Time Parameters

| TC-ID | Test Case | Steps | Expected Result | Priority | Automation |
|-------|-----------|-------|-----------------|----------|------------|
| TC-TIME-001 | GET `CurrentLocalTime` | `rbuscli get Device.Time.CurrentLocalTime` | Valid timestamp string | P1 | CLI |
| TC-TIME-002 | SET/GET `NTPServer1` | Set `"pool.ntp.org"` | Returns `"pool.ntp.org"` | P1 | CLI |
| TC-TIME-003 | SET/GET `NTPServer2` | Set `"time.nist.gov"` | Returns `"time.nist.gov"` | P2 | CLI |
| TC-TIME-004 | SET/GET `LocalTimeZone` | Set `"America/New_York"` | Returns `"America/New_York"` | P2 | CLI |
| TC-TIME-005 | SET/GET `Enable` | Set `true` | Returns `true` | P1 | CLI |
| TC-TIME-006 | GET `X_RDK_CurrentUTCTime` | GET only | Non-empty timestamp string | P2 | CLI |
| TC-TIME-007 | SET/GET `ChronyEnable` | Set `true` | Returns `true` | P2 | CLI |
| TC-TIME-008 | SET/GET `NTPMinpoll` boundary — min 1 | Set `1` | Returns `1` | P3 | BND | CLI |

---

## 13. Ethernet Parameters

| TC-ID | Test Case | Steps | Expected Result | Priority | Automation |
|-------|-----------|-------|-----------------|----------|------------|
| TC-ETH-001 | GET `InterfaceNumberOfEntries` | `rbuscli get Device.Ethernet.InterfaceNumberOfEntries` | unsignedInt ≥ 1 | P1 | CLI |
| TC-ETH-002 | GET interface `MACAddress` | `rbuscli get Device.Ethernet.Interface.1.MACAddress` | Valid MAC format | P1 | CLI |
| TC-ETH-003 | GET interface `Status` | GET Device.Ethernet.Interface.1.Status | One of: `Up`, `Down`, `Unknown` | P1 | CLI |
| TC-ETH-004 | GET `Stats.BytesSent` | GET Device.Ethernet.Interface.1.Stats.BytesSent | unsignedLong ≥ 0 | P2 | CLI |
| TC-ETH-005 | GET `Stats.BytesReceived` | GET Device.Ethernet.Interface.1.Stats.BytesReceived | unsignedLong ≥ 0 | P2 | CLI |
| TC-ETH-006 | SET/GET `Enable` | Set `Device.Ethernet.Interface.1.Enable = true` | Returns `true` | P2 | CLI |
| TC-ETH-007 | GET `LinkNumberOfEntries` | GET Device.Ethernet.LinkNumberOfEntries | unsignedInt ≥ 0 | P2 | CLI |

---

## 14. IP Interface Parameters

| TC-ID | Test Case | Steps | Expected Result | Priority | Automation |
|-------|-----------|-------|-----------------|----------|------------|
| TC-IP-001 | GET `IPv4Capable` | `rbuscli get Device.IP.IPv4Capable` | Returns `true` | P1 | CLI |
| TC-IP-002 | GET `IPv4Status` | GET Device.IP.IPv4Status | Non-empty string | P1 | CLI |
| TC-IP-003 | GET `InterfaceNumberOfEntries` | GET Device.IP.InterfaceNumberOfEntries | unsignedInt ≥ 1 | P1 | CLI |
| TC-IP-004 | GET interface `Status` for first interface | GET Device.IP.Interface.1.Status | One of: `Up`, `Down`, `Dormant` | P1 | CLI |
| TC-IP-005 | GET interface `IPv4Address.1.IPAddress` | GET Device.IP.Interface.1.IPv4Address.1.IPAddress | Valid IPv4 format | P1 | CLI |
| TC-IP-006 | GET interface `IPv4Address.1.SubnetMask` | GET Device.IP.Interface.1.IPv4Address.1.SubnetMask | Valid subnet mask format | P1 | CLI |
| TC-IP-007 | GET `ActivePortNumberOfEntries` | GET Device.IP.ActivePortNumberOfEntries | unsignedInt ≥ 0 | P2 | CLI |
| TC-IP-008 | GET `InterfaceStack` entries | GET Device.InterfaceStack.1.HigherLayer | Non-empty string | P2 | CLI |

---

## 15. IP Diagnostics

| TC-ID | Test Case | Steps | Expected Result | Priority | Automation |
|-------|-----------|-------|-----------------|----------|------------|
| TC-DIAG-001 | IPPing — set host and trigger | Set `DiagnosticsState="Requested"`, `Host="127.0.0.1"`, `NumberOfRepetitions=5` | `DiagnosticsState` transitions to `"Complete"` | P1 | CLI |
| TC-DIAG-002 | IPPing — verify SuccessCount | GET `Device.IP.Diagnostics.IPPing.SuccessCount` after ping | Returns 5 (matches repetitions) | P1 | CLI |
| TC-DIAG-003 | IPPing — verify AverageResponseTime | GET `AverageResponseTime` after ping to loopback | Returns unsignedInt > 0 | P1 | CLI |
| TC-DIAG-004 | TraceRoute — set host and trigger | Set `DiagnosticsState="Requested"`, `Host="127.0.0.1"` | `DiagnosticsState` transitions to `"Complete"` | P2 | CLI |
| TC-DIAG-005 | TraceRoute — RouteHopsNumberOfEntries | GET after trace | Returns unsignedInt ≥ 1 | P2 | CLI |
| TC-DIAG-006 | DownloadDiagnostics — state machine | Set `DiagnosticsState="Requested"`, valid `DownloadURL` | Transitions to `"Complete"`; `TestBytesReceived` > 0 | P2 | CLI |
| TC-DIAG-007 | UploadDiagnostics — state machine | Set `DiagnosticsState="Requested"`, valid `UploadURL`, `TestFileLength` | Transitions to `"Complete"`; `TotalBytesSent` > 0 | P2 | CLI |
| TC-DIAG-008 | SpeedTest — enable and run | Set `Enable=true`, `Run=true` | `Status` returns non-error code | P2 | CLI |
| TC-DIAG-009 | UDPEchoConfig — configure and verify | Set `Enable=true`, `UDPPort=5001`, `Interface` | GET shows configured values; no error | P3 | CLI |

---

## 16. WiFi Parameters

| TC-ID | Test Case | Steps | Expected Result | Priority | Automation |
|-------|-----------|-------|-----------------|----------|------------|
| TC-WIFI-001 | GET `RadioNumberOfEntries` | `rbuscli get Device.WiFi.RadioNumberOfEntries` | unsignedInt ≥ 1 | P1 | CLI |
| TC-WIFI-002 | GET `SSIDNumberOfEntries` | GET Device.WiFi.SSIDNumberOfEntries | unsignedInt ≥ 1 | P1 | CLI |
| TC-WIFI-003 | GET Radio `OperatingChannelBandwidth` | GET Device.WiFi.Radio.1.OperatingChannelBandwidth | Non-empty string (e.g. `"20MHz"`, `"80MHz"`) | P1 | CLI |
| TC-WIFI-004 | GET Radio `Stats.Noise` | GET Device.WiFi.Radio.1.Stats.Noise | int value (expected negative, e.g. -50 to -100 dBm) | P2 | CLI |
| TC-WIFI-005 | GET SSID `BSSID` | GET Device.WiFi.SSID.1.BSSID | Valid MAC format | P1 | CLI |
| TC-WIFI-006 | GET SSID `Status` | GET Device.WiFi.SSID.1.Status | Non-empty string | P1 | CLI |
| TC-WIFI-007 | SET/GET AccessPoint `SSIDAdvertisementEnabled` | Set `true` | Returns `true` | P2 | CLI |
| TC-WIFI-008 | GET AccessPoint associated device count | GET Device.WiFi.AccessPoint.1.AssociatedDeviceNumberOfEntries | unsignedInt ≥ 0 | P2 | CLI |
| TC-WIFI-009 | SET/GET `X_RDKCENTRAL-COM_ClientRoaming.Enable` | Set `true` | Returns `true` | P2 | CLI |
| TC-WIFI-010 | SET/GET `X_RDKCENTRAL-COM_ClientRoaming.80211kvrEnable` | Set `false` | Returns `false` | P2 | CLI |

---

## 17. STBService / Services Parameters

| TC-ID | Test Case | Steps | Expected Result | Priority | Automation |
|-------|-----------|-------|-----------------|----------|------------|
| TC-STB-001 | GET HDMI `SupportedResolutions` | `rbuscli get Device.Services.STBService.1.Capabilities.HDMI.SupportedResolutions` | Non-empty string | P1 | CLI |
| TC-STB-002 | GET VideoDecoder `VideoStandards` | GET STBService.1.Capabilities.VideoDecoder.VideoStandards | Non-empty string | P1 | CLI |
| TC-STB-003 | GET AudioOutput `AudioFormat` | GET STBService.1.Components.AudioOutput.1.AudioFormat | Non-empty string | P1 | CLI |
| TC-STB-004 | SET/GET AudioOutput `AudioLevel` | Set `50` (unsignedInt) | Returns `50` | P2 | CLI |
| TC-STB-005 | SET/GET AudioOutput `CancelMute` | Set `false` | Returns `false` | P2 | CLI |
| TC-STB-006 | GET HDMI DisplayDevice `PreferredResolution` | GET HDMI.1.DisplayDevice.PreferredResolution | Non-empty string (e.g. `"1920x1080p60"`) | P1 | CLI |
| TC-STB-007 | GET HDMI DisplayDevice `Status` | GET HDMI.1.DisplayDevice.Status | Non-empty string | P1 | CLI |
| TC-STB-008 | GET eMMC `FirmwareVersion` | GET X_RDKCENTRAL-COM_eMMCFlash.FirmwareVersion | Non-empty string | P2 | CLI |
| TC-STB-009 | GET RF4CE `rf4ceMACAddress` | GET X_RDKCENTRAL-COM_RF4CE.rf4ceMACAddress | MAC format string | P2 | CLI |
| TC-STB-010 | GET MPEGHPart2 `ProfileLevelNumberOfEntries` | GET X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevelNumberOfEntries | unsignedInt ≥ 0 | P2 | CLI |

---

## 18. Notification & Event Subscription

| TC-ID | Test Case | Steps | Expected Result | Priority | Automation |
|-------|-----------|-------|-----------------|----------|------------|
| TC-NOTIF-001 | Notification queue initialization | Check `NotificationHandler::GetNotificationQueue()` returns non-NULL | `GAsyncQueue*` non-NULL | P1 | API |
| TC-NOTIF-002 | Register update callback | Call `registerUpdateCallback(cb)` with test callback | `notifyUpdateCallback` is set | P1 | API |
| TC-NOTIF-003 | Value-change notification for active notification param | Change `SoftwareVersion` (notification=4); observe callback | Callback invoked with correct parameter and value | P1 | API |
| TC-NOTIF-004 | No notification for passive param | Change param with `notification=0`; verify callback not invoked | Callback not called | P2 | API |
| TC-NOTIF-005 | Parodus notification payload — JSON format | Trigger notification; capture callback payload | JSON contains `paramName`, `paramValue`, `dataType` fields | P2 | API |
| TC-NOTIF-006 | Re-subscribe after unsubscribe | Unsubscribe then re-subscribe to a parameter event | Second subscription accepted; notifications resume | P2 | CLI |

---

## 19. WebPA / Parodus Client

| TC-ID | Test Case | Steps | Expected Result | Priority | Automation |
|-------|-----------|-------|-----------------|----------|------------|
| TC-PARODUS-001 | Parodus thread starts without crash | Check `libpd_client_mgr` thread running after startup | Thread exists in process task list | P1 | L2 |
| TC-PARODUS-002 | WebPA server URL readable | GET `Device.X_RDK_WebPA_Server.URL` | Non-empty URL string | P1 | CLI |
| TC-PARODUS-003 | WebPA token server URL readable | GET `Device.X_RDK_WebPA_TokenServer.URL` | Non-empty URL string | P1 | CLI |
| TC-PARODUS-004 | WebPA DNS text URL read-write | SET `Device.X_RDK_WebPA_DNSText.URL`; GET | Returns same URL | P2 | CLI |
| TC-PARODUS-005 | Parodus reconnect on connection loss | Simulate parodus disconnect; verify reconnect attempt in logs | Log shows reconnect attempt within configured interval | P2 | L2 |

---

## 20. xOpsDeviceMgmt Parameters

| TC-ID | Test Case | Steps | Expected Result | Priority | Automation |
|-------|-----------|-------|-----------------|----------|------------|
| TC-XOPS-001 | GET `xOpsDMLogsUploadStatus` | GET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMLogsUploadStatus | String value (e.g. `"Not Started"`, `"Success"`) | P1 | CLI |
| TC-XOPS-002 | GET/SET `ForwardSSH.Enable` | Set `true`; GET | Returns `true` | P1 | CLI |
| TC-XOPS-003 | SET/GET ReverseSSH args | Set `xOpsReverseSshArgs = "-R 2222:localhost:22 user@testhost"` | GET returns same string | P2 | CLI |
| TC-XOPS-004 | GET `xOpsReverseSshStatus` | GET | Non-empty string | P2 | CLI |
| TC-XOPS-005 | GET hwHealthTest `Results` | GET Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.Results | Non-empty JSON string | P2 | CLI |
| TC-XOPS-006 | SET RPC `DeviceManageableNotification` | Set string value | GET returns same string | P2 | CLI |

---

## 21. T2 Telemetry Parameters

| TC-ID | Test Case | Steps | Expected Result | Priority | Automation |
|-------|-----------|-------|-----------------|----------|------------|
| TC-T2-001 | `ReportProfiles` excluded from rbus registration | Attempt `rbuscli get Device.X_RDKCENTRAL-COM_T2.ReportProfiles` | Returns error — parameter not registered via TR_Dml_GetHandler (excluded) | P1 | CLI |
| TC-T2-002 | `ReportProfiles` accessible via direct handler | Set `Device.X_RDKCENTRAL-COM_T2.ReportProfiles` via T2 handler | Value stored in T2 subsystem | P1 | L2 |
| TC-T2-003 | `ReportProfilesMsgPack` excluded from rbus | Attempt `rbuscli get Device.X_RDKCENTRAL-COM_T2.ReportProfilesMsgPack` | Returns error — excluded from rbus DML | P1 | CLI |

---

## 22. Negative / Boundary Tests

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-NEG-001 | GET empty parameter name | Call get handler with `paramName=""` | Returns error; no crash | P1 | NEG | API |
| TC-NEG-002 | SET with NULL value | Call set handler with `paramValue=NULL` | Returns error; no crash | P1 | NEG | API |
| TC-NEG-003 | SET unsignedInt with negative value | SET `PeriodicInformInterval = -1` | Returns type error or clamped to 0 | P2 | NEG | CLI |
| TC-NEG-004 | SET boolean with invalid string | SET boolean param with `"maybe"` | Returns error or maps to `false` | P2 | NEG | CLI |
| TC-NEG-005 | Parameter name at max length (511 chars) | Construct 511-char param name; call GET | Handled gracefully without buffer overflow | P1 | BND | API |
| TC-NEG-006 | Parameter value at max length | SET param with 511-char string value | Truncated or error returned; no crash | P1 | BND | API |
| TC-NEG-007 | SET read-only parameter via rbus | `rbuscli set Device.DeviceInfo.Manufacturer string X` | Returns `RBUS_ERROR_INVALID_INPUT` or equivalent | P1 | NEG | CLI |
| TC-NEG-008 | GET parameter not in DML | `rbuscli get Device.NonExistent.Param` | Returns not-found error; no crash | P2 | NEG | CLI |
| TC-NEG-009 | Daemon handles missing config file at start | Remove `tr69hostIf.conf`; start daemon | Daemon logs error; exits cleanly with non-zero code | P1 | NEG | L2 |
| TC-NEG-010 | Concurrent SET on same parameter | 10 threads simultaneously SET same param | Final value is one of the set values; no corruption | P1 | BND | API |
| TC-NEG-011 | HOSTIF_MsgData_t `paramLen` larger than buffer | Set `paramLen = 9999` (exceeds buffer) | Error returned; no heap overflow | P1 | BND | API |
| TC-NEG-012 | RFC store: set with empty param name | Call `setValue` with `paramName=""` | Returns error; no crash | P2 | NEG | API |

---

## 23. DHCPv4 Client Parameters

> **Source**: `src/hostif/profiles/DHCPv4/Device_DHCPv4_Client.cpp/h`
> **Build flag**: `USE_DHCPv4_PROFILE`
> **Existing unit tests**: gtest hooks for `isValidIPAddr`, `getInterfaceName`, `isIfnameInroutetoDNSServer`

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-DHCP-001 | GET `ClientNumberOfEntries` | `rbuscli get Device.DHCPv4.ClientNumberOfEntries` | unsignedInt ≥ 0 | P1 | FUNC | CLI |
| TC-DHCP-002 | GET `Client.1.InterfaceReference` | `rbuscli get Device.DHCPv4.Client.1.InterfaceReference` | Non-empty path string (e.g. `"Device.IP.Interface.1"`) | P1 | FUNC | CLI |
| TC-DHCP-003 | GET `Client.1.DnsServer` | `rbuscli get Device.DHCPv4.Client.1.DnsServer` | Comma-separated valid IPv4 addresses | P1 | FUNC | CLI |
| TC-DHCP-004 | GET `Client.1.IPRouters` | `rbuscli get Device.DHCPv4.Client.1.IPRouters` | Valid IPv4 gateway address | P1 | FUNC | CLI |
| TC-DHCP-005 | Unit: `isValidIPAddr` with valid IPv4 | Call `isValidIPAddr("192.168.1.1")` | Returns `true` | P1 | FUNC | API |
| TC-DHCP-006 | Unit: `isValidIPAddr` with invalid IP (alpha) | Call `isValidIPAddr("abc.def.ghi.jkl")` | Returns `false` | P1 | NEG | API |
| TC-DHCP-007 | Unit: `getInterfaceName` for active interface | Call `getInterfaceName(ifIndex)` for known index | Returns non-empty interface name string | P2 | FUNC | API |
| TC-DHCP-008 | Unit: `isIfnameInroutetoDNSServer` for valid DNS route | Call with interface name that has a DNS route | Returns `true` | P2 | FUNC | API |

---

## 24. MoCA Interface Parameters

> **Source**: `src/hostif/profiles/moca/Device_MoCA_Interface*.cpp/h`
> **Build flag**: `USE_MoCA_PROFILE`
> **IPC**: Uses RMH (RDK MoCA HAL) context via `MoCADevice::getRmhContext()`

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-MOCA-001 | GET `InterfaceNumberOfEntries` | `rbuscli get Device.MoCA.InterfaceNumberOfEntries` | unsignedInt ≥ 0 | P1 | FUNC | CLI |
| TC-MOCA-002 | GET Interface `Enable` status | `rbuscli get Device.MoCA.Interface.1.Enable` | Returns boolean | P1 | FUNC | CLI |
| TC-MOCA-003 | GET Interface `Status` | `rbuscli get Device.MoCA.Interface.1.Status` | One of: `Up`, `Down`, `Unknown`, `Dormant` | P1 | FUNC | CLI |
| TC-MOCA-004 | GET Interface `MACAddress` | `rbuscli get Device.MoCA.Interface.1.MACAddress` | Valid MAC format `XX:XX:XX:XX:XX:XX` | P1 | FUNC | CLI |
| TC-MOCA-005 | GET Interface `CurrentOperFreq` | `rbuscli get Device.MoCA.Interface.1.CurrentOperFreq` | unsignedInt > 0 (e.g. MHz range 1150–1675) | P2 | FUNC | CLI |
| TC-MOCA-006 | GET `AssociatedDeviceNumberOfEntries` | `rbuscli get Device.MoCA.Interface.1.AssociatedDeviceNumberOfEntries` | unsignedInt ≥ 0 | P1 | FUNC | CLI |
| TC-MOCA-007 | GET Stats `BytesSent` | `rbuscli get Device.MoCA.Interface.1.Stats.BytesSent` | unsignedLong ≥ 0 | P2 | FUNC | CLI |
| TC-MOCA-008 | GET Stats `BytesReceived` | `rbuscli get Device.MoCA.Interface.1.Stats.BytesReceived` | unsignedLong ≥ 0 | P2 | FUNC | CLI |
| TC-MOCA-009 | GET QoS `EgressNumFlows` | `rbuscli get Device.MoCA.Interface.1.QoS.EgressNumFlows` | unsignedInt ≥ 0 | P2 | FUNC | CLI |
| TC-MOCA-010 | GET MeshTable entries | `rbuscli get Device.MoCA.Interface.1.X_RDKCENTRAL-COM_MeshTableNumberOfEntries` | unsignedInt ≥ 0 | P2 | FUNC | CLI |

---

## 25. StorageService Parameters

> **Source**: `src/hostif/profiles/StorageService/Service_Storage.cpp/h`, `Service_Storage_PhyMedium.cpp/h`
> **Build flag**: `USE_STORAGESERVICE_PROFILE`

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-STOR-001 | GET `StorageServiceNumberOfEntries` | `rbuscli get Device.StorageServiceNumberOfEntries` | unsignedInt ≥ 1 | P1 | FUNC | CLI |
| TC-STOR-002 | GET `StorageService.1.StorageArrayNumberOfEntries` | `rbuscli get Device.StorageService.1.StorageArrayNumberOfEntries` | unsignedInt ≥ 0 | P1 | FUNC | CLI |
| TC-STOR-003 | GET physical medium capacity | `rbuscli get Device.StorageService.1.PhysicalMedium.1.Capacity` | unsignedInt > 0 (in MB) | P1 | FUNC | CLI |
| TC-STOR-004 | GET physical medium `Health` status | `rbuscli get Device.StorageService.1.PhysicalMedium.1.Health` | Non-empty string (e.g. `"OK"`, `"Error"`) | P1 | FUNC | CLI |
| TC-STOR-005 | GET physical medium `Vendor` | `rbuscli get Device.StorageService.1.PhysicalMedium.1.Vendor` | Non-empty string | P2 | FUNC | CLI |
| TC-STOR-006 | GET `PhysicalMediumNumberOfEntries` | `rbuscli get Device.StorageService.1.PhysicalMediumNumberOfEntries` | unsignedInt ≥ 1 | P2 | FUNC | CLI |

---

## 26. SNMP Adapter

> **Source**: `src/hostif/snmpAdapter/snmpAdapter.cpp/h`
> **Build flag**: `SNMP_ADAPTER_ENABLED`
> **Config**: `/etc/tr181_snmpOID.conf` maps TR-181 parameters to SNMP OIDs
> **SNMP targets**: CM at `192.168.100.1`, STB at `127.0.0.1`, community `hDaFHJG7`

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-SNMP-001 | OID config file loads successfully | Start daemon; check log for SNMP OID map loaded | `/etc/tr181_snmpOID.conf` parsed; map populated with ≥ 1 entry | P1 | FUNC | L2 |
| TC-SNMP-002 | TR-181 param mapped to SNMP OID | Call SNMP GET for a TR-181 param that has OID mapping | OID queried on correct SNMP target (CM or STB); value returned | P1 | FUNC | L2 |
| TC-SNMP-003 | TR-181 param not in OID map returns error | GET param with no OID mapping via SNMP adapter | Returns error code; no crash | P2 | NEG | L2 |
| TC-SNMP-004 | SNMP CM target unreachable | Simulate CM (192.168.100.1) down; GET CM-mapped param | Returns timeout error code; no crash | P2 | NEG | L2 |
| TC-SNMP-005 | SNMP OID config file missing | Remove `/etc/tr181_snmpOID.conf`; start daemon | Daemon logs warning; SNMP adapter gracefully disabled | P2 | NEG | L2 |

---

## 27. XRFCVarStore — RFC Variable Cache

> **Source**: `src/hostif/httpserver/src/XrdkCentralComRFCVar.cpp`
> **Config**: `/etc/rfc.properties` contains `RFC_VAR_FILENAME` key pointing to RFC variable file

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-RFCV-001 | RFC variable file loaded at startup | Start daemon; check RFC var cache initialized | `m_dict` populated from file specified in `/etc/rfc.properties` | P1 | FUNC | L2 |
| TC-RFCV-002 | `getValue` returns cached RFC variable | Call `XRFCVarStore::getValue("RFC_ENABLE_SOMEFEATURE")` | Returns expected string value from RFC var file | P1 | FUNC | API |
| TC-RFCV-003 | `getValue` returns empty for unknown key | Call `XRFCVarStore::getValue("NON_EXISTENT_KEY")` | Returns empty string or `nullptr`; no crash | P1 | NEG | API |
| TC-RFCV-004 | `reloadCache()` picks up updated file | Edit RFC var file; call `reloadCache()`; call `getValue` | Returns newly updated value | P2 | FUNC | API |
| TC-RFCV-005 | Missing `/etc/rfc.properties` handled | Remove file; restart daemon | Daemon logs error; RFC var cache empty; no crash | P2 | NEG | L2 |
| TC-RFCV-006 | `rfc.properties` missing `RFC_VAR_FILENAME` key | File exists but without the key; restart daemon | RFC var cache empty; daemon continues without crash | P2 | NEG | L2 |

---

## 28. WebConfig Parameters

> **Source**: `src/hostif/profiles/Device/x_rdk_profile.cpp/h`
> **Key params**: `Device.X_RDK_WebConfig.URL`, `Device.X_RDK_WebConfig.SupplementaryServiceUrls.Telemetry`, `Device.X_RDK_WebConfig.ForceSync`

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-WCFG-001 | GET `WebConfig.URL` | `rbuscli get Device.X_RDK_WebConfig.URL` | Non-empty URL string | P1 | FUNC | CLI |
| TC-WCFG-002 | SET/GET `WebConfig.URL` | Set to `"https://webconfig.test.com/api"` | GET returns same URL | P1 | FUNC | CLI |
| TC-WCFG-003 | GET `SupplementaryServiceUrls.Telemetry` | `rbuscli get Device.X_RDK_WebConfig.SupplementaryServiceUrls.Telemetry` | Non-empty URL string | P2 | FUNC | CLI |
| TC-WCFG-004 | GET WebPA Server URL | `rbuscli get Device.X_RDK_WebPA_Server.URL` | Non-empty URL string | P1 | FUNC | CLI |
| TC-WCFG-005 | GET WebPA TokenServer URL | `rbuscli get Device.X_RDK_WebPA_TokenServer.URL` | Non-empty URL string | P1 | FUNC | CLI |
| TC-WCFG-006 | SET/GET WebPA DNSText URL | Set `Device.X_RDK_WebPA_DNSText.URL = "https://dns.test.com"` | GET returns same URL | P2 | FUNC | CLI |
| TC-WCFG-007 | SET `WebConfig.ForceSync` triggers re-sync | Set `Device.X_RDK_WebConfig.ForceSync = "testDoc"` | Returns success; daemon logs webconfig sync initiated | P1 | FUNC | L2 |

---

## 29. WebPA Attributes API

> **Source**: `src/hostif/parodusClient/pal/webpa_attribute.cpp`
> **Routing**: `getAttributes()` → `HOSTIF_GETATTRIB`; `setAttributes()` → `HOSTIF_SETATTRIB`

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-WATT-001 | `getAttributes` for single parameter | Call `getAttributes(["Device.DeviceInfo.SoftwareVersion"], 1)` | Returns attribute struct with `notify` field populated | P1 | FUNC | API |
| TC-WATT-002 | `getAttributes` for batch of parameters | Call with array of 5 parameter names | Returns 5 attribute results; no partial failure | P1 | FUNC | API |
| TC-WATT-003 | `setAttributes` enable notification | Call `setAttributes` with `notify=1` on a watched param | Attribute persisted; subsequent `getAttributes` returns `notify=1` | P1 | FUNC | API |
| TC-WATT-004 | `setAttributes` disable notification | Call `setAttributes` with `notify=0` on previously enabled param | Attribute updated; `getAttributes` returns `notify=0` | P2 | FUNC | API |
| TC-WATT-005 | `getAttributes` for non-existent parameter | Call with unknown parameter name | Returns `WDMP_ERR_VALUE_IS_EMPTY` or equivalent; no crash | P2 | NEG | API |

---

## 30. Message Handler Diagnostics

> **Source**: `src/hostif/handlers/src/hostIf_msgHandler.cpp`
> **Key metrics**: `getCountSinceBoot`, `setCountSinceBoot` (atomic); rate-limit flags at 200 calls/60s and 1000 calls/300s

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-MSGH-001 | GET counter increments per request | Make 10 successive GET calls; check log for counter | Log shows `GET called N times` every 10th call (modulo 10 logging) | P2 | FUNC | L2 |
| TC-MSGH-002 | Rate-limit warning at 200 GETs within 60s | Send 200 GET requests within 60 seconds of boot | Log contains rate-limit warning for `loggedGet200Within1Min` | P2 | FUNC | L2 |
| TC-MSGH-003 | `paramValueToString` handles all data types | Call GET for int, bool, unsignedInt, string, unsignedLong params | All return non-empty string; no type-handling gap | P1 | FUNC | API |
| TC-MSGH-004 | GET and SET mutexes prevent data races | Concurrent 20-thread GET + SET on same param | No deadlock; all operations return; no garbled values | P1 | BND | API |

---

## 31. Update Handler Lifecycle

> **Source**: `src/hostif/handlers/src/hostIf_updateHandler.cpp`
> **Key APIs**: `updateHandler::Init()`, `stop()`, `reset()`; background thread via `g_thread_new`

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-UPDH-001 | `updateHandler::Init()` registers all module callbacks | Start daemon; check update handler initialized in logs | Callbacks registered for MoCA, Ethernet, IP, Device, WiFi, DHCPv4, InterfaceStack, XRE, BT | P1 | FUNC | L2 |
| TC-UPDH-002 | Update thread starts via `g_thread_new` | Start daemon; verify update thread in task list | Thread named `updateHandler` running | P1 | FUNC | L2 |
| TC-UPDH-003 | `stop()` terminates update thread cleanly | Call `updateHandler::stop()`; join thread | Thread exits within 5 seconds; no crash | P1 | FUNC | API |
| TC-UPDH-004 | `reset()` clears pending notifications | Trigger several value changes; call `reset()` | Pending notification queue cleared; handler restarts cleanly | P2 | FUNC | API |

---

## 32. STBService Extended (SPDIF / VideoOutput / SDCard)

> **Source**: `src/hostif/profiles/STBService/Components_SPDIF.cpp/h`, `Components_VideoOutput.cpp/h`, `Components_XrdkSDCard.cpp/h`
> **Note**: SDCard params sourced from `/proc` or device-specific HAL at runtime

| TC-ID | Test Case | Steps | Expected Result | Priority | Type | Automation |
|-------|-----------|-------|-----------------|----------|------|------------|
| TC-STB2-001 | GET SPDIF `Enable` | `rbuscli get Device.Services.STBService.1.Components.SPDIF.1.Enable` | Returns boolean | P1 | FUNC | CLI |
| TC-STB2-002 | GET SPDIF `ForcePCM` | `rbuscli get Device.Services.STBService.1.Components.SPDIF.1.ForcePCM` | Returns boolean | P2 | FUNC | CLI |
| TC-STB2-003 | GET SPDIF `AudioDelay` | `rbuscli get Device.Services.STBService.1.Components.SPDIF.1.AudioDelay` | unsignedInt ≥ 0 (ms) | P2 | FUNC | CLI |
| TC-STB2-004 | GET VideoOutput `Status` | `rbuscli get Device.Services.STBService.1.Components.VideoOutput.1.Status` | Non-empty string (e.g. `"Enabled"`) | P1 | FUNC | CLI |
| TC-STB2-005 | GET VideoOutput `VideoFormat` | `rbuscli get Device.Services.STBService.1.Components.VideoOutput.1.VideoFormat` | Non-empty string (e.g. `"HDMI"`, `"Component"`) | P2 | FUNC | CLI |
| TC-STB2-006 | GET SDCard `Capacity` | `rbuscli get Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_SDCard.Capacity` | unsignedInt > 0 (bytes) | P1 | FUNC | CLI |
| TC-STB2-007 | GET SDCard `Status` | `rbuscli get Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_SDCard.Status` | Non-empty string (e.g. `"Present"`, `"Absent"`) | P1 | FUNC | CLI |

---

## Coverage Matrix

| Module | TC Count | Existing (PDF) | New (this doc) |
|--------|----------|----------------|----------------|
| Initialization & Lifecycle | 6 | 2 | 4 |
| Data Model Loading | 6 | 1 | 5 |
| RBus DML Provider | 11 | 2 | 9 |
| RFC Store | 11 | 4 (unit) | 7 |
| Bootstrap Store | 6 | 3 (unit) | 3 |
| HTTP Server | 5 | 1 (unit) | 4 |
| DeviceInfo RO Params | 17 | subset | 17 |
| DeviceInfo RW Params | 8 | subset | 8 |
| Firmware Download | 7 | 0 | 7 |
| RFC Feature Flags | 10 | 0 | 10 |
| ManagementServer | 10 | 0 | 10 |
| Time | 8 | 0 | 8 |
| Ethernet | 7 | 0 | 7 |
| IP Interface | 8 | 0 | 8 |
| IP Diagnostics | 9 | 0 | 9 |
| WiFi | 10 | 0 | 10 |
| STBService | 10 | 0 | 10 |
| Notifications | 6 | 0 | 6 |
| WebPA/Parodus | 5 | 0 | 5 |
| xOpsDeviceMgmt | 6 | 0 | 6 |
| T2 Telemetry | 3 | 0 | 3 |
| Negative/Boundary | 12 | 0 | 12 |
| DHCPv4 Client | 8 | 0 | 8 |
| MoCA Interface | 10 | 0 | 10 |
| StorageService | 6 | 0 | 6 |
| SNMP Adapter | 5 | 0 | 5 |
| XRFCVarStore (RFC Var Cache) | 6 | 0 | 6 |
| WebConfig Parameters | 7 | 0 | 7 |
| WebPA Attributes API | 5 | 0 | 5 |
| Message Handler Diagnostics | 4 | 0 | 4 |
| Update Handler Lifecycle | 4 | 0 | 4 |
| STBService Extended (SPDIF/VideoOutput/SDCard) | 7 | 0 | 7 |
| **TOTAL** | **243** | **~13** | **~230** |

---

## See Also

- [dml_parameter_list.md](dml_parameter_list.md) — Full parameter list with access types and data types
- [docs/integration/testing.md](../integration/testing.md) — Build and run instructions for L2 tests
- [run_l2.sh](../../run_l2.sh) — L2 test runner script
- [run_ut.sh](../../run_ut.sh) — Unit test runner script
- [src/hostif/profiles/DeviceInfo/gtest/gtest_main.cpp](../../src/hostif/profiles/DeviceInfo/gtest/gtest_main.cpp) — Existing unit tests reference
