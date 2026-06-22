## Purpose
Define request/response contracts for the IP profile across global Device.IP parameters, interface/address tables, active port tables, and supported diagnostics objects.

## Requirements

### Requirement: IP Namespace Ownership
The system SHALL route Device.IP.* requests to the IP profile contract.

#### Scenario: Route IP parameter request
- **WHEN** a request targets a Device.IP.* parameter
- **THEN** the request is handled by the IP profile contract and returns a typed value or fault outcome

### Requirement: Global IP Capability Contracts
The system SHALL expose global Device.IP capability, enablement, status, and object-count contracts.

#### Scenario: Query global IP capabilities
- **WHEN** a caller requests global Device.IP capability or status fields
- **THEN** the profile returns current capability or status values derived from platform network state

#### Scenario: Query global object counts
- **WHEN** a caller requests Device.IP.InterfaceNumberOfEntries or Device.IP.ActivePortNumberOfEntries
- **THEN** the profile returns the current count derived from runtime enumeration

### Requirement: Interface and Address Table Contracts
The system SHALL support Device.IP.Interface.{i} and child address-table contracts for valid instance identifiers.

#### Scenario: Read interface field
- **WHEN** a caller reads a supported Device.IP.Interface.{i} field for an existing interface instance
- **THEN** the profile returns the resolved interface value with success fault semantics

#### Scenario: Read IPv4 or IPv6 address row field
- **WHEN** a caller reads a supported Device.IP.Interface.{i}.IPv4Address.{j} or IPv6Address.{j} field for an existing row
- **THEN** the profile returns the resolved address-row value with success fault semantics

#### Scenario: Address row does not exist
- **WHEN** a caller requests an address-table row that is not present
- **THEN** the profile returns an invalid-parameter-name or equivalent fault outcome

### Requirement: IP Writable Control Contracts
The system SHALL enforce explicit success or fault reporting for writable Device.IP and Device.IP.Interface.{i} controls.

#### Scenario: Apply writable IP control
- **WHEN** a caller sets a supported writable IP control parameter
- **THEN** the profile applies the update to platform networking state and returns explicit success or fault status

#### Scenario: Attempt write to non-writable field
- **WHEN** a caller sets a non-writable Device.IP field
- **THEN** the profile returns a non-writable-parameter fault outcome
