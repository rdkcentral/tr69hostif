# L2 BDD Feature Files — Source Code Analysis

This document indexes all BDD (Behavior-Driven Development) feature files generated from
source code analysis of the tr69hostif daemon. These features define L2 test specifications
for profiles and subsystems that currently lack automated integration tests.

## Overview

| # | Feature File | Scope | Scenarios | Source Profile |
|---|---|---|---|---|
| 1 | [tr69hostif_bootup_sequence.feature](tr69hostif_bootup_sequence.feature) | Daemon lifecycle and initialization | 18 | hostIf_main.cpp |
| 2 | [tr69hostif_handlers_communications.feature](tr69hostif_handlers_communications.feature) | Request handler dispatch and rbus registration | 22 | hostIf_tr69ReqHandler, handlers/ |
| 3 | [tr69hostif_deviceip.feature](tr69hostif_deviceip.feature) | Device.IP.* parameters | 17 | profiles/IP/ |
| 4 | [tr69hostif_webpa.feature](tr69hostif_webpa.feature) | WebPA/Parodus communication layer | 16 | parodusClient/ |
| 5 | [tr69hostif_ethernet.feature](tr69hostif_ethernet.feature) | Device.Ethernet.* parameters (sysfs-backed) | 25+ | profiles/Ethernet/ |
| 6 | [tr69hostif_thunder_plugins.feature](tr69hostif_thunder_plugins.feature) | All Thunder JSON-RPC backed parameters | 21 | profiles/wifi/, profiles/DeviceInfo/ |
| 7 | [tr69hostif_http_server.feature](tr69hostif_http_server.feature) | HTTP/WDMP-C server protocol | 14 | httpserver/ |
| 8 | [tr69hostif_time_chrony.feature](tr69hostif_time_chrony.feature) | Device.Time.* and Chrony NTP parameters | 30+ | profiles/Time/ |
| 9 | [tr69hostif_negative_tests.feature](tr69hostif_negative_tests.feature) | Error handling and edge cases | 25+ | Cross-cutting |

**Total scenarios: ~190+**

---

## Feature File Categories

### Pre-existing (from L2 test implementation analysis)

These were created by analyzing existing pytest L2 tests:

1. **Bootup Sequence** — Daemon startup, rbus registration, ready-file signaling
2. **Handlers Communications** — DML dispatch, profile routing, rbus provider model
3. **Device IP** — IP address, interface, IPv4/IPv6 parameter handlers
4. **WebPA** — Parodus client integration, CRUD operations, notification events

### Newly Generated (from source code analysis — no existing L2 tests)

These were derived from reading source code for profiles that have **no automated L2 tests**:

5. **Ethernet** — sysfs-backed interface parameters (`/sys/class/net/`), Stats counters
6. **Thunder Plugins** — WiFi SSID/EndPoint, AuthService, Account, MigrationPreparer, UserSettings
7. **HTTP Server** — libsoup server, WDMP-C JSON protocol, RFC variable store, error codes
8. **Time / Chrony** — libc time, file-backed chrony NTP configuration (`/opt/secure/RFC/chrony/`)
9. **Negative Tests** — Invalid params, type mismatches, plugin unavailability, permission errors

---

## Backing Data Sources

| Data Source Type | Feature Files | Example Parameters |
|---|---|---|
| **sysfs** (`/sys/class/net/`) | Ethernet | BytesSent, MACAddress, MaxBitRate |
| **libc** (time/network) | Time/Chrony | CurrentLocalTime, LocalTimeZone |
| **File-backed RFC** (`/opt/secure/RFC/`) | Time/Chrony | Chrony.Enable, NTPMinpoll |
| **Thunder JSON-RPC** (localhost:9998) | Thunder Plugins | WiFi SSID, Experience, STB_IP |
| **Runtime files** (`/tmp/`) | Bootup, Time | ntp_status, .tr69hostif_http_server_ready |
| **HTTP/WDMP-C** (port 11999) | HTTP Server | All params via REST interface |
| **rbus DML** | All | Primary access path for all params |

---

## Test Gap Summary

### Profiles WITH L2 Tests
- Device (partial)
- DeviceInfo (partial — only Thunder-backed subset via Automatics)
- IP

### Profiles WITHOUT L2 Tests (covered by new feature files)
- **Ethernet** — 25+ scenarios covering all Interface.{i}.* and Stats.*
- **WiFi** — 12 scenarios (conditional build; requires Thunder mock)
- **Time** — 30+ scenarios covering standard TR-181 + Chrony extensions
- **HTTP Server** — 14 scenarios covering GET/POST/error flows
- **STBService** — Not yet covered (complex, ~87 handlers)
- **StorageService** — Not yet covered (conditional build)
- **InterfaceStack** — Not yet covered (conditional build)
- **moca** — Not yet covered (conditional build)
- **DHCPv4** — Not yet covered (conditional build)

### Priority for L2 Test Implementation

| Priority | Profile | Reason |
|---|---|---|
| **P1** | HTTP Server | Core communication path; protocol validation critical |
| **P1** | Time/Chrony | File-backed; easy to test in container with mock files |
| **P1** | Ethernet | sysfs-backed; testable with network namespaces |
| **P2** | Thunder Plugins | Requires Thunder mock/stub framework |
| **P2** | Negative Tests | Cross-cutting; validates error resilience |
| **P3** | STBService | Large surface area; needs dedicated effort |
| **P3** | StorageService | Conditional build; hardware-dependent |

---

## How to Use These Feature Files

1. **As L2 test specifications** — Each scenario maps to a pytest test case
2. **As documentation** — Handler inventory tables document all parameters and backing sources
3. **For gap tracking** — Compare against actual test implementation to track coverage
4. **For code review** — Stub parameters (NOK) indicate incomplete implementations

## Related Documents

- [L2 Test Coverage](../../docs/L2_Test_Coverage.md)
- [Automatics Thunder Plugin Gap Analysis](../automatics/Automatics_Thunder_Plugin_Test_Gap_Analysis.md)
- [Testing Integration Guide](../../docs/integration/testing.md)
