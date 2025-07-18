/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
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


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#ifndef NEW_HTTP_SERVER_DISABLE
#include <fstream>
#endif
#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_jsonReqHandler.h"

#ifndef NEW_HTTP_SERVER_DISABLE
#include "hostIf_msgHandler.h"
#include "http_server.h"
#include "hostIf_utils.h"
#endif

#include "hostIf_updateHandler.h"
#include "XrdkCentralComBSStore.h"

#if defined(USE_WIFI_PROFILE)
#include "Device_WiFi.h"
#endif

#include "libpd.h"
#include <semaphore.h>
#include <errno.h>
#include "libIBus.h"

#define LOG_FILE 		"./tr69hostIflog.txt"
#ifdef WEBPA_RFC_ENABLED
#include <sys/stat.h>
#include <unistd.h>
#define RFC_BUFFER_SIZE 256
#endif

#ifdef ENABLE_SD_NOTIFY
#include <systemd/sd-daemon.h>
#endif
#include <mutex>
#include <condition_variable>

#ifdef WEB_CONFIG_ENABLED
#ifdef __cplusplus
extern "C" {
#include <webcfg.h>
}
#endif
#include "cJSON.h"
#include <sys/file.h>
#endif

#ifdef WEBCONFIG_LITE_ENABLE
#include<webconfig_lite.h>
#endif

#ifdef T2_EVENT_ENABLED
#include <telemetry_busmessage_sender.h>
#endif

#include "hostIf_rbus_Dml_Provider.h"
#include "Device_DeviceInfo.h"
#include "safec_lib.h"

//------------------------------------------------------------------------------
// Initialize global variables and functions.
//------------------------------------------------------------------------------
GThread *hostIf_JsonIfThread = NULL;
GThread *updateHandler_runThread = NULL;
#ifndef NEW_HTTP_SERVER_DISABLE
GThread *HTTPServerThread = NULL;
#define LEGACY_RFC_ENABLED_PATH "/opt/RFC/.RFC_LegacyRFCEnabled.ini"
#endif
//GTimeVal timeval;
static GMainLoop *main_loop = NULL;
gchar *date_str = NULL;
const gchar* logfilename = NULL;
FILE *logfile = NULL;
int rdk_logger_enabled = 0;
GHashTable* paramMgrhash = NULL;
static void usage();
T_ARGLIST argList = {{'\0'}, 0};
static int isShutdownTriggered = 0;

#define DEVICE_PROPS_FILE "/etc/device.properties"
#define GENERIC_XML_FILE "/etc/data-model-generic.xml"
#define STB_XML_FILE "/etc/data-model-stb.xml"
#define TV_XML_FILE "/etc/data-model-tv.xml"
#define WEBPA_DATA_MODEL_FILE "/tmp/data-model.xml"


std::mutex mtx_httpServerThreadDone;
std::condition_variable cv_httpServerThreadDone;
bool httpServerThreadDone = false;

#if defined(PARODUS_ENABLE)
// Parodus Init Thread
pthread_t parodus_init_tid;
#endif

#ifdef WEBCONFIG_LITE_ENABLE
pthread_t webconfig_threadId;
#endif
char *hostIf_JsonIfMsg = (char *)"hostIf_JsonIfThread";
#ifndef NEW_HTTP_SERVER_DISABLE
char *HTTPServerName = (char *)"HTTPServerThread";
GError *httpError = NULL;
#endif
GError *err1 = NULL ;

/* Globals associated with the shutdown thread which frees all resources and exits the process
   when signalled to do so */
static  pthread_t   shutdown_thread = 0;                /* The thread ID */
static  void        *shutdown_thread_entry(void *arg);  /* Thread entry function */
static  sem_t       shutdown_thread_sem;                /* Semaphore used to signal shutdown */
static  int         shutdown_sig_received = 0;          /* The signal that triggered the shutdown */
pthread_mutex_t     graceful_exit_mutex;
#define UNUSED(x)   ((void)(x))

