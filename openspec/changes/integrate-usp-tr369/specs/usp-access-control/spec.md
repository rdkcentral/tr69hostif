## ADDED Requirements

### Requirement: USP ControllerTrust role definitions for RDK-V
The obuspa deployment SHALL define at minimum three ControllerTrust roles that map to tr69hostif's existing source-type access model.

#### Scenario: FullAccess role defined
- **WHEN** obuspa starts with the factory reset configuration
- **THEN** a "FullAccess" role SHALL exist with Param:rw, Obj:rwa, InstantiatedObj:rwa, CommandEvent:rwx permissions on "Device." targets, equivalent to tr69hostif's HOSTIF_SRC_RFC access level

#### Scenario: Operations role defined
- **WHEN** obuspa starts with the factory reset configuration
- **THEN** an "Operations" role SHALL exist with Param:rw on general parameters and Param:r on bootstrap control parameters (`Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.*`), equivalent to tr69hostif's HOSTIF_SRC_WEBPA access level

#### Scenario: ReadOnly role defined
- **WHEN** obuspa starts with the factory reset configuration
- **THEN** a "ReadOnly" role SHALL exist with Param:r and Obj:r permissions on "Device." targets, with no write access to any parameter

### Requirement: Bootstrap store write protection via USP permissions
Bootstrap control parameters with side effects SHALL be restricted to the FullAccess role only.

#### Scenario: Bootstrap.Control.ClearDB restricted
- **WHEN** a USP controller with Operations or ReadOnly role attempts to SET `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDB`
- **THEN** obuspa SHALL reject the request with USP error code 7006 (Permission Denied)

#### Scenario: Bootstrap.Control.ClearDBEnd restricted
- **WHEN** a USP controller with Operations or ReadOnly role attempts to SET `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDBEnd`
- **THEN** obuspa SHALL reject the request with USP error code 7006 (Permission Denied)

#### Scenario: Syndication.PartnerId restricted
- **WHEN** a USP controller with Operations or ReadOnly role attempts to SET `Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId`
- **THEN** obuspa SHALL reject the request with USP error code 7006 (Permission Denied)

#### Scenario: FullAccess controller can modify bootstrap control params
- **WHEN** a USP controller with FullAccess role sends a SET for `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDB`
- **THEN** the request SHALL be forwarded to tr69hostif via RBUS and processed normally

### Requirement: HOSTIF_SRC_USP source type in tr69hostif
tr69hostif SHALL include a `HOSTIF_SRC_USP` value in the `HostIf_Source_Type_t` enum to enable proper source attribution for USP-originating requests.

#### Scenario: USP source type defined
- **WHEN** tr69hostif is compiled with `--enable-usp` or equivalent build flag
- **THEN** `HOSTIF_SRC_USP` SHALL be a valid value in `HostIf_Source_Type_t` and SHALL be accepted by the bootstrap store for write operations on RW parameters

#### Scenario: Bootstrap store accepts USP source for RW parameters
- **WHEN** a SET request arrives with `requestor = HOSTIF_SRC_USP` for a bootstrap parameter with RW access
- **THEN** the bootstrap store SHALL accept the write and record "USP" as the source in the XBSStoreJournal

### Requirement: Read-only parameter enforcement at both layers
Read-only parameters SHALL be enforced at both the USP permission layer (obuspa) and the tr69hostif handler layer.

#### Scenario: USP Set on read-only parameter rejected
- **WHEN** a USP controller sends a Set for a parameter marked access="readOnly" in the data model XML
- **THEN** obuspa SHALL reject the request before it reaches RBUS, returning USP error code 7006 (Permission Denied) or 7004 (Invalid Arguments)
