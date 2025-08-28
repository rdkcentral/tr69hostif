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

sed '/<\/model>/d; /<\/dm:document>/d' ./src/hostif/parodusClient/waldb/data-model/data-model-tv.xml > ./src/hostif/parodusClient/waldb/data-model/data-model-merged.xml
sed '/<?xml/,/<model/ d' ./src/hostif/parodusClient/waldb/data-model/data-model-generic.xml >> ./src/hostif/parodusClient/waldb/data-model/data-model-merged.xml

cp ./src/hostif/parodusClient/waldb/data-model/data-model-merged.xml /tmp/data-model.xml

cp ./src/unittest/stubs/rfc.properties /etc/rfc.properties
cp ./src/unittest/stubs/rfcdefaults.ini /tmp/rfcdefaults.ini

mkdir /opt/secure
mkdir /opt/secure/RFC
cp ./src/unittest/stubs/tr181store.ini /opt/secure/RFC/tr181store.ini
cp partners_defaults.json /etc/partners_defaults.json
cp ./src/unittest/stubs/partners_defaults_device.json /etc/partners_defaults_device.json
cp ./src/unittest/stubs/fwdnldstatus.txt  /opt/fwdnldstatus.txt

export TOP_DIR=`pwd`
cd ./src/

automake --add-missing
autoreconf --install

./configure --enable-libsoup3

make clean


echo "TOP_DIR = $TOP_DIR"

echo "**** Compiling data model gtest ****"
cd $TOP_DIR/src/hostif/parodusClient/gtest
rm dm_gtest
DEBUG_CFLAGS="${DEBUG_CFLAGS} -g -pg"
export LDFLAGS="-L/usr/lib/x86_64-linux-gnu -lglib-2.0 -pg"
make
./dm_gtest
echo "********************"

echo "**** Compiling DeviceInfo gtest ****"
cd $TOP_DIR/src/hostif/profiles/DeviceInfo/gtest
rm devieInfo_gtest
make
./devieInfo_gtest
echo "********************"

cd $TOP_DIR

if [ "$ENABLE_COV" = true ]; then
    lcov --capture --directory . --output-file coverage.info
    lcov --remove coverage.info '/usr/*' '*/gtest/*' '*/mocks/*' --output-file filtered.info
    lcov --extract filtered.info '*/src/hostif*' --output-file tr69hostif_coverage.info
    lcov --list tr69hostif_coverage.info
fi
 
