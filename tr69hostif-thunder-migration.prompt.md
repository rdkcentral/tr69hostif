---
description: "Migrate a tr69hostif STBService plugin from libds (device:: HAL) to the appropriate Thunder plugin (DisplaySettings, AVOutput, DisplayInfo, HdcpProfile, or PowerManager)"
---

# tr69hostif Thunder Migration

Migrate the plugin **`${input:module}`** from libds (`device::` HAL) to the appropriate Thunder plugin.
Focus on no regression and persistence across restart/reboot scenarios

## Context

**Workspace root**: `/mnt/home/gsanto722/iarmmgr/non-thunder-clients-migration/`

**Files to touch** (replace `<Module>` with the module name, e.g. HDMI, AudioOutput, SPDIF):
- Header  : `tr69hostif/src/hostif/profiles/STBService/Components_<Module>.h`
- Thunder : `tr69hostif/src/hostif/profiles/STBService/Components_<Module>_Thunder.cpp`
- Original: `tr69hostif/src/hostif/profiles/STBService/Components_<Module>.cpp`

**Reference implementations (already migrated — read these first)**:
- `Components_AudioOutput_Thunder.cpp` + `Components_AudioOutput.h`  — audio port, `org.rdk.DisplaySettings`, no port filter
- `Components_SPDIF_Thunder.cpp` + `Components_SPDIF.h`  — audio port, `org.rdk.DisplaySettings`, SPDIF-filtered
- `Components_HDMI_Thunder.cpp` + `Components_HDMI.h`  — video port, `org.rdk.DisplaySettings`, HDMI-filtered; uses `DisplayInfo.1.framerate` for frame rate reconstruction
- `Components_DisplayDevice_Thunder.cpp` + `Components_DisplayDevice.h`  — display sub-object (no own hash), instantiated by HDMI; `org.rdk.DisplaySettings`
- `Components_VideoOutput_Thunder.cpp` + `Components_VideoOutput.h`  — video port, `org.rdk.DisplaySettings` + `org.rdk.HdcpProfile`, all ports; raw `strstr` parsing for nested JSON
- `Components_VideoDecoder_Thunder.cpp` + `Components_VideoDecoder.h`  — single instance (device-wide), `org.rdk.PowerManager` + `DisplayInfo.1.displayinfo`

**Thunder helper utilities** (declared in `hostIf_utils.h`):
```cpp
bool invokeThunderPluginMethod(const std::string& method, const std::string& params, std::string& response);
bool invokeThunderPluginMethodAndExtractBoolField(const std::string& method, const std::string& params, const std::string& field, bool& out);
bool invokeThunderPluginMethodAndExtractStringField(const std::string& method, const std::string& params, const std::string& field, std::string& out);
bool invokeThunderPluginMethodAndExtractNumberField(const std::string& method, const std::string& params, const std::string& field, double& out);
bool invokeThunderPluginMethodAndExtractDelimitedStringArrayField(const std::string& method, const std::string& params, const std::string& field, const std::string& delimiter, std::string& out);
```

---

## Plugin Selection Guide

Before writing any code, determine which Thunder plugin owns the TR-69 parameter being migrated.

| Plugin callsign | Domain | Port discovery | Source repo |
|---|---|---|---|
| `org.rdk.DisplaySettings` | Audio/video **port** settings: encoding, volume, sound mode, loudness, audio delay, compression, MS12, HDMI resolution | `getSupportedAudioPorts` (audio) or `getSupportedVideoDisplays` (video) — filter by port type if needed | `entservices-displaysettings/` |
| `org.rdk.AVOutput` | **TV picture quality**: backlight, brightness, contrast, sharpness, saturation, hue, color temperature, DolbyVision mode, picture mode, zoom mode, video format, MEMC, DNR, LowLatency, HDR mode, WB, CMS | No port — TV-wide settings; no `buildPortNameHash` needed | `entservices-avoutput/` |
| `org.rdk.DisplayInfo` | **Display info**: GPU RAM, resolution (width/height in pixels), HDMI connection status, audio passthrough, HDCP protection level, HDR type | No port — property-based JSON-RPC | `entservices-displayinfo/` |
| `org.rdk.HdcpProfile` | **HDCP authentication**: isConnected, isHDCPCompliant, isHDCPEnabled, hdcpReason, supportedHDCPVersion, receiverHDCPVersion, currentHDCPVersion | No port — single device | `entservices-hdcpprofile/` |
| `org.rdk.PowerManager` | **Power state**: ON/STANDBY/DEEP_SLEEP, network standby mode, reboot, deep sleep timer, thermal thresholds | No port | `entservices-powermanager/` |

