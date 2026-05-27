## Why

TR-069 (CWMP) is frozen at Amendment 6 with no further evolution planned. The Broadband Forum's successor protocol — TR-369 (USP) — offers real-time persistent connections, multi-controller architecture, Protobuf encoding, end-to-end encryption with mutual TLS, and built-in role-based access control. The industry is actively migrating: OB-USP-Agent (obuspa) is already integrated in RDK-B, at v11.0 with monthly releases. RDK Video devices need USP support to remain aligned with the evolving RDK ecosystem and ISP infrastructure.

The integration is opportunistic now because tr69hostif already exposes all TR-181 parameters over RBUS via its `hostIf_rbus_Dml_Provider`. OB-USP-Agent's grouped vendor hook API is designed for exactly this pattern — consuming external data model providers over IPC. The "hard plumbing" exists on both sides; what's needed is the glue layer and operational configuration.

## What Changes

- **Add OB-USP-Agent (obuspa) as a new daemon** on RDK Video devices, running alongside tr69hostif in dual-stack mode (TR-069 + USP simultaneously)
- **Create an obuspa RDK-V vendor layer** that bridges USP protocol operations to tr69hostif's TR-181 parameters via RBUS IPC
- **Add `HOSTIF_SRC_USP` source type** to tr69hostif's `HostIf_Source_Type_t` enum for proper request source tracking and bootstrap store access control
- **Wire value-change notifications** from tr69hostif's RBUS event system to USP subscription/notify mechanism
- **Define USP Controller Trust role mappings** that align with tr69hostif's bootstrap store access control model (RFC-push and WebPA write levels)
- **Add systemd service configuration** for obuspa daemon on RDK-V targets with proper service dependencies (obuspa starts after tr69hostif)
- **Add build system integration** with `--enable-usp` configure flag for RDK-V builds

## Capabilities

### New Capabilities
- `usp-rbus-bridge`: obuspa vendor layer that auto-discovers tr69hostif's TR-181 data model from waldb XML and exposes it to USP controllers via RBUS get/set callbacks
- `usp-notification-forwarding`: Wiring of RBUS value-change events to USP ValueChange/ObjectCreation/ObjectDeletion notifications for real-time event delivery to USP controllers
- `usp-access-control`: Mapping of USP ControllerTrust roles to tr69hostif's source-type-based access control, including bootstrap store write permissions and read-only param enforcement
- `usp-daemon-deployment`: obuspa daemon packaging, systemd service, certificate provisioning, MTP configuration (MQTT + WebSocket), and build system integration for RDK-V targets

### Modified Capabilities
<!-- No existing specs to modify — this is a net-new capability addition -->

## Impact

- **New daemon process**: obuspa runs as a separate systemd service (~3-8 MB RSS), adds MQTT/WebSocket cloud connections
- **tr69hostif source change**: Minimal — one enum value addition to `src/hostif/include/hostIf_tr69ReqHandler.h`; existing RBUS DML Provider handles the rest
- **New dependencies**: obuspa brings libmosquitto (MQTT), libwebsockets, sqlite3, protobuf-c, zlib; build recipe needed for RDK-V
- **Existing paths unaffected**: IARM/ACS, WebPA/Parodus, JSON, and RBUS paths continue working unchanged during dual-stack operation
- **Cloud infrastructure**: ISPs need USP controller infrastructure (out of scope for device-side work, but a deployment prerequisite)
- **Test coverage**: Dual-protocol test matrix — every parameter needs validation through both TR-069 and USP paths; TP-469 USP conformance test suite applies
