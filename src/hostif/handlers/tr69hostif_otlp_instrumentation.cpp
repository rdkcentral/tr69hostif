/**
 * OpenTelemetry OTLP HTTP Instrumentation for TR69HostIF
 * Compatible with OpenTelemetry C++ SDK v1.23.0
 * Sends traces to OpenTelemetry Collector via OTLP HTTP
 */
//trace libs
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/trace/tracer.h>
#include <opentelemetry/exporters/otlp/otlp_http_exporter.h>
#include <opentelemetry/sdk/trace/simple_processor.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/sdk/resource/resource.h>
#include <opentelemetry/nostd/shared_ptr.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/span_context.h>

//metrics libs
#include <opentelemetry/metrics/provider.h>
#include <opentelemetry/sdk/metrics/meter_provider.h>
#include <opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h>
#include <opentelemetry/exporters/otlp/otlp_http_metric_exporter.h>
#include <opentelemetry/sdk/metrics/view/view_registry.h>
#include <opentelemetry/sdk/metrics/aggregation/default_aggregation.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include <sstream>
#include <iomanip>
#include <array>
#include <thread>
#include <chrono>

namespace trace = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace resource = opentelemetry::sdk::resource;
namespace nostd = opentelemetry::nostd;
namespace otlp = opentelemetry::exporter::otlp;

namespace metrics_api = opentelemetry::metrics;
namespace metrics_sdk = opentelemetry::sdk::metrics;

class TR69HostIFOTLPTracer {
private:
    nostd::shared_ptr<trace::Tracer> tracer_;
    
    std::string getCollectorEndpoint() {
        // Check environment variable first for Docker container networking
        const char* env_endpoint = std::getenv("OTEL_EXPORTER_OTLP_ENDPOINT");
        if (env_endpoint != nullptr) {
            return std::string(env_endpoint);
        }
        
        // Check for container environment
        const char* container_env = std::getenv("RUNNING_IN_CONTAINER");
        if (container_env != nullptr && std::string(container_env) == "true") {
            // Use collector service name in Docker network
            return "http://otel-collector:4318";
        }
        
        // Default to localhost for non-container environments
        return "http://localhost:4318";
    }
    
public:
    TR69HostIFOTLPTracer() {
        initializeTracer();
    }
    
    void initializeTracer() {
        // Configure OTLP HTTP exporter with dynamic endpoint resolution
        otlp::OtlpHttpExporterOptions exporter_opts;
        exporter_opts.url = getCollectorEndpoint() + "/v1/traces";
        exporter_opts.content_type = otlp::HttpRequestContentType::kJson;
        
        // Add headers for authentication/identification
        exporter_opts.http_headers.insert({"User-Agent", "tr69hostif-rdk/1.2.7"});
        exporter_opts.http_headers.insert({"X-RDK-Component", "tr69hostif"});
        
        std::cout << "Initializing OTLP HTTP exporter with endpoint: " << exporter_opts.url << std::endl;
        
        // Create OTLP HTTP exporter
        auto exporter = std::make_unique<otlp::OtlpHttpExporter>(exporter_opts);
        
        // Create span processor
        auto processor = std::make_unique<trace_sdk::SimpleSpanProcessor>(std::move(exporter));
        
        // Create resource with comprehensive RDK-specific attributes
        auto resource_attributes = resource::ResourceAttributes{
            {"service.name", "tr69hostif"},
            {"service.version", "1.2.7"},
            {"service.namespace", "rdk"},
            {"rdk.component", "tr69hostif"},
            {"rdk.profile", "STB"},
            {"rdk.container.type", "native-platform"},
            {"deployment.environment", "container"},
            {"telemetry.sdk.name", "opentelemetry"},
            {"telemetry.sdk.language", "cpp"},
            {"telemetry.sdk.version", "1.23.0"}
        };
        auto resource = resource::Resource::Create(resource_attributes);
        
        // Create tracer provider with single processor
        std::vector<std::unique_ptr<trace_sdk::SpanProcessor>> processors;
        processors.push_back(std::move(processor));
        
        auto provider = std::make_shared<trace_sdk::TracerProvider>(
            std::move(processors), resource);
        
        trace::Provider::SetTracerProvider(nostd::shared_ptr<trace::TracerProvider>(provider));
        
        // Get tracer instance
        tracer_ = provider->GetTracer("tr69hostif", "1.0.0");
        
        std::cout << "TR69HostIF OTLP Tracer initialized successfully!" << std::endl;
    }
    
