/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
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


/**
* @defgroup iarmmgrs
* @{
* @defgroup mfr
* @{
**/


#ifndef _MFR_TYPES_H
#define _MFR_TYPES_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum _mfrError_t {
    mfrERR_NONE = 0,
    mfrERR_GENERAL = 0x1000,
    mfrERR_INVALID_PARAM,
    mfrERR_NOT_INITIALIZED,
    mfrERR_OPERATION_NOT_SUPPORTED,
    mfrERR_UNKNOWN,
    /* Please add Error Code here */
    mfrERR_MEMORY_EXHAUSTED,
    mfrERR_SRC_FILE_ERROR,
    mfrERR_WRITE_FLASH_FAILED,
    mfrERR_UPDATE_BOOT_PARAMS_FAILED,
    mfrERR_FAILED_CRC_CHECK,
    mfrERR_BAD_IMAGE_HEADER,
    mfrERR_IMPROPER_SIGNATURE,
    mfrERR_IMAGE_TOO_BIG,
    mfrERR_FAILED_INVALID_SIGNING_TIME,
    mfrERR_FAILED_INVALID_SVN,
    mfrERR_FAILED_IMAGE_SIGNING_TIME_OLDER,
    mfrERR_FAILED_IMAGE_SVN_OLDER,
    mfrERR_FAILED_SAME_DRI_CODE_VERSION,
    mfrERR_FAILED_SAME_PCI_CODE_VERSION,
    mfrERR_IMAGE_FILE_OPEN_FAILED,
    mfrERR_GET_FLASHED_IMAGE_DETAILS_FAILED,
    mfrERR_FLASH_VERIFY_FAILED,
    mfrERR_ALREADY_INITIALIZED,
} mfrError_t;

typedef struct _mfrSerializedData_t {
    char * buf;                        // buffer containing the data read.
    size_t bufLen;                     // length of the data buffer;
    void (* freeBuf) (char *buf);      // function used to free the buffer. If NULL, the user does not need to free the buffer.
} mfrSerializedData_t;