//------------------------------------------------------------------------------
// shutdown_thread_entry: frees all resources and exits the process
//                        when signalled to do so
//------------------------------------------------------------------------------
static void *shutdown_thread_entry(void *arg)
{
    UNUSED(arg);

    /* Wait forever on the shutdown semaphore */
    while ((sem_wait(&shutdown_thread_sem) == -1) && (errno == EINTR))
    {
        continue;   /* Restart if interrupted by handler */
    }

    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s] TR69 Host-If received signal handler with sigNum : %d \n",__FILE__, __FUNCTION__, shutdown_sig_received);

    switch (shutdown_sig_received)
    {
    case SIGINT:
        RDK_LOG(RDK_LOG_NOTICE,LOG_TR69HOSTIF,"SIGINT received (CTRL+C was pressed) \n");
        break;
    case SIGTERM:
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"SIGTERM received\n");
        break;
    case SIGQUIT:
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"SIGQUIT received\n");
        break;
    case SIGSEGV:
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"SIGSEGV received (Segmentation fault was detected)\n");
        break;
    default:
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Signal %d received\n", shutdown_sig_received);
        break;
    }

    exit_gracefully(shutdown_sig_received);
    return NULL;
}

#ifdef WEBPA_RFC_ENABLED
/* Utility funciton to get the console output. */
bool GetFeatureEnabled(char *cmd)
{
   struct stat buffer;
   string fileName = "/opt/secure/RFC/" + string(".RFC_") + feature + ".ini";

   return (stat(fileName.c_str(), &buffer) == 0);
}
#endif

/* Description: Use for sending telemetry Log
 * @param marker: use for send marker details
 * @return : void
 * */
#ifdef T2_EVENT_ENABLED
void t2CountNotify(const char *marker, int val) {
    t2_event_d(marker, val);
}

void t2ValNotify( const char *marker, const char *val )
{
    t2_event_s(marker, val);
}
#endif


