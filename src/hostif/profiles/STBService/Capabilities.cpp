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
#include "rdk_debug.h" 
#include "hostIf_main.h"
#include "Capabilities.h"

#include "dsTypes.h"

#define MAX_RESOLUTION_LENGTH 30

#define THUNDER_DS_GET_SUPPORTED_VIDEO_CODING_FORMATS "org.rdk.DisplaySettings.getSupportedVideoCodingFormats"
#define THUNDER_DS_GET_VIDEO_CODEC_INFO "org.rdk.DisplaySettings.getVideoCodecInfo"
#define THUNDER_DS_GET_SUPPORTED_SETTOP_RESOLUTIONS "org.rdk.DisplaySettings.getSupportedSettopResolutions"

static bool parseSupportedVideoFormatsFromThunder(const std::string& response, int& supportedStandards)
{
    supportedStandards = 0;
    cJSON* root = cJSON_Parse(response.c_str());
    if (root == NULL)
    {
        return false;
    }

    cJSON* resultObj = cJSON_GetObjectItem(root, "result");
    cJSON* formatsObj = cJSON_GetObjectItem(resultObj, "supportedFormats");
    if (!cJSON_IsArray(formatsObj))
    {
        cJSON_Delete(root);
        return false;
    }

    const int formatCount = cJSON_GetArraySize(formatsObj);
    for (int i = 0; i < formatCount; ++i)
    {
        cJSON* formatObj = cJSON_GetArrayItem(formatsObj, i);
        if (!cJSON_IsString(formatObj) || (formatObj->valuestring == NULL))
        {
            continue;
        }

        const std::string format(formatObj->valuestring);
        if ((format == "MPEG2") || (format == "MPEG2-Part2") || (format == "MPEG2PART2"))
        {
            supportedStandards |= dsVIDEO_CODEC_MPEG2;
        }
        else if ((format == "H264") || (format == "MPEG4-Part10") || (format == "MPEG4PART10"))
        {
            supportedStandards |= dsVIDEO_CODEC_MPEG4PART10;
        }
        else if ((format == "MPEGHPart2") || (format == "HEVC") || (format == "H265"))
        {
            supportedStandards |= dsVIDEO_CODEC_MPEGHPART2;
        }
    }

    cJSON_Delete(root);
    return (supportedStandards != 0);
}

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
        int supported_standards = 0;
        if (!invokeThunderPluginMethodAndExtractNumberField(
                THUNDER_DS_GET_SUPPORTED_VIDEO_CODING_FORMATS,
                "{}",
                "supportedVideoCodingFormats",
                supported_standards))
        {
            std::string response;
            if (!invokeThunderPluginMethod(THUNDER_DS_GET_SUPPORTED_VIDEO_CODING_FORMATS, "{}", response) ||
                !parseSupportedVideoFormatsFromThunder(response, supported_standards))
            {
                RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Failed to fetch supported video coding formats from Thunder\n",__FUNCTION__);
                stMsgData->faultCode = fcInternalError;
                return NOK;
            }
        }

        if(0 != (supported_standards & dsVIDEO_CODEC_MPEGHPART2))
        {
            bytes_written += snprintf(&(stMsgData->paramValue[bytes_written]), (TR69HOSTIFMGR_MAX_PARAM_LEN - bytes_written), "%s,", "MPEGH-Part2 ([ISO/IEC23008-1]])");
        }
        if(0 != (supported_standards & dsVIDEO_CODEC_MPEG2))
        {
            bytes_written += snprintf(&(stMsgData->paramValue[bytes_written]), (TR69HOSTIFMGR_MAX_PARAM_LEN - bytes_written), "%s,", "MPEG2-Part2 ([ISO/IEC13818-1])");
        }
        if(0 != (supported_standards & dsVIDEO_CODEC_MPEG4PART10))
        {
            bytes_written += snprintf(&(stMsgData->paramValue[bytes_written]), (TR69HOSTIFMGR_MAX_PARAM_LEN - bytes_written), "%s,", "MPEG4-Part10 ([ISO/IEC14496-10])");
        }

        if (bytes_written <= 0)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Thunder returned no supported video standards\n",__FUNCTION__);
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }

        stMsgData->paramValue[bytes_written -1] = '\0'; //substitue string terminator for the final comma.
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

static bool mapHEVCProfileString(const std::string& profileName, dsVideoCodecHevcProfiles_t& profile)
{
    if ((profileName == "MAIN") || (profileName == "Main"))
    {
        profile = dsVIDEO_CODEC_HEVC_PROFILE_MAIN;
        return true;
    }
    if ((profileName == "MAIN 10") || (profileName == "Main 10") || (profileName == "MAIN10") || (profileName == "Main10"))
    {
        profile = dsVIDEO_CODEC_HEVC_PROFILE_MAIN10;
        return true;
    }
    if ((profileName == "MAIN STILL PICTURE") || (profileName == "Main Still Picture") ||
        (profileName == "MAINSTILLPICTURE") || (profileName == "MainStillPicture"))
    {
        profile = dsVIDEO_CODEC_HEVC_PROFILE_MAINSTILLPICTURE;
        return true;
    }

    return false;
}