typedef enum _mfrSerializedType_t {
    mfrSERIALIZED_TYPE_MANUFACTURER = 0,
    mfrSERIALIZED_TYPE_MANUFACTUREROUI,
    mfrSERIALIZED_TYPE_MODELNAME,
    mfrSERIALIZED_TYPE_DESCRIPTION,
    mfrSERIALIZED_TYPE_PRODUCTCLASS,
    mfrSERIALIZED_TYPE_SERIALNUMBER,
    mfrSERIALIZED_TYPE_HARDWAREVERSION,
    mfrSERIALIZED_TYPE_SOFTWAREVERSION,
    mfrSERIALIZED_TYPE_PROVISIONINGCODE,
    mfrSERIALIZED_TYPE_FIRSTUSEDATE,
    mfrSERIALIZED_TYPE_DEVICEMAC,
    mfrSERIALIZED_TYPE_MOCAMAC,
    mfrSERIALIZED_TYPE_HDMIHDCP,
    mfrSERIALIZED_TYPE_PDRIVERSION,
    mfrSERIALIZED_TYPE_WIFIMAC,
    mfrSERIALIZED_TYPE_BLUETOOTHMAC,
    mfrSERIALIZED_TYPE_WPSPIN,
    mfrSERIALIZED_TYPE_MANUFACTURING_SERIALNUMBER,
    mfrSERIALIZED_TYPE_ETHERNETMAC,
    mfrSERIALIZED_TYPE_ESTBMAC,
    mfrSERIALIZED_TYPE_RF4CEMAC,
    mfrSERIALIZED_TYPE_PROVISIONED_MODELNAME,
    mfrSERIALIZED_TYPE_PMI,
    mfrSERIALIZED_TYPE_HWID,
    mfrSERIALIZED_TYPE_MODELNUMBER,
    /* boot data */
    mfrSERIALIZED_TYPE_SOC_ID,
    mfrSERIALIZED_TYPE_IMAGENAME,
    mfrSERIALIZED_TYPE_IMAGETYPE,
    mfrSERIALIZED_TYPE_BLVERSION,
    /* provisional data */
    mfrSERIALIZED_TYPE_REGION,
    /* other data */
    mfrSERIALIZED_TYPE_BDRIVERSION,
    /* led data */
    mfrSERIALIZED_TYPE_LED_WHITE_LEVEL,
    mfrSERIALIZED_TYPE_LED_PATTERN,
    mfrSERIALIZED_TYPE_MAX,
#ifdef PANEL_SERIALIZATION_TYPES
    mfrSERIALIZED_TYPE_COREBOARD_SERIALNUMBER=0x51,
    mfrSERIALIZED_TYPE_FACTORYBOOT,
    mfrSERIALIZED_TYPE_COUNTRYCODE,
    mfrSERIALIZED_TYPE_LANGUAGECODE,
    mfrSERIALIZED_TYPE_MANUFACTURERDATA,
    mfrSERIALIZED_TYPE_CPD_SIZE,
    mfrSERIALIZED_TYPE_PANEL_ID,
    mfrSERIALIZED_TYPE_PANEL_TYPE,
    mfrSERIALIZED_TYPE_PANEL_HDMI_WB_DATA_NORMAL,
    mfrSERIALIZED_TYPE_PANEL_HDMI_WB_DATA_COLD,
    mfrSERIALIZED_TYPE_PANEL_HDMI_WB_DATA_WARM,
    mfrSERIALIZED_TYPE_PANEL_HDMI_WB_DATA_USER,
    mfrSERIALIZED_TYPE_PANEL_TV_WB_DATA_NORMAL,
    mfrSERIALIZED_TYPE_PANEL_TV_WB_DATA_COLD,
    mfrSERIALIZED_TYPE_PANEL_TV_WB_DATA_WARM,
    mfrSERIALIZED_TYPE_PANEL_TV_WB_DATA_USER,
    mfrSERIALIZED_TYPE_PANEL_AV_WB_DATA_NORMAL,
    mfrSERIALIZED_TYPE_PANEL_AV_WB_DATA_COLD,
    mfrSERIALIZED_TYPE_PANEL_AV_WB_DATA_WARM,
    mfrSERIALIZED_TYPE_PANEL_AV_WB_DATA_USER,
    mfrSERIALIZED_TYPE_PANEL_DTB_VERSION,
    mfrSERIALIZED_TYPE_PANEL_DTB_DATA_SIZE,
    mfrSERIALIZED_TYPE_PANEL_DTB_DATA,
    /* panel data*/
    mfrSERIALIZED_TYPE_PANEL_DATA_FUNCTION_STATUS,
    mfrSERIALIZED_TYPE_PANEL_DATA_AGEING_TIME,
    mfrSERIALIZED_TYPE_PANEL_DATA_POWER_ON_TIME,
    mfrSERIALIZED_TYPE_PANEL_DATA_BACKLIGHT_TIME,
    mfrSERIALIZED_TYPE_PANEL_DATA_VALID,
    mfrSERIALIZED_TYPE_PANEL_DATA_TPV_APP_VERSION,
    mfrSERIALIZED_TYPE_PANEL_ALS_CALIBRATION_INDEX0,
    mfrSERIALIZED_TYPE_PANEL_ALS_CALIBRATION_INDEX1,
    /*Gamma data*/
    mfrSERIALIZED_TYPE_PANEL_GAMMA_CALIBRATED_NORMAL,
    mfrSERIALIZED_TYPE_PANEL_GAMMA_CALIBRATED_COLD,
    mfrSERIALIZED_TYPE_PANEL_GAMMA_CALIBRATED_WARM,
    mfrSERIALIZED_TYPE_PANEL_GAMMA_CALIBRATED_BOOST_NORMAL,
    mfrSERIALIZED_TYPE_PANEL_GAMMA_CALIBRATED_BOOST_COLD,
    mfrSERIALIZED_TYPE_PANEL_GAMMA_CALIBRATED_BOOST_WARM,
    /*WB data boost*/
    mfrSERIALIZED_TYPE_PANEL_HDMI_WB_DATA_BOOST_NORMAL,
    mfrSERIALIZED_TYPE_PANEL_HDMI_WB_DATA_BOOST_COLD,
    mfrSERIALIZED_TYPE_PANEL_HDMI_WB_DATA_BOOST_WARM,
    /*Tmax*/
    mfrSERIALIZED_TYPE_PANEL_PEAK_BRIGHTNESS_NONBOOST,
    mfrSERIALIZED_TYPE_PANEL_PEAK_BRIGHTNESS_BOOST,
    mfrSERIALIZED_TYPE_PANEL_PEAK_BRIGHTNESS_BURST,
    mfrSERIALIZED_TYPE_PANEL_HDMI_WB_DATA_SUPERCOLD,
    mfrSERIALIZED_TYPE_PANEL_HDMI_WB_DATA_BOOST_SUPERCOLD,
    mfrSERIALIZED_TYPE_PANEL_GAMMA_CALIBRATED_SUPERCOLD,
    mfrSERIALIZED_TYPE_PANEL_GAMMA_CALIBRATED_BOOST_SUPERCOLD,
    mfrSERIALIZED_TYPE_PANEL_MAX,
#endif
} mfrSerializedType_t;

