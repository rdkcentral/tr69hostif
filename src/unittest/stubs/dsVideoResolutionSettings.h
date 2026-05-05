/*
 * dsVideoResolutionSettings.h — Native-build stub for tr69hostif L2/coverage builds.
 *
 * In mainline rdk-halif-device_settings the kResolutions table was moved to
 * platform-specific HAL implementation files and is no longer exported through
 * this header.  This stub provides the array so that Components_HDMI.cpp compiles
 * when building natively for test/coverage purposes without real hardware.
 *
 * The entry layout matches dsVideoPortResolution_t as defined in dsAVDTypes.h:
 *   char name[32], dsVideoResolution_t pixelResolution, dsVideoAspectRatio_t aspectRatio,
 *   dsVideoStereoScopicMode_t stereoScopicMode, dsVideoFrameRate_t frameRate, bool interlaced
 */
#ifndef _DS_VIDEO_RESOLUTION_SETTINGS_H_
#define _DS_VIDEO_RESOLUTION_SETTINGS_H_

#include "dsAVDTypes.h"

/* Scan-mode convenience macros used by both kResolutions[] entries and
 * devicesettings ds/ C++ sources (e.g. videoOutputPortConfig.cpp). */
#ifndef _PROGRESSIVE
#define _PROGRESSIVE false
#endif
#ifndef _INTERLACED
#define _INTERLACED  true
#endif

static dsVideoPortResolution_t kResolutions[] = {
    /* name       pixelResolution                aspectRatio                  stereoScopicMode    frameRate                interlaced */
    {"480i60",  dsVIDEO_PIXELRES_720x480,  dsVIDEO_ASPECT_RATIO_4x3,  dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_60,    true  },
    {"480p60",  dsVIDEO_PIXELRES_720x480,  dsVIDEO_ASPECT_RATIO_4x3,  dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_60,    false },
    {"576i50",  dsVIDEO_PIXELRES_720x576,  dsVIDEO_ASPECT_RATIO_4x3,  dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_50,    true  },
    {"576p50",  dsVIDEO_PIXELRES_720x576,  dsVIDEO_ASPECT_RATIO_4x3,  dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_50,    false },
    {"720p50",  dsVIDEO_PIXELRES_1280x720, dsVIDEO_ASPECT_RATIO_16x9, dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_50,    false },
    {"720p60",  dsVIDEO_PIXELRES_1280x720, dsVIDEO_ASPECT_RATIO_16x9, dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_60,    false },
    {"1080i50", dsVIDEO_PIXELRES_1920x1080,dsVIDEO_ASPECT_RATIO_16x9, dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_50,    true  },
    {"1080i60", dsVIDEO_PIXELRES_1920x1080,dsVIDEO_ASPECT_RATIO_16x9, dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_60,    true  },
    {"1080p24", dsVIDEO_PIXELRES_1920x1080,dsVIDEO_ASPECT_RATIO_16x9, dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_24,    false },
    {"1080p25", dsVIDEO_PIXELRES_1920x1080,dsVIDEO_ASPECT_RATIO_16x9, dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_25,    false },
    {"1080p30", dsVIDEO_PIXELRES_1920x1080,dsVIDEO_ASPECT_RATIO_16x9, dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_30,    false },
    {"1080p50", dsVIDEO_PIXELRES_1920x1080,dsVIDEO_ASPECT_RATIO_16x9, dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_50,    false },
    {"1080p60", dsVIDEO_PIXELRES_1920x1080,dsVIDEO_ASPECT_RATIO_16x9, dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_60,    false },
    {"2160p24", dsVIDEO_PIXELRES_3840x2160,dsVIDEO_ASPECT_RATIO_16x9, dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_24,    false },
    {"2160p30", dsVIDEO_PIXELRES_3840x2160,dsVIDEO_ASPECT_RATIO_16x9, dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_30,    false },
    {"2160p50", dsVIDEO_PIXELRES_3840x2160,dsVIDEO_ASPECT_RATIO_16x9, dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_50,    false },
    {"2160p60", dsVIDEO_PIXELRES_3840x2160,dsVIDEO_ASPECT_RATIO_16x9, dsVIDEO_SSMODE_2D, dsVIDEO_FRAMERATE_60,    false },
};

#endif /* _DS_VIDEO_RESOLUTION_SETTINGS_H_ */
