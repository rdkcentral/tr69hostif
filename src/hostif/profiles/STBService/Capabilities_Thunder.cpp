/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2017 RDK Management
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


#include <string>
#include <vector>
#include "rdk_debug.h" 
#include "hostIf_main.h"
#include "Capabilities.h"

#define MAX_RESOLUTION_LENGTH 30

#define THUNDER_DS_GET_SUPPORTED_VIDEO_CODING_FORMATS "org.rdk.DisplaySettings.getSupportedVideoCodingFormats"
#define THUNDER_DS_GET_VIDEO_CODEC_INFO "org.rdk.DisplaySettings.getVideoCodecInfo"
#define THUNDER_DS_GET_SUPPORTED_SETTOP_RESOLUTIONS "org.rdk.DisplaySettings.getSupportedSettopResolutions"

hostIf_STBServiceCapabilities* hostIf_STBServiceCapabilities::getInstance()
{
    hostIf_STBServiceCapabilities* pRet = NULL;

    if(!pRet)
    {
        try {
            pRet = new hostIf_STBServiceCapabilities();
        } catch(const std::exception& e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Caught exception \" %s\"\n", __FUNCTION__, e.what());
        }
    }
    return pRet;
}


void hostIf_STBServiceCapabilities::closeInstance(hostIf_STBServiceCapabilities *pDev)
{
    if(pDev)
    {
        delete pDev;
    }
}

hostIf_STBServiceCapabilities::hostIf_STBServiceCapabilities()
{
}

int hostIf_STBServiceCapabilities::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    stMsgData->faultCode = fcAttemptToSetaNonWritableParameter;
    return ret;
}

int hostIf_STBServiceCapabilities::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *path = NULL, *paramName = NULL, *attr = NULL;
    int index = 0;
    try {
        int str_len = strlen(CAPABILITIES_OBJ);
        path = stMsgData->paramName;

        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s]Entering...  \n", __FILE__, __FUNCTION__);

        if(NULL == path) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d]Failed : Parameter is NULL\n", __FILE__, __FUNCTION__, __LINE__);
            stMsgData->faultCode = fcInvalidParameterName;
            return ret;
        }

        if((strncasecmp(path, CAPABILITIES_OBJ, str_len) != 0)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d]Failed : Mismatch parameter path : %s  \n", __FILE__, __FUNCTION__, __LINE__, path);
            stMsgData->faultCode = fcInvalidParameterName;
            return ret;
        }

        /* Parse video decoder object.*/
        const char *tmp_ptr = strchr(path+str_len-1,'.');
        if(tmp_ptr == NULL)  {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Parameter is NULL  \n", __FILE__, __FUNCTION__);
            stMsgData->faultCode = fcInvalidParameterName;
            return ret;
        }

        tmp_ptr++;
        paramName = tmp_ptr; //Now points to STBService.1.Capabilities.*
        if (strcasecmp(paramName, VIDEO_STANDARDS_STRING) == 0)
        {
            ret = getVideoStandards(stMsgData);
        }
        else if(strcasecmp(paramName, PROFILE_NUM_ENTRIES_STRING) == 0)
        {
            ret = getNumHEVCProfileEntries(stMsgData); 
        }
        else if(matchComponent(stMsgData->paramName, HEVC_PROFILE_OBJ, &attr, index))
        {
            //Profile-specific details. One of many profiles.
            ret = getHEVCProfileDetails(stMsgData, attr, index);
        }
        else if(strcasecmp(stMsgData->paramName, HDMI_RESOLUTIONS_STRING) == 0)
        {
            ret = getSupportedResolutions(stMsgData);
        }
        else if(strstr(stMsgData->paramName, "AudioStandards") != NULL)
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s()] %s: not supported on RDK-E\n", __FUNCTION__, stMsgData->paramName);
            ret = NOT_HANDLED;
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Parameter \'%s\' is Not Supported  \n", __FILE__, __FUNCTION__, paramName);
            stMsgData->faultCode = fcInvalidParameterName;
            ret = NOK;
        }
    }
    catch (const std::exception& e )
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Exception caught %s   \n", __FILE__, __FUNCTION__, e.what());
        stMsgData->faultCode = fcInternalError;
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s]Exiting... \n", __FILE__, __FUNCTION__);
    return ret;
}

