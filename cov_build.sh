WORKDIR=`pwd`
export ROOT=/usr
export INSTALL_DIR=${ROOT}/local
mkdir -p $INSTALL_DIR

#Build rfc
cd $ROOT
git clone https://github.com/rdkcentral/rfc.git
cd rfc
autoreconf -i
./configure --enable-rfctool=yes --enable-tr181set=yes
cd rfcapi
make librfcapi_la_CPPFLAGS="-I/usr/include/cjson"
make install

#Build yajl - tr69 alone needs this specific version
cd $ROOT 
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
git clone https://github.com/rdkcentral/rdk-halif-device_settings.git
git clone https://github.com/rdkcentral/rdkvhal-devicesettings-raspberrypi4.git
git clone https://github.com/rdkcentral/iarmbus.git


# Build devicesettings version with fixes for native build and use that as a stub
# TODO This is not present in mainline versions. Component maintainers will have to provide this in future.
cd $ROOT
rm -rf devicesettings
git clone https://github.com/rdkcentral/devicesettings.git -b feature/RDKE-539
cd devicesettings
autoreconf -i
sed -i '/#include "dsAudio.h"/d' /usr/devicesettings/rpc/cli/dsAudio.c
sed -i '/device::HdmiInput::getInstance().isPortConnected(portId);/d' /usr/devicesettings/ds/audioOutputPort.cpp
./configure
make INCLUDE_FILES="-I/usr/rdk-halif-device_settings/include -I/usr/rpc/include -I/usr/devicesettings/rpc/cli -I/usr/devicesettings/rpc/include -I/usr/iarmbus/core/include -I$WORKDIR/src/unittest/stubs -I/usr/devicesettings/rpc/srv -I/usr/rdkvhal-devicesettings-raspberrypi4" libds_la_CPPFLAGS="-I/usr/rdk-halif-device_settings/include -I/usr/devicesettings/ds/include -I$WORKDIR/src/unittest/stubs/ -I/usr/rdkvhal-devicesettings-raspberrypi4 -I/usr/devicesettings/rpc/include -I/usr/devicesettings/rpc/cli/ -I/usr/devicesettings/rpc/srv -I/usr/devicesettings/ds/include -I/usr/devicesettings/ds/" CFLAGS="-fpermissive"
make install

# Build and deploy stubs for IARMBus
echo "Building IARMBus stubs"
cd $WORKDIR
cd ./src/unittest/stubs
g++ -fPIC -shared -o libIARMBus.so iarm_stubs.cpp  -I$WORKDIR/src/hostif/parodusClient/pal -I$WORKDIR/src/unittest/stubs -I$WORKDIR/src/hostif/parodusClient/waldb -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I$WORKDIR/src/hostif/include -I$WORKDIR/src/hostif/profiles/DeviceInfo -I$WORKDIR/src/hostif/parodusClient/pal -fpermissive
cp libIARMBus.so /usr/local/lib
cp libIBus.h /usr/local/include
cp libIARM.h /usr/local/include

echo "##### Building tr69hostif module"
cd $WORKDIR
sed -i '/PKG_CHECK_MODULES(\[PROCPS\], \[libproc >= 3.2.8\])/s/^/#/' ./configure.ac
rm -f ./src/unittest/stubs/rdk_debug.h
autoreconf -i
./configure
make AM_CXXFLAGS="-I$WORKDIR/src/unittest/stubs -I$WORKDIR/src/hostif/include -I/usr/include/cjson -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I$WORKDIR/src/hostif/handlers/include -I$WORKDIR/src/hostif/parodusClient/waldb -I$WORKDIR/src/hostif/profiles/DeviceInfo -I/usr/include/cjson -I$WORKDIR/src/hostif/profiles/Time -I$WORKDIR/src/hostif/profiles/Device -I/usr/include/libsoup-3.0 -I/usr/include/yajl -I$WORKDIR/src/hostif/profiles/STBService -I$WORKDIR/src/unittest/stubs/ds -I/usr/devicesettings/ds -I/usr/local/include -I$WORKDIR/src/hostif/profiles/IP -I$WORKDIR/src/hostif/profiles/Ethernet -I/usr/local/include/rbus -I$WORKDIR/src/hostif/parodusClient/pal -I/usr/rdk-halif-device_settings/include -I/usr/local/include/libparodus -I/usr/local/include -I/usr/rdkvhal-devicesettings-raspberrypi4 -I/usr/local/include/ -I/usr/include/yajl -I/usr/tinyxml2 -I/usr/devicesettings/ds -I/$WORKDIR/src/hostif/httpserver/include" \
AM_LDFLAGS="-L/usr/local/lib -lrbus -lsecure_wrapper -lcurl -lrfcapi -lrdkloggers -llibparodus  -lnanomsg  -lIARMBus -lds -ldshalcli -ldshalsrv  -lwrp-c -lwdmp-c -lprocps -ltrower-base64 -lcimplog -L/usr/lib/x86_64-linux-gnu -lyajl -L/usr/local/lib/x86_64-linux-gnu -ltinyxml2"  CXXFLAGS="-fpermissive"
