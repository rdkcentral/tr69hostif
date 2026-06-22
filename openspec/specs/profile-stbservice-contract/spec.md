## Purpose
Define request and response contracts for STBService profile behavior under Device.Services.STBService.1.*.

## Requirements

### Requirement: STBService Namespace Ownership
The system SHALL route Device.Services.STBService.1.* requests to the STBService profile contract.

#### Scenario: Route STBService request
- **WHEN** a request targets Device.Services.STBService.1.*
- **THEN** the request is processed by the STBService profile contract and returns a typed value or fault outcome

### Requirement: STBService Component Read Contracts
The system SHALL expose read contracts for supported STBService component paths including HDMI, AudioOutput, SPDIF, VideoDecoder, VideoOutput, and platform extension component paths.

#### Scenario: Read supported STBService component field
- **WHEN** a caller reads a supported Device.Services.STBService.1.Components.* field
- **THEN** the profile returns the resolved field value with success fault semantics

### Requirement: STBService Writable Control Contracts
The system SHALL enforce explicit success or fault reporting for supported writable STBService component controls.

#### Scenario: Set writable STBService component field
- **WHEN** a caller sets a supported writable Device.Services.STBService.1.Components.* field
- **THEN** the profile applies the update through its backend integration and returns explicit success or fault status

#### Scenario: Set non-writable STBService component field
- **WHEN** a caller sets a non-writable Device.Services.STBService.1.Components.* field
- **THEN** the profile returns a non-writable-parameter or invalid-parameter-style fault outcome

### Requirement: STBService Instance Validation
The system SHALL reject unsupported instance identifiers for STBService component tables.

#### Scenario: Read invalid STBService instance
- **WHEN** a caller requests a STBService component instance that does not exist
- **THEN** the profile returns an invalid-parameter-name or equivalent fault outcome