int hostIf_STBServiceCapabilities::getVideoStandards(HOSTIF_MsgData_t *stMsgData)
{
    int bytes_written = 0;
    try {
        // Thunder response: { "supportedFormats": ["HEVC", "H264", "MPEG2"], "success": true }
        // The field is a string array — use raw response + strstr per codec.
        std::string response;
        if (!invokeThunderPluginMethod(
                THUNDER_DS_GET_SUPPORTED_VIDEO_CODING_FORMATS,
                "{}",
                response))
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Failed to fetch supported video coding formats from Thunder\n",__FUNCTION__);
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }

        const char* resp = response.c_str();
        // Codec strings are set by the plugin as: HEVC, H264, MPEG2
        //   dsVIDEO_CODEC_MPEGHPART2  → "HEVC"
        //   dsVIDEO_CODEC_MPEG4PART10 → "H264"
        //   dsVIDEO_CODEC_MPEG2       → "MPEG2"
        if (strstr(resp, "\"HEVC\""))
        {
            bytes_written += snprintf(&(stMsgData->paramValue[bytes_written]), (TR69HOSTIFMGR_MAX_PARAM_LEN - bytes_written), "%s,", "MPEGH-Part2 ([ISO/IEC23008-1]])");
        }
        if (strstr(resp, "\"MPEG2\""))
        {
            bytes_written += snprintf(&(stMsgData->paramValue[bytes_written]), (TR69HOSTIFMGR_MAX_PARAM_LEN - bytes_written), "%s,", "MPEG2-Part2 ([ISO/IEC13818-1])");
        }
        if (strstr(resp, "\"H264\""))
        {
            bytes_written += snprintf(&(stMsgData->paramValue[bytes_written]), (TR69HOSTIFMGR_MAX_PARAM_LEN - bytes_written), "%s,", "MPEG4-Part10 ([ISO/IEC14496-10])");
        }

        if (bytes_written <= 0)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Thunder returned no supported video standards\n",__FUNCTION__);
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }

        stMsgData->paramValue[bytes_written -1] = '\0'; //substitute string terminator for the final comma.
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] : Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
    }
    catch (const std::exception &e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\n",__FUNCTION__);
        stMsgData->faultCode = fcInternalError;
        return NOK;
    }

    return OK;
}

struct ThunderHEVCEntry {
    std::string profile;   // directly from Thunder: "MAIN", "MAIN 10", "MAIN STILL PICTURE"
    float       level;     // raw libds float, e.g. 5.1 for Level 5.1
};

static bool getThunderVideoCodecInfo(unsigned int& numEntries, std::vector<ThunderHEVCEntry>& entries)
{
    numEntries = 0;
    entries.clear();

    std::string response;
    // Query specifically for MPEGHPart2 — matching the original libds getVideoCodecInfo(dsVIDEO_CODEC_MPEGHPART2).
    if (!invokeThunderPluginMethod(THUNDER_DS_GET_VIDEO_CODEC_INFO, "{\"codec\":\"MPEGH-Part2\"}", response))
    {
        return false;
    }

    cJSON* root = cJSON_Parse(response.c_str());
    if (root == NULL)
    {
        return false;
    }

    cJSON* resultObj = cJSON_GetObjectItem(root, "result");
    if (!cJSON_IsObject(resultObj))
    {
        cJSON_Delete(root);
        return false;
    }

    cJSON* numEntriesObj = cJSON_GetObjectItem(resultObj, "numberOfEntries");
    if (!cJSON_IsNumber(numEntriesObj))
    {
        cJSON_Delete(root);
        return false;
    }

    cJSON* entriesObj = cJSON_GetObjectItem(resultObj, "entries");
    if (!cJSON_IsArray(entriesObj))
    {
        cJSON_Delete(root);
        return false;
    }

    numEntries = numEntriesObj->valueint;

    bool ok = (numEntries > 0);
    for (unsigned int i = 0; ok && (i < numEntries); ++i)
    {
        cJSON* entryObj = cJSON_GetArrayItem(entriesObj, i);
        cJSON* profileObj = cJSON_GetObjectItem(entryObj, "profile");
        cJSON* levelObj = cJSON_GetObjectItem(entryObj, "level");

        if (!cJSON_IsString(profileObj) || (profileObj->valuestring == NULL) || !cJSON_IsNumber(levelObj))
        {
            ok = false;
            break;
        }

        ThunderHEVCEntry entry;
        entry.profile = profileObj->valuestring;
        entry.level = (float)levelObj->valuedouble;
        entries.push_back(entry);
    }

    cJSON_Delete(root);
    return ok;
}

