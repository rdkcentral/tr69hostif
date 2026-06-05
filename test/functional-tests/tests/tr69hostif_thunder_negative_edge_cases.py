####################################################################################
# If not stated otherwise in this file or this component's Licenses file the
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


import pytest
import json
import time
import threading
from http.server import BaseHTTPRequestHandler, HTTPServer

from helper_functions import *

class _ThunderEdgeState:
    def __init__(self):
        self.lock = threading.Lock()
        self.mode = "normal"
        self.delay_seconds = 12

    def set_mode(self, mode: str):
        with self.lock:
            self.mode = mode

    def get_mode(self) -> str:
        with self.lock:
            return self.mode


def _default_thunder_result(method: str):
    if method == "org.rdk.NetworkManager.GetConnectedSSID":
        return {
            "bssid": "AA:BB:CC:DD:EE:FF",
            "ssid": "L2_THUNDER_SSID",
            "strength": 67,
            "security": 4,
        }

    if method == "org.rdk.AuthService.getExperience":
        return {"experience": "TESTOS"}

    return {"success": True}


@pytest.fixture(scope="module")
def thunder_edge_server():
    state = _ThunderEdgeState()

    class ThunderEdgeHandler(BaseHTTPRequestHandler):
        def log_message(self, fmt, *args):
            return

        def do_POST(self):
            if self.path != "/jsonrpc":
                self.send_response(404)
                self.end_headers()
                return

            content_len = int(self.headers.get("Content-Length", "0"))
            raw_body = self.rfile.read(content_len).decode("utf-8") if content_len > 0 else "{}"

            try:
                request_json = json.loads(raw_body)
            except json.JSONDecodeError:
                request_json = {}

            mode = state.get_mode()
            if mode == "timeout":
                time.sleep(state.delay_seconds)
                return

            if mode == "kill_midrequest":
                # Simulate server crash by writing a partial payload and closing.
                partial = (
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: application/json\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "{\"incomplete"
                ).encode("utf-8")
                self.request.sendall(partial)
                self.close_connection = True
                return

            if mode == "empty":
                # Return bare empty object, no jsonrpc wrapper
                data = b"{}"
            else:
                response = {
                    "jsonrpc": "2.0",
                    "id": request_json.get("id", "3"),
                }
                method = request_json.get("method", "")
                response["result"] = _default_thunder_result(method)
                data = json.dumps(response).encode("utf-8")
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.send_header("Content-Length", str(len(data)))
            self.end_headers()
            self.wfile.write(data)

    try:
        server = HTTPServer(("127.0.0.1", 9998), ThunderEdgeHandler)
    except OSError as err:
        pytest.skip(f"Unable to bind Thunder edge mock on 127.0.0.1:9998: {err}")

    server_thread = threading.Thread(target=server.serve_forever, daemon=True)
    server_thread.start()
    time.sleep(0.2)

    yield state

    server.shutdown()
    server.server_close()
    server_thread.join(timeout=2)

@pytest.mark.run(order=49)
def test_ThunderPlugin_EmptyResponse_Returns_NOK(thunder_edge_server):
    """
    L2: Thunder empty response.
    A mock server returns HTTP 200 with body '{}' (valid JSON but no
    'result' field).  parseThunderResultObject() must reject this,
    tr69hostif must not crash, and the rbus GET must report failure.
    The log must contain the 'no result in the output' error string.
    """
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_Experience"
    HTTP_CODE_MSG = "getJsonRPCData: curl response : 0 http response code: 200"
    EMPTY_RESPONSE_MSG = "invokeThunderPluginMethod: curl response string = {}"
    PARSE_ERROR_MSG = "json parse error, no \"result\" in the output from Thunder plugin"
    ERROR_MSG = "failed to fetch experience from AuthService"

    thunder_edge_server.set_mode("empty")
    try:
        rstdout = rbus_get_data(DATA_ELEMENT_NAME)
        assert RBUS_EXCEPTION_STRING in rstdout
        assert HTTP_CODE_MSG in grep_tr69hostiflogs(HTTP_CODE_MSG)
        assert EMPTY_RESPONSE_MSG in grep_tr69hostiflogs(EMPTY_RESPONSE_MSG)
        assert PARSE_ERROR_MSG in grep_tr69hostiflogs(PARSE_ERROR_MSG)
        assert ERROR_MSG in grep_tr69hostiflogs(ERROR_MSG)
    finally:
        thunder_edge_server.set_mode("normal")

@pytest.mark.run(order=48)
def test_ThunderPlugin_Timeout_Returns_NOK(thunder_edge_server):
    """
    L2: Thunder timeout simulation.
    A mock server binds port 9998 and silently holds connections without
    responding.  tr69hostif uses curl with CURLOPT_CONNECTTIMEOUT=5 s and
    CURLOPT_TIMEOUT=10 s; a timeout produces curl error code 28
    (CURLE_OPERATION_TIMEDOUT) and http_code=0.
    Expected: rbus GET fails with exception string, log contains getJsonRPCData error.
    """
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID"
    HTTP_CODE_MSG = "getJsonRPCData: curl response : 28 http response code: 0"
    JSONRPC_ERROR_MSG = "getJsonRPCData failed"
    ERROR_MSG = "failed to fetch serviceAccountId"

    thunder_edge_server.set_mode("timeout")
    try:
        rstdout = rbus_get_data(DATA_ELEMENT_NAME)
        assert RBUS_EXCEPTION_STRING in rstdout
        assert HTTP_CODE_MSG in grep_tr69hostiflogs(HTTP_CODE_MSG)
        assert JSONRPC_ERROR_MSG in grep_tr69hostiflogs(JSONRPC_ERROR_MSG)
        assert ERROR_MSG in grep_tr69hostiflogs(ERROR_MSG)
    finally:
        thunder_edge_server.set_mode("normal")


@pytest.mark.run(order=50)
def test_ThunderPlugin_KillMidRequest_Returns_NOK(thunder_edge_server):
    """
    L2: Thunder server killed mid-request.
    A mock server accepts the connection, sends an incomplete HTTP response
    (simulating a server crash/kill), and closes abruptly. This results in
    incomplete JSON that fails to parse.
    Expected: rbus GET fails with exception string and log contains parse error.
    """
    DATA_ELEMENT_NAME = "Device.DeviceInfo.X_RDKCENTRAL-COM_Experience"
    CURL_RESPONSE_MSG = "invokeThunderPluginMethod: curl response string = {\"incomplete"
    PARSE_ERROR_MSG = "Failed to parse Thunder response JSON near: [incomplete]"

    thunder_edge_server.set_mode("kill_midrequest")
    try:
        rstdout = rbus_get_data(DATA_ELEMENT_NAME)
        assert RBUS_EXCEPTION_STRING in rstdout
        assert grep_tr69hostiflogs(CURL_RESPONSE_MSG), (
            f"Expected log entry '{CURL_RESPONSE_MSG}' in {LOG_FILE} but none found"
        )
        assert grep_tr69hostiflogs(PARSE_ERROR_MSG), (
            f"Expected log entry '{PARSE_ERROR_MSG}' in {LOG_FILE} but none found"
        )
    finally:
        thunder_edge_server.set_mode("normal")
