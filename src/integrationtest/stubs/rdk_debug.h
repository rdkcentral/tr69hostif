/*
 * Copyright 2024 Comcast Cable Communications Management, LLC
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
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RDK_DEBUG_H_
#define RDK_DEBUG_H_


#include <stdio.h>
#include "hostIf_main.h"
#include "rdk_logger_types.h"
#include "rdk_error.h"


#define RDK_LOG_TRACE1 1
#define RDK_LOG_TRACE2 RDK_LOG_TRACE1
#define RDK_LOG_DEBUG 2
#define RDK_LOG_INFO 3
#define RDK_LOG_WARN 4
#define RDK_LOG_ERROR 5

#define rdk_logger_init(DEBUG_INI_NAME) 0

/**
 * @enum rdk_LogLevel
 * @brief These values represent the logging 'levels' or 'types', they are each
 * independent.
 */
typedef enum
{
    ENUM_RDK_LOG_BEGIN = 0, /**< Used as array index. */

    RDK_LOG_FATAL = ENUM_RDK_LOG_BEGIN,
    RDK_LOG_NOTICE,
    RDK_LOG_TRACE,
    ENUM_RDK_LOG_COUNT
} rdk_LogLevel;

// The macro to convert RDK_LOG to printf
#define RDK_LOG(level, module, ...) \
    do { \
        if (( level == RDK_LOG_DEBUG )) { \
            printf("DEBUG: %s: ", module); \
        } \
        else if (( level == RDK_LOG_INFO )) { \
            printf("INFO: %s: ", module); \
        } \
        else if (( level == RDK_LOG_ERROR )) { \
            printf("ERROR: %s: ", module); \
        } \
        printf(__VA_ARGS__); \
} while (0)

/**
 * @ingroup RDKLOGGER_DEBUG_API
 * @{
 */

//rdk_Error rdk_logger_init(const char* debugConfigFile);

#endif

