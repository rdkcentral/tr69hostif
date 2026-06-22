## Purpose
Define request/response contracts for the WiFi profile across Device.WiFi top-level objects, radio/SSID/access-point subtrees, and roaming-related extensions.

## Requirements

### Requirement: WiFi Namespace Ownership
The system SHALL route Device.WiFi.* parameter requests to the WiFi profile contract when WiFi profile support is enabled.

#### Scenario: Route WiFi parameter
- **WHEN** a request targets a Device.WiFi.* parameter and WiFi profile support is enabled
- **THEN** the request is handled by the WiFi profile contract and returns a typed value or fault outcome

#### Scenario: WiFi profile disabled at build
- **WHEN** WiFi profile support is disabled in the build
- **THEN** Device.WiFi.* requests return an unsupported or invalid-parameter-style fault outcome via normal dispatcher semantics

### Requirement: WiFi Object Count Contracts
The system SHALL expose top-level WiFi object counts using deterministic profile responses.

#### Scenario: Query WiFi object counts
- **WHEN** a caller requests Device.WiFi.RadioNumberOfEntries, Device.WiFi.SSIDNumberOfEntries, or Device.WiFi.AccessPointNumberOfEntries
- **THEN** the profile returns numeric counts consistent with the active backend and platform support

### Requirement: WiFi Radio and SSID Parameter Contracts
The system SHALL support GET and supported SET operations for Device.WiFi.Radio.{i} and Device.WiFi.SSID.{i} contract fields.

#### Scenario: Read WiFi radio field
- **WHEN** a caller reads a supported Device.WiFi.Radio.{i} field
- **THEN** the profile returns the current radio field value mapped from the active WiFi backend

#### Scenario: Set writable WiFi field
- **WHEN** a caller sets a writable Device.WiFi.* field
- **THEN** the profile applies the requested update through the active backend and returns explicit success or fault status

### Requirement: WiFi Access Point and Roaming Extensions
The system SHALL expose Device.WiFi.AccessPoint.{i} and supported vendor extension contracts, including roaming controls.

#### Scenario: Read access point or roaming field
- **WHEN** a caller reads a supported Device.WiFi.AccessPoint.{i} or Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.* field
- **THEN** the profile returns the resolved value with typed response semantics

#### Scenario: Set roaming control value
- **WHEN** a caller sets a writable roaming control parameter
- **THEN** the profile applies the value through the active backend and returns explicit success or fault status
