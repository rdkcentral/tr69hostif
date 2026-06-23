# Components_AudioOutput — Pending Thunder Migration

The following APIs in `Components_AudioOutput_Thunder.cpp` still use **libds** (`aPort.*`)
because no equivalent Thunder (`org.rdk.DisplaySettings`) API exists yet.

These must be migrated once the corresponding Thunder APIs are available.

---

## Remaining APIs (libds → Thunder)

| # | TR-181 Parameter | Function | Current libds call | Required Thunder API |
|---|---|---|---|---|
| 1 | `AudioFormat` | `getX_COMCAST_COM_AudioFormat` | `aPort.getEncoding().getId()` | `getAudioFormat` _(TBD)_ |
| 2 | `X_COMCAST-COM_AudioDB` (set) | `setX_COMCAST_COM_AudioDB` | `aPort.setDB(value)` | `setAudioDB` _(TBD)_ |
| 3 | `X_COMCAST-COM_AudioDB` (get) | `getX_COMCAST_COM_AudioDB` | `aPort.getDB()` | `getAudioDB` _(TBD)_ |
| 4 | `X_COMCAST-COM_MinAudioDB` | `getX_COMCAST_COM_MinAudioDB` | `aPort.getMinDB()` | `getMinAudioDB` _(TBD)_ |
| 5 | `X_COMCAST-COM_MaxAudioDB` | `getX_COMCAST_COM_MaxAudioDB` | `aPort.getMaxDB()` | `getMaxAudioDB` _(TBD)_ |
| 6 | `X_COMCAST-COM_AudioLoopThru` (set) | `setX_COMCAST_COM_AudioLoopThru` | `aPort.setLoopThru(value)` | `setAudioLoopThru` _(TBD)_ |
| 7 | `X_COMCAST-COM_AudioLoopThru` (get) | `getX_COMCAST_COM_AudioLoopThru` | `aPort.isLoopThru()` | `getAudioLoopThru` _(TBD)_ |
| 8 | `X_COMCAST-COM_AudioOptimalLevel` | `getX_COMCAST_COM_AudioOptimalLevel` | `aPort.getOptimalLevel()` | `getAudioOptimalLevel` _(TBD)_ |

---

## Migration Steps (when Thunder API is available)

1. Add the new Thunder method call using `invokeThunderPluginMethod*` helpers from `hostIf_utils.h`
2. Replace the `aPort.*` libds call with the Thunder call
3. Remove the `/* TODO */` comment
4. Remove the libds dependency (`aPort` member) once all 8 are migrated

---

## Already Migrated

| TR-181 Parameter | Function | Thunder API |
|---|---|---|
| `Status` | `getStatus` | `getEnableAudioPort` + `getMuted` |
| `CancelMute` (set/get) | `setCancelMute` / `getCancelMute` | `setMuted` / `getMuted` |
| `X_COMCAST-COM_AudioStereoMode` (set/get) | `setX_COMCAST_COM_AudioStereoMode` / `get` | `setSoundMode` / `getSoundMode` |
| `AudioEncoding` (set/get) | `setAudioEncoding` / `getX_COMCAST_COM_AudioEncoding` | `setAudioEncoding` / `getAudioEncoding` |
| `X_COMCAST-COM_AudioCompression` (set/get) | `setX_COMCAST_COM_AudioCompression` / `get` | `setMS12AudioCompression` / `getMS12AudioCompression` |
| `AudioLevel` (set/get) | `setAudioLevel` / `getAudioLevel` | `setVolumeLevel` / `getVolumeLevel` |
| `X_COMCAST-COM_AudioGain` | `getX_COMCAST_COM_AudioGain` | `getGain` |
| `X_COMCAST-COM_DialogEnhancement` (set/get) | `setX_COMCAST_COM_DialogEnhancement` / `get` | `setDialogEnhancement` / `getDialogEnhancement` |
| `Enable` | `getEnable` | hardcoded `true` |
| `Name` | `getName` | `getSupportedAudioPorts` (via `portNameHash`) |