//------------------------------------------------------------------------------
// main: HostIfMgr main
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    try
    {
        int ch = 0;
        errno_t rc = -1;
    #ifdef WEBPA_RFC_ENABLED
        bool retVal=false;
    #endif
        const char* debugConfigFile = NULL;
        const char* webpaNotifyConfigFile = NULL;
        //------------------------------------------------------------------------------
        // Signal handlers:
        //------------------------------------------------------------------------------
        struct sigaction sigact;

        while (1)
        {
            static struct option long_options[] =
            {
                /* These options don't set a flag.
                    We distinguish them by their indices. */
                {"help",    		no_argument, 0, 'h'},
                {"logfile",     	required_argument, 0, 'l'},
                {"conffile",     	required_argument, 0, 'c'},
                {"port",       		required_argument, 0, 'p'},
    #ifndef NEW_HTTP_SERVER_DISABLE
                {"httpserverport",		required_argument, 0, 's'},
    #endif
                {"debugconfig",     required_argument, 0, 'd'},
                {"notifyconfig",    required_argument, 0, 'w'},
                {0, 0, 0, 0}
            };

            /* getopt_long stores the option index here. */
            int option_index = 0;
    #ifndef NEW_HTTP_SERVER_DISABLE
            ch = getopt_long (argc, argv, "hHl:c:p:s:d:w:",
                              long_options, &option_index);
    #else
            ch = getopt_long (argc, argv, "hHl:c:p:d:w:",
                              long_options, &option_index);
    #endif
            /* Detect the end of the options. */
            if (ch == -1)
                break;

            switch (ch)
            {
            case 'c':
                if(optarg)
                {
                    memset(argList.confFile, '\0', sizeof (argList.confFile));
                    rc=strcpy_s (argList.confFile,sizeof(argList.confFile),optarg);
		    if(rc!=EOK)
    		    {
	    		ERR_CHK(rc);
    		    }
//                    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"argList.confFile : %s optarg : %s\n", argList.confFile, optarg);
                }
                break;

            case 'd':
                if(optarg)
                {
                    debugConfigFile = optarg;
                }
                break;
            case 'w':
                if(optarg)
                {
                    webpaNotifyConfigFile = optarg;
                } 
                break;

            case 'p':
                if(optarg)
                {
                    argList.httpPort = atoi(optarg);
                }
                break;
    #ifndef NEW_HTTP_SERVER_DISABLE
            case 's':
                if(optarg)
                {
                    argList.httpServerPort = atoi(optarg);
                }
                break;
    #endif
            case 'h':
            case 'H':
            case '?':
            default:
                usage();
                exit(0);
            }
        }

        /* Enable RDK logger.*/
        if(rdk_logger_init(debugConfigFile) == 0) rdk_logger_enabled = 1;
        #ifdef T2_EVENT_ENABLED
         t2_init(const_cast<char*>("tr69hostif"));
        #endif
        if (optind < argc)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"non-option ARGV-elements: ");
            while (optind < argc)
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s ", argv[optind++]);
            putchar ('\n');
            usage();
            exit (0);
        } 

    #ifdef WEBPA_RFC_ENABLED
        retVal = GetFeatureEnabled("WEBPAXG");
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s] WEBPAXG returns %d\n", __FUNCTION__, retVal);
        if( retVal == false)
        {
	    // Send a notification to systemd to stop the service
            int ret = sd_pid_notify(0, SD_FINALIZING);

            if (ret < 0) 
	    {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error sending stop notification: %s\n", strerror(-ret));
                return 1;
            }

            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Service stop notification sent successfully.\n");
            return ch;
        }
    #endif

        if (sem_init(&shutdown_thread_sem, 0, 0) == -1)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] sem_init() failed\n", __FUNCTION__, __FILE__);
            return 1;
        }

        if (pthread_create(&shutdown_thread, NULL, shutdown_thread_entry, NULL) != 0)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] pthread_create() failed\n", __FUNCTION__, __FILE__);
            return 1;
        }

        // The actions for SIGINT, SIGTERM, SIGSEGV, and SIGQUIT are set
        sigemptyset(&sigact.sa_mask);
        sigact.sa_handler = quit_handler;
        sigact.sa_flags = SA_ONSTACK;

        sigaction (SIGINT, &sigact, NULL);
        sigaction (SIGTERM, &sigact, NULL);
        sigaction (SIGHUP, &sigact, NULL);
        signal (SIGPIPE, SIG_IGN);

        setvbuf(stdout, NULL, _IOLBF, 0);

        //------------------------------------------------------------------------------
        // Initialize the glib, g_time and logger
        //------------------------------------------------------------------------------
    #if GLIB_VERSION_CUR_STABLE <= GLIB_VERSION_2_32
        if(!g_thread_supported())
        {
            g_thread_init(NULL);
            RDK_LOG(RDK_LOG_NOTICE,LOG_TR69HOSTIF,"g_thread supported\n");
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"g_thread NOT supported\n");
        } 
    #endif
        /* Enable RDK logger.*/
        if(rdk_logger_init(debugConfigFile) == 0) rdk_logger_enabled = 1;

        #if defined(USE_WIFI_PROFILE)
            /* Perform the necessary operations to initialise the WiFi device */
            (void)WiFiDevice::init();
        #endif
        //    g_get_current_time(&timeval);
        //    char* logoutfile = (char *)LOG_FILE;
        #if 0
           /* Commented: Since logs are directed to /opt/logs/ folder,
            * so no need to use separate log file */
        char* logoutfile = (char *)argList.logFileName;


        g_log_set_handler(G_LOG_DOMAIN, (GLogLevelFlags)(G_LOG_LEVEL_INFO | G_LOG_LEVEL_MESSAGE | \
                          G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL | \
                          G_LOG_LEVEL_ERROR), tr69hostIf_logger, (void *)logoutfile);
    #endif
    RDK_LOG(RDK_LOG_NOTICE,LOG_TR69HOSTIF,"Starting tr69HostIf Service\n");



    /*Commented: This function will replace hostIf_initalize_ConfigManger()
     This shall read all configuration properties of hostif like, profile Manager List,
     any hardcoded path used in reboot/reset or config paths. This uses g_key_file_new() to read
     based on the group of configuration. */
    //if(false == hostIf_ConfigProperties_Init())

        if(false == hostIf_initalize_ConfigManger())
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed to hostIf_initalize_ConfigManger()\n");
        }

    #ifndef NEW_HTTP_SERVER_DISABLE
        ifstream ifs_legacyEnabled(LEGACY_RFC_ENABLED_PATH);
        if(!ifs_legacyEnabled.is_open())
        {
            setLegacyRFCEnabled(false);
        }
        else
        {
            setLegacyRFCEnabled(true);
            ifs_legacyEnabled.close();
        }
    #endif

        if(false == hostIf_IARM_IF_Start() )
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed to start hostIf_IARM_IF_Start()\n");
        }
        MergeStatus mergeStatus = mergeDataModel();
        if (mergeStatus != MERGE_SUCCESS) {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Error in merging Data Model\n");
            return DB_FAILURE; // Or handle the failure appropriately
        } 
        else {
             RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "Successfully merged Data Model.\n");
        }
        /* Load the data model xml file*/
        DB_STATUS status = loadDataModel();
        if(status != DB_SUCCESS)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error in Data Model Initialization\n");
            return DB_FAILURE;
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"Successfully initialize Data Model.\n");
        }

    //------------------------------------------------------------------------------
    // hostIf_HttpServerStart: Soup HTTP Server
    //------------------------------------------------------------------------------
        if( (hostIf_JsonIfThread = g_thread_try_new( "json_if_handler_thread", (GThreadFunc)jsonIfHandlerThread, (void *)hostIf_JsonIfMsg, &err1)) == NULL)
        {
            g_critical("Thread create failed: %s!!\n", err1->message );
            g_error_free (err1);
        }	
    #ifndef NEW_HTTP_SERVER_DISABLE
        if(!legacyRFCEnabled())
        {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"legacyRFC Set to False, Starting New HTTP Server\n");
            if((HTTPServerThread = g_thread_try_new("http_server_thread", (GThreadFunc)HTTPServerStartThread, (void *)HTTPServerName, &httpError)) == NULL)
            {
                g_critical("New HTTP Server Thread Create failed: %s!!\n", httpError->message );
                g_error_free (httpError);
            }
        }
        else
        {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"legacyRFC Set to True, New HTTP Server is not started\n");
        }
    #endif

    #ifdef PID_FILE_PATH
    #define xstr(s) str(s)
    #define str(s) #s
        // write pidfile because sd_notify() does not work inside container
        IARM_Bus_WritePIDFile(xstr(PID_FILE_PATH) "/tr69hostif.pid");
    #endif

        //------------------------------------------------------------------------------
        // updateHandler::init :  Update handler thread for polling table profiles
        //------------------------------------------------------------------------------
        updateHandler::Init();

     #if defined(PARODUS_ENABLE)
        //------------------------------------------------------------------------------
        // Initialize WebPA Module
        //------------------------------------------------------------------------------

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Starting WEBPA Parodus Connections\n");
        libpd_set_notifyConfigFile(webpaNotifyConfigFile);
        if(0 == pthread_create(&parodus_init_tid, NULL, libpd_client_mgr, NULL))
        {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"Initiating Connection with PARODUS success.. \n");
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Parodus init thread create failed\n");
        }
    #endif

    #if defined(WEB_CONFIG_ENABLED)
        initWebConfigMultipartTask(0);
    #elif defined(WEBCONFIG_LITE_ENABLE)
        if(0 == pthread_create(&webconfig_threadId, NULL, initWebConfigTask, NULL))
        {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"webconfig thread created success.. \n");
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"webconfig thread created failed.. \n");
        }
     #endif

        /* Initialized Rbus interface for TR181 Data*/
        init_rbus_dml_provider();

        // Send sd notify event after http server thread is complete.
        if (httpServerThreadDone == false)
        {
            std::unique_lock<std::mutex> lck(mtx_httpServerThreadDone);
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Waiting(max 10 sec) for http server thread to be complete...\n");
            auto sec = chrono::seconds(1);
            cv_httpServerThreadDone.wait_for(lck, 10*sec, [] {return httpServerThreadDone;});
        }
    #ifdef ENABLE_SD_NOTIFY
        sd_notifyf(0, "READY=1\n"
                   "STATUS=tr69hostif is Successfully Initialized\n"
                   "MAINPID=%lu", (unsigned long) getpid());
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"tr69hostif sd notify envent  is sent  Successfully, httpServerThreadDone=%d\n", httpServerThreadDone);
    #endif

        hostIf_DeviceInfo::send_DeviceManageableNotification();

        main_loop = g_main_loop_new (NULL, FALSE);

        if(main_loop) {
            g_main_loop_run(main_loop);
            g_main_loop_unref (main_loop);
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]Fails to Create a main loop.", __FUNCTION__);
        }

        if(hostIf_JsonIfThread)
            g_thread_join(hostIf_JsonIfThread);

    #ifndef NEW_HTTP_SERVER_DISABLE
        if(HTTPServerThread)
            g_thread_join(HTTPServerThread);
    #endif
    #if defined(PARODUS_ENABLE)
        if(parodus_init_tid)
            pthread_join(parodus_init_tid,NULL);
    #endif

        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"\n\n----------------------EXITING MAIN PROGRAM----------------------\n");
        return 0 ;
    }
  catch (const std::bad_cast& e)
    {
        std::cerr << "Bad cast exception: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception occurred" << std::endl;
        return 1;
    }
}

