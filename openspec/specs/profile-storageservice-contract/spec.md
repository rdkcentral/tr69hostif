## Purpose
Define request and response contracts for StorageService profile behavior under Device.StorageService.{i}.* and physical medium sub-objects.

## Requirements

### Requirement: StorageService Namespace Ownership
The system SHALL route Device.StorageService.* requests to the StorageService profile contract.

#### Scenario: Route StorageService request
- **WHEN** a request targets Device.StorageService.*
- **THEN** the request is processed by the StorageService profile contract and returns a typed value or fault outcome

### Requirement: StorageService Count Contracts
The system SHALL expose supported StorageService object counts as queryable contracts.

#### Scenario: Query storage service count field
- **WHEN** a caller requests a supported Device.StorageService count field
- **THEN** the profile returns the corresponding current count as an unsigned integer value

### Requirement: Physical Medium Read Contracts
The system SHALL expose read contracts for supported Device.StorageService.{i}.PhysicalMedium.{j} fields.

#### Scenario: Read supported physical medium field
- **WHEN** a caller reads a supported physical medium field for an existing instance
- **THEN** the profile returns the resolved field value with success fault semantics

#### Scenario: Read invalid physical medium instance
- **WHEN** a caller requests a physical medium row that does not exist
- **THEN** the profile returns an invalid-parameter-name or equivalent fault outcome

### Requirement: StorageService Read-Only Semantics
The system SHALL reject write attempts for non-writable StorageService and PhysicalMedium fields.

#### Scenario: Set StorageService or PhysicalMedium field
- **WHEN** a caller attempts to set a non-writable Device.StorageService parameter
- **THEN** the profile returns a non-writable-parameter or unsupported-operation fault outcome
