WORKDIR=`pwd`
export ROOT=/usr
export INSTALL_DIR=${ROOT}/local
mkdir -p $INSTALL_DIR
apt-get update && apt-get install -y libsoup-3.0

#Build rfc
cd $ROOT
rm -rf rfc
git clone https://github.com/rdkcentral/rfc.git
cd rfc
autoreconf -i
./configure --enable-rfctool=yes --enable-tr181set=yes --enable-tr69hostif=yes
cd rfcapi
make librfcapi_la_CPPFLAGS="-I/usr/include/cjson -DUSE_IARMBUS"
make install
cd ../tr181api
cp /usr/include/cjson/cJSON.h  ./
cp /usr/local/include/wdmp-c/wdmp-c.h ./
make AM_CXXFLAGS="-DUSE_TR69HOSTIF" && make install
cd ../utils
make && make install

#Build yajl - tr69 alone needs this specific version
cd $ROOT
rm -rf yajl
git clone https://github.com/lloyd/yajl.git -b 1.x
cd yajl
mkdir build
cd build
cmake ..
make
make install

cd $ROOT
rm -rf rdk-halif-device_settings
rm -rf rdkvhal-devicesettings-raspberrypi4
rm -rf iarmbus
rm -rf remote_debugger
git clone https://github.com/rdkcentral/rdk-halif-device_settings.git
git clone https://github.com/rdkcentral/rdkvhal-devicesettings-raspberrypi4.git
git clone https://github.com/rdkcentral/iarmbus.git
git clone https://github.com/rdkcentral/remote_debugger.git

<<<<<<< HEAD
# Install stubs into system paths before any dependent build:
#   1. telemetry_busmessage_sender.h — needed by dsTelemetry.h (angle-bracket include);
#      /usr/local/include is on gcc's default system search path so no -isystem flag required.
#   2. dsVideoResolutionSettings.h — mainline rdk-halif header defines the guard but omits
#      kResolutions; overwrite with our stub so kResolutions is always available regardless
#      of which copy is included first.
cp $WORKDIR/src/unittest/stubs/telemetry_busmessage_sender.h /usr/local/include/
cp $WORKDIR/src/unittest/stubs/dsVideoResolutionSettings.h /usr/rdk-halif-device_settings/include/dsVideoResolutionSettings.h
# dsAudioSettings.h — mainline dsAudioConfig.c (rpc/srv) uses kConfigs/kPorts from this header.
# If dsAudioConfig.h or any transitively-included header sets the _DS_AUDIOOUTPUTPORTSETTINGS_H
# guard before the rdkvhal version is reached, kConfigs/kPorts become undeclared.
# Overwriting the cloned rdkvhal copy with our stub guarantees the tables are always present.
cp $WORKDIR/src/unittest/stubs/dsAudioSettings.h /usr/rdkvhal-devicesettings-raspberrypi4/dsAudioSettings.h
# dsVideoPortSettings.h — same guard-collision pattern; kConfigs/kPorts for video ports.
cp $WORKDIR/src/unittest/stubs/dsVideoPortSettings.h /usr/rdkvhal-devicesettings-raspberrypi4/dsVideoPortSettings.h
# dsVideoResolutionSettings.h must also land in the rdkvhal directory: dsVideoPortSettings.h
# uses a quote-form include so gcc checks rdkvhal/ first; if an empty copy lives there the
# guard is set without kResolutions before our rdk-halif copy is ever reached.
cp $WORKDIR/src/unittest/stubs/dsVideoResolutionSettings.h /usr/rdkvhal-devicesettings-raspberrypi4/dsVideoResolutionSettings.h
# dsVideoDeviceSettings.h — same guard-collision pattern; rdkvhal version wraps kConfigs in
# anonymous namespace {} inside extern "C" {}, which GCC does not handle correctly when the
# .c TU is compiled as C.  Our stub omits the namespace so kConfigs is always at file scope.
cp $WORKDIR/src/unittest/stubs/dsVideoDeviceSettings.h /usr/rdkvhal-devicesettings-raspberrypi4/dsVideoDeviceSettings.h

