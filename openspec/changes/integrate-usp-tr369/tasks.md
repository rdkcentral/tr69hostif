## 1. tr69hostif Source Type Extension

- [ ] 1.1 Add `HOSTIF_SRC_USP` to `HostIf_Source_Type_t` enum in `src/hostif/include/hostIf_tr69ReqHandler.h`
- [ ] 1.2 Update bootstrap store (`XrdkCentralComBSStore.cpp`) to accept `HOSTIF_SRC_USP` for write operations on RW parameters
- [ ] 1.3 Update `XBSStoreJournal` to record "USP" source in journal entries for `HOSTIF_SRC_USP` writes
- [ ] 1.4 Guard `HOSTIF_SRC_USP` with `#ifdef USP_SUPPORT_ENABLED` preprocessor flag
- [ ] 1.5 Add `--enable-usp` flag to `configure.ac` that defines `USP_SUPPORT_ENABLED`

## 2. obuspa RDK-V Vendor Plug-in — Core Structure

- [ ] 2.1 Create plug-in project structure: `src/hostif/uspPlugin/` with `Makefile.am`, `rdkv_vendor.c`, and `rdkv_vendor.h`
- [ ] 2.2 Implement `VENDOR_Init()` stub that calls `rbus_open("obuspa_rdkv")` and verifies RBUS handle
- [ ] 2.3 Implement `VENDOR_Start()` stub that calls `USP_DM_InformInstance()` for dynamic table instances
- [ ] 2.4 Implement `VENDOR_Stop()` stub that calls `rbus_close()` for clean shutdown
- [ ] 2.5 Add plug-in build target to `Makefile.am` producing `rdkv_vendor.so` shared object

## 3. obuspa RDK-V Vendor Plug-in — Data Model Auto-Registration

- [ ] 3.1 Implement XML parser that reads the merged data model XML file (same format as waldb) and extracts parameter name, type, access, bsUpdate, and defaultValue
- [ ] 3.2 Implement parameter registration loop that calls `USP_REGISTER_GroupedVendorParam_ReadOnly()` or `USP_REGISTER_GroupedVendorParam_ReadWrite()` for each parameter based on access attribute
- [ ] 3.3 Implement table object registration using `USP_REGISTER_GroupedObject()` for parameters containing `{i}` placeholders
- [ ] 3.4 Implement `USP_REGISTER_GroupVendorHooks()` with group-level get/set/add/delete callbacks
- [ ] 3.5 Implement `RefreshInstances` callback using `USP_REGISTER_Object_RefreshInstances()` to query current table object instance numbers via RBUS
- [ ] 3.6 Add retry logic for data model XML file availability (configurable interval, max retries)

## 4. obuspa RDK-V Vendor Plug-in — RBUS Get/Set Bridge

- [ ] 4.1 Implement grouped Get callback: iterate `kv_vector_t` params, call `rbus_get()` for each, convert `rbusValue_t` to string, call `USP_ARG_Replace()` with result
- [ ] 4.2 Implement grouped Set callback: iterate `kv_vector_t` params, convert string values to appropriate `rbusValue_t` types, call `rbus_set()`, return error index on failure
- [ ] 4.3 Implement RBUS-to-USP error code translation (map `rbusError_t` values to USP error codes)
- [ ] 4.4 Implement RBUS-to-USP type mapping (map `rbusValueType_t` to USP DM types: DM_STRING, DM_BOOL, DM_INT, DM_UINT, DM_ULONG)
- [ ] 4.5 Add request logging with RDK_LOG for each RBUS get/set operation (debug level)

## 5. USP Notification Forwarding

