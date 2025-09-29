/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2019 RDK Management
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

#ifndef XRDKCENTRALCOMBSSTORE_H
#define XRDKCENTRALCOMBSSTORE_H

#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <XrdkCentralComBSStoreJournal.h>
#include "cJSON.h"

#if defined(GTEST_ENABLE)
#include <gtest/gtest.h>
#endif

using namespace std;

#if defined(GTEST_ENABLE)
void createFile(const char* path);
bool createBspCompleteFiles();
bool createDirectory(const char* path);
#endif

class XBSStore
{
public:
    static XBSStore* getInstance();
    void stop();
    faultCode_t  getValue(HOSTIF_MsgData_t *stMsgData);
    faultCode_t  overrideValue(HOSTIF_MsgData_t *stMsgData);
    // adding a member function to call loadjson
    bool call_loadJson() { return loadFromJson(); }

private:
    static XBSStore* xbsInstance;
    static XBSStoreJournal* xbsJournalInstance;
    string m_filename;
    bool m_initDone;
    bool m_initialUpdate;
    bool m_rfcUpdateInProgress;
    static thread partnerIdThread;
    static recursive_mutex mtx;
    static bool m_stopped;
    static mutex mtx_stopped;
    static condition_variable cv;
    static mutex g_instance_mutex;
    std::unordered_map<std::string, std::string> m_dict;

    XBSStore();
    XBSStore(XBSStore const&){};

    bool init();
    string getRawValue(const string &key);
    bool setRawValue(const string &key, const string &value, HostIf_Source_Type_t sourceType);

    void initBSPropertiesFileName();
    bool loadBSPropertiesIntoCache();
    void resetCacheAndStore();
    bool getPartnerDeviceConfig(cJSON* partnerConfig, const string & partnerId);
    bool loadFromJson();
    bool clearRfcValues();
    static void getAuthServicePartnerID();

#if defined(GTEST_ENABLE)
    FRIEND_TEST(bsStoreTest, getRawValue);
    FRIEND_TEST(bsStoreTest, setRawValue);
    FRIEND_TEST(bsStoreTest, initBSPropertiesFileName);
    FRIEND_TEST(StoreClearTest, resetCacheAndStore);
    FRIEND_TEST(StoreClearTest, clearRfcValues);
    FRIEND_TEST(bsStoreTest, getPartnerDeviceConfig);
    FRIEND_TEST(bsStoreTest, getPartnerDeviceConfig_generic);
    FRIEND_TEST(bsStoreTest, getPartnerDeviceConfig_FileRemoved);
    FRIEND_TEST(bsStoreTest, getRawValue_Empty);
    FRIEND_TEST(StoreClearTest, setRawValue_Flush);
#endif
};

#endif // XRDKCENTRALCOMBSSTORE_H