### Quick identification rules

- Parameter name contains **Audio**, **Volume**, **SoundMode**, **SPDIF**, **HDMI audio** → `org.rdk.DisplaySettings`
- Parameter name contains **Backlight**, **Brightness**, **Contrast**, **Sharpness**, **Saturation**, **Hue**, **ColorTemp**, **DolbyVision**, **PictureMode**, **VideoFormat** (TV-side) → `org.rdk.AVOutput`
- Parameter name contains **GPURam**, **DisplayResolution**, **Connected** (display), **HDRType**, **AudioPassthrough** → `org.rdk.DisplayInfo`
- Parameter name contains **HDCP**, **HDCPStatus**, **HDCPCompliant**, **HDCPVersion** → `org.rdk.HdcpProfile`
- Parameter name contains **PowerState**, **StandbyMode**, **DeepSleep**, **Reboot** → `org.rdk.PowerManager`

### Key API reference

**`org.rdk.DisplaySettings`** (grep `entservices-displaysettings/plugin/DisplaySettings.cpp` for `registerMethod`):
- Audio: `getSupportedAudioPorts`, `getAudioEncoding`/`setAudioEncoding`, `getVolumeLevel`/`setVolumeLevel`, `getSoundMode`/`setSoundMode`, `getAudioDelay`/`setAudioDelay`, `setEnableAudioPort`/`getEnableAudioPort`, `getAudioFormat`, `getMSSettings`/`setMSSettings`, `getLoudnessEquivalenceEnabled`/`setLoudnessEquivalenceEnabled`, `getMuted`/`setMuted`, `getGain`, `setDolbyVolumeMode`, `setMS12AudioCompression`, `setDialogEnhancement`/`getDialogEnhancement`
- Video: `getSupportedVideoDisplays`, `getResolutionList`, `getCurrentResolution`/`setCurrentResolution`, `getVideoFormat`, `isConnectedDeviceRepeater`, `getConnectedVideoDisplays`, `readEDID`, `getSupportedResolutions`, `getEnableVideoPort`/`setEnableVideoPort`, `getZoomSetting`/`setZoomSetting`, `getDisplayAspectRatio`, `getSupportedSettopResolutions`, `getSupportedVideoCodingFormats`, `getVideoCodecInfo`
- `getCurrentResolution` response: `{ "resolution": "1080p", "w": 1920, "h": 1080, "progressive": true, "success": true }` — use `w`, `h`, `progressive` fields plus `DisplayInfo.1.framerate` to reconstruct the full `"WxHp/FHz"` format string (e.g. `"1920x1080p/60Hz"`)
- **Under define** (no Thunder API yet — return `NOT_HANDLED`): `setDB`, `getDB`, `getMinDB`, `getMaxDB`, `setLoopThru`, `isLoopThru`, `getOptimalLevel`

