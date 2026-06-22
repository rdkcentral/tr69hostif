## Purpose
Define request and response contracts for STBService profile behavior under Device.Services.STBService.1.*.
## Requirements
### Requirement: STBService Namespace Ownership
The system SHALL route Device.Services.STBService.1.* requests to the STBService profile contract.

#### Scenario: Route STBService request
- **WHEN** a request targets Device.Services.STBService.1.*
- **THEN** the request is processed by the STBService profile contract and returns a typed value or fault outcome

### Requirement: STBService Component Read Contracts
The system SHALL expose read contracts for supported STBService component paths including HDMI, AudioOutput, SPDIF, VideoDecoder, VideoOutput, and platform extension component paths through their mapped Thunder backend integration.

#### Scenario: Read supported STBService component field
- **WHEN** a caller reads a supported Device.Services.STBService.1.Components.* field
- **THEN** the profile returns the resolved field value with success fault semantics using the mapped Thunder plugin method and response field

### Requirement: STBService Writable Control Contracts
The system SHALL enforce explicit success or fault reporting for supported writable STBService component controls backed by mapped Thunder plugin methods.

#### Scenario: Set writable STBService component field
- **WHEN** a caller sets a supported writable Device.Services.STBService.1.Components.* field
- **THEN** the profile applies the update through its mapped Thunder backend integration and returns explicit success or fault status

#### Scenario: Set non-writable STBService component field
- **WHEN** a caller sets a non-writable Device.Services.STBService.1.Components.* field
- **THEN** the profile returns a non-writable-parameter or invalid-parameter-style fault outcome

### Requirement: STBService Instance Validation
The system SHALL reject unsupported instance identifiers for STBService component tables, including Thunder-derived instance tables.

#### Scenario: Read invalid STBService instance
- **WHEN** a caller requests a STBService component instance that does not exist
- **THEN** the profile returns an invalid-parameter-name or equivalent fault outcome

### Requirement: STBService Thunder Plugin Domain Mapping
The system SHALL route STBService component backend operations to the Thunder plugin domain that owns the parameter semantics.

#### Scenario: Route audio or video port operations to DisplaySettings
- **WHEN** a request targets STBService component parameters representing audio-port or video-port controls and status
- **THEN** the backend integration resolves through `org.rdk.DisplaySettings` methods using the component port context

#### Scenario: Route TV-wide picture operations to AVOutput
- **WHEN** a request targets STBService component parameters representing TV-wide picture-quality or display-mode controls
- **THEN** the backend integration resolves through `org.rdk.AVOutput` methods without per-port arguments

#### Scenario: Route display info, HDCP, and power operations to domain plugins
- **WHEN** a request targets STBService component parameters for display information, HDCP state, or power state controls
- **THEN** the backend integration resolves through `org.rdk.DisplayInfo`, `org.rdk.HdcpProfile`, or `org.rdk.PowerManager` respectively

### Requirement: STBService Thunder Instance Construction Rules
The system SHALL preserve TR-69 instance behavior while deriving STBService backend instances from Thunder capabilities.

#### Scenario: Build instances from Thunder-discovered ports
- **WHEN** a component table is port-based and a caller requests instances
- **THEN** the implementation enumerates supported ports from Thunder, creates one instance per discovered port, and reports count based on discovered instances

#### Scenario: Build singleton instance for non-port component domains
- **WHEN** a component table is not port-based and a caller requests instances
- **THEN** the implementation creates and serves a single logical instance for the device-scoped domain

