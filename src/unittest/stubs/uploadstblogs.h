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
 * Mock uploadstblogs.h for native/L1/L2 test builds.
 */

#ifndef UPLOADSTBLOGS_H
#define UPLOADSTBLOGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "uploadstblogs_types.h"

int uploadstblogs_run(const UploadSTBLogsParams* params);
int uploadstblogs_execute(int argc, char** argv);

#ifdef __cplusplus
}
#endif

#endif /* UPLOADSTBLOGS_H */
