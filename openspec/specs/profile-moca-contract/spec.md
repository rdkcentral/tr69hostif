## Purpose
Define request and response contracts for MoCA profile behavior under Device.MoCA.Interface.{i}.* including associated-device, QoS flow, stats, and mesh-table sub-objects.

## Requirements

### Requirement: MoCA Namespace Ownership
The system SHALL route Device.MoCA.* requests to the MoCA profile contract.

#### Scenario: Route MoCA request
- **WHEN** a request targets Device.MoCA.InterfaceNumberOfEntries or Device.MoCA.Interface.{i}.*
- **THEN** the request is processed by the MoCA profile contract and returns a typed value or fault outcome

### Requirement: MoCA Count Contracts
The system SHALL expose supported MoCA count fields as queryable contracts.

#### Scenario: Query interface count
- **WHEN** a caller requests Device.MoCA.InterfaceNumberOfEntries
- **THEN** the profile returns the current MoCA interface row count as an unsigned integer value

#### Scenario: Query subtable count
- **WHEN** a caller requests a supported MoCA subtable count field for an existing interface
- **THEN** the profile returns the current subtable row count with success fault semantics

### Requirement: MoCA Interface and Subtable Read Contracts
The system SHALL expose read contracts for supported MoCA interface fields and subtable fields including AssociatedDevice, QoS.FlowStats, Stats, and X_RDKCENTRAL-COM_MeshTable.

#### Scenario: Read supported MoCA field
- **WHEN** a caller reads a supported MoCA interface or subtable field for an existing instance
- **THEN** the profile returns the resolved field value with success fault semantics

#### Scenario: Read invalid MoCA instance
- **WHEN** a caller requests a MoCA interface or subtable row that does not exist
- **THEN** the profile returns an invalid-parameter-name or equivalent fault outcome

### Requirement: MoCA Writable Control Semantics
The system SHALL enforce explicit success or fault reporting for supported writable MoCA control fields.

#### Scenario: Set supported MoCA control field
- **WHEN** a caller sets a supported writable Device.MoCA.Interface.{i} field
- **THEN** the profile applies the requested change via backend integration and returns explicit success or fault status

#### Scenario: Set non-writable MoCA field
- **WHEN** a caller sets a non-writable Device.MoCA parameter
- **THEN** the profile returns a non-writable-parameter or invalid-parameter-style fault outcome
