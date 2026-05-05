# tr69hostif Documentation

This directory contains implementation-oriented documentation for the tr69hostif daemon. The goal is to keep architecture, API, build, and test information close to the source tree and grounded in the current codebase.

## Documentation Index

### Architecture

- [System Overview](architecture/overview.md) describes the daemon's major components, startup sequence, and runtime boundaries.
- [JSON Usage](architecture/json-usage.md) maps the module's JSON request, config, notification, and JSON-RPC paths and records the current robustness gaps.
- [Threading Model](architecture/threading-model.md) documents worker threads, synchronization primitives, and shutdown behavior.
- [Data Flow](architecture/data-flow.md) traces request routing, event propagation, and RFC/bootstrap precedence.

### API

- [Public API](api/public-api.md) documents the shared request envelope, dispatcher entry points, IARM-facing APIs, and event contracts.

### Integration

- [Build Setup](integration/build-setup.md) covers the autotools workflow, feature flags, runtime prerequisites, and deployment notes.
- [Testing](integration/testing.md) covers the repo's unit-test and L2 test flows, including coverage generation.

### Troubleshooting

- [Common Errors](troubleshooting/common-errors.md) summarizes the most common startup, routing, and Parodus integration failures.

## Scope

The pages in this directory are intentionally implementation-specific. They reference the current source layout under `src/hostif/`, the shipped config files under `conf/`, and the repo-maintained validation scripts such as `run_ut.sh` and `run_l2.sh`.

## Dependent Components

The `tr69hostif` module depends on a mix of middleware services, platform-facing components, parser and transport libraries, and runtime data sources. The list below is intended as a module-wide dependency map for readers navigating the rest of the documentation.

### Core Middleware And IPC Dependencies

| Component | Role in tr69hostif |
|-----------|--------------------|
| IARM Bus (`libIARMBus`) | Primary local IPC path for TR-181 get/set requests, notifications, and manager integration |
| RBUS (`librbus`) | Optional data-model provider path and fallback routing for selected parameter flows |
| Parodus (`libparodus`) | WebPA message transport, request ingress, and outbound notification delivery |
| WRP-C (`libwrp-c`) | WebPA and Parodus message envelope handling |
| WDMP-C (`libwdmp-c`) | WDMP request parsing and response formatting for the newer HTTP JSON flow |

### Core Runtime Libraries

| Component | Role in tr69hostif |
|-----------|--------------------|
| GLib / GThread | Main loop integration, worker threads, async queues, and utility containers |
| libsoup 3 | Local HTTP server implementation for the current JSON request path |
| YAJL | Legacy local JSON request parsing in the older handler thread |
| cJSON | JSON parsing and serialization for HTTP, Parodus, config files, notifications, and JSON-RPC consumers |
| libcurl | Thunder JSON-RPC requests and other HTTP-based helper flows |
| libtinyxml2 | Data-model and XML-related helper parsing used by the module build and runtime flows |
| libsecure_wrapper | Safe file, process, and string helper wrappers used across the daemon |

### External Services And Platform Components

| Component | Role in tr69hostif |
|-----------|--------------------|
| ACS / CWMP stack | Remote management plane that reaches the daemon through local IPC paths |
| WebPA gateway | Remote management plane that uses the Parodus integration path |
| Parodus daemon | Local broker service required for WebPA request and notification exchange |
| Device Settings / DS HAL | Backing implementation for `STBService` and selected device state queries |
| WiFi HAL / WiFi manager | Backing implementation for the `Device.WiFi.*` profile |
| MoCA HAL | Backing implementation for the `Device.MoCA.*` profile when enabled |
| SNMP stack | Used by the SNMP adapter to expose mapped TR-181 values through OIDs |
| systemd notify | Optional readiness signaling for service startup integration |

### Thunder JSON-RPC Runtime Dependencies

The JSON-RPC helper in `tr69hostif` posts to the local Thunder endpoint at `http://127.0.0.1:9998/jsonrpc` and expects specific plugin callsigns and methods to be available at runtime. These are not link-time dependencies of the daemon binary, but they are runtime service dependencies for JSON-RPC-backed parameter retrieval.

The most direct runtime repository dependency in this path is `rdkcentral/networkmanager`, because `tr69hostif` actively calls the `org.rdk.NetworkManager` Thunder plugin for interface enumeration, primary-interface selection, IP settings lookup, interface enable or disable flows, and connected-SSID retrieval.