**`org.rdk.AVOutput`** (methods from `entservices-avoutput/AVOutputTV.cpp`):
- Picture: `getBacklight`/`setBacklight`/`resetBacklight`, `getBrightness`/`setBrightness`/`resetBrightness`, `getContrast`/`setContrast`/`resetContrast`, `getSharpness`/`setSharpness`/`resetSharpness`, `getSaturation`/`setSaturation`/`resetSaturation`, `getHue`/`setHue`/`resetHue`, `getColorTemperature`/`setColorTemperature`/`resetColorTemperature`
- Display mode: `getDolbyVisionMode`/`setDolbyVisionMode`/`resetDolbyVisionMode`, `getSupportedDolbyVisionModes`, `getPictureMode`/`setPictureMode`/`resetPictureMode`, `getSupportedPictureModes`, `getZoomMode`/`setZoomMode`/`resetZoomMode`, `getHDRMode`/`setHDRMode`/`resetHDRMode`
- Backlight: `getBacklightDimmingMode`/`setBacklightDimmingMode`, `getBacklightDimmingLevel`/`setBacklightDimmingLevel`, `getAutoBacklightMode`/`setAutoBacklightMode`
- Advanced: `getAISuperResolution`/`setAISuperResolution`, `getLocalContrastEnhancement`/`setLocalContrastEnhancement`, `getLowLatencyState`/`setLowLatencyState`, `getMEMC`/`setMEMC`, `getMPEGNoiseReduction`/`setMPEGNoiseReduction`, `getDigitalNoiseReduction`/`setDigitalNoiseReduction`, `getSDRGamma`/`setSDRGamma`
- Info: `getVideoFormat`, `getVideoResolution`, `getVideoFrameRate`, `getVideoSource`, `getVideoContentType`, `get2PointWB`/`set2PointWB`, `getCMS`/`setCMS`, `getWBMode`/`setWBMode`
- Caps: `getBacklightCaps`, `getBrightnessCaps`, `getContrastCaps`, `getDolbyVisionCalibrationCaps`, `getPictureModeCaps`, etc.

**`org.rdk.DisplayInfo`** (property-based — endpoint format `DisplayInfo.1.<property>`):
- `DisplayInfo.1.displayinfo` — composite: `totalgpuram`, `freegpuram`, `audiopassthrough`, `connected`, `width`, `height`, `hdcpprotection`, `hdrtype`
- `DisplayInfo.1.connected` — bool: display connected on primary output
- `DisplayInfo.1.totalgpuram` / `DisplayInfo.1.freegpuram` — uint64: GPU RAM bytes
- `DisplayInfo.1.width` / `DisplayInfo.1.height` — uint32: resolution in pixels
- `DisplayInfo.1.isaudiopassthrough` — bool: audio in passthrough mode
- `DisplayInfo.1.framerate` — double: display refresh rate in Hz (e.g. `60.0`, `59.94`); use `invokeThunderPluginMethodAndExtractNumberField` with field `"framerate"`
- Event: `updated` — fired on connection/resolution change

**`org.rdk.HdcpProfile`** (auto-generated from `Exchange::JHdcpProfile` — see `entservices-hdcpprofile/plugin/HdcpProfileImplementation.cpp`):
- Returns `HDCPStatus` struct: `isConnected`, `isHDCPCompliant`, `isHDCPEnabled`, `hdcpReason`, `supportedHDCPVersion` ("1.4" or "2.2"), `receiverHDCPVersion`, `currentHDCPVersion`
- No port argument — single device scope

**`org.rdk.PowerManager`** (auto-generated from `Exchange::JPowerManager` — see `entservices-powermanager/plugin/PowerController.h`):
- `SetPowerState(keyCode, powerState, reason)` / `GetPowerState(currentState, prevState)`
- `GetPowerStateBeforeReboot(state)`
- `SetNetworkStandbyMode(standbyMode)` / `GetNetworkStandbyMode(standbyMode)`
- `Reboot(requestor, reasonCustom, reasonOther)`
- `SetDeepSleepTimer(timeOut)`
- `GetThermalState` — returns CPU temperature (maps from libds `getCPUTemperature`)
- Power states: `POWER_STATE_ON`, `POWER_STATE_STANDBY`, `POWER_STATE_STANDBY_DEEP_SLEEP`, `POWER_STATE_UNKNOWN`

### libds → Thunder API mapping

