/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2018 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include <glib.h>
#include <cJSON.h>
#include <stdlib.h>
#include <fstream>

#ifdef __cplusplus
extern "C"
{
#endif
#include <wdmp-c.h>
#include <wdmp_internal.h>
#ifdef __cplusplus
}
#endif

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "http_server.h"
#include "waldb.h"
#include "request_handler.h"
#include "libsoup/soup.h"
#include <mutex>
#include <condition_variable>

#include <thread>
#include <future>
#include <unordered_map>
#include <atomic>
struct AsyncResponseContext {
#ifdef LIBSOUP3_ENABLE
    SoupServerMessage* msg;
#else
    SoupMessage* msg;
#endif
    std::string request_id;
    cJSON* jsonRequest;
    req_struct* reqSt;
    const char* pcCallerID;
    std::string method;
};

extern std::mutex mtx_httpServerThreadDone;
extern std::condition_variable cv_httpServerThreadDone;
extern bool httpServerThreadDone;

extern T_ARGLIST argList;
static SoupServer  *http_server = NULL;

// Global map to track async requests
static std::unordered_map<std::string, AsyncResponseContext*> g_pending_requests;
static std::mutex g_pending_requests_mutex;
static std::atomic<uint64_t> g_request_counter{0};


// Thread pool for processing requests
class SimpleThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;

public:
    SimpleThreadPool(size_t num_threads = 8) : stop(false) {
        for(size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                for(;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });
                        if(this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    ~SimpleThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for(std::thread &worker: workers)
            worker.join();
    }

    template<class F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if(stop)
                return;
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }
};

// Global thread pool
static SimpleThreadPool* g_thread_pool = nullptr;

#ifdef LIBSOUP3_ENABLE
static void HTTPRequestHandler(
    SoupServer        *server,
    SoupServerMessage *msg,
    const char        *path,
    GHashTable        *query,
    void           *user_data)
#else
static void HTTPRequestHandler(
    SoupServer        *server,
    SoupMessage       *msg,
    const char        *path,
    GHashTable        *query,
    SoupClientContext *client,
    void           *user_data)
#endif
{
    struct timespec start, end, *startPtr, *endPtr;
    startPtr = &start;
    endPtr = &end;

    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    getCurrentTime(startPtr);

    // Get request body
#ifdef LIBSOUP3_ENABLE
    SoupMessageBody *req_body = soup_server_message_get_request_body(msg);
#else
    SoupMessageBody *req_body = msg->request_body;
#endif

    if (!req_body || !req_body->data || !req_body->length) {
#ifdef LIBSOUP3_ENABLE
        soup_server_message_set_status(msg, SOUP_STATUS_BAD_REQUEST, "No request data.");
#else
        soup_message_set_status_full(msg, SOUP_STATUS_BAD_REQUEST, "No request data.");
#endif
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. Failed due to no message data.\n", __FUNCTION__, __FILE__);
        return;
    }

    // Get headers
#ifdef LIBSOUP3_ENABLE
    SoupMessageHeaders *req_headers = soup_server_message_get_request_headers(msg);
    const char *pcCallerID = soup_message_headers_get_one(req_headers, "CallerID");
    const char *method = soup_server_message_get_method(msg);
#else
    const char *pcCallerID = soup_message_headers_get_one(msg->request_headers, "CallerID");
    const char *method = msg->method;
#endif

    // Parse JSON
    cJSON *jsonRequest = cJSON_Parse((const char *) req_body->data);
    if (!jsonRequest) {
#ifdef LIBSOUP3_ENABLE
        soup_server_message_set_status(msg, SOUP_STATUS_BAD_REQUEST, "Bad Request");
#else
        soup_message_set_status_full(msg, SOUP_STATUS_BAD_REQUEST, "Bad Request");
#endif
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. Failed to parse JSON Message\n", __FUNCTION__, __FILE__);
        return;
    }

    // Validate method and CallerID (same logic as before)
    if (strcmp(method, "GET") != 0 && strcmp(method, "POST") != 0) {
#ifdef LIBSOUP3_ENABLE
        soup_server_message_set_status(msg, SOUP_STATUS_NOT_IMPLEMENTED, "Method not implemented");
#else
        soup_message_set_status_full(msg, SOUP_STATUS_NOT_IMPLEMENTED, "Method not implemented");
#endif
        cJSON_Delete(jsonRequest);
        return;
    }

    if (strcmp(method, "POST") == 0 && (!pcCallerID || !strlen(pcCallerID))) {
#ifdef LIBSOUP3_ENABLE
        soup_server_message_set_status(msg, SOUP_STATUS_INTERNAL_SERVER_ERROR, "POST Not Allowed without CallerID");
#else
        soup_message_set_status_full(msg, SOUP_STATUS_INTERNAL_SERVER_ERROR, "POST Not Allowed without CallerID");
#endif
        cJSON_Delete(jsonRequest);
        return;
    }

    if (!pcCallerID || !strlen(pcCallerID)) {
        pcCallerID = "Unknown";
    }

    // Create request structure
    req_struct *reqSt = (req_struct *)malloc(sizeof(req_struct));
    if (!reqSt) {
#ifdef LIBSOUP3_ENABLE
        soup_server_message_set_status(msg, SOUP_STATUS_INTERNAL_SERVER_ERROR, "Cannot create return object");
#else
        soup_message_set_status_full(msg, SOUP_STATUS_INTERNAL_SERVER_ERROR, "Cannot create return object");
#endif
        cJSON_Delete(jsonRequest);
        return;
    }
    memset(reqSt, 0, sizeof(req_struct));

    // Parse request based on method
    if (strcmp(method, "GET") == 0) {
        parse_get_request(jsonRequest, &reqSt, WDMP_TR181);
    } else {
        parse_set_request(jsonRequest, &reqSt, WDMP_TR181);
    }

    // Generate unique request ID
    std::string request_id = std::to_string(++g_request_counter);

    // Create async context
    AsyncResponseContext* context = new AsyncResponseContext();
    context->msg = msg;
    context->request_id = request_id;
    context->jsonRequest = jsonRequest;
    context->reqSt = reqSt;
    context->pcCallerID = pcCallerID;
    context->method = method;

    // Store context for async processing
    {
        std::lock_guard<std::mutex> lock(g_pending_requests_mutex);
        g_pending_requests[request_id] = context;
    }

    // Submit to thread pool for async processing
    g_thread_pool->enqueue([request_id]() {
        processRequestAsync(request_id);
    });

    // IMPORTANT: Do NOT send response here - it will be sent asynchronously
    // The HTTP connection remains open until processRequestAsync completes

    getCurrentTime(endPtr);
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "HTTP request queued for async processing: %s, time: %lu ms\n", 
            request_id.c_str(), timeValDiff(startPtr, endPtr));
    
    // Function returns immediately - response will be sent later!
    return;
}

