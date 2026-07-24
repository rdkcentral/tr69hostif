# STBService Thunder Migration Mapping

## Scope

This document inventories STBService components that currently use libds (`device::` HAL) and maps each component to the target Thunder plugin domain.

Source review performed against:
- `src/hostif/profiles/STBService/Components_*.cpp`
- `src/hostif/profiles/STBService/Components_*.h`
- `openspec/specs/profile-stbservice-contract/tr69hostif-thunder-migration.prompt.md`

## Component-to-Plugin Mapping

| Component | Current backend pattern | Target Thunder plugin | Locked methods / response fields |
|---|---|---|---|
| `Components_AudioOutput` | `device::AudioOutputPort`, `device::Host::getAudioOutputPorts()` | `org.rdk.DisplaySettings` | `getSupportedAudioPorts`→`supportedAudioPorts`; `getEnableAudioPort`→`enable` / `setEnableAudioPort`; `getMuted`→`muted` / `setMuted`; `getVolumeLevel`→`volumeLevel` / `setVolumeLevel`; `getAudioEncoding`→`audioEncoding` / `setAudioEncoding`; `getAudioFormat`→`audioFormat`; `getSoundMode`→`soundMode` / `setSoundMode`; `getAudioCompression`→`compressionLevel` / `setAudioCompression`; `getDialogEnhancement`→`dialogEnhancementlevel` / `setDialogEnhancement` |
| `Components_SPDIF` | `device::AudioOutputPort`, SPDIF-specific port selection | `org.rdk.DisplaySettings` | `getSupportedAudioPorts`→`supportedAudioPorts` (SPDIF filter); `getEnableAudioPort`→`enable` / `setEnableAudioPort`; `getAudioEncoding`→`audioEncoding` / `setAudioEncoding` (PCM=ForcePCM, non-PCM=passthrough); `getAudioDelay`→`audioDelay` |
| `Components_HDMI` | `device::VideoOutputPort`, resolution control via DS HAL | `org.rdk.DisplaySettings` | `getSupportedVideoDisplays`→`supportedVideoDisplays` (port discovery); `getCurrentResolution`→`resolution` / `setCurrentResolution`; enable/disable: `getEnableAudioPort`-style on video port using `getVideoPortStatusInfo`→`isConnected`; `getDefaultResolution`→`defaultResolution` |
| `Components_DisplayDevice` | `device::VideoOutputPort` display capabilities | `org.rdk.DisplayInfo` | `DisplayInfo.1.displayinfo`→`connected`, `width`, `height`, `hdrtype`, `hdcpprotection`; `DisplayInfo.1.connected`→bool; `DisplayInfo.1.width` / `DisplayInfo.1.height` (pixel resolution for SupportedResolutions/PreferredResolution strings). EDID fields (`EEDID`, `X_COMCAST-COM_EDID`, `EDID_BYTES`) have no Thunder equivalent — guard with `#if 0` |
| `Components_VideoOutput` | `device::VideoOutputPort`, HDCP status | `org.rdk.DisplaySettings` + `org.rdk.HdcpProfile` | DisplaySettings: `getSupportedVideoDisplays`→port discovery; `getCurrentResolution`→`resolution` (DisplayFormat); `getVideoFormat`→`videoFormat`. HdcpProfile: `getHDCPStatus`→`isHDCPEnabled`, `currentHDCPVersion`, `isHDCPCompliant`, `isConnected`. AspectRatioBehaviour: `org.rdk.AVOutput.getZoomMode`→`zoomMode` |
| `Components_VideoDecoder` | `device::Host::getPowerMode`/`setPowerMode`, `device::VideoOutputPort::Display::getAspectRatio` | `org.rdk.PowerManager` + `org.rdk.DisplayInfo` | PowerManager: `GetPowerState`→`currentState` (`POWER_STATE_STANDBY`=standby on/off) / `SetPowerState`; DisplayInfo: `DisplayInfo.1.displayinfo`→`width`/`height` for aspect ratio approximation. ContentAspectRatio and HEVC flag: no direct Thunder equivalent — return `NOT_HANDLED` |
| `Components_XrdkEMMC` | `rdkStorageMgr` (not libds) | No Thunder migration in this change | Continue using storage HAL-backed path |
| `Components_XrdkSDCard` | `rdkStorageMgr` (not libds) | No Thunder migration in this change | Continue using storage HAL-backed path |

## libds Usage Inventory Notes

- Port-reference fields still present in headers:
  - `device::AudioOutputPort& aPort` in AudioOutput and SPDIF
  - `device::VideoOutputPort& vPort` in HDMI, DisplayDevice, and VideoOutput
- Runtime instance creation still uses libds host lookups in component constructors/getInstance flows.
- No `Components_*_Thunder.cpp` files currently exist under `src/hostif/profiles/STBService`.

## No Direct Thunder Equivalent (Current Gaps)

The following parameter areas have no confirmed one-to-one Thunder API in the migration guide and require explicit fallback/fault behavior during implementation:

- DisplayDevice EDID-oriented fields:
  - `EEDID`, `X_COMCAST-COM_EDID`, `EDID_BYTES`
- DisplayDevice sink capability fields:
  - `CECSupport`, `AutoLipSyncSupport`, `HDMI3DPresent`, `VideoLatency`
- VideoDecoder codec capability-style fields where source is DS capability introspection rather than a direct Thunder property.

## Recommended Fault Handling for Unmapped Fields

- Return explicit non-writable or invalid-parameter style faults for unsupported SET requests.
- Return explicit backend failure faults for unmapped/unsupported GET requests rather than stale synthetic defaults.
- Keep parameter presence stable in the TR-69 tree; only backend behavior and fault semantics change.