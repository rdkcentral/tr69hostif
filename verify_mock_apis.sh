#!/bin/bash

################################################################################
# Verify Thunder Mock Server APIs for Failing L2 Tests
################################################################################

echo "══════════════════════════════════════════════════════════════════════"
echo " Thunder Mock Server API Verification"
echo "══════════════════════════════════════════════════════════════════════"

# Start mock server
cd $(pwd)
node test/test-artifacts/native-platform/thunder-mock-server.js > /tmp/mock-verify.log 2>&1 &
MOCK_PID=$!
sleep 3

if ! kill -0 $MOCK_PID 2>/dev/null; then
    echo "❌ Mock server failed to start!"
    cat /tmp/mock-verify.log
    exit 1
fi

echo "✓ Mock server started (PID: $MOCK_PID)"
echo ""

# Test function
test_api() {
    local api_name=$1
    local expected_field=$2
    local test_name=$3
    
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "Testing: $test_name"
    echo "API: $api_name"
    echo "Expected field: $expected_field"
    
    response=$(curl -s -X POST http://127.0.0.1:9998/jsonrpc \
        -H "Content-Type: application/json" \
        -d "{\"jsonrpc\":\"2.0\",\"id\":\"1\",\"method\":\"$api_name\",\"params\":{\"audioPort\":\"SPEAKER0\"}}")
    
    echo "Response: $response"
    
    # Check if response contains expected field
    if echo "$response" | grep -q "\"$expected_field\""; then
        echo "✓ PASS: Response contains '$expected_field' field"
    else
        echo "❌ FAIL: Response missing '$expected_field' field"
    fi
    
    # Check if response has result
    if echo "$response" | grep -q "\"result\""; then
        echo "✓ PASS: Response has 'result' field"
    else
        echo "❌ FAIL: Response missing 'result' field"
    fi
    
    echo ""
}

# Test all failing APIs
echo ""
echo "TESTING APIs FOR FAILING L2 TESTS:"
echo "══════════════════════════════════════════════════════════════════════"
echo ""

# 1. AudioOutput.Status
test_api "org.rdk.DisplaySettings.getEnableAudioPort" "enable" "AudioOutput.Status (getEnableAudioPort)"
test_api "org.rdk.DisplaySettings.getMuted" "muted" "AudioOutput.Status (getMuted)"

# 2. AudioOutput.AudioLevel  
test_api "org.rdk.DisplaySettings.getVolumeLevel" "volumeLevel" "AudioOutput.AudioLevel"

# 3. DisplayDevice.Status
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "Testing: DisplayDevice.Status"
echo "API: DisplayInfo.1.connected"
echo "Expected: Scalar boolean (result: true, NOT result: {connected: true})"

response=$(curl -s -X POST http://127.0.0.1:9998/jsonrpc \
    -H "Content-Type: application/json" \
    -d '{"jsonrpc":"2.0","id":"1","method":"DisplayInfo.1.connected"}')

echo "Response: $response"

# Check for scalar boolean (should be result: true, not result: {connected: true})
if echo "$response" | grep -q '"result":true' || echo "$response" | grep -q '"result":false'; then
    echo "✓ PASS: Returns scalar boolean"
else
    echo "❌ FAIL: Does not return scalar boolean"
fi
echo ""

# 4. DisplayDevice.SupportedResolutions
test_api "org.rdk.DisplaySettings.getSupportedResolutions" "supportedResolutions" "DisplayDevice.SupportedResolutions"

# 5. DisplayDevice.PreferredResolution
test_api "org.rdk.DisplaySettings.getDefaultResolution" "defaultResolution" "DisplayDevice.PreferredResolution"

# 6 & 7. DisplayDevice.EEDID and X_COMCAST_EDID
test_api "org.rdk.DisplaySettings.readEDID" "EDID" "DisplayDevice.EEDID/X_COMCAST_EDID"

echo "══════════════════════════════════════════════════════════════════════"
echo " ADDITIONAL API CHECKS"
echo "══════════════════════════════════════════════════════════════════════"
echo ""

# Check other STBService APIs that might be called
test_api "org.rdk.DisplaySettings.getSupportedAudioPorts" "supportedAudioPorts" "getSupportedAudioPorts"
test_api "org.rdk.DisplaySettings.getSupportedVideoDisplays" "supportedVideoDisplays" "getSupportedVideoDisplays"
test_api "org.rdk.DisplaySettings.getZoomSetting" "zoomSetting" "getZoomSetting"
test_api "org.rdk.PowerManager.GetPowerState" "currentState" "GetPowerState"

echo "══════════════════════════════════════════════════════════════════════"
echo " Mock Server Cleanup"
echo "══════════════════════════════════════════════════════════════════════"

kill $MOCK_PID 2>/dev/null
pkill -f thunder-mock-server.js 2>/dev/null || true
echo "✓ Mock server stopped"

echo ""
echo "══════════════════════════════════════════════════════════════════════"
echo " Verification Complete"
echo "══════════════════════════════════════════════════════════════════════"
