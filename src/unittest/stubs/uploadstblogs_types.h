
/*
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:
*
* Copyright 2026 RDK Management
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

/*
 * Mock uploadstblogs_types.h for native/L1/L2 test builds.
 */

#ifndef UPLOADSTBLOGS_TYPES_H
#define UPLOADSTBLOGS_TYPES_H

#include <stdbool.h>

typedef enum {
    TRIGGER_SCHEDULED = 0,
    TRIGGER_MANUAL = 1,
    TRIGGER_REBOOT = 2,
    TRIGGER_CRASH = 3,
    TRIGGER_DEBUG = 4,
    TRIGGER_ONDEMAND = 5,
    TRIGGER_MEMCAPTURE = 6
} TriggerType;

typedef struct {
    int flag;
    int dcm_flag;
    bool upload_on_reboot;
    const char* upload_protocol;
    const char* upload_http_link;
    TriggerType trigger_type;
    bool rrd_flag;
    const char* rrd_file;
    bool uploadlogsnow_mode;
} UploadSTBLogsParams;

#endif /* UPLOADSTBLOGS_TYPES_H */
