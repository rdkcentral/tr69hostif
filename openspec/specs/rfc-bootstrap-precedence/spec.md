## Purpose
Define the effective value-resolution precedence and persistence behavior for RFC overrides and bootstrap defaults in DeviceInfo-managed parameters.

## Requirements

### Requirement: RFC and Bootstrap Value Resolution Order
The system SHALL resolve effective parameter values using the precedence order: RFC override, explicit WebPA set, bootstrap default, then firmware default.

#### Scenario: RFC override exists
- **WHEN** a parameter has an RFC override value persisted in the RFC store
- **THEN** the resolved value returned to callers uses the RFC override

#### Scenario: No RFC override but WebPA set exists
- **WHEN** a parameter has no RFC override and has an explicit WebPA-set value
- **THEN** the resolved value returned to callers uses the explicit WebPA-set value

#### Scenario: No RFC override or WebPA set
- **WHEN** a parameter has no RFC override and no explicit WebPA-set value
- **THEN** the resolved value returned to callers uses bootstrap defaults when present, otherwise firmware defaults

### Requirement: Persistent RFC Storage
The system SHALL persist RFC override data in durable storage and reload it across daemon restarts.

#### Scenario: Service restart after RFC update
- **WHEN** the service restarts after RFC override values were previously written
- **THEN** RFC override values are restored from persistent storage and applied to subsequent resolution

### Requirement: Bootstrap Partner Defaults Loading
The system SHALL load partner-specific bootstrap defaults and make them available to runtime value resolution.

#### Scenario: Partner defaults are available
- **WHEN** bootstrap partner defaults are present and partner identity is resolved
- **THEN** partner defaults are loaded and used as bootstrap-level values for eligible parameters
