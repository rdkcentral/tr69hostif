# Build Setup

## Overview

`tr69hostif` uses autotools and libtool as the primary build system. Feature areas are enabled with `./configure` flags, and the resulting binary composition depends heavily on the platform profile and enabled subsystems.

## Standard Build Flow

```sh
autoreconf --install
./configure [feature flags]
make -j"$(nproc)"
```

For repo-local testing, the current scripts also run:

```sh
automake --add-missing
autoreconf --install
./configure --enable-libsoup3
```

## Common Configure Flags

The top-level `configure.ac` currently exposes feature toggles including:

| Flag | Effect |
|------|--------|
| `--enable-xre` | Enable XRE-related profile support |
| `--enable-moca` / `--enable-moca2` | Enable MoCA profile support |
| `--enable-wifi` | Enable WiFi profile support |
| `--enable-DHCPv4` | Enable DHCPv4 profile support |
| `--enable-StorageService` | Enable StorageService profile support |
| `--enable-InterfaceStack` | Enable InterfaceStack profile support |
| `--enable-notification` | Enable value-change notification support |
| `--enable-t2api` | Enable telemetry hooks |
| `--enable-webpaRFC` | Enable WebPA RFC behavior |
| `--enable-IPv6` | Enable IPv6 behavior in IP profile |
| `--enable-SpeedTest` | Enable speed-test diagnostics |
| `--enable-systemd-notify` | Enable `sd_notify()` integration |
| `--enable-hwselftest` | Enable hardware self-test profile |

Not every platform uses every flag. The effective feature set should match the device image, available HALs, and deployment requirements.

## External Dependencies

The repository test flows install or reference these representative dependencies:

- autotools and libtool
- GLib
- libprocps or libprocps-ng
- libtinyxml2
- libsoup 3
- libnanomsg
- libparodus headers and libraries
- platform HALs and RDK middleware such as IARM-related components

The unit-test workflow also clones external RDK repositories used for stubs and device-settings integration.

## Runtime Files Required by the Daemon

| Path | Purpose |
|------|---------|
| `/etc/mgrlist.conf` or configured manager map | Parameter-prefix to manager mapping |
| `/etc/data-model-generic.xml` | Generic data-model fragment |
| `/etc/data-model-stb.xml` or `/etc/data-model-tv.xml` | Platform-specific data-model fragment |
| `/tmp/data-model.xml` | Merged data model for WebPA path |
| `/opt/secure/RFC/` | RFC and bootstrap persistence |
| `/etc/partners_defaults.json` | Partner defaults consumed by bootstrap subsystem |

## Service Files

The repository includes multiple systemd unit files:

- `tr69hostif.service`
- `tr69hostif_no_new_http_server.service`
- `ip-iface-monitor.service`

Choose the unit that matches the build-time feature set and deployment model.

## Build Notes

- The daemon is highly feature-gated. Missing headers or libraries typically indicate a mismatched `./configure` flag set for the target platform.
- Data-model availability is a runtime prerequisite even when the binary builds successfully.
- WebPA/Parodus support depends on both build-time enablement and valid runtime configuration in `/etc/webpa_cfg.json`.

## Example Development Build

```sh
autoreconf --install
./configure --enable-wifi --enable-DHCPv4 --enable-notification --enable-systemd-notify
make -j4
```

## See Also

- [Testing](testing.md)
- [System Overview](../architecture/overview.md)
- [Common Errors](../troubleshooting/common-errors.md)