typedef enum _mfrImageType_t {
    mfrIMAGE_TYPE_CDL,
    mfrIMAGE_TYPE_RCDL,
    mfrUPGRADE_IMAGE_MONOLITHIC,
    mfrUPGRADE_IMAGE_PACKAGEHEADER
} mfrImageType_t;

typedef enum _mfrUpgradeProgress_t {
  mfrUPGRADE_PROGRESS_NOT_STARTED = 0,
  mfrUPGRADE_PROGRESS_VERIFYING,
  mfrUPGRADE_PROGRESS_FLASHING,
  mfrUPGRADE_PROGRESS_REBOOTING,
  mfrUPGRADE_PROGRESS_ABORTED,
  mfrUPGRADE_PROGRESS_COMPLETED,
} mfrUpgradeProgress_t;

typedef enum _mfrBlPattern_t
{
    mfrBL_PATTERN_NORMAL = 0,
    mfrBL_PATTERN_SILENT,
    mfrBL_PATTERN_SILENT_LED_ON
} mfrBlPattern_t;

typedef struct _mfrUpgradeStatus_t {
  mfrUpgradeProgress_t progress;
  mfrError_t error;
  char error_string[32];
  int percentage;
} mfrUpgradeStatus_t;

typedef struct _mfrUpgradeStatusNotify_t {
   void * cbData;
   void (*cb) (mfrUpgradeStatus_t *status);
   int interval; // number of seconds between two callbacks. 0 means invoking callback only once to report final upgrade result.
} mfrUpgradeStatusNotify_t;

/**
 * @brief Initialize the mfr library.  
 * This function should be call once before the functions in this API can be used.
 *
 * @param [in]  :  None.
 * @param [out] :  None. 
 *
 * @return Error Code:  If error code is returned, the init has failed.
 */
mfrError_t mfr_init( void );
/**
 * @brief Initialize the mfr partition.
 * mfr_init invokes this mfr_partition_init.
 * This function should be call once before accessing serialized data.
 *
 * @param [in]  :  None.
 * @param [out] :  None.
 *
 * @return Error Code:  If error code is returned, the init has failed.
*/
mfrError_t mfr_partition_init(void);
/**
 * @brief Retrieve Serialized Read-Only data from device.  The serialized data is returned as a byte stream. It is upto the
 * application to deserialize and make sense of the data returned.   Please note that even if the serialized data returned is 
 * "string", the buffer is not required to contain the null-terminator.
 *
 * @param [in] type:  specifies the serialized data to read.
 * @param [in]data:  contains information about the returned data (buffer location, length, and func to free the buffer). 
 *
 * @return Error Code:  If error code is returned, the read has failed and values in data should not be used.
 */
mfrError_t mfrGetSerializedData( mfrSerializedType_t type,  mfrSerializedData_t *data );
/**
 * @brief Sets R/W Serialized data on device.
 *
 * @param [in] type:  specifies the serialized data to write.
 * @param [in]data:  contains information about the data (buffer location, length, and func to free the buffer).
 *
 * @return Error Code:  If error code is returned, the write has failed and values in data should not be used.
 */