int hostIf_STBServiceCapabilities::getNumHEVCProfileEntries(HOSTIF_MsgData_t *stMsgData)
{
    try {
        unsigned int numEntries = 0;
        std::vector<ThunderHEVCEntry> entries;
        if (!getThunderVideoCodecInfo(numEntries, entries))
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Failed to fetch HEVC codec info from Thunder\n",__FUNCTION__);
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }

        if(0 == numEntries)
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Zero profile entries reported.\n",__FUNCTION__);
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }
        put_int(stMsgData->paramValue, numEntries);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] : Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        stMsgData->paramLen = sizeof(unsigned int); 
    }
    catch (const std::exception &e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\n",__FUNCTION__);
        stMsgData->faultCode = fcInternalError;
        return NOK;
    }

    return OK;
}

static const char* getTR181ResolutionString(const std::string& resolution)
{
    if (resolution == "720p")                                    return "1280x720p/59.94Hz";
    if (resolution == "1080i")                                   return "1920x1080i/59.94Hz";
    if (resolution == "1080p60" || resolution == "1080p")        return "1920x1080p/59.94Hz";
    if (resolution == "2160p30")                                 return "3840x2160p/30Hz";
    if (resolution == "2160p60")                                 return "3840x2160p/59.94Hz";
    if (resolution == "480i")                                    return "720x480i/59.94Hz";
    if (resolution == "480p")                                    return "720x480p/59.94Hz";
    if (resolution == "576p50" || resolution == "576p")          return "720x576p/50Hz";
    if (resolution == "720p50")                                  return "1280x720p/50Hz";
    if (resolution == "1080p30")                                 return "1920x1080p/30Hz";
    if (resolution == "1080i50" || resolution == "1080i25")      return "1920x1080i/50Hz";
    if (resolution == "1080p24")                                 return "1920x1080p/24Hz";
    if (resolution == "1080p50")                                 return "1920x1080p/50Hz";
    if (resolution == "2160p50")                                 return "3840x2160p/50Hz";
    if (resolution == "1080p25")                                 return "1920x1080p/25Hz";
    RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Unhandled resolution: %s. Cannot translate!\n", __FUNCTION__, resolution.c_str());
    return "";
}

static unsigned int getMaxHEVCDecodeKBitRate(const ThunderHEVCEntry& entry)
{
    unsigned int kbit_rate = 0;
    if ((entry.profile == "MAIN 10") && (5.1f == entry.level))
    {
        kbit_rate = 40000;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Unknown profile (%s) and level (%g) combination!\n",__FUNCTION__, entry.profile.c_str(), entry.level);
    }
    return kbit_rate;
}