Use this table when you encounter a libds `device::` call and need to find the Thunder equivalent quickly.

| libds method | Sub-component | Thunder Plugin API | Thunder Plugin | Status |
|---|---|---|---|---|
| `getCPUTemperature` | Host | `GetThermalState` | PowerManager | Defined |
| `enableMS12Config(DAPV2, enable)` | AudioOutputPort | `setDolbyVolumeMode` | DisplaySettings | Defined |
| `enableMS12Config(DE, enable)` | AudioOutputPort | `setDialogEnhancement` | DisplaySettings | Defined |
| `enableLEConfig` | AudioOutputPort | — | — | Not required |
| `setResolution` | VideoOutputPort | `setCurrentResolution` | DisplaySettings | Defined |
| `getPixelResolution` | VideoOutputPort | `getCurrentResolution` | DisplaySettings | Defined |
| `isInterlaced` | VideoOutputPort | `getVideoResolution` | AVOutput | Defined |
| `getFrameRate` | VideoOutputPort | `framerate` (property) | DisplayInfo | Defined |
| `enable` / `disable` | VideoOutputPort | `setEnableVideoPort` | DisplaySettings | Defined |
| `isDisplayConnected` | VideoOutputPort | `connected` (property) | DisplayInfo | Defined |
| `isEnabled` | VideoOutputPort | `getEnableVideoPort` | DisplaySettings | Defined |
| `getSupportedVideoCodingFormats` | VideoDevice | `getSupportedVideoCodingFormats` | DisplaySettings | Defined |
| `getVideoCodecInfo` | VideoDevice | `getVideoCodecInfo` | DisplaySettings | Defined |
| `getSettopSupportedResolutions` | VideoDevice | `getSupportedSettopResolutions` | DisplaySettings | Defined |
| `getProductCode` / `getEDIDBytes` | VideoOutputPort | `readEDID` | DisplaySettings | Defined |
| `getSerialNumber` / `getManufacturerWeek` / `getManufacturerYear` | VideoOutputPort | `readEDID` | DisplaySettings | Defined |
| `getSupportedResolutions` | VideoOutputPort | `getSupportedResolutions` | DisplaySettings | Defined |
| `setEncoding` / `getEncoding` | AudioOutputPort | `setAudioEncoding` / `getAudioEncoding` | DisplaySettings | Defined |
| `getHDCPStatus` | VideoOutputPort | `GetHDCPStatus` | HdcpProfile | Defined |
| `getAspectRatio` | Display | `getDisplayAspectRatio` | DisplaySettings | Defined |
| `getPowerMode` / `setPowerMode` | Host | `GetPowerState` / `SetPowerState` | PowerManager | Defined |
| `isEnabled()` | AudioOutputPort | `getEnableAudioPort` | DisplaySettings | Defined |
| `isMuted()` / `setMuted(toMute)` | AudioOutputPort | `getMuted` / `setMuted` | DisplaySettings | Defined |
| `setStereoMode(mode)` | AudioOutputPort | `setSoundMode` | DisplaySettings | Defined |
| `setCompression(newComp)` | AudioOutputPort | `setMS12AudioCompression` | DisplaySettings | Defined |
| `setLevel(newLevel)` / `getLevel()` | AudioOutputPort | `setVolumeLevel` / `getVolumeLevel` | DisplaySettings | Defined |
| `getGain()` | AudioOutputPort | `getGain` | DisplaySettings | Defined |
| `setDialogEnhancement()` / `getDialogEnhancement()` | AudioOutputPort | `setDialogEnhancement` / `getDialogEnhancement` | DisplaySettings | Defined |
| `getName()` | VideoOutputPort | `getSupportedVideoDisplays` | DisplaySettings | Defined |
| `getName()` | AudioOutputPort | `getSupportedAudioPorts` | DisplaySettings | Defined |
| `setDB` / `getDB` / `getMinDB` / `getMaxDB` | AudioOutputPort | — | DisplaySettings | **Under define** |
| `setLoopThru` / `isLoopThru` / `getOptimalLevel` | AudioOutputPort | — | DisplaySettings | **Under define** |