mfrError_t mfrSetSerializedData( mfrSerializedType_t type,  mfrSerializedData_t *data);
/**
* @brief Write the image into flash.  The process should follow these major steps:
*    1) Flash the image.
*    2) Update boot params and switch banks to prepare for a reboot event.
*    3) All upgrades should be done in the alternate bank. The current bank should not be disturbed at any cost.
*         i.e. a second upgrade will before a reboot will overwrite the non-current bank only.
*
*    State Transition:
*    0) Before the API is invoked, the Upgrade process should be in PROGRESS_NOT_STARTED state. 
*    1) After the API returnes with success, the Upgrade process moves to PROGRESS_STARTED state.
*    2) After the API returnew with error,   the Upgrade process stays in PROGRESS_NO_STARTED state. Notify function will not be invoked.
*    3) The notify function is called at regular interval with proress = PROGRESS_STARTED.
*    4) The last invocation of notify function should have either progress = PROGRESS_COMPLETED or progress = PROGRESS_ABORTED with error code set.
*
* @param [in] name:  the path of the image file in the STB file system.
* @param [in] path:  the filename of the image file. 
* @param [in] type:  the type (e.g. format, signature type) of the image.  This can dictate the handling of the image within the MFR library.
* @param[in] callback: function to provide status of the image flashing process.  
* @return Error Code:  If error code is returned, the image flashing is not initiated..
*/
mfrError_t mfrWriteImage(const char *name,  const char *path, mfrImageType_t type,  mfrUpgradeStatusNotify_t notify);
/**
* @brief Delete the P-DRI image if it is present
*
* @return Error Code:  Return mfrERR_NONE if P-DRI is succesfully deleted or not present, mfrERR_GENERAL if deletion fails
*/
mfrError_t mfrDeletePDRI(void);

/**
* @brief Delete the platform images
*
* @return Error Code:  Return mfrERR_NONE if the images are scrubbed, mfrERR_GENERAL if scrubbing fails
*/
mfrError_t mfrScrubAllBanks(void);
/**
* @brief Sets how the frontpanel LED(s) (and TV backlight on applicable devices) behave when running bootloader.
* @param [in] pattern : options are defined by enum mfrBlPattern_t.
* @return Error Code:  Return mfrERR_NONE if operation is successful, mfrERR_GENERAL if it fails
*/
mfrError_t mfrSetBootloaderPattern(mfrBlPattern_t pattern);
/**
 * @brief API to update Primary Splash screen Image and to override the default the Splash screen image
 *
 * @param [in] path : char pointer which holds the path of input bootloader OSD image.
 *
 * @return mfrError_t                      - Status
 * @retval mfrERR_NONE                     - Success
 * @retval mfrERR_NOT_INITIALIZED          - Module is not initialised
 * @retval mfrERR_INVALID_PARAM            - Parameter passed to this function is invalid
 * @retval mfrERR_IMAGE_FILE_OPEN_FAILED   - Failed to open the downloaded splash screen file
 * @retval mfrERR_MEMORY_EXHAUSTED         - memory allocation failure
 *
 * @pre  mfr_init() should be called before calling this API. If this precondition is not met,
 * the API will return mfrERR_NOT_INITIALIZED.
 * @warning  This API is Not thread safe
 *
 */
mfrError_t mfrSetBlSplashScreen(const char *path);
/**
 * @brief API to clear the primary Splash screen Image and to make
 * use of default Splash screen image
 *
 * @return mfrError_t                      - Status
 * @retval mfrERR_NONE                     - Success
 * @retval mfrERR_NOT_INITIALIZED          - Module is not initialised
 * @retval mfrERR_IMAGE_FILE_OPEN_FAILED   - Failed to open the downloaded splash screen file
 * @retval mfrERR_MEMORY_EXHAUSTED         - memory allocation failure
 *
 * @pre  mfr_init() should be called before calling this API. If this precondition is not met,
 * the API will return mfrERR_NOT_INITIALIZED.
 * @warning  This API is Not thread safe
 *
 */
mfrError_t mfrClearBlSplashScreen(void);

#endif


/** @} */
/** @} */
