## Purpose
Define request and response contracts for the Device vendor-extension profile namespace under Device.X_RDK_*. 

## Requirements

### Requirement: Device Vendor Namespace Ownership
The system SHALL route supported Device.X_RDK_* requests to the Device profile contract.

#### Scenario: Route Device vendor-extension request
- **WHEN** a request targets a supported Device.X_RDK_* parameter path
- **THEN** the request is processed by the Device profile contract and returns a typed value or fault outcome

### Requirement: Device Vendor URL Read Contracts
The system SHALL expose supported read contracts for Device X_RDK URL parameters backed by bootstrap storage.

#### Scenario: Read supported Device X_RDK URL
- **WHEN** a caller reads a supported Device.X_RDK_* URL parameter
- **THEN** the profile returns the currently resolved URL value with success fault semantics

### Requirement: Device Vendor URL Write Contracts
The system SHALL enforce explicit success or fault reporting for Device X_RDK URL write operations.

#### Scenario: Set writable Device X_RDK URL
- **WHEN** a caller sets a writable Device.X_RDK_* URL parameter
- **THEN** the profile applies the update to its configured backend and returns explicit success or fault status

#### Scenario: Set unsupported Device X_RDK URL
- **WHEN** a caller sets a Device.X_RDK_* URL parameter that is not writable
- **THEN** the profile returns a non-writable-parameter or invalid-parameter-style fault outcome