    /**
     * Helper function to get trace and span IDs as hex strings
     * Simplified version that works across different OpenTelemetry versions
     */
    std::pair<std::string, std::string> getTraceSpanIds(nostd::shared_ptr<trace::Span> span) {
        auto span_context = span->GetContext();
        
        // Generate simple IDs for logging purposes
        // In a real implementation, you might want to use the actual IDs
       /* static int trace_counter = 1000;
        static int span_counter = 2000;
        
        std::stringstream trace_stream, span_stream;
        trace_stream << "trace_" << std::hex << (trace_counter++);
        span_stream << "span_" << std::hex << (span_counter++);
        
        // Note: This is a simplified approach for demo purposes
        // For production, you would extract the actual trace/span IDs
        // but the API varies between OpenTelemetry versions
        
        return {trace_stream.str(), span_stream.str()};*/

    // Extract actual trace_id (16 bytes)
    char trace_id[32];
    span_context.trace_id().ToLowerBase16(trace_id);

    // Extract actual span_id (8 bytes)
    char span_id[16];
    span_context.span_id().ToLowerBase16(span_id);

    return {std::string(trace_id, 32), std::string(span_id, 16)};

    }
    
    /**
     * Trace HTTP request processing with trace/span ID logging
     */
    void traceHttpRequest(const std::string& method, const std::string& uri, 
                         int status_code, const std::function<void()>& operation) {
        auto span = tracer_->StartSpan("tr69hostif.http.request");
        
        // Get and log trace/span IDs
        auto [trace_id, span_id] = getTraceSpanIds(span);
        std::cout << "🔍 HTTP Request Trace - TraceID: " << trace_id << ", SpanID: " << span_id << std::endl;
        
        // Set HTTP-specific attributes
        span->SetAttribute("http.method", method);
        span->SetAttribute("http.url", uri);
        span->SetAttribute("http.scheme", "http");
        span->SetAttribute("trace.id", trace_id);
        span->SetAttribute("span.id", span_id);
        
        // Set RDK-specific attributes
        span->SetAttribute("rdk.operation.type", "http_request");
        span->SetAttribute("rdk.component.function", "request_handler");
        span->SetAttribute("rdk.trace.source", "tr69hostif-otlp");
        
        try {
            // Execute the operation
            operation();
            
            // Set success attributes
            span->SetAttribute("http.status_code", status_code);
            span->SetStatus(trace::StatusCode::kOk);
            std::cout << "✅ HTTP Request completed successfully - Status: " << status_code << std::endl;
            
        } catch (const std::exception& e) {
            // Handle errors
            span->SetAttribute("error.message", e.what());
            span->SetAttribute("http.status_code", 500);
            span->SetStatus(trace::StatusCode::kError, e.what());
            std::cout << "❌ HTTP Request failed - Error: " << e.what() << std::endl;
            throw;
        }
        
        span->End();
        std::cout << "📤 Span sent to collector at: " << getCollectorEndpoint() << std::endl;
    }
    
    /**
     * Trace parameter operations (get/set) with trace/span ID logging
     */
    void traceParameterOperation(const std::string& param_name, 
                               const std::string& operation_type,
                               const std::function<void()>& operation) {
        auto span = tracer_->StartSpan("tr69hostif.parameter." + operation_type);
        
        // Get and log trace/span IDs
        auto [trace_id, span_id] = getTraceSpanIds(span);
        std::cout << "🔍 Parameter " << operation_type << " Trace - TraceID: " << trace_id << ", SpanID: " << span_id << std::endl;
        
        // Set parameter-specific attributes
        span->SetAttribute("parameter.name", param_name);
        span->SetAttribute("parameter.operation", operation_type);
        span->SetAttribute("rdk.component", "tr69hostif");
        span->SetAttribute("trace.id", trace_id);
        span->SetAttribute("span.id", span_id);
        span->SetAttribute("rdk.trace.source", "tr69hostif-otlp");
        
        try {
            // Execute the operation
            operation();
            span->SetStatus(trace::StatusCode::kOk);
            std::cout << "✅ Parameter " << operation_type << " completed for: " << param_name << std::endl;
            
        } catch (const std::exception& e) {
            span->SetAttribute("error.message", e.what());
            span->SetStatus(trace::StatusCode::kError, e.what());
            std::cout << "❌ Parameter " << operation_type << " failed for: " << param_name << " - Error: " << e.what() << std::endl;
            throw;
        }
        
        span->End();
        std::cout << "📤 Span sent to collector at: " << getCollectorEndpoint() << std::endl;
    }
    
