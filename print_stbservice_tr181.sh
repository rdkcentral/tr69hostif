#!/bin/bash
#
# print_stbservice_tr181.sh
# Prints all Device.Services.STBService TR-181 parameter values.
#
# Supported query methods (auto-detected in order):
#   1. tr181    — tr181 -g <param>
#   2. rbuscli  — rbuscli getValues <param>
#   3. rbus_cli — rbus_cli get <param>
#   4. curl     — curl http://localhost:8080/api/v2/device/tr181/<param>
#
# Usage:
#   chmod +x print_stbservice_tr181.sh
#   ./print_stbservice_tr181.sh
#   ./print_stbservice_tr181.sh --component AudioOutput
#   ./print_stbservice_tr181.sh --component SPDIF
#   ./print_stbservice_tr181.sh --component VideoOutput
#   ./print_stbservice_tr181.sh --component HDMI
#   ./print_stbservice_tr181.sh --component VideoDecoder
#   ./print_stbservice_tr181.sh --component Capabilities
#
# Note: DisplayDevice is nested under HDMI (HDMI.N.DisplayDevice.*);
#       it is printed as part of --component HDMI.

BASE="Device.Services.STBService.1"
WEBPA_URL="http://localhost:8080/api/v2/device/tr181"
MAX_INSTANCES=4
TMPFILE="/tmp/.tr181_$$"
trap 'rm -f "$TMPFILE"' EXIT

FILTER=""
if [ "$1" = "--component" ] && [ -n "$2" ]; then
    FILTER="$2"
fi

# ---------------------------------------------------------------
# Detect available query tool
# ---------------------------------------------------------------
detect_tool() {
    if command -v tr181 >/dev/null 2>&1; then
        echo "tr181"
    elif command -v rbuscli >/dev/null 2>&1; then
        echo "rbuscli"
    elif command -v rbus_cli >/dev/null 2>&1; then
        echo "rbus_cli"
    elif command -v curl >/dev/null 2>&1; then
        echo "curl"
    else
        echo "none"
    fi
}

TOOL=$(detect_tool)

if [ "$TOOL" = "none" ]; then
    echo "ERROR: No supported query tool found (tr181, rbuscli, rbus_cli, curl)."
    exit 1
fi

echo "Using tool: $TOOL"
echo ""

# ---------------------------------------------------------------
# Query a single TR-181 parameter and print it
# ---------------------------------------------------------------
getparam() {
    local param="$1"
    local val=""

    # Capture both stdout and stderr — tr181 on some RDK platforms writes to stderr.
    # grep filters out error/diagnostic lines; head -1 takes the first clean value line.
    case "$TOOL" in
        tr181)
            tr181 -g "$param" > "$TMPFILE" 2>&1; sleep 2
            val=$(grep -iv "error\|fault\|exception\|fail\|invalid\|not found\|usage:" "$TMPFILE" | head -n 1 | tr -d '\r')
            ;;
        rbuscli)
            rbuscli getValues "$param" > "$TMPFILE" 2>/dev/null; sleep 2
            val=$(grep -oE 'VALUE:.*' "$TMPFILE" | sed 's/VALUE: //' | head -n 1)
            ;;
        rbus_cli)
            rbus_cli get "$param" > "$TMPFILE" 2>/dev/null; sleep 2
            val=$(cat "$TMPFILE")
            ;;
        curl)
            curl -sf "$WEBPA_URL/$param" > "$TMPFILE" 2>/dev/null; sleep 2
            val=$(grep -oE '"value":"[^"]*"' "$TMPFILE" | head -n 1 | cut -d'"' -f4)
            ;;
    esac

    printf "  %-75s = %s\n" "$param" "${val:-<empty>}"
}