//------------------------------------------------------------------------------
// returns id of current thread, which called this function
//------------------------------------------------------------------------------
pid_t getTid()
{
    return (pid_t)syscall(__NR_gettid);
}

//------------------------------------------------------------------------------
// quit_handler: signal the shutdown thread to log and exit
//
// NOTE: This function is called in signal handler context, meaning that we can
//       only call a small set of functions that are deemed async-signal-safe.
//       Refer to the "overview of signals" man page (man 7 signal) to view the
//       set of functions. It's probably worth pointing out that printf() is NOT
//       part of the set of async-signal-safe functions.
//------------------------------------------------------------------------------
void quit_handler (int sig_received)
{
    /* Save the received signal and increment the shutdown semaphore to instruct
       the shutdown thread to log and exit */
    shutdown_sig_received = sig_received;
    (void)sem_post(&shutdown_thread_sem);
}

//------------------------------------------------------------------------------
// exit_gracefully: close all global file descriptors, ports, clear and exit
//------------------------------------------------------------------------------

void exit_gracefully (int sig_received)
{
    if(isShutdownTriggered == 0) {
        if(pthread_mutex_trylock(&graceful_exit_mutex) == 0) {
            RDK_LOG(RDK_LOG_NOTICE,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
            isShutdownTriggered = 1;
#ifdef T2_EVENT_ENABLED
            t2_uninit();
#endif
#if defined(USE_WIFI_PROFILE)
            /* Perform the necessary operations to shut down the WiFi device */
            WiFiDevice::shutdown();
#endif

#if defined(PARODUS_ENABLE)
            // Kill Parodus Thread
            stop_parodus_recv_wait();
#endif

            /*Stop libSoup server and exit Json Thread */
            hostIf_HttpServerStop();

            updateHandler::stop();
            XBSStore::getInstance()->stop();

            if(logfile) fclose (logfile);

            if(paramMgrhash) {
                g_hash_table_destroy(paramMgrhash);
                paramMgrhash = NULL;
            }
            hostIf_IARM_IF_Stop();

            RDK_LOG(RDK_LOG_NOTICE,LOG_TR69HOSTIF,"[%s:%s] Exiting program gracefully..\n", __FUNCTION__, __FILE__);
            if (g_main_loop_is_running(main_loop)) {
                g_main_loop_quit(main_loop);
#ifndef NEW_HTTP_SERVER_DISABLE
                /*Stop HTTP Server Thread*/
                HttpServerStop();
#endif
            }
	    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s] Unlocking Mutex..\n", __FUNCTION__, __FILE__);
            pthread_mutex_unlock(&graceful_exit_mutex);
        }
    }
}