| JSON-RPC plugin or API family | Used by tr69hostif for | Repository relationship |
|--------------------------------|------------------------|-------------------------|
| `org.rdk.NetworkManager` | `GetPrimaryInterface`, `GetIPSettings`, `GetAvailableInterfaces`, `EnableInterface`, `DisableInterface`, `GetConnectedSSID` | Primary runtime implementation is provided by the `rdkcentral/networkmanager` Thunder plugin repository, which exposes `org.rdk.NetworkManager` over JSON-RPC and COM-RPC |
| `org.rdk.System` | system-level data such as privacy mode and other device state helpers | API contract is documented in `rdkcentral/entservices-apis`; the deployed runtime plugin is provided through the SystemServices plugin line, currently tracked in `rdkcentral/entservices-systemservices` |
| `org.rdk.Account` | account-related values such as checkout reset time | API contract is documented in `rdkcentral/entservices-apis`; device images must include the corresponding Thunder plugin implementation |
| `org.rdk.AuthService` | service account and experience-related data | API contract is documented in `rdkcentral/entservices-apis`; device images must include the corresponding Thunder plugin implementation |
| `org.rdk.MigrationPreparer` | migration readiness and component-readiness state | device images must include the corresponding Thunder plugin implementation for migration-preparer flows |
| `rdkcentral/entservices-apis` | source of Ent Services API definitions used by the Thunder JSON-RPC ecosystem | API-definition repository, not by itself a runtime plugin implementation |

In practice, the JSON-RPC path depends on two layers being present on the device image:

- the Thunder or WPEFramework service host
- the specific plugin implementations that back the callsigns used by `tr69hostif`

For the current source tree, the known JSON-RPC method usage is concentrated in DeviceInfo and WiFi profile code and includes these method families:

- `org.rdk.NetworkManager.*`
- `org.rdk.System.*`
- `org.rdk.Account.*`
- `org.rdk.AuthService.*`
- `org.rdk.MigrationPreparer.*`

For `org.rdk.NetworkManager.*`, the dependency should be read as a concrete runtime dependency on the `rdkcentral/networkmanager` plugin repository rather than only as an Ent Services API reference. That repository owns the Thunder method surface used by `tr69hostif`, including `GetAvailableInterfaces`, `GetPrimaryInterface`, `GetIPSettings`, `SetInterfaceState`, and `GetConnectedSSID`.

When these plugins are absent, disabled, renamed, or version-mismatched, the affected `tr69hostif` parameters fall back to empty or failed JSON-RPC responses.

### Runtime Data And Configuration Dependencies

| Component | Role in tr69hostif |
|-----------|--------------------|
| `mgrlist.conf` / `tr69hostIf.conf` | Manager-to-parameter routing map and runtime defaults |
| TR-181 data-model XML files | Source model fragments merged into the runtime data model |
| `partners_defaults.json` | Partner-specific bootstrap defaults consumed by the bootstrap store |
| `/opt/secure/RFC/` store | RFC override and bootstrap persistence area |
| `webpa_cfg.json` | WebPA and Parodus runtime configuration |
| `notify_webpa_cfg.json` | Initial WebPA notification subscription list |
| `tr181_snmpOID.conf` | SNMP OID to TR-181 mapping for the SNMP adapter |

### Feature-Scoped Internal Components

| Internal component | Dependency focus |
|--------------------|------------------|
| Request handlers under `src/hostif/handlers/` | IARM, RBUS, common dispatcher, notification routing |
| Profiles under `src/hostif/profiles/` | Platform HALs, sysfs, process utilities, JSON-RPC helpers, bootstrap stores |
| Parodus client under `src/hostif/parodusClient/` | Parodus, WRP-C, WDMP-C, notification config, data-model lookup |
| HTTP server under `src/hostif/httpserver/` | libsoup 3, WDMP-C, cJSON, WAL DB support |
| SNMP adapter under `src/hostif/snmpAdapter/` | SNMP OID map, local TR-181 parameter access, GLib threading support |

For build-time package expectations and runtime file prerequisites, see [Build Setup](integration/build-setup.md). For component-specific dependency details, use the documentation under `src/hostif/**/docs/`.

## Maintenance Rules

- Update the relevant page when thread ownership, feature flags, or request routing changes.
- Keep Mermaid diagrams synchronized with the current code paths.
- Prefer linking to source files and config files already present in the repository instead of copying large code blocks into docs.