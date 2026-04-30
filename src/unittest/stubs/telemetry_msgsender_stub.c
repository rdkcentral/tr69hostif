/*
 * telemetry_msgsender_stub.c — Stub shared library for tr69hostif L2/coverage builds.
 *
 * devicesettings rpc/srv links against -ltelemetry_msgsender (hardcoded in
 * libdshalsrv_la_LIBADD).  The real library belongs to the telemetry2 component
 * which is not present in the native/container build environment.
 *
 * All functions are no-ops that satisfy the linker without pulling in telemetry2.
 * Because the header declares these as static inline, every TU already has inline
 * copies; these exported symbols are only needed for the shared-library link step.
 */

int t2_init(const char *component) { (void)component; return 0; }
int t2_event_s(const char *marker, const char *value) { (void)marker; (void)value; return 0; }
int t2_event_d(const char *marker, double value) { (void)marker; (void)value; return 0; }
int t2_event_f(const char *marker, double value) { (void)marker; (void)value; return 0; }
