# STBService Thunder Migration — TR-069 Host-IF

## Overview

This document describes the migration of the TR-069 `STBService.Components.*` profiles
from the **libds** device-settings HAL (`device::` C++ namespace) to the
**Thunder plugin** framework (`invokeThunderPluginMethod*` utilities from `hostIf_utils.h`).

Four component modules were migrated. Each original `.cpp` file has a corresponding
`_Thunder.cpp` replacement that provides the same `handleGetMsg` / `handleSetMsg`
interface while routing all HAL queries through Thunder JSON-RPC calls.

---

## Build Integration

### Enabling the Thunder build

Pass `--enable-rdk-thunder-plugin` to `./configure`:

```sh
./configure --enable-rdk-thunder-plugin [other options]
```

This sets the autoconf variable `RDK_THUNDER_PLUGIN_ENABLE=yes` and activates
the `WITH_RDK_THUNDER_PLUGIN` Makefile conditional in
`src/hostif/profiles/STBService/Makefile.am`.

### Makefile.am conditional

```makefile
if WITH_RDK_THUNDER_PLUGIN
    # Thunder implementations (no libds dependency at link time)
    libstbservice_la_SOURCES = Components_AudioOutput_Thunder.cpp \
                               Components_SPDIF_Thunder.cpp \
                               Components_HDMI_Thunder.cpp \
                               Components_DisplayDevice_Thunder.cpp \
                               Components_VideoOutput_Thunder.cpp \
                               Components_VideoDecoder_Thunder.cpp \
                               Capabilities.cpp
else
    # Original libds implementations
    libstbservice_la_SOURCES = Components_AudioOutput.cpp \
                               Components_SPDIF.cpp \
                               Components_HDMI.cpp \
                               Components_DisplayDevice.cpp \
                               Components_VideoOutput.cpp \
                               Components_VideoDecoder.cpp \
                               Capabilities.cpp
endif
```

---

## Thunder Helper Utilities

All Thunder calls use helpers declared in `hostIf_utils.h`:

| Helper | Description |
|---|---|
| `invokeThunderPluginMethod(method, paramsJson, response)` | Raw JSON-RPC call; response is full JSON string |
| `invokeThunderPluginMethodAndExtractStringField(method, params, field, out)` | Extracts a top-level string field from response |
| `invokeThunderPluginMethodAndExtractBoolField(method, params, field, out)` | Extracts a top-level bool field |
| `invokeThunderPluginMethodAndExtractNumberField(method, params, field, out)` | Extracts a top-level number field |
| `invokeThunderPluginMethodAndExtractDelimitedStringArrayField(method, params, field, delimiter, out)` | Joins a JSON string array into a delimited string |

---

## Module: Components_DisplayDevice

**Files changed:**
- `Components_DisplayDevice.h` — constructor signature updated; `m_portName` replaces `device::VideoOutputPort& vPort`
- `Components_DisplayDevice_Thunder.cpp` — new file (replaces `Components_DisplayDevice.cpp`)

**Port discovery:** instantiated by `hostIf_STBServiceHDMI` constructor; one `DisplayDevice` instance per HDMI port.

### API Mapping