> **Under define**: No Thunder API exists yet for these. Return `NOT_HANDLED` and wrap the TR-69 parameter handler in `#if 0 / #endif` in the header.

---

## Step-by-step Migration Plan

### Step 1 — Identify plugin and discover Thunder APIs

1. Use the **Plugin Selection Guide** above to determine which plugin owns the module's parameters.
2. For `org.rdk.DisplaySettings`: search `entservices-displaysettings/plugin/DisplaySettings.cpp` for `registerMethod` calls matching each TR-69 parameter name.
3. For `org.rdk.AVOutput`: search `entservices-avoutput/AVOutputTV.cpp` for `registerMethod` calls.
4. For `org.rdk.DisplayInfo`: the API is property-based — use `invokeThunderPluginMethod` with endpoint `DisplayInfo.1.displayinfo` (or individual properties listed above).
5. For `org.rdk.HdcpProfile` / `org.rdk.PowerManager`: the API is auto-generated from Exchange interfaces — use method names exactly as they appear in `PowerController.h` or `HdcpProfileImplementation.cpp`.

For each TR-69 parameter, note:
- Thunder method name and full callsign (e.g., `org.rdk.DisplaySettings.getAudioEncoding`)
- JSON request fields (port name, value fields)
- JSON response fields and their types

### Step 2 — Update the Header (`Components_<Module>.h`)

#### Port-based modules (DisplaySettings: AudioOutput, SPDIF, VideoOutput)

Replace the libds port reference with an in-memory string:

```cpp
// BEFORE
class hostIf_STBService<Module>
{
    static GHashTable *ifHash;
    hostIf_STBService<Module>(int dev_id, device::<PortType>& port);
    static GMutex m_mutex;
    int dev_id;
    device::<PortType>& vPort;   // or aPort
    ...
};

// AFTER
class hostIf_STBService<Module>
{
    static GHashTable *ifHash;        /* dev_id → hostIf_STBService<Module>* */
    hostIf_STBService<Module>(int dev_id, const std::string& portName);
    static GMutex m_mutex;
    int dev_id;
    std::string m_portName;
    static void buildPortNameHash();
    ...
};
```

#### Non-port modules (AVOutput, DisplayInfo, HdcpProfile, PowerManager)

These have no per-port instances. Use a single-instance pattern:

```cpp
// AFTER — single instance, no portName needed
class hostIf_STBService<Module>
{
    static GHashTable *ifHash;        /* always one entry: dev_id=1 → instance */
    hostIf_STBService<Module>(int dev_id);
    static GMutex m_mutex;
    int dev_id;
    static void buildPortNameHash();  // creates one instance with dev_id=1
    ...
};
```

Wrap any function declarations that cannot yet be migrated in `#if 0 / #endif`.

### Step 3 — Create `Components_<Module>_Thunder.cpp`

Structure the file in this order:

