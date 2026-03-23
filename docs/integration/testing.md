# Testing

## Overview

This repository currently validates `tr69hostif` with a mix of component-level Google Test binaries and Python-based functional tests. The main entry points are `run_ut.sh` for unit-style coverage and `run_l2.sh` for L2 functional coverage.

## Unit and Component Tests

### Entry point

```sh
./run_ut.sh
```

### What the script does

- installs additional build dependencies with `apt-get`
- clones supporting RDK repositories used by the test environment
- prepares RFC, bootstrap, data-model, and stub files under `/etc`, `/opt`, and `/tmp`
- runs autotools bootstrap and `./configure --enable-libsoup3`
- builds and runs multiple gtest binaries under component-specific directories

### GTest targets exercised by the script

- handlers gtest
- Parodus data-model gtest
- HTTP server gtest
- core source gtest
- DHCPv4 gtest
- Device gtest
- Ethernet gtest
- Time gtest
- DeviceInfo gtest

### Coverage mode

```sh
./run_ut.sh --enable-cov
```

When coverage is enabled, the script adds GCC coverage flags and emits filtered `lcov` output for selected `src/hostif/` areas.

## L2 Functional Tests

### Entry point

```sh
./run_l2.sh
```

### What the script does

- stages data-model fragments into `/etc`
- writes test device metadata such as `RDK_PROFILE=STB`
- prepares RFC/bootstrap persistence files
- copies `mgrlist.conf` into `/etc`
- kills any already-running `tr69hostif`
- launches `/usr/local/bin/tr69hostif` with explicit config and ports
- runs Python `pytest` functional suites and writes JSON reports into `/tmp/l2_test_report`

### Functional suites currently invoked

- `test_bootup_sequence.py`
- `test_handlers_communications.py`
- `tr69hostif_deviceip.py`
- `tr69hostif_webpa.py`

## Environment Considerations

- Both scripts are environment-mutating. They create directories and files under `/etc`, `/opt`, `/tmp`, and `/usr`.
- The scripts are intended for disposable development or CI environments, not for production devices.
- `run_ut.sh` edits some source files transiently with `sed`, so use a clean workspace or review changes after the run.

## Recommended Validation Order

1. Run component tests first to catch local regressions quickly.
2. Run L2 functional tests after interface, profile, or WebPA changes.
3. Check logs and JSON reports together when debugging failures.

## Test Artifacts

| Artifact | Location |
|----------|----------|
| L2 JSON reports | `/tmp/l2_test_report` |
| Service log during L2 run | `/opt/logs/tr69hostIf.log.0` |
| Coverage report inputs | `coverage.info`, `filtered.info`, `tr69hostif_coverage.info` |

## Debugging Failures

- If a gtest binary fails to build, first verify the prerequisite headers and cloned dependencies are present.
- If functional tests fail early, inspect the staged data-model files and manager map.
- If WebPA tests fail, verify Parodus-related config and the merged data model in `/tmp/data-model.xml`.

## See Also

- [Build Setup](build-setup.md)
- [Common Errors](../troubleshooting/common-errors.md)