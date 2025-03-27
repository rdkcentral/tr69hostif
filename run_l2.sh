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



sed '/<\/model>/d; /<\/dm:document>/d' ./src/hostif/parodusClient/waldb/data-model/data-model-tv.xml > ./src/hostif/parodusClient/waldb/data-model/data-model-merged.xml
sed '/<?xml/,/<model/ d' ./src/hostif/parodusClient/waldb/data-model/data-model-generic.xml >> ./src/hostif/parodusClient/waldb/data-model/data-model-merged.xml

cp ./src/hostif/parodusClient/waldb/data-model/data-model-merged.xml /tmp/data-model.xml





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

/usr/local/bin/tr69hostif -c /etc/mgrlist.conf -d /etc/debug.ini -p 10999 -s 11999 | tee /opt/logs/tr69hostIf.log.0 &

pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/bootup_sequence.json test/functional-tests/tests/test_bootup_sequence.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/handlers_communications.json test/functional-tests/tests/test_handlers_communications.py