#### 3a. Includes and defines
```cpp
#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "Components_<Module>.h"
#include <sstream>

#define PARAM1_STRING   "ParamName1"
#define PARAM2_STRING   "ParamName2"
// ... all parameter name strings used by handleSetMsg / handleGetMsg

#define BASE_NAME            "Device.Services.STBService.1.Components.<Module>"
#define UPDATE_FORMAT_STRING "%s.%d.%s"
#define DEV_NAME             "<MODULE>"

// ----- DisplaySettings (audio port modules: AudioOutput, SPDIF) -----
#define THUNDER_DS_GET_SUPPORTED_AUDIO_PORTS  "org.rdk.DisplaySettings.getSupportedAudioPorts"
#define THUNDER_DS_GET_XXX                    "org.rdk.DisplaySettings.getXxx"
#define THUNDER_DS_SET_XXX                    "org.rdk.DisplaySettings.setXxx"

// ----- DisplaySettings (video port modules: VideoOutput, HDMI) -----
#define THUNDER_DS_GET_SUPPORTED_VIDEO_DISPLAYS "org.rdk.DisplaySettings.getSupportedVideoDisplays"
#define THUNDER_DS_GET_CURRENT_RESOLUTION       "org.rdk.DisplaySettings.getCurrentResolution"

// ----- AVOutput (TV picture quality — no port argument) -----
#define THUNDER_AVO_GET_BACKLIGHT    "org.rdk.AVOutput.getBacklight"
#define THUNDER_AVO_SET_BACKLIGHT    "org.rdk.AVOutput.setBacklight"
#define THUNDER_AVO_GET_BRIGHTNESS   "org.rdk.AVOutput.getBrightness"
// ... add get/set/reset for each picture parameter

// ----- DisplayInfo (property-based — endpoint format: "DisplayInfo.1.<property>") -----
#define THUNDER_DI_DISPLAYINFO       "DisplayInfo.1.displayinfo"
#define THUNDER_DI_FRAMERATE         "DisplayInfo.1.framerate"
// Other individual properties: "DisplayInfo.1.connected", "DisplayInfo.1.width",
//   "DisplayInfo.1.height", "DisplayInfo.1.isaudiopassthrough"
// Use invokeThunderPluginMethod with "{}"; parse named fields from response.

// ----- HdcpProfile (single device — no port argument) -----
#define THUNDER_HDCP_GET_STATUS      "org.rdk.HdcpProfile.getHDCPStatus"
// Response fields: isConnected, isHDCPCompliant, isHDCPEnabled, hdcpReason,
//   supportedHDCPVersion, receiverHDCPVersion

// ----- PowerManager (no port argument) -----
#define THUNDER_PM_GET_POWER_STATE   "org.rdk.PowerManager.GetPowerState"
#define THUNDER_PM_SET_POWER_STATE   "org.rdk.PowerManager.SetPowerState"
#define THUNDER_PM_REBOOT            "org.rdk.PowerManager.Reboot"
#define THUNDER_PM_GET_NW_STANDBY    "org.rdk.PowerManager.GetNetworkStandbyMode"
#define THUNDER_PM_SET_NW_STANDBY    "org.rdk.PowerManager.SetNetworkStandbyMode"
```

#### 3b. Static definitions
```cpp
GHashTable * hostIf_STBService<Module>::ifHash = NULL;   /* dev_id → instance* */
GMutex hostIf_STBService<Module>::m_mutex;
```

#### 3c. Instance lifecycle — copy this pattern exactly
```cpp
hostIf_STBService<Module>* hostIf_STBService<Module>::getInstance(int dev_id)
{
    if (!ifHash)
        buildPortNameHash();
    hostIf_STBService<Module>* pRet =
        (hostIf_STBService<Module>*)g_hash_table_lookup(ifHash, (gpointer)(intptr_t)dev_id);
    if (!pRet)
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                "[%s:%s:%d]: No instance for dev_id=%d\n", __FILE__, __FUNCTION__, __LINE__, dev_id);
    return pRet;
}

GList* hostIf_STBService<Module>::getAllInstances()
{
    if(ifHash) return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_STBService<Module>::closeInstance(hostIf_STBService<Module> *pDev)
{
    if(pDev) {
        if (ifHash)
            g_hash_table_remove(ifHash, (gconstpointer)(intptr_t)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBService<Module>::closeAllInstances()
{
    if(ifHash) {
        GList* tmp_list = g_hash_table_get_values(ifHash);
        GList* current = tmp_list;
        while(current) {
            hostIf_STBService<Module>* pDev = (hostIf_STBService<Module>*)current->data;
            current = current->next;
            delete pDev;
        }
        g_list_free(tmp_list);
        g_hash_table_destroy(ifHash);
        ifHash = NULL;   // triggers rebuild on next getInstance()
    }
}

void hostIf_STBService<Module>::getLock()
{
    g_mutex_init(&hostIf_STBService<Module>::m_mutex);
    g_mutex_lock(&hostIf_STBService<Module>::m_mutex);
}

void hostIf_STBService<Module>::releaseLock()
{
    g_mutex_unlock(&hostIf_STBService<Module>::m_mutex);
}
```

