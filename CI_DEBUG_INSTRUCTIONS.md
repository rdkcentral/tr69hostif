# CI Debugging Instructions - Thunder Mock Server Error 5

## Current Status

All Thunder mock server fixes have been **committed and pushed**, but CI is still showing Error 5 (RBUS connection failures) in 7 STBService tests.

## Local vs CI Behavior

✅ **LOCAL**: Thunder mock server starts successfully, all infrastructure works
❌ **CI**: Error 5 (tr69hostif cannot connect to Thunder on 127.0.0.1:9998)

## Root Cause Analysis

The hardcoded Thunder URL in `src/hostif/include/hostIf_utils.h`:
```cpp
#define JSONRPC_URL    "http://127.0.0.1:9998/jsonrpc"
```

This works fine when tr69hostif and Thunder mock server run in the **same container/host**, but CI environment might have:
- Different network namespaces
- Multiple containers with isolated networking  
- Docker network configuration issues
- CI environment differences

## Debug Steps for CI

### Step 1: Add Debug Script to CI Pipeline

Add this to your CI pipeline **before** the L2 tests run:

```yaml
- name: Debug Thunder Mock Server
  run: |
    cd tr69hostif
    chmod +x debug_thunder_ci.sh
    ./debug_thunder_ci.sh
```

The script `debug_thunder_ci.sh` is now in the repo and will output:
1. Node.js version
2. Mock server file existence
3. Port 9998 availability
4. Running processes
5. Mock server startup test
6. Port listening verification
7. curl connectivity test
8. Mock server logs

### Step 2: Check CI Logs

After running the debug script in CI, look for:

❌ **Problem Signs:**
- "❌ Mock server process died!" → Check log output
- "❌ Port 9998 not listening!" → Network configuration issue
- "❌ curl failed to connect!" → Firewall or routing issue

✅ **Success Signs:**
- "✓ Mock server process is alive"
- Port 9998 shows "LISTEN 0 511 127.0.0.1:9998"
- curl returns: `{"jsonrpc":"2.0","id":"1","result":true}`

### Step 3: Verify run_l2.sh Execution

Check if the enhanced run_l2.sh is actually running in CI:

```bash
grep -A 10 "Clean up any stale Thunder mock servers" run_l2.sh
```

Expected output should show:
```bash
# Clean up any stale Thunder mock server from previous runs
echo "[L2] Cleaning up stale Thunder mock servers..."
pkill -f "thunder-mock-server.js" 2>/dev/null || true
sleep 1
```

### Step 4: Check tr69hostif Startup

Verify tr69hostif daemon is running before tests:

```bash
pidof tr69hostif || echo "tr69hostif not running!"
ps aux | grep tr69hostif
```

### Step 5: Network Isolation Check

If CI uses Docker, verify network mode:

```bash
# Check if containers can communicate via 127.0.0.1
docker inspect <container-id> | grep NetworkMode
```

If using `--network=none` or bridge mode with isolation, tr69hostif and mock server might not see each other on localhost.

## Possible Solutions

### Solution 1: Use 0.0.0.0 for Mock Server

If containers are isolated, modify `thunder-mock-server.js` to bind to 0.0.0.0 instead of 127.0.0.1:

```javascript
const THUNDER_HOST = process.env.THUNDER_HOST || '0.0.0.0';
```

But this requires tr69hostif to know the container IP, not localhost.

### Solution 2: Share Network Namespace

If CI uses multiple containers, use `--network=host` or `--network=container:tr69hostif` so they share localhost.

### Solution 3: Wait Longer

Current `sleep 3` might not be enough in CI. Try increasing:

```bash
sleep 5  # instead of sleep 3
```

### Solution 4: Check CI Container Architecture

Some CI environments run multiple phases in separate containers:
- Build container
- Test container  
- Deployment container

Ensure tr69hostif and Thunder mock server run in the **same container**.

## Quick Fix to Try

In `run_l2.sh`, after starting the mock server, add explicit connectivity test:

```bash
# Start Thunder mock server for STBService tests
echo "[L2] Starting Thunder mock server on port 9998..."
cd $top_srcdir
node test/test-artifacts/native-platform/thunder-mock-server.js > /tmp/thunder-mock-server.log 2>&1 &
THUNDER_MOCK_PID=$!
sleep 3

# Verify server is running
if kill -0 $THUNDER_MOCK_PID 2>/dev/null; then
    echo "[L2] Thunder mock server started (PID: $THUNDER_MOCK_PID)"
else
    echo "[L2] ERROR: Thunder mock server failed to start"
    cat /tmp/thunder-mock-server.log
    exit 1  # ← Add this to fail fast
fi

# NEW: Test connectivity before running tests
echo "[L2] Testing Thunder mock server connectivity..."
for i in {1..10}; do
    if curl -s -X POST http://127.0.0.1:9998/jsonrpc \
        -H "Content-Type: application/json" \
        -d '{"jsonrpc":"2.0","id":"1","method":"test"}' > /dev/null 2>&1; then
        echo "[L2] ✓ Thunder mock server is responding"
        break
    fi
    if [ $i -eq 10 ]; then
        echo "[L2] ❌ Thunder mock server not responding after 10 attempts"
        cat /tmp/thunder-mock-server.log
        exit 1
    fi
    echo "[L2] Attempt $i/10: waiting for server..."
    sleep 1
done
```

## Expected Timeline

- ✅ Changes committed and pushed
- 🔄 Need CI debug output to diagnose environment-specific issues
- ⏳ Once diagnosed, apply targeted fix

## Files Modified

- ✅ `run_l2.sh` - Mock server cleanup and startup (committed)
- ✅ `helper_functions.py` - rbus_get_value() function (committed)
- ✅ `tr69hostif_stbservice_thunder.py` - Tests using rbus_get_value() (committed)
- ✅ `thunder-mock-server.js` - 21 STBService mock APIs (committed)
- ✅ All C++ implementation files (committed)
- 🆕 `debug_thunder_ci.sh` - CI debugging script (ready to commit)

## Next Steps

1. **Commit debug script:**
   ```bash
   git add debug_thunder_ci.sh
   git commit -m "Add Thunder mock server CI debugging script"
   git push
   ```

2. **Add to CI pipeline** (or run manually in CI environment)

3. **Share debug output** with development team

4. **Apply targeted fix** based on debug findings