# Build and install libtelemetry_msgsender stub shared library.
# devicesettings rpc/srv hardcodes -ltelemetry_msgsender in libdshalsrv_la_LIBADD;
# the real library belongs to the telemetry2 component which is absent here.
gcc -fPIC -shared -o /usr/local/lib/libtelemetry_msgsender.so \
    $WORKDIR/src/unittest/stubs/telemetry_msgsender_stub.c

# Remove the tr69hostif-specific rdk_debug.h stub BEFORE building devicesettings.
# The stub includes hostIf_main.h (a tr69hostif-internal header) which is not available
# during the devicesettings build.  devicesettings uses the system rdkloggers rdk_debug.h.
rm -f $WORKDIR/src/unittest/stubs/rdk_debug.h

# Build devicesettings from mainline using standard automake variables.
#   1. CPPFLAGS (not INCLUDE_FILES) is always honoured by automake-generated Makefiles.
#   2. RDK_DSHAL_NAME and DSMGR_LOGGER_ENABLED are passed via CFLAGS to avoid
#      patching source files.
#   3. libdshalsrv_la_CPPFLAGS must be set explicitly so rpc/srv picks up iarmUtil.h.
#   4. make + make install are merged into one invocation so CPPFLAGS are not lost during
#      the libtool relink phase triggered by install.
=======
cp $WORKDIR/src/unittest/stubs/telemetry_busmessage_sender.h /usr/local/include/
cp $WORKDIR/src/unittest/stubs/dsVideoResolutionSettings.h /usr/rdk-halif-device_settings/include/dsVideoResolutionSettings.h
cp $WORKDIR/src/unittest/stubs/dsAudioSettings.h /usr/rdkvhal-devicesettings-raspberrypi4/dsAudioSettings.h
cp $WORKDIR/src/unittest/stubs/dsVideoPortSettings.h /usr/rdkvhal-devicesettings-raspberrypi4/dsVideoPortSettings.h
cp $WORKDIR/src/unittest/stubs/dsVideoResolutionSettings.h /usr/rdkvhal-devicesettings-raspberrypi4/dsVideoResolutionSettings.h
cp $WORKDIR/src/unittest/stubs/dsVideoDeviceSettings.h /usr/rdkvhal-devicesettings-raspberrypi4/dsVideoDeviceSettings.h

gcc -fPIC -shared -o /usr/local/lib/libtelemetry_msgsender.so $WORKDIR/src/unittest/stubs/telemetry_msgsender_stub.c
rm -f $WORKDIR/src/unittest/stubs/rdk_debug.h

>>>>>>> be553849fc3dca6895c5af1515e205ff57ea0a47
cd $ROOT
rm -rf devicesettings
git clone https://github.com/rdkcentral/devicesettings.git
cd devicesettings
autoreconf -i
sed -i '/#include "dsAudio.h"/d' /usr/devicesettings/rpc/cli/dsAudio.c
sed -i '/device::HdmiInput::getInstance().isPortConnected(portId);/d' /usr/devicesettings/ds/audioOutputPort.cpp
./configure
DS_COMMON_INCLUDES="-I/usr/rdk-halif-device_settings/include \
    -I/usr/rpc/include \
    -I/usr/devicesettings/rpc/cli \
    -I/usr/devicesettings/rpc/include \
    -I/usr/iarmbus/core/include \
    -I$WORKDIR/src/unittest/stubs \
    -I/usr/devicesettings/rpc/srv \
    -I/usr/rdkvhal-devicesettings-raspberrypi4"
make \
    CPPFLAGS="$DS_COMMON_INCLUDES" \
    libds_la_CPPFLAGS="-I/usr/rdk-halif-device_settings/include -I/usr/devicesettings/ds/include -I$WORKDIR/src/unittest/stubs/ -I/usr/rdkvhal-devicesettings-raspberrypi4 -I/usr/devicesettings/rpc/include -I/usr/devicesettings/rpc/cli/ -I/usr/devicesettings/rpc/srv -I/usr/devicesettings/ds/include -I/usr/devicesettings/ds/ -DRDK_DSHAL_NAME='\"libdshal.so\"'" \
    libdshalsrv_la_CPPFLAGS="-I/usr/iarmbus/core/include -I/usr/devicesettings/rpc/include -I/usr/devicesettings/rpc/srv -I/usr/rdk-halif-device_settings/include -I$WORKDIR/src/unittest/stubs -isystem $WORKDIR/src/unittest/stubs -DDSMGR_LOGGER_ENABLED=ON -DRDK_DSHAL_NAME='\"libdshal.so\"'" \
    CFLAGS="-fpermissive" \
    install