#### 3d. Constructor
```cpp
hostIf_STBService<Module>::hostIf_STBService<Module>(int devid, const std::string& portName)
    : dev_id(devid), m_portName(portName)
{
    // initialise all backup* fields and bCalled* flags to defaults
}
```

#### 3e. buildPortNameHash — queries Thunder, creates all instances eagerly

**Port-based variant** (DisplaySettings audio/video modules — copy AudioOutput/SPDIF pattern):
```cpp
void hostIf_STBService<Module>::buildPortNameHash()
{
    if (ifHash) g_hash_table_destroy(ifHash);
    ifHash = g_hash_table_new(NULL, NULL);

    std::string delimitedPorts;
    if (!invokeThunderPluginMethodAndExtractDelimitedStringArrayField(
            THUNDER_DS_GET_SUPPORTED_AUDIO_PORTS, "{}", "supportedAudioPorts", ",", delimitedPorts))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,
                "[%s:%d] Failed to get supported ports\n", __FUNCTION__, __LINE__);
        return;
    }

    int devId = 1;
    std::istringstream ss(delimitedPorts);
    std::string portName;
    while (std::getline(ss, portName, ','))
    {
        // Optional: filter by type, e.g. strcasestr(portName.c_str(), "spdif") for SPDIF
        if (!portName.empty())
        {
            hostIf_STBService<Module>* pInst = new hostIf_STBService<Module>(devId, portName);
            g_hash_table_insert(ifHash, (gpointer)(intptr_t)devId, pInst);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
                    "[%s:%d] dev_id=%d portName=%s\n", __FUNCTION__, __LINE__, devId, portName.c_str());
            devId++;
        }
    }
}
```

**Non-port variant** (AVOutput, DisplayInfo, HdcpProfile, PowerManager — single instance):
```cpp
void hostIf_STBService<Module>::buildPortNameHash()
{
    if (ifHash) g_hash_table_destroy(ifHash);
    ifHash = g_hash_table_new(NULL, NULL);

    // Single device — no port enumeration needed
    hostIf_STBService<Module>* pInst = new hostIf_STBService<Module>(1);
    g_hash_table_insert(ifHash, (gpointer)(intptr_t)1, pInst);
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,
            "[%s:%d] Single instance created (dev_id=1)\n", __FUNCTION__, __LINE__);
}

int hostIf_STBService<Module>::getNumberOfInstances(HOSTIF_MsgData_t *stMsgData)
{
    if (!ifHash) buildPortNameHash();
    put_int(stMsgData->paramValue, g_hash_table_size(ifHash));
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);
    return OK;
}
```

#### 3f. Each migrated getter — templates

**Port-based getter** (DisplaySettings — pass `audioPort` or `videoDisplay` in JSON):
```cpp
int hostIf_STBService<Module>::getSomeParam(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    <type> value;
    const std::string paramsJson = "{\"audioPort\":\"" + m_portName + "\"}";
    // or: "{\"videoDisplay\":\"" + m_portName + "\"}" for video

    if (!invokeThunderPluginMethodAndExtract<Type>Field(
            THUNDER_DS_GET_XXX, paramsJson, "<fieldName>", value))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder call failed for port %s\n",
                __FUNCTION__, m_portName.c_str());
        return NOK;
    }
    // populate stMsgData->paramValue, paramtype, paramLen
    // update backup field and bCalled flag for change detection
    return OK;
}
```

**No-port getter** (AVOutput / HdcpProfile / PowerManager — no port field):
```cpp
int hostIf_STBService<Module>::getSomeParam(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    <type> value;
    if (!invokeThunderPluginMethodAndExtract<Type>Field(
            THUNDER_AVO_GET_XXX, "{}", "<fieldName>", value))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder call failed\n", __FUNCTION__);
        return NOK;
    }
    // populate stMsgData->paramValue, paramtype, paramLen
    return OK;
}
```

