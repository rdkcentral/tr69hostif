## Context

tr69hostif is the TR-069 host interface daemon for RDK Video devices. It exposes ~2000+ TR-181 parameters via four transport adapters (IARM, WebPA/Parodus, JSON/HTTP, RBUS DML) that converge on a shared `HOSTIF_MsgData_t` dispatch contract. The RBUS DML Provider (`hostIf_rbus_Dml_Provider.cpp`) already registers every parameter from the waldb XML data model with typed get/set handlers and routes them through the core `hostIf_GetMsgHandler()` / `hostIf_SetMsgHandler()` dispatch.

OB-USP-Agent (obuspa) v11.0 is the Broadband Forum's open-source USP reference implementation. It supports five MTPs (MQTT, WebSocket, CoAP, STOMP, UDS), Protobuf encoding, multi-controller RBAC, USP Broker/Service patterns, and a grouped vendor hook API designed for integrating external data model providers. obuspa is already integrated in RDK-B.

The integration target is RDK Video (STB/OTT) devices where tr69hostif runs today.

## Goals / Non-Goals

**Goals:**
- Enable USP controllers to read and write all TR-181 parameters currently served by tr69hostif
- Run in dual-stack mode: TR-069/CWMP and USP operate simultaneously without interference
- Use RBUS as the IPC bridge between obuspa and tr69hostif (aligned with RDK's IPC direction)
- Map USP ControllerTrust roles to tr69hostif's bootstrap store access control semantics
- Forward value-change events from tr69hostif to USP subscriptions in real time
- Package obuspa as a configurable, build-time-optional systemd service for RDK-V

**Non-Goals:**
- Replacing tr69hostif with obuspa — both daemons coexist
- Modifying the WebPA/Parodus path — it remains a separate module
- Implementing USP controller-side infrastructure — that's ISP-side work
- Supporting USP Broker mode for local on-device services (future phase)
- Migrating away from TR-069 — dual-stack is the target, not deprecation

## Decisions

### Decision 1: obuspa as a Separate Daemon, Not Embedded in tr69hostif

**Choice**: Run obuspa as its own systemd service alongside tr69hostif.

**Alternatives considered**:
- *Embed USP protocol stack inside tr69hostif*: Would avoid a second daemon but requires reimplementing the USP protocol engine (Protobuf, MTP, RBAC) — obuspa already does this with 11 releases of maturity. Rejected due to massive duplication.
- *Replace tr69hostif entirely with obuspa*: Would break all existing IARM/WebPA consumers, lose bootstrap/RFC store logic, and require migrating all profile handler code. Rejected due to extreme risk and scope.

**Rationale**: Separate daemon leverages obuspa's mature, conformance-tested implementation. tr69hostif changes are minimal. Clean separation of concerns: obuspa owns USP protocol, tr69hostif owns RDK device data model.

### Decision 2: RBUS as the IPC Bridge (Not UDS, Not Direct Embedding)

**Choice**: obuspa's vendor layer calls `rbus_get()` / `rbus_set()` to read and write tr69hostif's parameters.

**Alternatives considered**:
- *USP UDS MTP (Unix Domain Socket)*: obuspa's native service pattern. Would require tr69hostif to implement USP message handling — essentially embedding a USP protocol client. Too invasive.
- *Direct function linking*: Link obuspa and tr69hostif code into one binary. Defeats modularity, complicates builds and updates.
- *IARM Bus*: tr69hostif already exposes IARM RPCs, but RBUS is RDK's strategic IPC direction and already has a working DML provider in tr69hostif.

**Rationale**: RBUS is already proven — `init_rbus_dml_provider()` in tr69hostif registers all ~2000 parameters with typed handlers. The obuspa vendor layer mirrors this pattern from the consumer side. RBUS latency (~1-2ms per param) is well under USP's expected response times.

### Decision 3: Auto-Registration from waldb XML Data Model

**Choice**: The obuspa vendor layer reads tr69hostif's merged data model XML (same source `init_rbus_dml_provider()` uses) and auto-registers all parameters with the USP engine via `USP_REGISTER_GroupedVendorParam_ReadOnly()` / `USP_REGISTER_GroupedVendorParam_ReadWrite()`.

**Alternatives considered**:
- *Hand-coded parameter registration*: Explicit `USP_REGISTER_*` calls for each parameter. Rejected — unmaintainable with 2000+ params, and desynchronizes from the data model XML.
- *RBUS discovery at runtime*: Use RBUS introspection to find registered elements. RBUS doesn't provide a rich enough discovery API for types, access levels, and metadata.

**Rationale**: The waldb XML is the single source of truth. Both tr69hostif's RBUS DML Provider and the obuspa vendor layer consume it, ensuring they stay synchronized. Parameter type, access level, and bsUpdate attributes are all available in the XML.

### Decision 4: obuspa Plug-in Architecture (Not Compiled-In Vendor Layer)

**Choice**: Implement the RDK-V integration as an obuspa shared-object plug-in loaded via `-x plugin.so`.

**Alternatives considered**:
- *Modify obuspa's src/vendor/vendor.c directly*: Works but forks from upstream obuspa, making updates difficult.

**Rationale**: obuspa v10+ supports plug-in shared objects that provide `VENDOR_Init()`, `VENDOR_Start()`, and `VENDOR_Stop()`. This keeps the obuspa core binary identical to upstream, isolating all RDK-V integration in the plug-in. Upstream updates apply cleanly.

### Decision 5: Source Type Strategy — Start with SRC_WEBPA, Evolve to SRC_USP

**Choice**: Phase 1 uses `HOSTIF_SRC_WEBPA` in the RBUS DML path for USP-originating requests (it already does this). Phase 2 adds `HOSTIF_SRC_USP` to the enum and propagates USP caller identity through RBUS.

**Rationale**: `HOSTIF_SRC_WEBPA` already grants write access to bootstrap parameters — the same access level USP controllers need. Using it initially means zero tr69hostif changes for the Phase 1 MVP. The proper `HOSTIF_SRC_USP` enum gives better source tracking, journal attribution, and future RBAC refinement.

### Decision 6: Value-Change Notifications via On-Demand RBUS Subscriptions

**Choice**: When a USP controller creates a subscription (Device.LocalAgent.Subscription), the vendor layer registers an RBUS event subscription on the target parameter. The RBUS callback calls `USP_SIGNAL_DataModelEvent()` to propagate to USP.

**Alternatives considered**:
- *Subscribe to all params at startup*: Creates unnecessary RBUS event traffic for unsubscribed parameters.
- *Poll from obuspa side*: Defeats the purpose of USP's real-time event model.

**Rationale**: On-demand subscription is efficient — only parameters with active USP subscriptions generate RBUS events. tr69hostif's RBUS DML Provider already registers `EventSubHandler` with `autoPublish=true` for most parameters, so RBUS-side infrastructure is in place.

### Decision 7: MTP Configuration — MQTT Primary, WebSocket Secondary

**Choice**: Configure obuspa with MQTT as the primary MTP and WebSocket as a fallback.

**Rationale**: MQTT is the most widely deployed MTP for USP in production ISP environments. WebSocket provides a fallback path and aligns with Parodus's transport. CoAP and STOMP are available but not configured by default.

## Risks / Trade-offs

**[Resource overhead on constrained devices]** → Measure obuspa RSS on target RDK-V SoC during spike phase. Expect 3-8 MB. If excessive, evaluate `--disable-coap --disable-stomp` to reduce binary size. obuspa can also run without Broker mode to save memory.

**[Bootstrap RBAC misconfiguration]** → Design the ControllerTrust permission targets before any USP SET operations reach bootstrap params. The permission model must restrict `Bootstrap.Control.ClearDB*` and `Syndication.PartnerId` to a FullAccess-equivalent role. Validate with integration tests that ReadOnly USP controllers cannot modify bootstrap params.

**[Data model desynchronization]** → If the waldb XML changes and the obuspa plug-in is not restarted, the USP-visible data model will be stale. Mitigate by making the plug-in re-read the XML on a configurable signal (SIGHUP) or by ensuring both daemons restart together on firmware update.

**[Write conflicts in dual-stack mode]** → An ACS may SET a param via TR-069 while a USP controller SETs the same param. Both converge on `hostIf_SetMsgHandler()` with serialized mutexes, so no data corruption occurs. Last-writer-wins semantics apply. The XBSStoreJournal tracks source type, providing auditability.

**[obuspa upstream divergence]** → Using the plug-in architecture mitigates this. The core obuspa binary tracks upstream releases. Only the plug-in is RDK-V-specific code.

**[MQTT broker dependency]** → USP over MQTT requires an MQTT broker endpoint. If the ISP's MQTT broker is unreachable, USP management is unavailable. TR-069/CWMP remains operational as a fallback — this is the primary benefit of dual-stack.

## Open Questions

1. **Shared data model XML path**: Should the obuspa plug-in read `/tmp/data-model.xml` (tr69hostif's merged output) or a separate copy? Reading the same file is simplest but creates a startup ordering dependency.

2. **obuspa EndpointID format**: What EndpointID scheme should RDK-V devices use? The default is MAC-based (`os::OUID-MACADDR`). Should this align with existing WebPA device identifiers?

3. **Factory reset database location**: Where should obuspa's SQLite database and factory reset configuration live on RDK-V filesystem? Suggest `/opt/secure/USP/usp.db` and `/etc/usp_factory_reset.txt`.

4. **Certificate provisioning**: How are TLS client certificates for mutual authentication provisioned on RDK-V devices? Can the existing device certificate infrastructure (used by Parodus) be reused?

5. **RBUS batch get support**: Does the RDK-V RBUS version support `rbus_getExt()` for batch parameter retrieval? This affects throughput of grouped vendor hook callbacks.
