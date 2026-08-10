#!/bin/bash
##############################################################################
# Thunder Mock Server Test - Verify L2 test infrastructure is working
##############################################################################

set -e

cd "$(dirname "$0")"

echo "========================================="
echo " Thunder Mock Server Infrastructure Test"
echo "========================================="
echo

# 1. Check Node.js
echo "[1/6] Checking Node.js installation..."
if ! command -v node &> /dev/null; then
    echo "  ✗ FAIL: Node.js not found in PATH"
    exit 1
fi
NODE_VERSION=$(node --version)
echo "  ✓ Node.js installed: $NODE_VERSION"
echo

# 2. Check mock server file
echo "[2/6] Checking thunder-mock-server.js..."
MOCK_SERVER="test/test-artifacts/native-platform/thunder-mock-server.js"
if [ ! -f "$MOCK_SERVER" ]; then
    echo "  ✗ FAIL: Mock server file not found: $MOCK_SERVER"
    exit 1
fi
echo "  ✓ Mock server file exists"
echo

# 3. Check helper_functions.py has rbus_get_value
echo "[3/6] Checking helper_functions.py..."
HELPER_FILE="test/functional-tests/tests/helper_functions.py"
if ! grep -q "def rbus_get_value" "$HELPER_FILE"; then
    echo "  ✗ FAIL: rbus_get_value() function not found in $HELPER_FILE"
    exit 1
fi
echo "  ✓ rbus_get_value() function exists"
echo

# 4. Check run_l2.sh has Thunder mock server startup
echo "[4/6] Checking run_l2.sh..."
if ! grep -q "thunder-mock-server.js" "run_l2.sh"; then
    echo "  ✗ FAIL: Thunder mock server startup not found in run_l2.sh"
    exit 1
fi
echo "  ✓ Thunder mock server startup configured"
echo

# 5. Start mock server and test connectivity
echo "[5/6] Testing Thunder mock server connectivity..."
pkill -f thunder-mock-server || true
sleep 1

node "$MOCK_SERVER" > /tmp/thunder-mock-verify.log 2>&1 &
MOCK_PID=$!
sleep 3

if ! kill -0 $MOCK_PID 2>/dev/null; then
    echo "  ✗ FAIL: Mock server failed to start"
    echo "  Log contents:"
    cat /tmp/thunder-mock-verify.log
    exit 1
fi

# Test API call
RESPONSE=$(curl -s -H 'content-type:text/plain;' \
    --data-binary '{"jsonrpc": "2.0", "id": 1, "method": "DisplayInfo.1.connected"}' \
    http://127.0.0.1:9998/jsonrpc)

if [[ "$RESPONSE" != *'"result":true'* ]]; then
    echo "  ✗ FAIL: Unexpected response from mock server"
    echo "  Response: $RESPONSE"
    kill $MOCK_PID 2>/dev/null || true
    exit 1
fi

echo "  ✓ Mock server responding correctly"
echo "  Response: $RESPONSE"

# Cleanup
kill $MOCK_PID 2>/dev/null || true
wait $MOCK_PID 2>/dev/null || true
echo

# 6. Check test file has correct imports
echo "[6/6] Checking tr69hostif_stbservice_thunder.py..."
TEST_FILE="test/functional-tests/tests/tr69hostif_stbservice_thunder.py"
if ! grep -q "from helper_functions import" "$TEST_FILE"; then
    echo "  ✗ FAIL: helper_functions import not found in $TEST_FILE"
    exit 1
fi

# Check if any test uses rbus_get_value
if ! grep -q "rbus_get_value" "$TEST_FILE"; then
    echo "  ⚠ WARNING: No tests use rbus_get_value() yet"
else
    echo "  ✓ Tests use rbus_get_value()"
fi
echo

echo "========================================="
echo " ✓ ALL CHECKS PASSED"
echo "========================================="
echo
echo "Infrastructure is ready for L2 tests."
echo
echo "To run STBService L2 tests manually:"
echo "  1. cd $PWD"
echo "  2. node test/test-artifacts/native-platform/thunder-mock-server.js &"
echo "  3. sleep 2"
echo "  4. pytest test/functional-tests/tests/tr69hostif_stbservice_thunder.py"
echo "  5. kill %1"
echo
echo "To run full L2 suite:"
echo "  ./run_l2.sh"
echo

