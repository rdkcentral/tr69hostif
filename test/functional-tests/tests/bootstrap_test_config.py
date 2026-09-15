####################################################################################
# Bootstrap Initialization Test Configuration
# This file contains constants and configuration for the bootstrap initialization tests
####################################################################################

# Test timing constants
BOOTSTRAP_STABILIZATION_TIME = 15  # seconds to wait for bootstrap completion
PROCESS_STABILITY_CHECK_TIME = 5   # seconds to wait for stability verification
FUNCTIONAL_TEST_TIMEOUT = 5        # seconds timeout for functional tests

# Expected log messages for bootstrap phases
BOOTSTRAP_LOG_MESSAGES = {
    'startup': [
        "Starting tr69HostIf Service",
        "tr69hostif Service"
    ],
    'config_init': [
        "hostIf_initalize_ConfigManger",
        "Configuration Manager"
    ],
    'iarm_init': [
        "TR69_HostIf_Mgr_Init",
        "TR69_HostIf_Mgr_Connect",
        "hostIf_IARM_IF_Start"
    ],
    'data_model': [
        "Successfully merged Data Model",
        "Successfully initialize Data Model"
    ],
    'threads': [
        "json_if_handler_thread",
        "http_server_thread",
        "SERVER: Started server successfully"
    ],
    'rbus': [
        "[rbusdml]Successfully get the complete parameter list",
        "rbus_regDataElements registered successfully"
    ],
    'completion': [
        "tr69hostif is Successfully Initialized",
        "tr69hostif sd notify envent  is sent  Successfully"
    ]
}

# Error messages that should NOT appear during successful bootstrap
BOOTSTRAP_ERROR_MESSAGES = [
    "Failed to hostIf_initalize_ConfigManger()",
    "Error in Data Model Initialization",
    "Error in merging Data Model", 
    "Failed to start hostIf_IARM_IF_Start()",
    "pthread_create() failed",
    "Thread create failed",
    "sem_init() failed",
    "[rbusdml] Failed to initialized",
    "consumer: rbus_open failed",
    "Fails to Create a main loop",
    "New HTTP Server Thread Create failed",
    "Parodus init thread create failed"
]

# Configuration files that should exist
EXPECTED_CONFIG_FILES = [
    "/etc/tr69hostif.conf",
    "/etc/mgrlist.conf",
    "/etc/rfc.properties"
]

# RFC-related files
RFC_FILES = [
    "/etc/rfc.properties",
    "/opt/secure/RFC/rfcVariable.ini", 
    "/opt/secure/RFC/tr181store.ini",
    "/opt/RFC/.RFC_LegacyRFCEnabled.ini"
]

# Data model files
DATA_MODEL_FILES = [
    "/tmp/data-model.xml",
    "/etc/data-model-generic.xml",
    "/etc/data-model-stb.xml",
    "/etc/data-model-tv.xml"
]

# Partner configuration files
PARTNER_FILES = [
    "/etc/partners_defaults.json",
    "/etc/partners_defaults_device.json"
]

# Essential manager mappings that should be in config
ESSENTIAL_MANAGERS = [
    "Device.DeviceInfo",
    "Device.Ethernet",
    "Device.IP",
    "deviceMgr", 
    "ethernetMgr",
    "ipMgr"
]

# Test thresholds
MIN_BOOTSTRAP_HEALTH_SCORE = 70  # Minimum percentage for passing
MIN_SUCCESS_INDICATORS = 3       # Minimum success messages required
MAX_ALLOWED_ERRORS = 2          # Maximum error messages tolerated

# Process stability requirements
MIN_PROCESS_UPTIME = 10         # Minimum seconds process should run
STABILITY_CHECK_INTERVAL = 2    # Seconds between stability checks

# HTTP endpoints for functional testing
HTTP_TEST_ENDPOINTS = [
    "http://127.0.0.1:11999/api/v1/device/config",
    "http://127.0.0.1:11999"
]

# RBUS test parameters
RBUS_TEST_PARAMS = [
    "Device.DeviceInfo.Manufacturer",
    "Device.DeviceInfo.ModelName",
    "Device.DeviceInfo.SoftwareVersion"
]

# Bootstrap phases for detailed testing
BOOTSTRAP_PHASES = [
    {
        'name': 'Command Line Processing',
        'description': 'Parse arguments and set defaults',
        'error_indicators': ['non-option ARGV-elements'],
        'timeout': 2
    },
    {
        'name': 'Signal Handler Setup', 
        'description': 'Initialize signal handlers and threads',
        'error_indicators': ['sem_init() failed', 'pthread_create() failed'],
        'timeout': 3
    },
    {
        'name': 'Configuration Manager',
        'description': 'Load parameter manager mappings',
        'success_indicators': ['hostIf_initalize_ConfigManger'],
        'error_indicators': ['Failed to hostIf_initalize_ConfigManger()'],
        'timeout': 5
    },
    {
        'name': 'IARM Initialization',
        'description': 'Initialize IARM bus communication',
        'success_indicators': ['TR69_HostIf_Mgr_Init', 'TR69_HostIf_Mgr_Connect'],
        'error_indicators': ['Failed to start hostIf_IARM_IF_Start()'],
        'timeout': 8
    },
    {
        'name': 'Data Model Processing',
        'description': 'Merge and load TR-069 data model',
        'success_indicators': ['Successfully merged Data Model', 'Successfully initialize Data Model'],
        'error_indicators': ['Error in merging Data Model', 'Error in Data Model Initialization'],
        'timeout': 10
    },
    {
        'name': 'Server Threads',
        'description': 'Start JSON and HTTP server threads',
        'success_indicators': ['json_if_handler_thread', 'SERVER: Started server successfully'],
        'error_indicators': ['Thread create failed', 'New HTTP Server Thread Create failed'],
        'timeout': 8
    },
    {
        'name': 'RBUS Interface',
        'description': 'Initialize RBUS data model provider',
        'success_indicators': ['rbus_regDataElements registered successfully'],
        'error_indicators': ['[rbusdml] Failed to initialized', 'consumer: rbus_open failed'],
        'timeout': 10
    },
    {
        'name': 'Service Ready',
        'description': 'Send ready notification and enter main loop',
        'success_indicators': ['tr69hostif is Successfully Initialized'],
        'error_indicators': ['Fails to Create a main loop'],
        'timeout': 5
    }
]