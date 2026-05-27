## Context

tr69hostif exposes the TR-181 data model to the RDK-B stack. Three documentation files in `docs/api/` enumerate the parameters implemented by the daemon:

- `rfc_namespace_parameters.txt` (~220 entries) — `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.*`
- `non_rfc_namespace_parameters.txt` (~371 entries) — all other TR-181 paths
- `bootstrap_rfc_parameters.txt` (39+ entries) — parameters backed by the Bootstrap store (XBSStore)

Each file currently records parameter path, access mode (RO/RW), and a short description, but omits the TR-069 data type. Integrators (WebPA, CWMP managers, test harnesses) need the type to validate values and construct correct GET/SET payloads.

Type information is authoritative in two places:
1. **`data-model-generic.xml` / `data-model-stb.xml`** — waldb data model XMLs that carry a `type` attribute per `<parameter>` element for parameters declared there.
2. **C++ get/set handlers** — for parameters not in the XML (mainly RFC opaque key-value store), the type is implicit in how values are read and written in `Device_DeviceInfo.cpp`, `XrdkCentralComRFCStore.cpp`, `XrdkCentralComBSStore.cpp`.

The `bootstrap_rfc_parameters.txt` file already has a partial `type` column; it needs to be made consistent and complete.

## Goals / Non-Goals

**Goals:**

- Add a `Type` column (using standard TR-069 types: `string`, `boolean`, `unsignedInt`, `int`, `dateTime`, `base64`) to every parameter entry in all three files
- Derive types primarily from the XML data model; fall back to source-code inspection for RFC opaque-store parameters
- Normalise the existing partial type column in `bootstrap_rfc_parameters.txt` to match the same format
- Produce human-readable, column-aligned plain-text output consistent with the existing file format

**Non-Goals:**

- Changing any runtime code, build system, or configuration files
- Adding enumeration constraint lists (e.g. allowed string values) — that is a separate documentation task
- Introducing machine-parseable formats (JSON, CSV) — the files remain plain text for now
- Covering parameters not already listed in the three files

## Decisions

### Decision 1 — Type derivation priority

**Choice**: XML data model first, then C++ source inspection, then `string` as fallback.

**Rationale**: The XML is the normative source for parameters that go through the waldb router. For RFC opaque-store parameters that are not declared in the XML (they are passed as raw strings through the key-value INI), the C++ handler provides the only available signal. A `string` fallback avoids leaving cells blank and is correct for the majority of untyped RFC entries.

**Alternatives considered**:
- *XML only* — would leave most RFC namespace entries blank because most are not declared in waldb XML.
- *Source only* — slower and less reliable; the XML is already machine-readable.

### Decision 2 — Column format

**Choice**: Extend existing column layout with a right-aligned `Type` field between the path and the access-mode/description columns, using spaces for alignment. Use short TR-069 base-type names only.

**Rationale**: The files are consumed by humans in a text editor; consistent column width improves readability without requiring a parser. The short type names (`string`, `boolean`, `unsignedInt`, `int`, `dateTime`) match the TR-069 spec vocabulary directly.

**Alternatives considered**:
- *Inline parenthetical* e.g. `(boolean)` — harder to grep/scan; breaks existing alignment.
- *Separate column at end* — disrupts the description readability.

### Decision 3 — Handling RFC opaque-store parameters (no XML declaration)

**Choice**: Inspect `XrdkCentralComRFCStore.cpp` and `Device_DeviceInfo.cpp` set-handlers. Parameters whose values are parsed with `atoi`/`strcmp("true"…)` are typed `unsignedInt`/`int`/`boolean`; all others default to `string`.

**Rationale**: Most RFC parameters are stored as opaque strings in the INI file; only those with explicit conversion in the setter carry a meaningful non-string type.

## Risks / Trade-offs

- **[Risk] Type inferred from code may diverge from the actual CWMP schema** → Mitigation: Document that types are derived from implementation and may not reflect a normative CWMP DM schema; add a header disclaimer to each file.
- **[Risk] New parameters added to tr69hostif after this change will not have types automatically** → Mitigation: The tasks spec will include a note that future parameter additions must include type annotation.
- **[Risk] `bootstrap_rfc_parameters.txt` partial type column formatting changes may affect tools that parse the file today** → Mitigation: The file is human-readable documentation; no tool currently parses it. Low risk.

## Migration Plan

Documentation-only change. No deployment or rollback steps required.

1. Update `docs/api/rfc_namespace_parameters.txt` — add `Type` column
2. Update `docs/api/non_rfc_namespace_parameters.txt` — add `Type` column
3. Update `docs/api/bootstrap_rfc_parameters.txt` — normalise and complete `Type` column
4. Verify column alignment is consistent within each file
5. Submit for review — no merge-order dependency between the three files

## Open Questions

- Should `unsignedInt` parameters that are visibly used as bitmasks be annotated differently (e.g. `unsignedInt (bitmask)`)? → Deferred; out of scope for this change.
- Should `dateTime` parameters include the TR-069 UTC timestamp format note? → Deferred; a description-level note is sufficient and can be added per-entry in a follow-up.
