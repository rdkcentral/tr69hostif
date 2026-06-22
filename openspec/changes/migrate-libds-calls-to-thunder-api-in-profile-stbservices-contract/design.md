## Context

The STBService profile currently serves TR-69 reads and writes under `Device.Services.STBService.1.Components.*` through legacy libds (`device::`) integrations. Platform direction is to route these operations through Thunder JSON-RPC plugins while preserving external TR-69 contract behavior. The existing migration prompt for STBService Thunder work defines plugin ownership, method patterns, instance construction, and fallback handling that need to be reflected in the OpenSpec change.

The latest prompt revision adds concrete API constraints that must be treated as design inputs rather than implementation details:
- DisplayInfo access uses property-style endpoint names (`DisplayInfo.1.displayinfo`, `DisplayInfo.1.framerate`) and typed extraction.
- HDMI resolution formatting must be reconstructed from structured fields (`w`, `h`, `progressive`) plus DisplayInfo frame rate.
- Some VideoOutput and HDCP paths use nested JSON payloads that may require controlled string parsing when helper extraction is insufficient.
- A defined subset of legacy audio methods remains "under define" and must return `NOT_HANDLED`.

## Goals / Non-Goals

**Goals:**
- Define a contract-preserving migration path from libds-backed STBService component handlers to Thunder-backed handlers.
- Standardize plugin ownership for STBService domains (DisplaySettings, AVOutput, DisplayInfo, HdcpProfile, PowerManager).
- Standardize instance lifecycle expectations for port-based components versus single-instance components.
- Define failure semantics when Thunder methods fail, return invalid payloads, or do not support a parameter.
- Enforce no-regression behavior for externally visible TR-69 GET/SET semantics through migration.
- Enforce clean API mapping with no leftover libds calls in migrated Thunder implementation paths.
- Keep migrated code maintainable/readable through consistent helper usage and predictable module patterns.

**Non-Goals:**
- Re-specifying every component parameter schema in this change.
- Introducing new TR-69 parameter names or changing namespace ownership.
- Defining transport-level Thunder retry policy, timeout tuning, or plugin deployment mechanics.

## Decisions

1. Decision: Keep STBService TR-69 surface unchanged while replacing backend integration with Thunder plugin calls.
Rationale: Prevent client-visible contract churn while allowing implementation migration.
Alternatives considered: Introducing new TR-69 aliases for Thunder-backed values; rejected because it would create unnecessary dual contracts.

2. Decision: Use explicit plugin-domain mapping rules in the contract for component operations.
Rationale: Mapping rules avoid ambiguous ownership and reduce inconsistent implementation across modules.
Alternatives considered: Leaving plugin selection as implementation detail; rejected because multiple plugins overlap in video/audio domains and need deterministic contract guidance.

3. Decision: Preserve multi-instance semantics for port-discovered components and require single-instance semantics for non-port domains.
Rationale: Aligns TR-69 instance tables with Thunder data model capabilities (enumerated ports vs global device state).
Alternatives considered: Forcing all modules to single-instance; rejected because AudioOutput/SPDIF/VideoOutput require port-scoped behavior.

4. Decision: Treat Thunder call failures and unsupported mappings as explicit fault outcomes instead of silent fallback behavior.
Rationale: Fault transparency is required for robust ACS behavior and consistent diagnostics.
Alternatives considered: Best-effort success with stale cache values; rejected because it hides backend failures and violates explicit control expectations.

5. Decision: Adopt explicit DisplayInfo property endpoint naming (`DisplayInfo.1.<property>`) as part of the compatibility contract.
Rationale: Callsign-style and property-style DisplayInfo addressing are both seen in code and docs; pinning one avoids drift and extraction bugs.
Alternatives considered: Allowing mixed endpoint naming per module; rejected because it increases integration variance and test ambiguity.

6. Decision: Define deterministic HDMI resolution reconstruction (`WxH[p|i]/FHz`) from split-source Thunder data.
Rationale: `getCurrentResolution` returns structured fields that are not always presentation-ready; ACS-facing value must remain stable and comparable.
Alternatives considered: Forwarding raw `resolution` field; rejected due to inconsistent plugin output forms.

7. Decision: Keep "under define" methods compile-safe and visible but non-operational (`NOT_HANDLED`) with guarded call paths.
Rationale: Preserves contract surface while avoiding false positives for unsupported backend operations.
Alternatives considered: Hard-removing parameters; rejected due to TR-69 surface compatibility risk.

8. Decision: Treat clean API mapping as a first-class migration requirement and explicitly prohibit residual libds calls in Thunder implementation files.
Rationale: Mixed backend paths create hidden regressions and make behavior validation ambiguous.
Alternatives considered: Permitting partial migration with fallback libds calls; rejected because it weakens migration guarantees and increases maintenance burden.

9. Decision: Define regression baseline against pre-migration TR-69 behavior for supported parameters and faults.
Rationale: Migration success is behavior equivalence under a new backend, not just successful Thunder method invocation.
Alternatives considered: Method-by-method unit-level parity only; rejected because caller-visible behavior can still regress.

## Risks / Trade-offs

- Risk: Plugin response field drift across Thunder service versions can break value extraction. -> Mitigation: Require method/field mapping verification per parameter and add focused tests for extraction paths.
- Risk: Port discovery differences can change instance counts and identifiers. -> Mitigation: Define deterministic enumeration and validation behavior, and verify against existing component tables.
- Risk: Some legacy libds operations may not have direct Thunder equivalents. -> Mitigation: Specify explicit unsupported/fault semantics and isolate non-migrated code paths.
- Trade-off: Stronger plugin coupling increases dependency on Thunder service availability. -> Mitigation: Keep clear fault contracts and startup diagnostics for missing plugin methods.
- Risk: DisplayInfo endpoint mismatch (`org.rdk.DisplayInfo.*` vs `DisplayInfo.1.*`) can yield runtime method-not-found behavior. -> Mitigation: Standardize on `DisplayInfo.1.*` in migration artifacts and cross-check helper invocation in module code.
- Risk: Resolution format regressions if framerate is absent or floating-point precision differs (for example `60.0` vs `59.94`). -> Mitigation: enforce canonical formatter logic and define defaulting behavior when framerate extraction fails.
- Trade-off: Temporary raw nested JSON parsing improves coverage but is less robust than schema-aware extraction. -> Mitigation: contain parsing to known response keys and schedule follow-up helper upgrades.
- Risk: Residual libds call paths in migrated modules create dual-behavior drift and can mask Thunder mapping defects. -> Mitigation: add explicit static/grep checks and code-review gate for `device::` usage removal in migrated Thunder codepaths.
- Risk: Ad hoc per-module parsing and helper usage decreases readability and future maintainability. -> Mitigation: standardize method naming constants, helper invocation patterns, and error handling style across migrated components.