void *HTTPServerStartThread(void *msg)
{
    /*Start HTTP server */
    GError *error = NULL;
    int status =-1;
    guint httpServerPort = argList.httpServerPort;

    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

#ifndef GLIB_VERSION_2_36
    g_type_init ();
#endif

    status = checkDataModelStatus();
    if(status != 0)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error in Data Model Initialization\n");
        return NULL;
    }

    if(http_server == NULL)
#ifdef LIBSOUP3_ENABLE
        http_server = soup_server_new("server-header", "HTTPServer", NULL);
#else
        http_server = soup_server_new (SOUP_SERVER_SERVER_HEADER, "HTTPServer", NULL);
#endif

    if (!http_server)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"SERVER: Could not create server.\n");
        return NULL;
    }
    else
    {
        soup_server_add_handler (http_server, (const char*)"/", HTTPRequestHandler, NULL, NULL);

        if(FALSE == soup_server_listen_local (http_server,  httpServerPort, SOUP_SERVER_LISTEN_IPV4_ONLY, &error))
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"SERVER: failed in soup_server_listen_local. (%s).\n", error->message);
        }

        //Create a .http_server_ready file in /tmp for RFC to check whether http server is ready to accept requests
        ofstream ofs("/tmp/.tr69hostif_http_server_ready", ios::trunc | ios::out);
        if(!ofs.is_open())
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : /tmp/.tr69hostif_http_server_ready \n");
        else
            ofs.close();

        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"SERVER: Started server successfully.\n");
    }

    {
        std::unique_lock<std::mutex> lck(mtx_httpServerThreadDone);
        httpServerThreadDone = true;
        cv_httpServerThreadDone.notify_all();
    }

    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return NULL;
}

void HttpServerStop()
{
    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"SERVER: Stopping HTTP Server....\n");
    if(http_server) {
        std::unique_lock<std::mutex> lck(mtx_httpServerThreadDone);
        httpServerThreadDone = false;
        soup_server_disconnect(http_server);
        RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"SERVER: Stopped server successfully.\n");
    }
}
// Function to process request asynchronously
void processRequestAsync(const std::string& request_id) {
    AsyncResponseContext* context = nullptr;
    
    // Get the context
    {
        std::lock_guard<std::mutex> lock(g_pending_requests_mutex);
        auto it = g_pending_requests.find(request_id);
        if (it != g_pending_requests.end()) {
            context = it->second;
            g_pending_requests.erase(it);
        }
    }
    
    if (!context) {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Context not found for request_id: %s\n", request_id.c_str());
        return;
    }

    cJSON* jsonResponse = nullptr;
    res_struct* respSt = nullptr;
    char* buf = nullptr;

    try {
        // Process the request - THIS IS WHERE YOUR EXISTING LOGIC RUNS
        respSt = handleRequest(context->pcCallerID, context->reqSt);
        
        if (respSt) {
            jsonResponse = cJSON_CreateObject();
            
            if (context->method == "GET") {
                wdmp_form_get_response(respSt, jsonResponse);
            } else {
                wdmp_form_set_response(respSt, jsonResponse);
            }

            // Handle status code logic (same as original)
            int new_st_code = 0;
            for(size_t paramIndex = 0; paramIndex < respSt->paramCnt; paramIndex++) {
                if(respSt->retStatus[paramIndex] != 0 || paramIndex == respSt->paramCnt-1) {
                    new_st_code = respSt->retStatus[paramIndex];
                    break;
                }
            }
            cJSON* stcode = cJSON_GetObjectItem(jsonResponse, "statusCode");
            if(stcode != NULL) {
                cJSON_SetIntValue(stcode, new_st_code);
            }
            
            buf = cJSON_Print(jsonResponse);
        }

// Initialize and cleanup functions
void InitializeAsyncHttpServer() {
    if (!g_thread_pool) {
        g_thread_pool = new SimpleThreadPool(8); // 8 worker threads
    }
}