# ---------------------------------------------------------------
# Probe if an instance exists (returns 0=yes / 1=no).
# Same capture logic as getparam — uses 2>&1 to catch tr181 stderr output.
# ---------------------------------------------------------------
probe_exists() {
    local param="$1"
    local val=""

    case "$TOOL" in
        tr181)
            tr181 -g "$param" > "$TMPFILE" 2>&1; sleep 2
            val=$(grep -iv "error\|fault\|exception\|fail\|invalid\|not found\|usage:" "$TMPFILE" | head -n 1 | tr -d '\r')
            ;;
        rbuscli)
            rbuscli getValues "$param" > "$TMPFILE" 2>/dev/null; sleep 2
            val=$(grep -oE 'VALUE:.*' "$TMPFILE" | sed 's/VALUE: //' | head -n 1)
            ;;
        rbus_cli)
            rbus_cli get "$param" > "$TMPFILE" 2>/dev/null; sleep 2
            val=$(cat "$TMPFILE")
            ;;
        curl)
            curl -sf "$WEBPA_URL/$param" > "$TMPFILE" 2>/dev/null; sleep 2
            val=$(grep -oE '"value":"[^"]*"' "$TMPFILE" | cut -d'"' -f4)
            ;;
    esac

    [ -n "$val" ]
}

# ---------------------------------------------------------------
print_header() {
    echo "============================================================="
    echo "  $1"
    echo "============================================================="
}

# ---------------------------------------------------------------
# Capabilities
# ---------------------------------------------------------------
print_capabilities() {
    print_header "Capabilities"
    getparam "${BASE}.Capabilities.VideoDecoder.VideoStandards"
    getparam "${BASE}.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.AudioStandards"
    getparam "${BASE}.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevelNumberOfEntries"
    for i in 1 2 3 4; do
        local pfx="${BASE}.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.${i}"
        probe_exists "${pfx}.Profile" || break
        getparam "${pfx}.Profile"
        getparam "${pfx}.Level"
    done
    echo ""
}

# ---------------------------------------------------------------
# AudioOutput
# Probe: .Enable — hardcoded true, always non-empty when instance exists
# Params from Components_AudioOutput.cpp defines
# ---------------------------------------------------------------
print_audio_output() {
    print_header "Components.AudioOutput"
    for (( N=1; N<=MAX_INSTANCES; N++ )); do
        local base="${BASE}.Components.AudioOutput.${N}"
        probe_exists "${base}.Enable" || break
        echo "  --- AudioOutput Instance ${N} ---"
        getparam "${base}.Status"
        getparam "${base}.Enable"
        getparam "${base}.CancelMute"
        getparam "${base}.Name"
        getparam "${base}.AudioLevel"
        getparam "${base}.AudioFormat"
        getparam "${base}.X_COMCAST-COM_AudioStereoMode"
        getparam "${base}.X_COMCAST-COM_AudioEncoding"
        getparam "${base}.X_COMCAST-COM_AudioCompression"
        getparam "${base}.X_COMCAST-COM_AudioDB"
        getparam "${base}.X_COMCAST-COM_MinAudioDB"
        getparam "${base}.X_COMCAST-COM_MaxAudioDB"
        getparam "${base}.X_COMCAST-COM_AudioOptimalLevel"
        getparam "${base}.X_COMCAST-COM_AudioLoopThru"
        getparam "${base}.X_COMCAST-COM_AudioGain"
        getparam "${base}.X_COMCAST-COM_DialogEnhancement"
        echo ""
    done
}

# ---------------------------------------------------------------
# SPDIF
# Probe: .Name — returns port name e.g. "SPDIF0"
# Params from Components_SPDIF.cpp defines
# ---------------------------------------------------------------
print_spdif() {
    print_header "Components.SPDIF"
    for (( N=1; N<=MAX_INSTANCES; N++ )); do
        local base="${BASE}.Components.SPDIF.${N}"
        probe_exists "${base}.Enable" || break
        echo "  --- SPDIF Instance ${N} ---"
        getparam "${base}.Status"
        getparam "${base}.Enable"
        getparam "${base}.Name"
        getparam "${base}.ForcePCM"
        getparam "${base}.Passthrough"
        getparam "${base}.AudioDelay"
        echo ""
    done
}

