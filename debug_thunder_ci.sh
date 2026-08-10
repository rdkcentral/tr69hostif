#!/bin/bash

################################################################################
# Thunder Mock Server CI Debugging Script
################################################################################

echo "══════════════════════════════════════════════════════════════════════"
echo " Thunder Mock Server CI Environment Debug"
echo "══════════════════════════════════════════════════════════════════════"

echo ""
echo "[1/8] Node.js Installation:"
which node
node --version 2>&1 || echo "❌ Node.js not found!"

echo ""
echo "[2/8] Thunder Mock Server File:"
ls -lh test/test-artifacts/native-platform/thunder-mock-server.js 2>&1 || echo "❌ Mock server file not found!"

echo ""
echo "[3/8] Check if port 9998 is already in use:"
ss -tln | grep 9998 || netstat -tln | grep 9998 || echo "✓ Port 9998 is free"

echo ""
echo "[4/8] Check for running thunder-mock-server processes:"
ps aux | grep thunder-mock-server | grep -v grep || echo "✓ No thunder-mock-server processes found"

echo ""
echo "[5/8] Test starting Thunder mock server:"
cd $(pwd)
node test/test-artifacts/native-platform/thunder-mock-server.js > /tmp/thunder-debug.log 2>&1 &
MOCK_PID=$!
echo "Started mock server PID: $MOCK_PID"
sleep 3

echo ""
echo "[6/8] Check if mock server is running:"
if kill -0 $MOCK_PID 2>/dev/null; then
    echo "✓ Mock server process is alive"
else
    echo "❌ Mock server process died!"
    echo "Log output:"
    cat /tmp/thunder-debug.log
fi

echo ""
echo "[7/8] Check if port 9998 is now listening:"
ss -tln | grep 9998 || netstat -tln | grep 9998 || echo "❌ Port 9998 not listening!"

echo ""
echo "[8/8] Test Thunder API connectivity:"
curl -s -X POST http://127.0.0.1:9998/jsonrpc \
  -H "Content-Type: application/json" \
  -d '{"jsonrpc":"2.0","id":"1","method":"DisplayInfo.1.connected"}' \
  || echo "❌ curl failed to connect!"

echo ""
echo "Mock server log:"
cat /tmp/thunder-debug.log

echo ""
echo "Cleaning up..."
kill $MOCK_PID 2>/dev/null
pkill -f thunder-mock-server.js 2>/dev/null || true

echo ""
echo "══════════════════════════════════════════════════════════════════════"
echo " Debug Complete"
echo "══════════════════════════════════════════════════════════════════════"