static bool getThunderVideoCodecInfo(dsVideoCodecInfo_t& info)
{
    memset(&info, 0, sizeof(info));

    std::string response;
    const char* requestShapes[] = {
        "{\"videoCodingFormat\":\"MPEGHPart2\"}",
        "{\"videoCodingFormat\":\"HEVC\"}",
        "{}"
    };

    for (size_t attempt = 0; attempt < (sizeof(requestShapes) / sizeof(requestShapes[0])); ++attempt)
    {
        if (!invokeThunderPluginMethod(THUNDER_DS_GET_VIDEO_CODEC_INFO, requestShapes[attempt], response))
        {
            continue;
        }

        cJSON* root = cJSON_Parse(response.c_str());
        if (root == NULL)
        {
            continue;
        }

        cJSON* resultObj = cJSON_GetObjectItem(root, "result");
        if (!cJSON_IsObject(resultObj))
        {
            cJSON_Delete(root);
            continue;
        }

        cJSON* entriesObj = cJSON_GetObjectItem(resultObj, "entries");
        if (!cJSON_IsArray(entriesObj))
        {
            cJSON_Delete(root);
            continue;
        }

        const int entryCount = cJSON_GetArraySize(entriesObj);
        info.num_entries = (entryCount > 10) ? 10 : entryCount;

        bool ok = (info.num_entries > 0);
        for (unsigned int i = 0; ok && (i < info.num_entries); ++i)
        {
            cJSON* entryObj = cJSON_GetArrayItem(entriesObj, i);
            cJSON* profileObj = cJSON_GetObjectItem(entryObj, "profile");
            cJSON* levelObj = cJSON_GetObjectItem(entryObj, "level");
            if (!cJSON_IsNumber(levelObj))
            {
                ok = false;
                break;
            }

            dsVideoCodecHevcProfiles_t profile = dsVIDEO_CODEC_HEVC_PROFILE_MAIN;
            if (cJSON_IsNumber(profileObj))
            {
                profile = (dsVideoCodecHevcProfiles_t)profileObj->valueint;
            }
            else if (cJSON_IsString(profileObj) && (profileObj->valuestring != NULL))
            {
                ok = mapHEVCProfileString(profileObj->valuestring, profile);
            }
            else
            {
                ok = false;
            }

            if (!ok)
            {
                break;
            }

            info.entries[i].profile = profile;
            info.entries[i].level = (float)levelObj->valuedouble;
        }

        cJSON_Delete(root);
        if (ok)
        {
            return true;
        }
    }

    return false;
}

int hostIf_STBServiceCapabilities::getNumHEVCProfileEntries(HOSTIF_MsgData_t *stMsgData)
{
    try {
        dsVideoCodecInfo_t info = {};
        if (!getThunderVideoCodecInfo(info))
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Failed to fetch HEVC codec info from Thunder\n",__FUNCTION__);
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }

        if(0 == info.num_entries)
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Zero profile entries reported.\n",__FUNCTION__);
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }
        put_int(stMsgData->paramValue, info.num_entries);
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

static const char * convertHEVCProfileNameToString(dsVideoCodecHevcProfiles_t profile)
{
    switch(profile)
    {
        case dsVIDEO_CODEC_HEVC_PROFILE_MAIN:
            return "MAIN";
        case dsVIDEO_CODEC_HEVC_PROFILE_MAIN10:
            return "MAIN 10";
        case dsVIDEO_CODEC_HEVC_PROFILE_MAINSTILLPICTURE:
            return "MAIN STILL PICTURE";
        default:
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Unknown profile 0x%x!\n",__FUNCTION__, (unsigned int)profile);
            return "";
    }
}

static unsigned int getMaxHEVCDecodeKBitRate(dsVideoCodecProfileSupport_t &entry)
{
    unsigned int kbit_rate = 0; 
    if((dsVIDEO_CODEC_HEVC_PROFILE_MAIN10 == entry.profile) && (5.1f == entry.level))
    {
        kbit_rate = 40000;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Unknown profile (0x%x) and level (%g) combination!\n",__FUNCTION__, entry.profile, entry.level);
    }
    return kbit_rate;
}

int hostIf_STBServiceCapabilities::getHEVCProfileDetails(HOSTIF_MsgData_t * stMsgData, const char* attr, unsigned int index)
{
    int bytes_written = 0;
    try {
        dsVideoCodecInfo_t info = {};
        if (!getThunderVideoCodecInfo(info))
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Failed to fetch HEVC codec info from Thunder\n",__FUNCTION__);
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }

        if((0 == info.num_entries) || (index > info.num_entries))
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Could not find profiles matching index %d.\n",__FUNCTION__, index);
            stMsgData->faultCode = ((0 == info.num_entries) ? fcInternalError : fcInvalidParameterName );
            return NOK;
        }

        if(strcasecmp(attr, PROFILE_NAME_STRING) == 0)
        {
            bytes_written = snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN, "%s", convertHEVCProfileNameToString(info.entries[index - 1].profile));
            stMsgData->paramValue[bytes_written] = '\0';
            stMsgData->paramtype = hostIf_StringType;
            stMsgData->paramLen = bytes_written;
        }
        else if(strcasecmp(attr, PROFILE_LEVEL_STRING) == 0)
        {
            bytes_written = snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN, "L%g", info.entries[index - 1].level);
            stMsgData->paramValue[bytes_written] = '\0';
            stMsgData->paramtype = hostIf_StringType;
            stMsgData->paramLen = bytes_written;
        }
        else if(strcasecmp(attr, PROFILE_MAX_DECODE_CAPABILITY_STRING) == 0)
        {
            put_int(stMsgData->paramValue,getMaxHEVCDecodeKBitRate(info.entries[index -1])); 
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
        std::string supportedResolutions;
        if (!invokeThunderPluginMethodAndExtractStringField(
                THUNDER_DS_GET_SUPPORTED_SETTOP_RESOLUTIONS,
                "{}",
                "supportedSettopResolutions",
                supportedResolutions))
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Failed to fetch supported settop resolutions from Thunder\n",__FUNCTION__);
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }

        snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN, "%s", supportedResolutions.c_str());
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
