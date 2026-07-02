## Purpose
Define request and response contracts for DHCPv4 profile behavior under Device.DHCPv4.Client.{i}.*.

## Requirements

### Requirement: DHCPv4 Namespace Ownership
The system SHALL route Device.DHCPv4.* requests to the DHCPv4 profile contract.

#### Scenario: Route DHCPv4 request
- **WHEN** a request targets Device.DHCPv4.ClientNumberOfEntries or Device.DHCPv4.Client.{i}.*
- **THEN** the request is handled by the DHCPv4 profile contract and returns a typed value or fault outcome

### Requirement: DHCPv4 Client Count Contract
The system SHALL expose Device.DHCPv4.ClientNumberOfEntries as a queryable object-count contract.

#### Scenario: Query client count
- **WHEN** a caller requests Device.DHCPv4.ClientNumberOfEntries
- **THEN** the profile returns the current DHCPv4 client entry count as an unsigned integer value

### Requirement: DHCPv4 Client Field Contracts
The system SHALL expose supported read contracts for Device.DHCPv4.Client.{i}.Interface, Device.DHCPv4.Client.{i}.DNSServers, and Device.DHCPv4.Client.{i}.IPRouters.

#### Scenario: Read supported client field
- **WHEN** a caller reads a supported Device.DHCPv4.Client.{i} field for an existing instance
- **THEN** the profile returns the resolved field value with success fault semantics

#### Scenario: Read unsupported or invalid client field
- **WHEN** a caller requests an unsupported DHCPv4 client field or non-existent instance
- **THEN** the profile returns an invalid-parameter-name or equivalent fault outcome

### Requirement: DHCPv4 Read-Only Semantics
The system SHALL reject write attempts for DHCPv4 client fields unless explicitly implemented as writable.

#### Scenario: Set DHCPv4 client field
- **WHEN** a caller attempts to set a non-writable Device.DHCPv4.Client.{i} field
- **THEN** the profile returns a non-writable-parameter or unsupported-operation fault outcome
