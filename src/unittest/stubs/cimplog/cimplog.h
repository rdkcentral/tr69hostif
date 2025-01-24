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

#ifndef __CIMPLOG_H__
#define __CIMPLOG_H__

#include <stdarg.h>
#include <stdio.h>

#define LEVEL_ERROR    0
#define LEVEL_INFO     1
#define LEVEL_DEBUG    2

#define cimplog_error(module, ...)    printf(__VA_ARGS__)
#define cimplog_info(module, ...)     printf(__VA_ARGS__)
#define cimplog_debug(module, ...)    printf(__VA_ARGS__)



#define WdmpError(...)                      printf(__VA_ARGS__)
#define WdmpInfo(...)                       printf(__VA_ARGS__)
#define WdmpPrint(...)                      printf(__VA_ARGS__)

#endif
