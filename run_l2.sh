#!/bin/sh

####################################################################################
# If not stated otherwise in this file or this component's Licenses.txt file the
# following copyright and licenses apply:
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
####################################################################################

export top_srcdir=`pwd`
RESULT_DIR="/tmp/l2_test_report"
mkdir -p "$RESULT_DIR"

apt-get update && apt-get install -y iproute2


 cp ./src/hostif/parodusClient/waldb/data-model/data-model-tv.xml /etc/data-model-tv.xml
 cp ./src/hostif/parodusClient/waldb/data-model/data-model-generic.xml /etc/data-model-generic.xml
 cp ./src/hostif/parodusClient/waldb/data-model/data-model-stb.xml /etc/data-model-stb.xml

sed -i '/ModelName/ {n; n; a\
        <default type="factory" value="DOCKER"/>
}' /etc/data-model-stb.xml

dos2unix /etc/data-model-stb.xml

  echo "RDK_PROFILE=STB" > /etc/device.properties

echo "VERSION=99.99.15.07" >> /version.txt
echo "Proto|http" >> /opt/fwdnldstatus.txt
echo "Status|Download In Progress" >> /opt/fwdnldstatus.txt
echo "DnldFile|ELTE11MWR_E037.000.00.8.1s22_DEV.bin" >> /opt/fwdnldstatus.txt
echo "DnldURL|https://dac15cdlserver.ae.ccp.xcal.tv/Images" >> /opt/fwdnldstatus.txt
echo "FwUpdateState|Download complete" >> /opt/fwdnldstatus.txt

cp ./src/integrationtest/conf/mgrlist.conf /etc/

mkdir -p /opt/secure/RFC/
cp ./src/integrationtest/conf/rfcdefaults.ini /tmp/
cp ./src/integrationtest/conf/rfc.properties /etc/
cp ./src/integrationtest/conf/tr181store.ini /opt/secure/RFC/
cp ./src/integrationtest/conf/bootstrap.ini /opt/secure/RFC/
cp ./partners_defaults.json /etc/
touch /opt/secure/RFC/tr181localstore.ini
touch /opt/persistent/tr181localstore.ini
touch /opt/secure/RFC/bootstrap.journal

pid=`pidof tr69hostif`
if [ ! -z "$pid" ]; then
    kill -9 `pidof tr69hostif`
fi

/usr/local/bin/tr69hostif -c /etc/mgrlist.conf -p 10999 -s 11999 | tee /opt/logs/tr69hostIf.log.0 &

pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/bootup_sequence.json test/functional-tests/tests/test_bootup_sequence.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/handlers_communications.json test/functional-tests/tests/test_handlers_communications.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/deviceip.json test/functional-tests/tests/tr69hostif_deviceip.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/webpa.json test/functional-tests/tests/tr69hostif_webpa.py