int hostIf_STBServiceCapabilities::getHEVCProfileDetails(HOSTIF_MsgData_t * stMsgData, const char* attr, unsigned int index)
{
    int bytes_written = 0;
    try {
        unsigned int numEntries = 0;
        std::vector<ThunderHEVCEntry> entries;
        if (!getThunderVideoCodecInfo(numEntries, entries))
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Failed to fetch HEVC codec info from Thunder\n",__FUNCTION__);
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }

        if((0 == numEntries) || (index > numEntries))
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Could not find profiles matching index %d.\n",__FUNCTION__, index);
            stMsgData->faultCode = ((0 == numEntries) ? fcInternalError : fcInvalidParameterName);
            return NOK;
        }

        const ThunderHEVCEntry& entry = entries[index - 1];

        if(strcasecmp(attr, PROFILE_NAME_STRING) == 0)
        {
            bytes_written = snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN, "%s", entry.profile.c_str());
            stMsgData->paramValue[bytes_written] = '\0';
            stMsgData->paramtype = hostIf_StringType;
            stMsgData->paramLen = bytes_written;
        }
        else if(strcasecmp(attr, PROFILE_LEVEL_STRING) == 0)
        {
            bytes_written = snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN, "L%g", entry.level);
            stMsgData->paramValue[bytes_written] = '\0';
            stMsgData->paramtype = hostIf_StringType;
            stMsgData->paramLen = bytes_written;
        }
        else if(strcasecmp(attr, PROFILE_MAX_DECODE_CAPABILITY_STRING) == 0)
        {
            put_int(stMsgData->paramValue, getMaxHEVCDecodeKBitRate(entry));
            stMsgData->paramtype = hostIf_UnsignedIntType;
            stMsgData->paramLen = sizeof(unsigned int);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Parameter \'%s\' is Not Supported  \n", __FILE__, __FUNCTION__, attr);
            stMsgData->faultCode = fcInvalidParameterName;
        }

    }
    catch (...) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\n",__FUNCTION__);
        stMsgData->faultCode = fcInternalError;
        return NOK;
    }

    return OK;

}

int hostIf_STBServiceCapabilities::getSupportedResolutions(HOSTIF_MsgData_t *stMsgData)
{
    try
    {
        // Thunder returns: { "supportedSettopResolutions": ["720p", "1080p", ...], "success": true }
        // Each entry is a short-form name — must be translated to TR-181 "WxHp/FHz" format,
        // matching the behaviour of the original libds getSettopSupportedResolutions().
        std::string response;
        if (!invokeThunderPluginMethod(
                THUNDER_DS_GET_SUPPORTED_SETTOP_RESOLUTIONS,
                "{}",
                response))
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Failed to fetch supported settop resolutions from Thunder\n",__FUNCTION__);
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }

        cJSON* root = cJSON_Parse(response.c_str());
        if (root == NULL)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Failed to parse Thunder response\n",__FUNCTION__);
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }

        // Response may be the full JSON-RPC envelope ({ result: { ... } }) or just the result object.
        cJSON* resultObj = cJSON_GetObjectItem(root, "result");
        cJSON* arrayObj = cJSON_IsObject(resultObj)
            ? cJSON_GetObjectItem(resultObj, "supportedSettopResolutions")
            : cJSON_GetObjectItem(root, "supportedSettopResolutions");

        if (!cJSON_IsArray(arrayObj))
        {
            cJSON_Delete(root);
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] supportedSettopResolutions not found or not an array\n",__FUNCTION__);
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }

        memset(stMsgData->paramValue, 0, TR69HOSTIFMGR_MAX_PARAM_LEN);
        bool first = true;
        const int count = cJSON_GetArraySize(arrayObj);
        for (int i = 0; i < count; ++i)
        {
            cJSON* item = cJSON_GetArrayItem(arrayObj, i);
            if (!cJSON_IsString(item) || (item->valuestring == NULL))
                continue;

            const char* tr181 = getTR181ResolutionString(std::string(item->valuestring));
            if (tr181[0] == '\0')
                continue;   // unknown short name — skip

            if (!first)
                strncat(stMsgData->paramValue, ",", TR69HOSTIFMGR_MAX_PARAM_LEN - strlen(stMsgData->paramValue) - 1);
            strncat(stMsgData->paramValue, tr181, TR69HOSTIFMGR_MAX_PARAM_LEN - strlen(stMsgData->paramValue) - 1);
            first = false;
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] : resolution: %s -> %s\n",__FUNCTION__, item->valuestring, tr181);
        }

        cJSON_Delete(root);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s]  : Value: %s \n",__FUNCTION__, stMsgData->paramValue);
    }
    catch (...) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\n",__FUNCTION__);
        stMsgData->faultCode = fcInternalError;
        return NOK;
    }
    return OK;

}