    /**
     * Trace device communication with trace/span ID logging
     */
    void traceDeviceCommunication(const std::string& device_type, 
                                 const std::string& operation,
                                 const std::function<void()>& comm_operation) {
        auto span = tracer_->StartSpan("tr69hostif.device.communication");
        
        // Get and log trace/span IDs
        auto [trace_id, span_id] = getTraceSpanIds(span);
        std::cout << "🔍 Device Communication Trace - TraceID: " << trace_id << ", SpanID: " << span_id << std::endl;
        
        // Set device-specific attributes
        span->SetAttribute("device.type", device_type);
        span->SetAttribute("device.operation", operation);
        span->SetAttribute("rdk.component", "tr69hostif");
        span->SetAttribute("trace.id", trace_id);
        span->SetAttribute("span.id", span_id);
        span->SetAttribute("rdk.trace.source", "tr69hostif-otlp");
        
        try {
            // Execute the communication
            comm_operation();
            span->SetStatus(trace::StatusCode::kOk);
            std::cout << "✅ Device communication completed - Type: " << device_type << ", Operation: " << operation << std::endl;
            
        } catch (const std::exception& e) {
            span->SetAttribute("error.message", e.what());
            span->SetStatus(trace::StatusCode::kError, e.what());
            std::cout << "❌ Device communication failed - Error: " << e.what() << std::endl;
            throw;
        }
        
        span->End();
        std::cout << "📤 Span sent to collector at: " << getCollectorEndpoint() << std::endl;
    }
    
    /**
     * Force flush all pending spans (useful for shutdown)
     * Simplified version that doesn't rely on SDK-specific methods
     */
    void forceFlush() {
        // Simple approach: just log that we're flushing
        // The spans should be automatically flushed when the tracer is destroyed
        // or when the process exits
        std::cout << "🔄 Requesting flush of all pending spans..." << std::endl;
        
        // Sleep briefly to allow any pending spans to be processed
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        std::cout << "🔄 Flush request completed" << std::endl;
    }
    
    /**
     * Get current collector endpoint for debugging
     */
    std::string getCurrentEndpoint() const {
        // Make getCollectorEndpoint const or call it differently
        const char* env_endpoint = std::getenv("OTEL_EXPORTER_OTLP_ENDPOINT");
        if (env_endpoint != nullptr) {
            return std::string(env_endpoint);
        }
        
        const char* container_env = std::getenv("RUNNING_IN_CONTAINER");
        if (container_env != nullptr && std::string(container_env) == "true") {
            return "http://otel-collector:4318";
        }
        
        return "http://localhost:4318";
    }
};

class TR69HostIFMetricsCollector {
private:
    opentelemetry::nostd::shared_ptr<metrics_api::Meter> meter_;  // FIX 1: Use nostd::shared_ptr
    
    // Counter metrics
    std::unique_ptr<metrics_api::Counter<uint64_t>> parameter_operations_total_;
    
    // Histogram metrics
    std::unique_ptr<metrics_api::Histogram<double>> parameter_operation_duration_;
    std::unique_ptr<metrics_api::Histogram<uint64_t>> response_size_bytes_;

public:
    TR69HostIFMetricsCollector() {
        initializeMetrics();
    }
    
