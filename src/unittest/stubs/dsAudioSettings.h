/*
 * dsAudioSettings.h — Native-build stub for tr69hostif L2/coverage builds.
 *
 * Mainline devicesettings rpc/srv/dsAudioConfig.c includes dsAudioSettings.h and
 * uses the kConfigs / kPorts tables defined here.  If dsAudioConfig.h (or another
 * header) sets the _DS_AUDIOOUTPUTPORTSETTINGS_H guard before the real rdkvhal
 * header is reached, kConfigs and kPorts become invisible.  Copying this file over
 * the cloned rdkvhal-devicesettings-raspberrypi4/dsAudioSettings.h guarantees the
 * tables are always present regardless of include order.
 *
 * Content mirrors rdkvhal-devicesettings-raspberrypi4/dsAudioSettings.h (main branch).
 * Type definitions (dsAudioTypeConfig_t, dsAudioPortConfig_t, etc.) come from
 * rdk-halif-device_settings/include/dsAVDTypes.h via dsTypes.h / dsUtl.h.
 */
#ifndef _DS_AUDIOOUTPUTPORTSETTINGS_H
#define _DS_AUDIOOUTPUTPORTSETTINGS_H

#include "dsUtl.h"
#include "dsAVDTypes.h"

/* Supported configurations for a single HDMI audio port */
static const dsAudioPortType_t kSupportedPortTypes[] = {
    dsAUDIOPORT_TYPE_HDMI
};

static const dsAudioEncoding_t kSupportedHDMIEncodings[] = {
    dsAUDIO_ENC_PCM, dsAUDIO_ENC_AC3
};

static const dsAudioCompression_t kSupportedHDMICompressions[] = {
    dsAUDIO_CMP_NONE, dsAUDIO_CMP_LIGHT, dsAUDIO_CMP_MEDIUM, dsAUDIO_CMP_HEAVY,
};

static const dsAudioStereoMode_t kSupportedHDMIStereoModes[] = {
    dsAUDIO_STEREO_STEREO, dsAUDIO_STEREO_SURROUND,
};

static const dsAudioTypeConfig_t kConfigs[] = {
    {
        /* .typeId =               */ dsAUDIOPORT_TYPE_HDMI,
        /* .name =                 */ "HDMI",
        /* .numSupportedCompressions = */ dsUTL_DIM(kSupportedHDMICompressions),
        /* .compressions =         */ kSupportedHDMICompressions,
        /* .numSupportedEncodings = */ dsUTL_DIM(kSupportedHDMIEncodings),
        /* .encodings =            */ kSupportedHDMIEncodings,
        /* .numSupportedStereoModes = */ dsUTL_DIM(kSupportedHDMIStereoModes),
        /* .stereoModes =          */ kSupportedHDMIStereoModes,
    }
};

static const dsVideoPortPortId_t connectedVOPs[dsAUDIOPORT_TYPE_MAX][dsVIDEOPORT_TYPE_MAX] = {
    { /* VOPs connected to LR Audio — none */ },
    { /* VOPs connected to HDMI Audio */
        { dsVIDEOPORT_TYPE_HDMI, 0 },
    }
};

static const dsAudioPortConfig_t kPorts[] = {
    {
        /* .id =           */ { dsAUDIOPORT_TYPE_HDMI, 0 },
        /* .connectedVOPs =*/ connectedVOPs[dsAUDIOPORT_TYPE_HDMI],
    }
};

#endif /* _DS_AUDIOOUTPUTPORTSETTINGS_H */
