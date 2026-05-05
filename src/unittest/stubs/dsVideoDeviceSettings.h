/*
 * dsVideoDeviceSettings.h — Native-build stub for tr69hostif L2/coverage builds.
 *
 * The rdkvhal-devicesettings-raspberrypi4 version wraps kConfigs inside
 * an anonymous namespace {} placed inside extern "C" {}.  GCC does not
 * handle this combination correctly when the .c translation unit is compiled
 * as C, causing 'kConfigs' was not declared in this scope at the call site
 * in dsVideoDeviceConfig.c.  cov_build.sh copies this stub over the cloned
 * rdkvhal copy so the tables are always visible at file scope.
 *
 * Content mirrors rdkvhal-devicesettings-raspberrypi4/dsVideoDeviceSettings.h
 * (main branch) without the anonymous namespace.
 */
#ifndef _DS_VIDEODEVICESETTINGS_H_
#define _DS_VIDEODEVICESETTINGS_H_

#include "dsUtl.h"
#include "dsVideoDeviceTypes.h"

static const dsVideoZoom_t kSupportedDFCs[] = {
    dsVIDEO_ZOOM_NONE,
    dsVIDEO_ZOOM_FULL,
    dsVIDEO_ZOOM_PLATFORM,
};

static const dsVideoZoom_t kDefaultDFC = dsVIDEO_ZOOM_FULL;

static const int kNumVideoDevices = 1;

static const dsVideoConfig_t kConfigs[] = {
    {
        /* .numSupportedDFCs = */ dsUTL_DIM(kSupportedDFCs),
        /* .supportedDFCs =    */ kSupportedDFCs,
        /* .defaultDFC =       */ dsVIDEO_ZOOM_FULL,
    },
};

#endif /* _DS_VIDEODEVICESETTINGS_H_ */
