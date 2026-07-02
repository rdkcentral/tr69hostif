## Purpose
Define request and response contracts for Time profile behavior under Device.Time.* including supported Chrony extension controls.

## Requirements

### Requirement: Time Namespace Ownership
The system SHALL route Device.Time.* requests to the Time profile contract.

#### Scenario: Route Time request
- **WHEN** a request targets Device.Time.*
- **THEN** the request is processed by the Time profile contract and returns a typed value or fault outcome

### Requirement: Time Read Contracts
The system SHALL expose read contracts for supported time and timezone fields.

#### Scenario: Read local time field
- **WHEN** a caller reads Device.Time.CurrentLocalTime or Device.Time.LocalTimeZone and the field is supported
- **THEN** the profile returns the current formatted value with success fault semantics

#### Scenario: Read UTC time extension field
- **WHEN** a caller reads a supported UTC-related extension field in Device.Time namespace
- **THEN** the profile returns the current UTC-derived value with success fault semantics

### Requirement: Chrony Control Contracts
The system SHALL enforce explicit success or fault reporting for supported Device.Time.Chrony.* writable controls.

#### Scenario: Set supported Chrony control
- **WHEN** a caller sets a supported Device.Time.Chrony.* parameter
- **THEN** the profile applies the update to its Chrony configuration backend and returns explicit success or fault status

#### Scenario: Set unsupported Time field
- **WHEN** a caller sets a Device.Time parameter that is not writable or not implemented for writing
- **THEN** the profile returns a non-writable-parameter or invalid-parameter-style fault outcome

### Requirement: Unsupported Standard NTP Field Semantics
The system SHALL return explicit fault outcomes for standard Device.Time NTP fields that are not implemented.

#### Scenario: Read or write unsupported standard NTP field
- **WHEN** a caller reads or writes a Device.Time.NTPServer{n} field that is not implemented
- **THEN** the profile returns an unsupported or invalid-parameter-style fault outcome