//------------------------------------------------------------------------------
// hostIf_logger: logged the messages
//------------------------------------------------------------------------------
static void usage()
{
#ifndef NEW_HTTP_SERVER_DISABLE
    cout << "\nUsage: tr69hostIf -p[http port] [-l][LogFileName]\n\
        \nDisplay tr69hostIf options:\n\
        -p  HTTP PORT			HTTP Server port number.\n\
    	-s  HTTP PORT           New HTTP Server Port. \n\
        -l  LOG FILE NAME   		Output log file name with path.\n\
    	-c  CONFIG FILE NAME   		Configure file contains Manager list .\n\
	-d  DEBUG CONFIG FILE NAME 	Config file contain debug configurations. \n\
        -h             			Help\n\
        \nExamples:\n\
        ================================================================================\n\
    	tr69hostIf -l /opt/logs/tr69hostIf.log -c mgrlist.conf -p 5001 -d /etc/debug.ini\n\
        ================================================================================\n\
        \n" << endl;
#else
    cout << "\nUsage: tr69hostIf -p[http port] [-l][LogFileName]\n\
        \nDisplay tr69hostIf options:\n\
        -p  HTTP PORT                   HTTP Server port number.\n\
        -l  LOG FILE NAME               Output log file name with path.\n\
        -c  CONFIG FILE NAME            Configure file contains Manager list .\n\
        -d  DEBUG CONFIG FILE NAME      Config file contain debug configurations. \n\
        -h                              Help\n\
        \nExamples:\n\
        ================================================================================\n\
        tr69hostIf -l /opt/logs/tr69hostIf.log -c mgrlist.conf -p 5001 -d /etc/debug.ini\n\
        ================================================================================\n\
        \n" << endl;
#endif
}


