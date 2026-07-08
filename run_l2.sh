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
echo "DnldFile|TESTIMAGE_DEV.bin" >> /opt/fwdnldstatus.txt
echo "DnldURL|https://mockserver.tv/Images" >> /opt/fwdnldstatus.txt
echo "FwUpdateState|Download complete" >> /opt/fwdnldstatus.txt
echo "uploaded" > /opt/loguploadstatus.txt

cp ./src/integrationtest/conf/mgrlist.conf /etc/

mkdir -p /opt/secure/RFC/
cp ./src/integrationtest/conf/rfcdefaults.ini /tmp/
cp ./src/integrationtest/conf/rfc.properties /etc/
cp ./src/integrationtest/conf/tr181store.ini /opt/secure/RFC/
cp ./src/integrationtest/conf/bootstrap.ini /opt/secure/RFC/
cp ./partners_defaults.json /etc/
cp ./src/integrationtest/conf/rfcVariable.ini /opt/secure/RFC/

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
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/authservice_thunder_plugin.json test/functional-tests/tests/tr69hostif_authservice_thunder_plugin.py
# TODO: These NetworkManager Thunder suites are currently excluded from the L2 run.
# Enable them for regression coverage, or remove/relocate them if this exclusion is intentional.
#pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/networkmanager_ssid_thunder_plugin.json test/functional-tests/tests/tr69hostif_networkmanager_ssid_thunder_plugin.py
#pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/networkmanager_endpoint_thunder_plugin.json test/functional-tests/tests/tr69hostif_networkmanager_endpoint_thunder_plugin.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/account_thunder_plugin.json test/functional-tests/tests/tr69hostif_account_thunder_plugin.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/system_thunder_plugin.json test/functional-tests/tests/tr69hostif_system_thunder_plugin.py

pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/http_server.json test/functional-tests/tests/tr69hostif_http_server.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/rfc_store.json test/functional-tests/tests/tr69hostif_rfc_store.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/rfc_store_params.json test/functional-tests/tests/tr69hostif_rfc_store_params.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/negative_edge.json test/functional-tests/tests/tr69hostif_negative_edge_cases.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/webpa_negative_edge.json test/functional-tests/tests/tr69hostif_webpa_negative_edge_cases.py

pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/ethernet_handlers.json test/functional-tests/tests/tr69hostif_ethernet_handlers.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/devicetime.json test/functional-tests/tests/tr69hostif_devicetime.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/ip.json test/functional-tests/tests/tr69hostif_ip.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/ipremotesupport.json test/functional-tests/tests/tr69hostif_ipremotesupport.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/processor_processstatus.json test/functional-tests/tests/tr69hostif_processor_processstatus.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/std_params.json test/functional-tests/tests/tr69hostif_std_params.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/webpa_rdkdlmgr.json test/functional-tests/tests/tr69hostif_webpa_rdkdlmgr.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/custom.json test/functional-tests/tests/tr69hostif_custom.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/dhcpv4.json test/functional-tests/tests/tr69hostif_dhcpv4.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/moca.json test/functional-tests/tests/tr69hostif_moca.py

pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/device_info.json test/functional-tests/tests/tr69hostif_device_info.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/interfacestack.json test/functional-tests/tests/tr69hostif_interfacestack.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/opsdevicemgmt_logging.json test/functional-tests/tests/tr69hostif_opsdevicemgmt_logging.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/opsdevicemgmt_rpc.json test/functional-tests/tests/tr69hostif_opsdevicemgmt_rpc.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/storageservice.json test/functional-tests/tests/tr69hostif_storageservice.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/bluetooth.json test/functional-tests/tests/tr69hostif_bluetooth.py
pkill -f thunder-mock-server.js
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/thunder_negative_edge.json test/functional-tests/tests/tr69hostif_thunder_negative_edge_cases.py