| TR-069 Parameter | libds (original) | Thunder API | Plugin | Notes |
|---|---|---|---|---|
| `Status` | `vPort.isDisplayConnected()` | `org.rdk.DisplaySettings.getConnectedVideoDisplays` | entservices-displaysettings | Check if `m_portName` is in `"connectedVideoDisplays"` array → "Present"/"Absent" |
| `EEDID` / `X_COMCAST-COM_EDID` | parsed EDID bytes → `"pcode=0x%x,..."` | `org.rdk.DisplaySettings.readEDID` | entservices-displaysettings | Returns base64-encoded EDID blob via `"EDID"` field. **Format change**: original returned parsed fields. |
| `EDID_BYTES` | raw bytes as uppercase hex string | `org.rdk.DisplaySettings.readEDID` | entservices-displaysettings | Same base64 blob as above. **Format change**: original returned hex. |
| `SupportedResolutions` | `vPort.getDisplay().getSupportedResolutions()` | `org.rdk.DisplaySettings.getSupportedResolutions {"videoDisplay":"<port>"}` | entservices-displaysettings | `"supportedResolutions"` array joined with commas |
| `PreferredResolution` | `vPort.getDisplay().getDefaultResolution().getName()` | `org.rdk.DisplaySettings.getCurrentResolution {"videoDisplay":"<port>"}` | entservices-displaysettings | `"resolution"` string field |
| `VideoLatency` | `vPort.getDisplay().getVideoLatency()` | — | — | **Not migrated** — no Thunder equivalent → `NOT_HANDLED` |
| `CECSupport` | `vPort.getDisplay().hasCECSupport()` | — | — | **Not migrated** → `NOT_HANDLED` |
| `AutoLipSyncSupport` | `vPort.getDisplay().hasAutoLipSyncSupport()` | — | — | **Not migrated** → `NOT_HANDLED` |
| `HDMI3DPresent` | `vPort.getDisplay().hasHDMI3DSupport()` | — | — | **Not migrated** → `NOT_HANDLED` |

---

## Module: Components_HDMI -- Done code check.

**Files changed:**
- `Components_HDMI.h` — constructor updated; `m_portName` replaces `vPort`; `buildPortNameHash()` added; `HDMI_RESOLUTION_VALUE_DEFAULT` fixed to `"1280x720p/59.94Hz"`
- `Components_HDMI_Thunder.cpp` — new file (replaces `Components_HDMI.cpp`)

**Port discovery:** `buildPortNameHash()` calls `getSupportedVideoDisplays` and filters for ports containing `"hdmi"` (case-insensitive). Each HDMI port becomes one `hostIf_STBServiceHDMI` instance starting at `dev_id=1`.

### API Mapping

| TR-069 Parameter | libds (original) | Thunder API | Plugin | Notes |
|---|---|---|---|---|
| `Enable` (GET) | `vPort.isEnabled()` | `org.rdk.DisplaySettings.getEnableVideoPort {"videoDisplay":"<port>"}` | entservices-displaysettings | `"enable"` bool field |code-Migrated-correct [gets value without calls isDisplayConnected.need to check output] 
| `Enable` (SET) | `vPort.enable()` / `vPort.disable()` | `org.rdk.DisplaySettings.setEnableVideoPort {"videoDisplay":"<port>","enable":<bool>}` | entservices-displaysettings | Checks `"success"` in response | code-Migrated-correct
| `Status` | `vPort.isEnabled()` → "Enabled"/"Disabled" | Derived from `getEnableVideoPort` | entservices-displaysettings | Maps `enable` bool → "Enabled"/"Disabled" | code-Migrated-correct
| `Name` | `vPort.getName()` | — | — | In-memory: returns `m_portName` directly |code-Migrated-correct
| `ResolutionMode` (GET) | static `dsHDMIResolutionMode[]` | — | — | In-memory static (same as original) |code-Migrated-correct
| `ResolutionMode` (SET) | sets static; if Auto triggers default resolution | In-memory + `setCurrentResolution` if mode=Auto | entservices-displaysettings | Default resolution: `"1280x720p/59.94Hz"` |code-Migrated-correct
| `ResolutionValue` (GET) | `vPort.getResolution().getName()` | `org.rdk.DisplaySettings.getCurrentResolution {"videoDisplay":"<port>"}` | entservices-displaysettings | `"resolution"` string field |code-Migrated-correct
| `ResolutionValue` (SET) | `vPort.setResolution(...)` | `org.rdk.DisplaySettings.setCurrentResolution {"videoDisplay":"<port>","resolution":"<name>"}` | entservices-displaysettings | Only settable when `ResolutionMode=Manual` |
| `DisplayDevice.*` | delegated to `hostIf_STBServiceDisplayDevice` | Delegated to `Components_DisplayDevice_Thunder` | (see above) | — |code-Migrated-correct