bool filter_and_merge_xml(const char *input1, const char *input2, const char *output) {
    FILE *in_fp1 = fopen(input1, "r"); 
    FILE *in_fp2 = fopen(input2, "r"); 
    FILE *out_fp = fopen(output, "w"); 

    if (!in_fp1 || !in_fp2 || !out_fp) {
        perror("Error opening files");
        if (in_fp1) fclose(in_fp1);
        if (in_fp2) fclose(in_fp2);
        if (out_fp) fclose(out_fp);
         return false;
    }
    char line[1024];
    char last_model_line[1024] = {0};
    char last_dm_document_line[1024] = {0};
    long model_last_line_pos = -1, dm_document_last_line_pos = -1; 
    long line_pos = 0;
    while (fgets(line, sizeof(line), in_fp2)) {
        if (strstr(line, "</model>")) {
            strcpy(last_model_line, line); 
            model_last_line_pos = line_pos; 
        }
        if (strstr(line, "</dm:document>")) {
            strcpy(last_dm_document_line, line); 
            dm_document_last_line_pos = line_pos; 
        }
        line_pos++;
    }
    rewind(in_fp2);
    line_pos = 0;
    while (fgets(line, sizeof(line), in_fp2)) {
        if ((line_pos == model_last_line_pos && strstr(line, "</model>")) ||
            (line_pos == dm_document_last_line_pos && strstr(line, "</dm:document>"))) {
            line_pos++;
            continue;
        }
        fputs(line, out_fp); 
        line_pos++;
    }
    int skip_range = 0;
    while (fgets(line, sizeof(line), in_fp1)) {
        if (strstr(line, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>")) {
            skip_range = 1; 
            continue;
        }
        if (skip_range && strstr(line, "<model name=\"data-model\">")) {
            skip_range = 0; 
            continue; 
        }
        if (skip_range) {
            continue; 
        }
        fputs(line, out_fp); 
    }  

fclose(in_fp1);
fclose(in_fp2);
fclose(out_fp);

RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "Merged XML files successfully into %s\n", output);
return true;

}


MergeStatus mergeDataModel()  {
    RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Entering \n");
    FILE *fp = fopen(DEVICE_PROPS_FILE, "r");
    if (fp != NULL) {
        char line[256];
        char rdk_profile[256] = {0};
        while (fgets(line, sizeof(line), fp))
        {
            int sscanf_result = sscanf(line, "RDK_PROFILE=%s", rdk_profile);
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "mergeDataModel: sscanf result: %d, line: %s", sscanf_result, line);
            if (sscanf_result == 1) 
	    {
                break;
            }
         }
        fclose(fp);
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "mergeDataModel: Closed /etc/device.properties\n");
        const char *generic_file = GENERIC_XML_FILE;
        const char *output_file = WEBPA_DATA_MODEL_FILE;
        if (strcmp(rdk_profile, "TV") == 0) {
            const char *tv_file = TV_XML_FILE;
	    if (!filter_and_merge_xml(generic_file, tv_file, output_file)) {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Error while merging XML files for TV profile\n");
                return MERGE_FAILURE;
            }
        } 
        else if (strcmp(rdk_profile, "STB") == 0) {
            const char *stb_file = STB_XML_FILE;
	    if (!filter_and_merge_xml(generic_file, stb_file, output_file)) {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Error while merging XML files for STB profile\n");
                return MERGE_FAILURE;
            }
        } 
        else {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Unsupported RDK_PROFILE: %s\n", rdk_profile);
	    return MERGE_FAILURE;
        }
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "Merged XML written to %s\n", output_file);
	return MERGE_SUCCESS;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open /etc/device.properties\n");
	return MERGE_FAILURE;
    }
}

/** @} */
/** @} */
