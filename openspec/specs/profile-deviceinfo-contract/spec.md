## Purpose
Define request/response contracts for the DeviceInfo profile, including core DeviceInfo parameters, process tables, and RFC/bootstrap-backed value resolution behavior.

## Requirements

### Requirement: DeviceInfo Namespace Ownership
The system SHALL route DeviceInfo profile requests for Device.DeviceInfo.* paths to the DeviceInfo profile contract.

#### Scenario: Route core DeviceInfo parameter
- **WHEN** a request targets a Device.DeviceInfo.* parameter
- **THEN** the request is processed by the DeviceInfo profile handler contract and returns a typed value or fault outcome

### Requirement: DeviceInfo Process Table Exposure
The system SHALL expose Device.DeviceInfo.ProcessStatus and ProcessStatus.Process.{i} contracts as queryable read paths.

#### Scenario: Query process count
- **WHEN** a caller requests Device.DeviceInfo.ProcessStatus.ProcessNumberOfEntries
- **THEN** the profile returns the current number of process table entries as an unsigned integer response value

#### Scenario: Query process row field
- **WHEN** a caller requests a ProcessStatus.Process.{i} field for an existing row
- **THEN** the profile returns the requested row field value with success fault semantics

### Requirement: DeviceInfo Reset Control Semantics
The system SHALL enforce explicit success or fault reporting for reset-control style DeviceInfo set operations.

#### Scenario: Apply supported reset control value
- **WHEN** a caller sets a supported reset-control parameter in Device.DeviceInfo.X_* scope
- **THEN** the profile executes the corresponding reset-control workflow and returns a success outcome only if the workflow is accepted

#### Scenario: Apply unsupported reset control value
- **WHEN** a caller sets an unsupported reset-control value
- **THEN** the profile returns an invalid-parameter-value style fault outcome

### Requirement: DeviceInfo RFC and Bootstrap Integration
The system SHALL surface DeviceInfo RFC/bootstrap-backed parameters through the same DeviceInfo GET/SET contract with deterministic precedence.

#### Scenario: Read RFC or bootstrap-backed parameter
- **WHEN** a caller reads a DeviceInfo parameter backed by RFC or bootstrap storage
- **THEN** the profile resolves and returns the effective value according to configured precedence rules

#### Scenario: Update RFC or bootstrap-backed parameter
- **WHEN** a caller sets a writable DeviceInfo parameter backed by RFC or bootstrap storage
- **THEN** the profile persists or applies the update according to source semantics and returns explicit success or fault status
