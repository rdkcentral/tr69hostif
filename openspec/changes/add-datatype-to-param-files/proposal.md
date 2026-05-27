## Why

The three parameter inventory files in `docs/api/` (`rfc_namespace_parameters.txt`, `non_rfc_namespace_parameters.txt`, `bootstrap_rfc_parameters.txt`) describe TR-181 datamodel parameters implemented in tr69hostif but omit data type information, making them insufficient as API references for integrators and testers who need to know the expected value domain for each parameter.

## What Changes

- Add a `Type` column to every parameter entry in `docs/api/rfc_namespace_parameters.txt` (~220 parameters)
- Add a `Type` column to every parameter entry in `docs/api/non_rfc_namespace_parameters.txt` (~371 parameters)
- Normalise and complete the existing `type` column in `docs/api/bootstrap_rfc_parameters.txt` (39 routable entries + non-routable boot-time defaults) so all entries carry a consistent type value

## Capabilities

### New Capabilities

- `param-type-inventory`: Data-type annotations for every tr69hostif TR-181 datamodel parameter, derived from `data-model-generic.xml`, `data-model-stb.xml`, and C++ source handlers. Covers TR-069 base types (`string`, `boolean`, `unsignedInt`, `int`, `dateTime`, `base64`) as well as vendor-specific enumeration constraints where determinable from source.

### Modified Capabilities

<!-- No existing spec-level requirements are changing — this is purely a documentation enrichment. -->

## Impact

- `docs/api/rfc_namespace_parameters.txt` — file format extended (new column); no code changes
- `docs/api/non_rfc_namespace_parameters.txt` — file format extended (new column); no code changes
- `docs/api/bootstrap_rfc_parameters.txt` — existing type column made consistent; no code changes
- Source files read for type derivation (read-only): `src/hostif/parodusClient/waldb/data-model/data-model-generic.xml`, `data-model-stb.xml`, `src/hostif/profiles/DeviceInfo/Device_DeviceInfo.cpp`, `XrdkCentralComRFCStore.cpp`, `XrdkCentralComBSStore.cpp`
- No runtime behaviour, APIs, or build system affected
