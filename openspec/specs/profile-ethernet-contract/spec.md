## Purpose
Define request/response contracts for Ethernet interface and statistics objects under Device.Ethernet.Interface.{i}.*.

## Requirements

### Requirement: Ethernet Namespace Ownership
The system SHALL route Device.Ethernet.* parameter requests to the Ethernet profile contract.

#### Scenario: Route Ethernet parameter request
- **WHEN** a request targets Device.Ethernet.*
- **THEN** the request is handled by the Ethernet profile contract and returns a typed value or fault outcome

### Requirement: Ethernet Interface Count Contract
The system SHALL expose Device.Ethernet.InterfaceNumberOfEntries as a queryable object-count contract.

#### Scenario: Query interface count
- **WHEN** a caller requests Device.Ethernet.InterfaceNumberOfEntries
- **THEN** the profile returns the current count of Ethernet interface instances as an unsigned integer value

### Requirement: Ethernet Interface Field Contracts
The system SHALL support GET operations for implemented Device.Ethernet.Interface.{i} fields and SET operations only for explicitly writable fields.

#### Scenario: Read supported interface field
- **WHEN** a caller reads a supported Device.Ethernet.Interface.{i} field for an existing instance
- **THEN** the profile returns the resolved interface value with success fault semantics

#### Scenario: Set non-writable interface field
- **WHEN** a caller attempts to set a non-writable Device.Ethernet.Interface.{i} field
- **THEN** the profile returns a non-writable-parameter fault outcome

### Requirement: Ethernet Statistics Contracts
The system SHALL expose Device.Ethernet.Interface.{i}.Stats.* counters for valid interface instances.

#### Scenario: Read supported stats counter
- **WHEN** a caller reads a supported Device.Ethernet.Interface.{i}.Stats counter for an existing interface
- **THEN** the profile returns the current counter value with success fault semantics

#### Scenario: Read stats for invalid instance
- **WHEN** a caller requests Device.Ethernet.Interface.{i}.Stats.* for a non-existent interface instance
- **THEN** the profile returns an invalid-parameter-name or equivalent fault outcome
