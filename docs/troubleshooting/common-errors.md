# Common Errors

## Missing or Incorrect Manager Map

### Symptom

Requests for valid TR-181 paths return invalid-parameter-style failures or never reach the expected handler.

### Why it happens

Routing depends on the parameter-prefix map loaded from `conf/tr69hostIf.conf` or the runtime copy in `/etc/mgrlist.conf`. If the prefix is missing or mapped to the wrong manager, dispatch resolution fails before the profile handler is invoked.

### What to check

- confirm the requested prefix exists in the active manager map
- confirm the binary was built with the corresponding profile enabled
- confirm the target handler is actually compiled into the image

## Data Model Not Available

### Symptom

WebPA initialization fails, data-model loading fails at startup, or requests relying on merged XML behave incorrectly.

### Why it happens

The daemon expects platform data-model fragments under `/etc` and a merged model under `/tmp/data-model.xml` for WebPA-related flows.

### What to check

- verify `/etc/data-model-generic.xml` exists
- verify the platform fragment such as `/etc/data-model-stb.xml` or `/etc/data-model-tv.xml` exists
- verify the merged output was generated successfully

## Parodus Unavailable or Misconfigured

### Symptom

WebPA requests do not arrive, notifications are not sent, or logs show repeated retry behavior.

### Why it happens

The Parodus client reads endpoint details from `/etc/webpa_cfg.json` and retries connection with exponential backoff. If the config is missing or the service is down, the worker stays in retry mode.

### What to check

- verify `/etc/webpa_cfg.json` is present and valid
- verify the Parodus service is running and reachable
- inspect runtime logs for `libparodus_init` retry messages

## Shutdown Assumptions About Parodus Thread

### Symptom

Cleanup changes that try to join the Parodus worker crash or hang unexpectedly.

### Why it happens

The Parodus worker detaches itself inside `connect_parodus()`. A detached thread cannot be joined later.

### What to check

- make sure shutdown signals the receive loop instead of attempting `pthread_join()`
- keep thread lifecycle documentation aligned with any future Parodus changes

## Bootstrap or RFC Value Confusion

### Symptom

Returned values do not match firmware defaults or live HAL expectations.

### Why it happens

DeviceInfo-related values may resolve from override storage or bootstrap defaults rather than from a live source.

### What to check

- inspect files under `/opt/secure/RFC/`
- verify bootstrap values and partner defaults
- confirm whether a value was previously set through WebPA or RFC override paths

## Slow Notification Propagation

### Symptom

Value changes are visible eventually but not immediately.

### Why it happens

The update handler uses a polling loop and sleeps for 60 seconds between passes.

### What to check

- verify the affected profile participates in `registerUpdateCallback()` and `checkForUpdates()`
- account for the poll interval when interpreting latency

## See Also

- [Threading Model](../architecture/threading-model.md)
- [Data Flow](../architecture/data-flow.md)
- [Testing](../integration/testing.md)