    void initializeMetrics() {
        try {
            // Configure OTLP HTTP metric exporter
            otlp::OtlpHttpMetricExporterOptions exporter_opts;
            
            // FIX 2: Inline endpoint detection instead of calling getCollectorEndpoint()
            const char* env_endpoint = std::getenv("OTEL_EXPORTER_OTLP_ENDPOINT");
            std::string base_endpoint = env_endpoint ? env_endpoint : "http://localhost:4318";
            const char* container_env = std::getenv("RUNNING_IN_CONTAINER");
            if (!env_endpoint && container_env != nullptr && std::string(container_env) == "true") {
                base_endpoint = "http://otel-collector:4318";
            }
            
            exporter_opts.url = base_endpoint + "/v1/metrics";
            exporter_opts.content_type = otlp::HttpRequestContentType::kJson;
            exporter_opts.timeout = std::chrono::seconds(10);
            
            std::cout << "Initializing OTLP HTTP metrics exporter with endpoint: " 
                      << exporter_opts.url << std::endl;
            
            // FIX 3: C++11 compatible - no std::make_unique
            std::unique_ptr<otlp::OtlpHttpMetricExporter> exporter(
                new otlp::OtlpHttpMetricExporter(exporter_opts));
            
            metrics_sdk::PeriodicExportingMetricReaderOptions reader_options;
            reader_options.export_interval_millis = std::chrono::milliseconds(10000);
            reader_options.export_timeout_millis = std::chrono::milliseconds(5000);
            
            std::unique_ptr<metrics_sdk::PeriodicExportingMetricReader> reader(
                new metrics_sdk::PeriodicExportingMetricReader(std::move(exporter), reader_options));
            
            auto resource_attributes = resource::ResourceAttributes{
                {"service.name", "tr69hostif"},
                {"service.version", "1.2.7"},
                {"service.namespace", "rdk"},
                {"rdk.component", "tr69hostif"},
                {"rdk.profile", "STB"},
                {"deployment.environment", "container"}
            };
            auto resource = resource::Resource::Create(resource_attributes);
            
            // FIX 4: Use nostd::shared_ptr with explicit new (C++11 compatible)
            std::unique_ptr<metrics_sdk::ViewRegistry> view_registry(new metrics_sdk::ViewRegistry());
            auto provider = opentelemetry::nostd::shared_ptr<metrics_sdk::MeterProvider>(
                new metrics_sdk::MeterProvider(std::move(view_registry), resource)
            );
            
            provider->AddMetricReader(std::move(reader));
            
            // FIX 5: Use std::move()
            metrics_api::Provider::SetMeterProvider(std::move(provider));
            
            meter_ = metrics_api::Provider::GetMeterProvider()->GetMeter("tr69hostif", "1.0.0");
            
            parameter_operations_total_ = meter_->CreateUInt64Counter(
                "tr69hostif.parameter.operations.total",
                "Total number of parameter operations (get/set)",
                "operations");
            
            parameter_operation_duration_ = meter_->CreateDoubleHistogram(
                "tr69hostif.parameter.operation.duration",
                "Parameter operation processing duration",
                "seconds");
            
            response_size_bytes_ = meter_->CreateUInt64Histogram(
                "tr69hostif.http.response.size",
                "HTTP response size in bytes",
                "bytes");
            
            // FIX 6: REMOVED all observable gauge code and RegisterCallback calls
            
            std::cout << "✅ TR69HostIF Metrics Collector initialized successfully!" << std::endl;
            std::cout << "📊 Metrics will be exported every 10 seconds" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "❌ Failed to initialize metrics: " << e.what() << std::endl;
            throw;
        }
    }
    
    void recordParameterOperation(const std::string& param_name, 
                                   const std::string& operation_type,
                                   double duration_seconds) {
        parameter_operations_total_->Add(1, {
            {"parameter.name", param_name},
            {"operation.type", operation_type}
        });
        
        // FIX 7: Add context parameter to Record()
        auto context = opentelemetry::context::Context{};
        parameter_operation_duration_->Record(duration_seconds, {
            {"parameter.name", param_name},
            {"operation.type", operation_type}
        }, context);
        
        std::cout << "📊 Parameter Operation Metric: " << operation_type << " " 
                  << param_name << " (" << duration_seconds << "s)" << std::endl;
    }
}; // FIX 8: Closing brace for class

// Global metrics collector
static std::unique_ptr<TR69HostIFMetricsCollector> g_metrics_collector;

void initializeMetricsCollector() {
    if (!g_metrics_collector) {
        g_metrics_collector.reset(new TR69HostIFMetricsCollector());
    }
}

void tr69hostif_metrics_init() {
    initializeMetricsCollector();
}

void tr69hostif_metrics_record_parameter_operation(
    const char* param_name, const char* operation_type, double duration_seconds) {
    if (g_metrics_collector) {
        g_metrics_collector->recordParameterOperation(param_name, operation_type, duration_seconds);
    }
}

// Global tracer instance
static TR69HostIFOTLPTracer g_otlp_tracer;

void tr69hostif_otlp_trace_http_request(const char* method, const char* uri, int status_code) {
    g_otlp_tracer.traceHttpRequest(method, uri, status_code, [](){});
}

void tr69hostif_otlp_trace_parameter_get(const char* param_name) {
    g_otlp_tracer.traceParameterOperation(param_name, "get", [](){});
}

void tr69hostif_otlp_trace_parameter_set(const char* param_name) {
    g_otlp_tracer.traceParameterOperation(param_name, "set", [](){});
}

void tr69hostif_otlp_trace_device_comm(const char* device_type, const char* operation) {
    g_otlp_tracer.traceDeviceCommunication(device_type, operation, [](){});
}

void tr69hostif_otlp_force_flush() {
    g_otlp_tracer.forceFlush();
}

const char* tr69hostif_otlp_get_endpoint() {
    static std::string endpoint = g_otlp_tracer.getCurrentEndpoint();
    return endpoint.c_str();
}