---

## Module: Components_VideoOutput

**Files changed:**
- `Components_VideoOutput.h` — constructor updated; `m_portName` replaces `vPort`; `buildPortNameHash()` and `getNumberOfInstances()` added
- `Components_VideoOutput_Thunder.cpp` — new file (replaces `Components_VideoOutput.cpp`)

**Port discovery:** `buildPortNameHash()` calls `getSupportedVideoDisplays` and includes **all** ports (no HDMI filter), unlike the HDMI module.

### API Mapping

| TR-069 Parameter | libds (original) | Thunder API | Plugin | Notes |
|---|---|---|---|---|
| `Status` | `vPort.isEnabled()` → "Enabled"/"Disabled" | `org.rdk.DisplaySettings.getEnableVideoPort {"videoDisplay":"<port>"}` | entservices-displaysettings | `"enable"` bool → "Enabled"/"Disabled" |
| `Enable` | hardcoded `true` | — | — | Hardcoded (same as original) |
| `DisplayFormat` | `vPort.getResolution().getName()` | `org.rdk.DisplaySettings.getCurrentResolution {"videoDisplay":"<port>"}` | entservices-displaysettings | `"resolution"` string field |
| `VideoFormat` | hardcoded `"HDMI"` | — | — | Hardcoded (same as original) |
| `AspectRatioBehaviour` | `device::VideoDevice::getDFC().getName()` | `org.rdk.DisplaySettings.getZoomSetting {}` | entservices-displaysettings | `"zoomSetting"` string (device-wide) |
| `HDCP` | `vPort.getHDCPStatus()` | `org.rdk.HdcpProfile.getHDCPStatus {}` | entservices-hdcpprofile | Parses `"HDCPStatus"."isHDCPEnabled"` via `strstr` (nested JSON) |
| `Name` | `vPort.getName()` | — | — | In-memory: returns `m_portName` |
| `NumberOfInstances` (static) | `device::Host::getInstance().getVideoOutputPorts().size()` | `g_hash_table_size(ifHash)` after `buildPortNameHash()` | entservices-displaysettings | Static method, requires `buildPortNameHash()` first |

### HDCP parsing note

`getHDCPStatus` returns a **nested** JSON object:
```json
{ "HDCPStatus": { "isHDCPEnabled": true, "isConnected": true, ... }, "success": true }
```
Because `isHDCPEnabled` is nested inside `HDCPStatus`, the standard
`invokeThunderPluginMethodAndExtractBoolField` cannot access it directly.
The implementation uses raw `invokeThunderPluginMethod` and `strstr` to locate
`"isHDCPEnabled":` in the response string.

---

## Module: Components_VideoDecoder

**Files changed:**
- `Components_VideoDecoder.h` — `buildSingleInstance()` added as private static method
- `Components_VideoDecoder_Thunder.cpp` — new file (replaces `Components_VideoDecoder.cpp`)

**Port discovery:** VideoDecoder is device-wide (not per-port). `buildSingleInstance()` creates exactly one instance at `dev_id=1`.

### API Mapping

