## Why

STBService component handlers currently rely on libds (`device::` HAL) calls that are being replaced by Thunder plugin APIs across the platform. Defining this migration at the contract level now prevents backend drift and keeps TR-69 behavior stable while implementation paths change.

The latest migration prompt also fixes API-level contract details that were previously ambiguous, including DisplayInfo property endpoint naming (`DisplayInfo.1.*`), `getCurrentResolution` response-shape handling (`w`, `h`, `progressive`), and explicit behavior for methods that are still under definition.

The updated prompt additionally requires explicit no-regression behavior, clean API mapping with no leftover libds calls, and maintainable/readable Thunder-side implementation patterns.

## What Changes

- Update the STBService profile contract to require Thunder-backed integration for supported component reads and writes that previously used libds.
- Define plugin ownership and API mapping expectations for STBService parameters across `org.rdk.DisplaySettings`, `org.rdk.AVOutput`, `org.rdk.DisplayInfo`, `org.rdk.HdcpProfile`, and `org.rdk.PowerManager`.
- Specify instance and port-discovery behavior for port-scoped components (for example AudioOutput/SPDIF/VideoOutput) and single-instance behavior for non-port domains.
- Clarify required fault outcomes when Thunder methods are unavailable, return invalid payloads, or reject unsupported parameters.
- Pin HDMI `ResolutionValue` reconstruction to a deterministic format using mixed plugin sources:
	- resolution dimensions and scan type from `org.rdk.DisplaySettings.getCurrentResolution` (`w`, `h`, `progressive`)
	- frame rate from `DisplayInfo.1.framerate`
	- final format: `WxH[p|i]/FHz` (example: `1920x1080p/60Hz`)
- Record and enforce "under define" behavior for unresolved audio methods (`setDB/getDB/getMinDB/getMaxDB/setLoopThru/isLoopThru/getOptimalLevel`) as `NOT_HANDLED` with guarded call paths.
- Enforce code-level migration quality rules for readability and maintainability, including consistent helper usage, explicit method/field mapping, and removal of legacy libds call paths from Thunder implementation files.

## Capabilities

### New Capabilities
- None.

### Modified Capabilities
- `profile-stbservice-contract`: Change backend integration requirements from libds-oriented behavior to Thunder API integration while preserving existing TR-69 request/response semantics.

## Impact

- Affects STBService profile contract deltas under OpenSpec and implementation files under `src/hostif/profiles/STBService`.
- Requires Thunder plugin method availability and response-shape compatibility for the mapped APIs.
- Reduces dependence on libds-backed handlers for STBService component controls and status reads.
- Adds strict endpoint and field compatibility requirements for property-based DisplayInfo access (`DisplayInfo.1.<property>`) and nested Thunder response parsing paths.
- Requires regression validation evidence and code-quality checks proving there are no leftover libds calls in migrated Thunder codepaths.
