#!/bin/sh

 #########################################################################
 # Copyright 2024 Comcast Cable Communications Management, LLC
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 # http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
 #
 # SPDX-License-Identifier: Apache-2.0
 ############################################################################

WORKDIR=`pwd`
sh dependent_rdk_pkg_installer.sh
export ROOT=/usr

cd $ROOT
rm -rf remote_debugger
rm -rf rdk-halif-device_settings
rm -rf rdkvhal-devicesettings-raspberrypi4
git clone https://github.com/rdkcentral/remote_debugger.git
git clone https://github.com/rdkcentral/rdk-halif-device_settings.git
git clone https://github.com/rdkcentral/rdkvhal-devicesettings-raspberrypi4.git

cd $WORKDIR
ls -l /usr/local/include/libparodus/
ENABLE_COV=false

if [ "x$1" = "x--enable-cov" ]; then
      echo "Enabling coverage options"
      export CXXFLAGS="-g -O0 -fprofile-arcs -ftest-coverage"
      export CFLAGS="-g -O0 -fprofile-arcs -ftest-coverage"
      export LDFLAGS="-lgcov --coverage"
      ENABLE_COV=true
fi

apt-get update
apt-get -y install libtinyxml2-dev
apt-get -y install libsoup-3.0-dev
apt-get -y install libprocps-dev
apt-get -y install libnanomsg-dev
apt-get -y install iproute2
sed '/<\/model>/d; /<\/dm:document>/d' ./src/hostif/parodusClient/waldb/data-model/data-model-tv.xml > ./src/hostif/parodusClient/waldb/data-model/data-model-merged.xml
sed '/<?xml/,/<model/ d' ./src/hostif/parodusClient/waldb/data-model/data-model-generic.xml >> ./src/hostif/parodusClient/waldb/data-model/data-model-merged.xml

cp ./src/hostif/parodusClient/waldb/data-model/data-model-merged.xml /tmp/data-model.xml

cp ./src/unittest/stubs/rfc.properties /etc/rfc.properties
cp ./src/unittest/stubs/rfcdefaults.ini /tmp/rfcdefaults.ini

mkdir /opt/secure
mkdir /opt/secure/RFC
mkdir -p /opt/secure/reboot/
mkdir -p /opt/www/authService/
mkdir -p /tmp/webpa
mkdir -p /opt/persistent/
mkdir -p /etc/rfcdefaults
mkdir -p /etc/apparmor.d
mkdir -p /opt/secure/persistent/
mkdir -p /etc/rrd/
cp ./src/unittest/stubs/tr181store.ini /opt/secure/RFC/tr181store.ini
cp ./src/integrationtest/conf/bootstrap.ini /opt/secure/RFC/
cp ./src/integrationtest/conf/rfcVariable.ini /opt/secure/RFC/
cp partners_defaults.json /etc/partners_defaults.json
cp ./src/unittest/stubs/partners_defaults_device.json /etc/partners_defaults_device.json
cp ./src/unittest/stubs/fwdnldstatus.txt  /opt/fwdnldstatus.txt
cp ./src/integrationtest/conf/mgrlist.conf /etc/mgrlist.conf
cp ./src/unittest/stubs/remote_debugger.json /etc/rrd/remote_debugger.json
touch /tmp/timeReceivedNTP
touch /tmp/webpa/start_time 
touch /opt/persistent/firstNtpTime
touch /etc/rfcdefaults/rfcdefaults.ini
touch /opt/XRE_container_enable /opt/dab-enable
touch /opt/.ntpEnabled
touch /var/tmp/rssh.pid
cat /version.txt
rm -rf /version.txt
echo "Method|" >> /opt/fwdnldstatus.txt

export TOP_DIR=$WORKDIR
cd ./src/

automake --add-missing
autoreconf --install

./configure --enable-libsoup3

make clean


echo "TOP_DIR = $TOP_DIR"

echo "**** Compiling handlers gtest ****"
cd $TOP_DIR/src/hostif/handlers/src/gtest
rm handlers_gtest
sed -i '/getCurrentTime/,/^ *}/d' ../../../src/hostIf_utils.cpp
make clean
make
./handlers_gtest
echo "********************"


echo "**** Compiling data model gtest ****"
cd $TOP_DIR/src/hostif/parodusClient/gtest
rm dm_gtest
#sed -i '/getCurrentTime/,/^ *}/d' ../../src/hostIf_utils.cpp
make
./dm_gtest
echo "********************"

echo "**** Compiling httpserver gtest ****"
cd $TOP_DIR/src/hostif/httpserver/src/gtest
rm httpserver_gtest
#sed -i '$a void getCurrentTime(struct timespec *timer)\n{\n    clock_gettime(CLOCK_REALTIME, timer);\n}' ../../../src/hostIf_utils.cpp
#sed -i '/getCurrentTime/,/^ *}/d' ../../src/hostIf_utils.cpp
make clean
make
./httpserver_gtest
echo "********************"

echo "**** Compiling src gtest ****"
cd $TOP_DIR/src/hostif/src/gtest
rm src_gtest
sed -i '$a void getCurrentTime(struct timespec *timer)\n{\n    clock_gettime(CLOCK_REALTIME, timer);\n}' ../hostIf_utils.cpp
make clean
make
./src_gtest
echo "********************"

echo "**** Compiling DHCPv4 gtest ****"
cd $TOP_DIR/src/hostif/profiles/DHCPv4/gtest
rm dhcpv4_gtest
make
./dhcpv4_gtest
echo "********************"

echo "**** Compiling Device gtest ****"
cd $TOP_DIR/src/hostif/profiles/Device/gtest
rm device_gtest
make
./device_gtest
echo "********************"

echo "**** Compiling Ethernet gtest ****"
cd $TOP_DIR/src/hostif/profiles/Ethernet/gtest
rm ethernet_gtest
make
./ethernet_gtest
echo "********************"

echo "**** Compiling Time gtest ****"
cd $TOP_DIR/src/hostif/profiles/Time/gtest
rm time_gtest
make
./time_gtest
echo "********************"

echo "**** Compiling DeviceInfo gtest ****"
cd $TOP_DIR/src/hostif/profiles/DeviceInfo/gtest
cp ../../../../unittest/stubs/rfc.properties /etc/rfc.properties
rm devieInfo_gtest /opt/www/authService/partnerId3.dat
make
./devieInfo_gtest
echo "********************"

cd $TOP_DIR

if [ "$ENABLE_COV" = true ]; then
    lcov --capture --directory . --output-file coverage.info
    lcov --remove coverage.info '/usr/*' '*/gtest/*' '*/mocks/*' --output-file filtered.info
    lcov --extract filtered.info '*/src/hostif/httpserver/*' '*/src/hostif/parodusClient/*' '*/src/hostif/src/*' '*/src/hostif/profiles/DHCPv4/*' '*/src/hostif/profiles/Device/*' '*/src/hostif/profiles/DeviceInfo/*' '*/src/hostif/profiles/Ethernet/*' '*/src/hostif/profiles/Time/*' --output-file tr69hostif_coverage.info
    lcov --list tr69hostif_coverage.info
fi
 
