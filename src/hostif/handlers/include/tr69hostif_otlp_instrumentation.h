/**
 * OpenTelemetry OTLP HTTP Instrumentation Header for TR69HostIF
 * C-compatible interface for tracing TR69HostIF operations with distributed tracing support
 */

#ifndef TR69HOSTIF_OTLP_INSTRUMENTATION_H
#define TR69HOSTIF_OTLP_INSTRUMENTATION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Trace context structure for distributed tracing
 * Contains W3C Trace Context fields for propagation
 */
typedef struct {
    char trace_id[33];     // 32 hex chars + null terminator (128-bit trace ID)
    char span_id[17];      // 16 hex chars + null terminator (64-bit span ID)
    char trace_flags[3];   // 2 hex chars + null terminator (8-bit flags)
} tr69hostif_trace_context_t;

/**
 * Start a child span for tr69hostif parameter operation with parent context
 * @param param_name The parameter name being processed
 * @param operation_type Operation type ("get" or "set")
 * @param parent_trace_id Parent trace ID (32 hex chars) from RFC
 * @param parent_span_id Parent span ID (16 hex chars) from RFC
 * @param parent_trace_flags Parent trace flags (2 hex chars) from RFC
 * @return Span handle (void*) to be passed to tr69hostif_otlp_end_span, or NULL on error
 */
void* tr69hostif_otlp_start_child_span(const char* param_name,
                                        const char* operation_type,
                                        const char* parent_trace_id,
                                        const char* parent_span_id,
                                        const char* parent_trace_flags);

/**
 * End a tr69hostif span
 * @param span_handle The span handle returned from tr69hostif_otlp_start_child_span
 * @param success true if operation succeeded, false otherwise
 * @param error_msg Error message if success is false, NULL otherwise
 */
void tr69hostif_otlp_end_span(void* span_handle, bool success, const char* error_msg);

/**
 * Legacy API - Trace a parameter GET operation (non-distributed)
 * @param param_name The parameter name being retrieved
 */
void tr69hostif_otlp_trace_parameter_get(const char* param_name);

/**
 * Legacy API - Trace a parameter SET operation (non-distributed)
 * @param param_name The parameter name being set
 */
void tr69hostif_otlp_trace_parameter_set(const char* param_name);

/**
 * Force flush all pending spans to the collector
 */
void tr69hostif_otlp_force_flush(void);

/**
 * Get the current OTLP collector endpoint
 * @return The endpoint URL string
 */
const char* tr69hostif_otlp_get_endpoint(void);

/**
 * Initialize metrics collector
 */
void tr69hostif_metrics_init();

/**
 * Record a parameter operation metric
 */
void tr69hostif_metrics_record_parameter_operation(const char* param_name,
                                                     const char* operation_type,
                                                     double duration_seconds);

#ifdef __cplusplus
}
#endif

#endif // TR69HOSTIF_OTLP_INSTRUMENTATION_H
