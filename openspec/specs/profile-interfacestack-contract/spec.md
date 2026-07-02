## Purpose
Define request and response contracts for interface stacking relationships exposed via Device.InterfaceStack.{i}.*.

## Requirements

### Requirement: InterfaceStack Namespace Ownership
The system SHALL route Device.InterfaceStack* requests to the InterfaceStack profile contract.

#### Scenario: Route InterfaceStack request
- **WHEN** a request targets Device.InterfaceStackNumberOfEntries or Device.InterfaceStack.{i}.*
- **THEN** the request is processed by the InterfaceStack profile contract and returns a typed value or fault outcome

### Requirement: InterfaceStack Count Contract
The system SHALL expose Device.InterfaceStackNumberOfEntries as a queryable object-count contract.

#### Scenario: Query InterfaceStack count
- **WHEN** a caller requests Device.InterfaceStackNumberOfEntries
- **THEN** the profile returns the current number of stack rows as an unsigned integer value

### Requirement: InterfaceStack Row Field Contracts
The system SHALL expose read contracts for Device.InterfaceStack.{i}.HigherLayer and Device.InterfaceStack.{i}.LowerLayer.

#### Scenario: Read valid stack row
- **WHEN** a caller reads HigherLayer or LowerLayer for an existing Device.InterfaceStack.{i} row
- **THEN** the profile returns the corresponding TR-181 reference value with success fault semantics

#### Scenario: Read invalid stack row
- **WHEN** a caller reads Device.InterfaceStack.{i} for a non-existent row
- **THEN** the profile returns an invalid-parameter-name or equivalent fault outcome

### Requirement: InterfaceStack Read-Only Semantics
The system SHALL reject write attempts for InterfaceStack rows.

#### Scenario: Set InterfaceStack field
- **WHEN** a caller attempts to set Device.InterfaceStack.{i}.HigherLayer or Device.InterfaceStack.{i}.LowerLayer
- **THEN** the profile returns a non-writable-parameter or unsupported-operation fault outcome
