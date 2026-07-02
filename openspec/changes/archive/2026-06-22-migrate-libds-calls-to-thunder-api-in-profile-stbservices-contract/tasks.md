## 1. Thunder Mapping Baseline

- [x] 1.1 Inventory STBService parameters that currently call libds in `src/hostif/profiles/STBService` and map each to the owning Thunder plugin domain.
- [x] 1.2 Validate Thunder method names, request keys, and response fields for each mapped parameter against plugin interfaces.
- [x] 1.3 Document any parameters with no direct Thunder equivalent and define expected fault handling behavior.
- [x] 1.4 Lock endpoint naming and field conventions from the latest prompt, including `DisplayInfo.1.<property>` and `getCurrentResolution` structured response fields (`w`, `h`, `progressive`).
- [x] 1.5 Capture libds-to-Thunder mapping updates for newly clarified methods (for example `setMS12AudioCompression`, `getGain`, `setDialogEnhancement`, `getSupportedSettopResolutions`, `getVideoCodecInfo`).

## 2. STBService Component Refactor

- [x] 2.1 Update targeted `Components_<Module>.h` files to replace libds port references with Thunder-compatible instance state (`m_portName` or singleton shape).
- [x] 2.2 Implement `Components_<Module>_Thunder.cpp` migrations for getter/setter paths, including Thunder invocation and typed extraction.
- [x] 2.3 Implement Thunder-backed instance lifecycle (`buildPortNameHash`, `getInstance`, `closeAllInstances`) for port and non-port module patterns.
- [x] 2.4 Wrap unsupported legacy libds-only functions and call paths with compile-safe guards (`#if 0`) where required.
- [x] 2.5 For HDMI `ResolutionValue`, reconstruct canonical TR-69 format (`WxH[p|i]/FHz`) using `getCurrentResolution` fields plus `DisplayInfo.1.framerate`.
- [x] 2.6 Keep "under define" audio operations (`setDB/getDB/getMinDB/getMaxDB/setLoopThru/isLoopThru/getOptimalLevel`) as `NOT_HANDLED` and guarded in handler paths.

## 3. Legacy Isolation and Validation

- [x] 3.1 Gut migrated logic in original `Components_<Module>.cpp` files so active codepaths are Thunder-backed.
- [x] 3.2 Verify STBService read/write behavior for supported parameters, including invalid instance and non-writable error paths.
- [x] 3.3 Run build and targeted tests (`run_ut.sh` and relevant STBService coverage) to confirm no libds symbol regressions in migrated modules.
- [x] 3.4 Validate endpoint/response compatibility for DisplayInfo property methods and nested JSON extraction paths used by VideoOutput/HDCP flows.
- [x] 3.5 Execute no-regression parity checks against pre-migration TR-69 behavior for representative GET, SET, and fault cases across migrated modules.
- [ ] 3.6 Run explicit no-leftover-libds scan for migrated Thunder codepaths (`Components_*_Thunder.cpp`) and resolve any residual `device::` / DS-HAL references.
- [ ] 3.7 Perform maintainability/readability review across migrated modules (consistent constants, helper patterns, and error/fault handling style).

## 4. Contract and Documentation Updates

- [x] 4.1 Align STBService contract documentation with Thunder plugin mapping and instance construction rules.
- [x] 4.2 Capture migration notes for module-specific caveats and operational troubleshooting in docs where needed.
- [x] 4.3 Update migration contract notes with latest reference implementations and mapping-table status (Defined vs Under define).
- [ ] 4.4 Record zero-regression validation evidence and no-leftover-libds scan results in migration notes before closure.
