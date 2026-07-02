## Purpose
Define how tr69hostif accepts requests from multiple ingress paths and normalizes them into a shared internal dispatch contract.

## Requirements

### Requirement: Multi-Channel Ingress Normalization
The system SHALL accept requests from supported ingress channels and normalize them into the shared internal request envelope before dispatch.

#### Scenario: IARM ingress request
- **WHEN** a request is received via IARM RPC
- **THEN** the system translates the request into the shared internal contract and routes it through the common dispatcher

#### Scenario: WebPA/Parodus ingress request
- **WHEN** a request is received via WebPA/Parodus WRP messaging
- **THEN** the system translates the request into the shared internal contract and routes it through the common dispatcher

#### Scenario: Local JSON ingress request
- **WHEN** a request is received via the local JSON interface
- **THEN** the system translates the request into the shared internal contract and routes it through the common dispatcher

### Requirement: RBUS Data Model Exposure
The system SHALL provide an RBUS-facing data-model provider that interoperates with the same parameter ownership and dispatch model.

#### Scenario: RBUS client reads a managed parameter
- **WHEN** an RBUS client reads a parameter owned by a registered manager
- **THEN** the provider resolves and serves the value through the same underlying manager ownership model

#### Scenario: RBUS client writes a managed parameter
- **WHEN** an RBUS client writes a parameter owned by a registered manager
- **THEN** the provider applies the update through the same underlying manager ownership model and returns a success or fault outcome

### Requirement: Correlated Fault Reporting
The system SHALL preserve fault semantics and correlation metadata from normalized requests through adapter responses.

#### Scenario: Adapter returns request failure
- **WHEN** dispatch or handler processing fails for a normalized request
- **THEN** the adapter response contains the associated fault code and caller correlation metadata