# ---------------------------------------------------------------
# VideoOutput
# Probe: .VideoFormat — confirmed working on device
# Params from Components_VideoOutput.cpp defines
# ---------------------------------------------------------------
print_video_output() {
    print_header "Components.VideoOutput"
    for (( N=1; N<=MAX_INSTANCES; N++ )); do
        local base="${BASE}.Components.VideoOutput.${N}"
        probe_exists "${base}.Enable" || break
        echo "  --- VideoOutput Instance ${N} ---"
        getparam "${base}.Status"
        getparam "${base}.Enable"
        getparam "${base}.Name"
        getparam "${base}.VideoFormat"
        getparam "${base}.DisplayFormat"
        getparam "${base}.AspectRatioBehaviour"
        getparam "${base}.HDCP"
        echo ""
    done
}

# ---------------------------------------------------------------
# HDMI + DisplayDevice (nested under HDMI.N.DisplayDevice.*)
# Probe: .ResolutionValue — confirmed working on device
# HDMI params from Components_HDMI.cpp defines
# DisplayDevice params from Components_DisplayDevice.cpp defines
# ---------------------------------------------------------------
print_hdmi() {
    print_header "Components.HDMI"
    for (( N=1; N<=MAX_INSTANCES; N++ )); do
        local base="${BASE}.Components.HDMI.${N}"
        probe_exists "${base}.Enable" || break
        echo "  --- HDMI Instance ${N} ---"
        getparam "${base}.Status"
        getparam "${base}.Enable"
        getparam "${base}.Name"
        getparam "${base}.ResolutionMode"
        getparam "${base}.ResolutionValue"
        echo ""

        echo "  --- HDMI.${N}.DisplayDevice ---"
        local dd="${base}.DisplayDevice"
        getparam "${dd}.Status"
        getparam "${dd}.EEDID"
        getparam "${dd}.SupportedResolutions"
        getparam "${dd}.PreferredResolution"
        getparam "${dd}.VideoLatency"
        getparam "${dd}.AutoLipSyncSupport"
        getparam "${dd}.CECSupport"
        getparam "${dd}.HDMI3DPresent"
        getparam "${dd}.X_COMCAST-COM_EDID"
        echo ""
    done
}

# ---------------------------------------------------------------
# VideoDecoder
# Probe: .Name — returns decoder name
# Params from Components_VideoDecoder.cpp defines
# ---------------------------------------------------------------
print_video_decoder() {
    print_header "Components.VideoDecoder"
    for (( N=1; N<=MAX_INSTANCES; N++ )); do
        local base="${BASE}.Components.VideoDecoder.${N}"
        probe_exists "${base}.Enable" || break
        echo "  --- VideoDecoder Instance ${N} ---"
        getparam "${base}.Status"
        getparam "${base}.Enable"
        getparam "${base}.Name"
        getparam "${base}.ContentAspectRatio"
        getparam "${base}.X_COMCAST-COM_Standby"
        getparam "${base}.X_RDKCENTRAL-COM_MPEGHPart2"
        echo ""
    done
}

# ---------------------------------------------------------------
# Main — run selected or all components
# ---------------------------------------------------------------
case "$FILTER" in
    AudioOutput)  print_audio_output ;;
    SPDIF)        print_spdif ;;
    VideoOutput)  print_video_output ;;
    HDMI)         print_hdmi ;;
    VideoDecoder) print_video_decoder ;;
    Capabilities) print_capabilities ;;
    "")
        print_capabilities
        print_audio_output
        print_spdif
        print_video_output
        print_hdmi
        print_video_decoder
        ;;
    *)
        echo "Unknown component: $FILTER"
        echo "Valid: AudioOutput SPDIF VideoOutput HDMI VideoDecoder Capabilities"
        exit 1
        ;;
esac