| TR-069 Parameter | libds (original) | Thunder API | Plugin | Notes |
|---|---|---|---|---|
| `X_COMCAST-COM_Standby` (GET) | `PowerManagerClient::getState()` | `org.rdk.PowerManager.GetPowerState {}` | entservices-powermanager | `currentState` is "STANDBY"/"DEEP_SLEEP"/"LIGHT_SLEEP" → `true` |
| `X_COMCAST-COM_Standby` (SET) | `PowerManagerClient::setState()` | `org.rdk.PowerManager.SetPowerState {"keyCode":0,"powerState":"STANDBY"\|"ON","reason":"TR69"}` | entservices-powermanager | `true` → STANDBY, `false` → ON |
| `Status` | power state mapped to string | `org.rdk.PowerManager.GetPowerState {}` | entservices-powermanager | ON→"Enabled", STANDBY/DEEP\_SLEEP/LIGHT\_SLEEP→"X_COMCAST-COM_Standby", OFF→"Disabled", other→"Error" |
| `ContentAspectRatio` | `vPort.getDisplay().getDisplayAspectRatio()` | `DisplayInfo.1.displayinfo {}` | entservices-displayinfo | Parses `"width"`/`"height"` from response; width/height > 1.55 → "16x9", else "4x3" |
| `Enable` | hardcoded `true` | — | — | Hardcoded (same as original) |
| `Name` | hardcoded `"VideoDecoderHDMI0"` | — | — | Hardcoded (same as original) |
| `X_RDKCENTRAL-COM_MPEGHPart2` | hardcoded path string | — | — | Hardcoded (same as original) |

### Power state strings

`org.rdk.PowerManager.GetPowerState` returns `"currentState"` with one of:
`"ON"`, `"STANDBY"`, `"DEEP_SLEEP"`, `"LIGHT_SLEEP"`, `"OFF"`, `"UNKNOWN"`

The response JSON is parsed with `strstr` (raw) since the PowerManager response
does not use nested objects:
```json
{ "currentState": "ON", "prevState": "STANDBY", "success": true }
```

---

## Known Differences / Behaviour Changes

| Module | Parameter | Original behaviour | Thunder behaviour |
|---|---|---|---|
| DisplayDevice | `EEDID` / `X_COMCAST-COM_EDID` | Returned `"pcode=0x%x,pserial=0x%x,year=%d,week=%d"` parsed from EDID bytes | Returns raw base64-encoded EDID blob from `readEDID` |
| DisplayDevice | `EDID_BYTES` | Returned EDID as uppercase hex string | Returns raw base64-encoded EDID blob from `readEDID` |
| DisplayDevice | `VideoLatency`, `CECSupport`, `AutoLipSyncSupport`, `HDMI3DPresent` | Returned values from libds | Returns `NOT_HANDLED` — no Thunder equivalent API |
| VideoDecoder | `ContentAspectRatio` | Used display device connected to HDMI output | Uses `DisplayInfo.1.displayinfo` display width/height; no per-port selection |

---

## File Summary

| File | Type | Status | Description |
|---|---|---|---|
| `Components_DisplayDevice.h` | Header | **Modified** | Constructor `(int, const std::string&)`; `m_portName` member; removed `device::VideoOutputPort&` |
| `Components_HDMI.h` | Header | **Modified** | Constructor updated; `buildPortNameHash()` added; `HDMI_RESOLUTION_VALUE_DEFAULT` corrected to `"1280x720p/59.94Hz"` |
| `Components_VideoOutput.h` | Header | **Modified** | Constructor updated; `buildPortNameHash()`, `getNumberOfInstances()` added |
| `Components_VideoDecoder.h` | Header | **Modified** | `buildSingleInstance()` private static method added |
| `Components_DisplayDevice_Thunder.cpp` | Source | **New** | Thunder implementation; uses `org.rdk.DisplaySettings` |
| `Components_HDMI_Thunder.cpp` | Source | **New** | Thunder implementation; uses `org.rdk.DisplaySettings` |
| `Components_VideoOutput_Thunder.cpp` | Source | **New** | Thunder implementation; uses `org.rdk.DisplaySettings` + `org.rdk.HdcpProfile` |
| `Components_VideoDecoder_Thunder.cpp` | Source | **New** | Thunder implementation; uses `org.rdk.PowerManager` + `DisplayInfo.1` |
| `Makefile.am` | Build | **Modified** | `WITH_RDK_THUNDER_PLUGIN` conditional selects Thunder or libds sources |
| `configure.ac` | Build | **Modified** | `--enable-rdk-thunder-plugin` AC option + `AM_CONDITIONAL` added |