**DisplayInfo property getter** (read a single named property):
```cpp
int hostIf_STBService<Module>::getSomeParam(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool value;
    if (!invokeThunderPluginMethodAndExtractBoolField(
            THUNDER_DI_DISPLAYINFO, "{}", "connected", value))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] DisplayInfo Thunder call failed\n", __FUNCTION__);
        return NOK;
    }
    put_boolean(stMsgData->paramValue, value);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);
    return OK;
}
```

#### 3g. Each migrated setter — templates

**Port-based setter**:
```cpp
int hostIf_STBService<Module>::setSomeParam(HOSTIF_MsgData_t *stMsgData)
{
    <type> value = get_<type>(stMsgData->paramValue);
    const std::string paramsJson = "{\"audioPort\":\"" + m_portName
                                   + "\",\"<field>\":" + std::to_string(value) + "}";
    std::string response;
    if (!invokeThunderPluginMethod(THUNDER_DS_SET_XXX, paramsJson, response))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder call failed\n", __FUNCTION__);
        return NOK;
    }
    return OK;
}
```

**No-port setter** (AVOutput / PowerManager):
```cpp
int hostIf_STBService<Module>::setSomeParam(HOSTIF_MsgData_t *stMsgData)
{
    <type> value = get_<type>(stMsgData->paramValue);
    const std::string paramsJson = "{\"<field>\":" + std::to_string(value) + "}";
    std::string response;
    if (!invokeThunderPluginMethod(THUNDER_AVO_SET_XXX, paramsJson, response))
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Thunder call failed\n", __FUNCTION__);
        return NOK;
    }
    return OK;
}
```

#### 3h. handleSetMsg / handleGetMsg / doUpdates
Move from the original `.cpp` verbatim, updating any `vPort.*` / `aPort.*` call sites to use Thunder methods.

#### 3i. Functions without Thunder equivalent
```cpp
// Wrap call sites in handleSetMsg/handleGetMsg/doUpdates:
#if 0
    else if (strcasecmp(paramName, SOME_STRING) == 0)
        ret = someFunction(stMsgData);
#endif

// Wrap the definition:
#if 0
int hostIf_STBService<Module>::someFunction(...) { /* libds impl */ }
#endif
```

### Step 4 — Gut the original `Components_<Module>.cpp`

Keep the license header and includes, then:
```cpp
/* All functions moved to Components_<Module>_Thunder.cpp */
#if 0

// ... paste original content here unchanged ...

#endif /* 0 */
```

---

## Checklist

- [ ] Identified which Thunder plugin owns each TR-69 parameter (use Plugin Selection Guide)
- [ ] Discovered all Thunder method names, request/response field names, and types
- [ ] Header updated: `m_portName` replaces port reference (port-based); or removed entirely (no-port)
- [ ] Header has `buildPortNameHash()` declared
- [ ] Static `ifHash` defined in Thunder file; `instanceHash` removed (single table)
- [ ] `buildPortNameHash()` queries Thunder (port-based: enumerate ports) or creates single instance (no-port)
- [ ] `getInstance()` uses single `g_hash_table_lookup(ifHash, (gpointer)(intptr_t)dev_id)`
- [ ] `closeAllInstances()` sets `ifHash = NULL` so next call triggers rebuild
- [ ] All `vPort.*` / `aPort.*` / `device::` calls removed from Thunder file
- [ ] Non-migratable functions guarded with `#if 0` in: definitions, call sites (handleSetMsg/Get/doUpdates), header declarations
- [ ] Original `.cpp` gutted with `#if 0`
- [ ] File compiles without errors referencing libds symbols


---

### Notes for AI
Focus on:
- Zero regression
- Persistence across restart/reboot
- Clean API mapping (no leftover libds calls)
- Maintainability and readability