# Build and deploy stubs for IARMBus
echo "Building IARMBus stubs"
cd $WORKDIR
cd ./src/unittest/stubs
g++ -fPIC -shared -o libIARMBus.so iarm_stubs.cpp  -I$WORKDIR/src/hostif/parodusClient/pal -I$WORKDIR/src/unittest/stubs -I$WORKDIR/src/hostif/parodusClient/waldb -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I$WORKDIR/src/hostif/include -I$WORKDIR/src/hostif/profiles/DeviceInfo -I$WORKDIR/src/hostif/parodusClient/pal -fpermissive
g++ -fPIC -shared -o libWPEFrameworkPowerController.so powerctrl_stubs.cpp  -I$WORKDIR/src/unittest/stubs -fpermissive
cp libIARMBus.so /usr/local/lib
cp libIBus.h /usr/local/include
cp libIARM.h /usr/local/include
cp libWPEFrameworkPowerController.so /usr/local/lib/libWPEFrameworkPowerController.so


echo "##### Building tr69hostif module"
cd $WORKDIR
sed -i '/PKG_CHECK_MODULES(\[PROCPS\], \[libproc >= 3.2.8\])/s/^/#/' ./configure.ac
<<<<<<< HEAD
=======

>>>>>>> be553849fc3dca6895c5af1515e205ff57ea0a47
autoreconf -i
./configure  --enable-IPv6=yes

make AM_CXXFLAGS="-I$WORKDIR/src/unittest/stubs -I$WORKDIR/src/hostif/include -I/usr/include/cjson -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I$WORKDIR/src/hostif/handlers/include -I$WORKDIR/src/hostif/parodusClient/waldb -I$WORKDIR/src/hostif/profiles/DeviceInfo -I/usr/include/cjson -I$WORKDIR/src/hostif/profiles/Time -I$WORKDIR/src/hostif/profiles/Device -I/usr/include/libsoup-3.0 -I/usr/include/yajl -I$WORKDIR/src/hostif/profiles/STBService -I$WORKDIR/src/unittest/stubs/ds -I/usr/devicesettings/ds -I/usr/local/include -I$WORKDIR/src/hostif/profiles/IP -I$WORKDIR/src/hostif/profiles/Ethernet -I/usr/local/include/rbus -I$WORKDIR/src/hostif/parodusClient/pal -I/usr/rdk-halif-device_settings/include -I/usr/local/include/libparodus -I/usr/local/include -I/usr/rdkvhal-devicesettings-raspberrypi4 -I/usr/local/include/ -I/usr/include/yajl -I/usr/tinyxml2 -I/usr/devicesettings/ds -I/$WORKDIR/src/hostif/httpserver/include -I/usr/remote_debugger/src/ -DIPV6_SUPPORT" \
AM_LDFLAGS="-L/usr/local/lib -lrbus -lsecure_wrapper -lcurl -lrfcapi -lrdkloggers -llibparodus -lglib-2.0 -lnanomsg  -lIARMBus -lWPEFrameworkPowerController -lds -ldshalcli -ldshalsrv  -lwrp-c -lwdmp-c -lprocps -ltrower-base64 -lcimplog -lsoup-3.0 -L/usr/lib/x86_64-linux-gnu -lyajl -L/usr/local/lib/x86_64-linux-gnu -ltinyxml2"  CXXFLAGS="-fpermissive -DPARODUS_ENABLE -DUSE_REMOTE_DEBUGGER" \
    install

cd ./src/hostif/parodusClient/pal/mock-parodus/
sh mock_parodus_build.sh

ln -sf /usr/local/bin/tr181 /usr/bin/tr181Set
rbuscli set Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MOCASSH.Enable boolean true