- [ ] 5.1 Implement RBUS event subscription callback registration in `VENDOR_Init()` — hook into obuspa's subscription lifecycle
- [ ] 5.2 Implement subscription creation hook: when USP subscription is created for a plug-in parameter, call RBUS event subscribe on that parameter name
- [ ] 5.3 Implement RBUS event handler callback: on value-change event, call `USP_SIGNAL_DataModelEvent()` with parameter name and new value
- [ ] 5.4 Implement subscription deletion hook: when USP subscription is removed, call RBUS event unsubscribe
- [ ] 5.5 Implement async notification queue to avoid blocking RBUS event delivery thread (enqueue notification, process on data model thread via `USP_PROCESS_DoWork()`)
- [ ] 5.6 Implement ObjectAdded/ObjectDeleted signal forwarding for table objects

## 6. USP Access Control Configuration

- [ ] 6.1 Create factory reset configuration file (`usp_factory_reset.txt`) with three ControllerTrust roles: FullAccess, Operations, ReadOnly
- [ ] 6.2 Define FullAccess permission targets: `Device.` with Param:rw, Obj:rwa, InstantiatedObj:rwa, CommandEvent:rwx
- [ ] 6.3 Define Operations permission targets: `Device.` with Param:rw EXCEPT `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.*` (Param:r) and `Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId` (Param:r)
- [ ] 6.4 Define ReadOnly permission targets: `Device.` with Param:r, Obj:r
- [ ] 6.5 Add test controller entries in factory reset file for MQTT and WebSocket MTPs with placeholder endpoints

## 7. Daemon Deployment Configuration

- [ ] 7.1 Create `obuspa.service` systemd unit file with `After=tr69hostif.service`, `Wants=tr69hostif.service`, `Restart=on-failure`, `RestartSec=10`
- [ ] 7.2 Configure ExecStart with `-x /usr/lib/obuspa/plugins/rdkv_vendor.so -r /etc/usp_factory_reset.txt -f /opt/secure/USP/usp.db`
- [ ] 7.3 Create RFC-based enable/disable script that checks `/opt/secure/RFC/.RFC_USPEnabled.ini` before starting the service
- [ ] 7.4 Add MQTT MTP configuration in factory reset file with TLS enabled and EndpointID-derived topic paths
- [ ] 7.5 Add WebSocket MTP configuration in factory reset file as secondary MTP with TLS enabled
- [ ] 7.6 Configure TLS certificate paths in ExecStart: `--authcert` and `--truststore` pointing to RDK device certificate locations

## 8. Build System Integration

- [ ] 8.1 Add obuspa source as an external dependency in the RDK-V build recipe (bitbake or equivalent)
- [ ] 8.2 Add obuspa build dependencies: libmosquitto, libwebsockets, sqlite3, protobuf-c, zlib, openssl
- [ ] 8.3 Add `rdkv_vendor.so` plug-in build step that links against RBUS and tinyxml2
- [ ] 8.4 Add install targets for `obuspa` binary, `rdkv_vendor.so`, `obuspa.service`, `usp_factory_reset.txt`
- [ ] 8.5 Gate all USP artifacts behind the `--enable-usp` configure flag

## 9. Testing and Validation

- [ ] 9.1 Write L2 integration test: USP Get for `Device.DeviceInfo.ModelName` via obuspa CLI returns same value as `rbuscli get`
- [ ] 9.2 Write L2 integration test: USP Set for a writable RFC parameter and verify value via `rbuscli get`
- [ ] 9.3 Write L2 integration test: USP Get for a bootstrap parameter returns correct value from XBSStore
- [ ] 9.4 Write L2 integration test: Verify ReadOnly USP controller cannot SET bootstrap control parameters
- [ ] 9.5 Write L2 integration test: Verify value-change notification delivered to USP subscription after parameter modification via RBUS
- [ ] 9.6 Run TP-469 USP conformance test suite against obuspa with RDK-V plug-in loaded
- [ ] 9.7 Verify dual-stack operation: TR-069 ACS Set and USP Controller Set on same parameter, confirm last-writer-wins and correct journal entries
- [ ] 9.8 Measure obuspa RSS memory footprint on target RDK-V SoC and document results
- [ ] 9.9 Measure USP Get/Set throughput (params/sec) via RBUS bridge and compare against WebPA baseline
