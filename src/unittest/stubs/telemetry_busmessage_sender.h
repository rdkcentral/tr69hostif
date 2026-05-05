/*
 * telemetry_busmessage_sender.h — Native-build stub for tr69hostif L2/coverage builds.
 *
 * The real header is part of the telemetry2 component which is not built in the
 * native/container test environment.  All APIs are stubbed as no-ops so that
 * devicesettings rpc/srv compiles without a telemetry2 install.
 */
#ifndef _TELEMETRY_BUSMESSAGE_SENDER_H_
#define _TELEMETRY_BUSMESSAGE_SENDER_H_

#ifdef __cplusplus
extern "C" {
#endif

static inline int t2_init(const char *component) { (void)component; return 0; }
static inline int t2_event_s(const char *marker, const char *value) { (void)marker; (void)value; return 0; }
static inline int t2_event_d(const char *marker, double value) { (void)marker; (void)value; return 0; }
static inline int t2_event_f(const char *marker, double value) { (void)marker; (void)value; return 0; }

#ifdef __cplusplus
}
#endif

#endif /* _TELEMETRY_BUSMESSAGE_SENDER_H_ */
