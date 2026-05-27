## ADDED Requirements

### Requirement: Auto-discover TR-181 data model from waldb XML
The obuspa plug-in SHALL read the merged data model XML file (same source used by tr69hostif's `init_rbus_dml_provider()`) at startup and extract all parameter definitions including name, type, access level, and bsUpdate attribute.

#### Scenario: Successful data model load at startup
- **WHEN** the obuspa plug-in starts and the merged data model XML file is present
- **THEN** the plug-in SHALL parse all parameter elements and extract name, data type, access (readOnly/readWrite), and bsUpdate attributes for each parameter

#### Scenario: Data model file missing at startup
- **WHEN** the obuspa plug-in starts and the merged data model XML file is not present
- **THEN** the plug-in SHALL log an error and retry at a configurable interval until the file becomes available or a maximum retry count is exceeded

### Requirement: Register all parameters with USP engine via grouped vendor hooks
The obuspa plug-in SHALL register every discovered parameter with the USP engine using `USP_REGISTER_GroupedVendorParam_ReadOnly()` for read-only parameters and `USP_REGISTER_GroupedVendorParam_ReadWrite()` for read-write parameters, with grouped vendor hooks for batch get/set operations.

#### Scenario: Read-only parameter registration
- **WHEN** a parameter in the data model XML has access="readOnly"
- **THEN** it SHALL be registered with `USP_REGISTER_GroupedVendorParam_ReadOnly()` and a get callback that calls `rbus_get()` on the parameter name

#### Scenario: Read-write parameter registration
- **WHEN** a parameter in the data model XML has access="readWrite"
- **THEN** it SHALL be registered with `USP_REGISTER_GroupedVendorParam_ReadWrite()` with both get and set callbacks that call `rbus_get()` and `rbus_set()` respectively

#### Scenario: Table object instance discovery
- **WHEN** the data model XML contains parameters with `{i}` instance placeholders
- **THEN** the plug-in SHALL register the parent table object and implement `USP_REGISTER_Object_RefreshInstances()` to query current instance numbers via RBUS at runtime

### Requirement: Translate USP get/set operations to RBUS calls
The grouped vendor hook callbacks SHALL translate USP parameter operations to RBUS API calls, converting between USP/Protobuf types and RBUS value types.

#### Scenario: USP Get resolves via RBUS
- **WHEN** a USP controller sends a Get message for a registered parameter
- **THEN** the plug-in SHALL call `rbus_get()` with the parameter name, convert the `rbusValue_t` result to a USP string representation, and return it in the USP GetResponse

#### Scenario: USP Set resolves via RBUS
- **WHEN** a USP controller sends a Set message for a writable parameter
- **THEN** the plug-in SHALL convert the USP value to the appropriate `rbusValue_t` type and call `rbus_set()`, returning the RBUS result as a USP success or error response

#### Scenario: RBUS connection failure during operation
- **WHEN** an RBUS call fails due to connection loss or timeout
- **THEN** the plug-in SHALL return USP error code 7012 (General Failure) with a descriptive error message and log the RBUS error details

### Requirement: RBUS handle lifecycle management
The plug-in SHALL manage the RBUS connection lifecycle including open, health monitoring, and graceful close.

#### Scenario: RBUS handle initialization
- **WHEN** `VENDOR_Init()` is called
- **THEN** the plug-in SHALL call `rbus_open()` with a component name of "obuspa_rdkv" and verify the handle is valid before proceeding with parameter registration

#### Scenario: RBUS handle cleanup on shutdown
- **WHEN** `VENDOR_Stop()` is called
- **THEN** the plug-in SHALL call `rbus_close()` to release the RBUS handle and all registered data elements
