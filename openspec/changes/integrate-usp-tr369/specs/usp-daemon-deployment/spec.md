## ADDED Requirements

### Requirement: obuspa systemd service for RDK-V
obuspa SHALL be packaged as a systemd service that starts after tr69hostif and loads the RDK-V vendor plug-in.

#### Scenario: Service starts after tr69hostif
- **WHEN** the RDK-V device boots
- **THEN** the obuspa systemd service SHALL have `After=tr69hostif.service` and `Wants=tr69hostif.service` dependencies, ensuring tr69hostif's RBUS DML Provider is registered before the plug-in attempts to connect

#### Scenario: Service invocation with RDK-V plug-in
- **WHEN** the obuspa service starts
- **THEN** it SHALL invoke obuspa with the `-x` flag pointing to the RDK-V vendor plug-in shared object (e.g., `-x /usr/lib/obuspa/plugins/rdkv_vendor.so`) and the `-r` flag pointing to the factory reset configuration file

#### Scenario: Service restart on failure
- **WHEN** the obuspa process exits unexpectedly
- **THEN** systemd SHALL restart it after a 10-second delay with `Restart=on-failure` and `RestartSec=10`

### Requirement: MTP configuration with MQTT and WebSocket
The factory reset configuration SHALL include MQTT as the primary MTP and WebSocket as a secondary MTP.

#### Scenario: MQTT MTP configured
- **WHEN** obuspa starts with the factory reset configuration for the first time
- **THEN** the MQTT MTP SHALL be configured with a controller endpoint URL, TLS enabled, and topic paths derived from the device's EndpointID

#### Scenario: WebSocket MTP configured as fallback
- **WHEN** obuspa starts with the factory reset configuration for the first time
- **THEN** a WebSocket MTP SHALL be configured as a secondary connection to a controller endpoint, with TLS enabled

### Requirement: Build system integration with --enable-usp flag
USP support SHALL be a build-time option for RDK-V builds.

#### Scenario: USP enabled in build
- **WHEN** `./configure --enable-usp` is passed to the tr69hostif build system
- **THEN** the build SHALL compile with `HOSTIF_SRC_USP` defined in the source type enum and the obuspa vendor plug-in SHALL be built as a shared object

#### Scenario: USP disabled in build (default)
- **WHEN** `./configure` is run without `--enable-usp`
- **THEN** `HOSTIF_SRC_USP` SHALL NOT be defined and no obuspa-related artifacts SHALL be built or installed

### Requirement: TLS certificate provisioning
The obuspa deployment SHALL use device TLS certificates for mutual authentication with USP controllers.

#### Scenario: Device certificate loaded at startup
- **WHEN** obuspa starts
- **THEN** it SHALL load the device's TLS client certificate and private key via the `--authcert` command-line option or the `get_agent_cert_cb` vendor hook, using the same certificate store used by other RDK security components where possible

#### Scenario: Trust store configured
- **WHEN** obuspa starts
- **THEN** it SHALL load the CA trust store via the `--truststore` command-line option, including the CA certificates needed to validate USP controller server certificates

### Requirement: USP EndpointID based on device identity
The obuspa EndpointID SHALL be deterministic and unique per device.

#### Scenario: EndpointID derived from device MAC
- **WHEN** obuspa starts without an explicit EndpointID override in the database
- **THEN** the EndpointID SHALL follow the format `os::<OUI>-<MAC>` where OUI is the manufacturer OUI and MAC is the primary network interface MAC address

### Requirement: Runtime USP enable/disable via RFC
USP functionality SHALL be controllable at runtime via an RFC toggle.

#### Scenario: USP disabled via RFC
- **WHEN** the RFC parameter for USP is set to disabled (e.g., `/opt/secure/RFC/.RFC_USPEnabled.ini` contains `RFC_ENABLE_USP=false`)
- **THEN** the obuspa service SHALL not start or SHALL be stopped if currently running

#### Scenario: USP enabled via RFC
- **WHEN** the RFC parameter for USP is set to enabled
- **THEN** the obuspa service SHALL start and connect to configured USP controllers
