## Purpose
Define how tr69hostif resolves, routes, and synchronizes top-level TR-181 request handling across managed handler domains.

## Requirements

### Requirement: Prefix-Based Manager Resolution
The system SHALL resolve each TR-181 parameter request to a single manager based on configured parameter-prefix ownership.

#### Scenario: Route a known parameter path
- **WHEN** a request targets a parameter path that matches a configured manager prefix (for example, `Device.DeviceInfo.*`)
- **THEN** the dispatcher routes the request to the corresponding handler instance for that manager

#### Scenario: Reject an unmapped parameter path
- **WHEN** a request targets a parameter path that does not match any configured manager prefix
- **THEN** the dispatcher returns a fault outcome indicating invalid or unmapped parameter ownership

### Requirement: Unified Request Envelope
The system SHALL process GET, SET, GETATTRIB, and SETATTRIB requests using the shared HOSTIF_MsgData_t contract.

#### Scenario: Process a GET request
- **WHEN** a caller submits a GET operation through any supported ingress path
- **THEN** the request is represented in `HOSTIF_MsgData_t`, processed by the selected handler, and returned with a populated fault/result state

#### Scenario: Process a SET request
- **WHEN** a caller submits a SET operation through any supported ingress path
- **THEN** the request is represented in `HOSTIF_MsgData_t`, delegated to the selected handler, and completed with an explicit fault/result state

### Requirement: Serialized Top-Level Dispatch
The system SHALL serialize top-level GET and SET dispatch entry points to avoid conflicting concurrent mutation and retrieval flows.

#### Scenario: Concurrent GET requests arrive
- **WHEN** multiple GET requests arrive concurrently at the top-level dispatcher
- **THEN** dispatcher-level synchronization ensures deterministic serialized entry handling before per-manager processing

#### Scenario: Concurrent SET requests arrive
- **WHEN** multiple SET requests arrive concurrently at the top-level dispatcher
- **THEN** dispatcher-level synchronization ensures serialized SET entry handling before per-manager processing
