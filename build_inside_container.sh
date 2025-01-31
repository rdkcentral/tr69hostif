##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
#
# Copyright 2024 RDK Management
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
##########################################################################


export INSTALL_DIR='/usr/local'
export top_srcdir=`pwd`
export top_builddir=`pwd`

autoreconf --install

export CXXFLAGS="-I/usr/include/cjson -I${INSTALL_DIR}/include/rtmessage -I${INSTALL_DIR}/include/msgpack -I${INSTALL_DIR}/include/rbus -I${INSTALL_DIR}/include -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/local/include -DFEATURE_SUPPORT_WEBCONFIG -I/usr/include/yajl -DYAJL_V2 -Wno-error=format-truncation -I/usr/local/include/safeclib/ -I$(pwd)/src/integrationtest/stubs/ -I$(pwd)/src/hostif/profiles/DeviceInfo/ -DPARODUS_ENABLE"

export LDFLAGS="-L/usr/lib/x86_64-linux-gnu -L/usr/local/lib -lglib-2.0 -lnanomsg -lsecure_wrapper -lrbus -lcimplog -ltrower-base64"

cp ./src/integrationtest/stubs/src/Stubs.cpp ./src/hostif/profiles/STBService/
git apply ./src/integrationtest/patch/STBService.patch
git apply ./src/integrationtest/patch/Yajl.patch

cp ./src/integrationtest/stubs/dsTypes.h /usr/local/include/dsTypes.h
cp ./src/integrationtest/stubs/audioOutputPort.hpp /usr/local/include/audioOutputPort.hpp
cp ./src/integrationtest/stubs/host.hpp /usr/local/include/host.hpp
cp ./src/integrationtest/conf/data-model.xml /etc/
cp ./src/integrationtest/conf/mgrlist.conf /etc/

mkdir -p /opt/secure/RFC/
cp ./src/integrationtest/conf/rfcdefaults.ini /tmp/
cp ./src/integrationtest/conf/rfc.properties /etc/
cp ./src/integrationtest/conf/tr181store.ini /opt/secure/RFC/
touch /opt/secure/RFC/bootstrap.ini
touch /opt/secure/RFC/tr181localstore.ini
touch /opt/persistent/tr181localstore.ini
touch /opt/secure/RFC/bootstrap.journal

./configure --prefix=${INSTALL_DIR} && make && make install
