/*
 * dsVideoPortSettings.h — Native-build stub for tr69hostif L2/coverage builds.
 *
 * Same guard-collision pattern as dsAudioSettings.h / dsVideoResolutionSettings.h:
 * if any transitively-included header sets _DS_VIDEOOUTPUTPORTSETTINGS_H_ before
 * the rdkvhal-devicesettings-raspberrypi4 version is reached, kConfigs and kPorts
 * become undeclared in dsVideoPortConfig.c.  cov_build.sh copies this file over
 * the cloned rdkvhal copy to guarantee the tables are always defined.
 *
 * Content mirrors rdkvhal-devicesettings-raspberrypi4/dsVideoPortSettings.h (main branch).
 */
#ifndef _DS_VIDEOOUTPUTPORTSETTINGS_H_
#define _DS_VIDEOOUTPUTPORTSETTINGS_H_

#include "dsAVDTypes.h"
#include "dsUtl.h"
#include "dsVideoResolutionSettings.h"

static const dsVideoPortType_t kSupportedPortTypes[] = {
    dsVIDEOPORT_TYPE_HDMI
};

static const dsVideoPortTypeConfig_t kConfigs[] = {
    {
        /* .typeId =                  */ dsVIDEOPORT_TYPE_HDMI,
        /* .name =                    */ "HDMI",
        /* .dtcpSupported =           */ false,
        /* .hdcpSupported =           */ false,
        /* .restrictedResolution =    */ -1,
        /* .numSupportedResolutions = */ dsUTL_DIM(kResolutions),
        /* .supportedResolutions =    */ kResolutions,
    },
};

static const dsVideoPortPortConfig_t kPorts[] = {
    {
        /* .id =                */ { dsVIDEOPORT_TYPE_HDMI, 0 },
        /* .connectedAOP =      */ { dsAUDIOPORT_TYPE_HDMI, 0 },
        /* .defaultResolution = */ "720p60",
    },
};

/* Index into kResolutions[] for the default resolution ("720p60" == index 5). */
static const int kDefaultResIndex = 5;

#endif /* _DS_VIDEOOUTPUTPORTSETTINGS_H_ */
