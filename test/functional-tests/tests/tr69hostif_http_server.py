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

import json
import re
import subprocess
import pytest

from helper_functions import *

HTTP_SERVER_URL = "http://127.0.0.1:11999"


class _CmdResponse:
    def __init__(self, status_code: int, text: str, reason: str = ""):
        self.status_code = status_code
        self.text = text
        self.reason = reason

    def json(self):
        return json.loads(self.text)


def profile_init_run_command(method: str = "GET", payload=None, caller_id: str = None, raw_body: str = None):
    data = raw_body if raw_body is not None else (json.dumps(payload) if payload is not None else None)

    headers = '-H "Content-Type: application/json"'
    headers = f'{headers} -H "CallerID: {caller_id}"' if caller_id else headers

    data_arg = ""
    if data is not None:
        escaped_data = data.replace("'", "'\"'\"'")
        data_arg = f"-d '{escaped_data}'"

    cmd = f"curl -s -i --connect-timeout 2 --max-time 10  -X {method} {headers} {data_arg} {HTTP_SERVER_URL}"
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)

    if result.returncode != 0:
        raise RuntimeError(f"Command failed with error: {result.stderr}")

    return result.stdout


def http_request(method: str, payload=None, caller_id: str = None, raw_body: str = None):
    output = profile_init_run_command(method=method, payload=payload, caller_id=caller_id, raw_body=raw_body)

    status_code = 0
    reason = ""
    normalized = output.replace("\r\n", "\n")

    for line in normalized.split("\n"):
        match = re.match(r"^HTTP/\S+\s+(\d{3})(?:\s+(.*))?$", line)
        if match:
            status_code = int(match.group(1))
            reason = (match.group(2) or "").strip()

    sections = normalized.split("\n\n")
    body = sections[-1].strip() if sections else ""

    return _CmdResponse(status_code=status_code, text=body, reason=reason)


def _assert_json_http_200(resp):
    assert resp.status_code == 200, f"Expected HTTP 200, got {resp.status_code}: {resp.text}"
    body = resp.json()
    assert "statusCode" in body, f"Missing statusCode in response: {body}"
    return body


@pytest.mark.run(order=69)
def test_HTTPServer_GET_Single_Parameter():
    payload = {"names": ["Device.DeviceInfo.SoftwareVersion"]}
    resp = http_request("GET", payload=payload)

    body = _assert_json_http_200(resp)
    assert body["statusCode"] == 0, f"Expected statusCode=0 for single GET: {body}"


@pytest.mark.run(order=70)
def test_HTTPServer_GET_Multiple_Parameters():
    payload = {"names": ["Device.DeviceInfo.Description", "Device.DeviceInfo.X_COMCAST-COM_FirmwareFilename"]}
    resp = http_request("GET", payload=payload)

    body = _assert_json_http_200(resp)
    assert body["statusCode"] == 0, f"Expected statusCode=0 for multi-GET: {body}"


@pytest.mark.run(order=71)
def test_HTTPServer_GET_Wildcard_DeviceInfo():
    payload = {"names": ["Device.DeviceInfo."]}
    resp = http_request("GET", payload=payload)

    body = _assert_json_http_200(resp)
    assert body["statusCode"] in (0, 22), f"Expected statusCode in (0, 22) for wildcard GET: {body}"
    if body["statusCode"] == 22:
        assert "Parameter value field is not available" in body.get("message", ""), (
            f"Expected partial-data message for wildcard GET, got: {body}"
        )


@pytest.mark.run(order=72)
def test_HTTPServer_SET_With_CallerID():
    payload = {
        "parameters": [
            {
                "name": "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.XconfUrl",
                "dataType": 0,
                "value": "https://mockurl/featurecontrol/getSettings",
            }
        ]
    }
    resp = http_request("POST", payload=payload, caller_id="webpa")

    body = _assert_json_http_200(resp)
    assert body["statusCode"] == 0, f"Expected statusCode=0 for POST with CallerID: {body}"


@pytest.mark.run(order=73)
def test_HTTPServer_SET_Without_CallerID_Not_Allowed():
    payload = {
        "parameters": [
            {
                "name": "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.XconfUrl",
                "dataType": 0,
                "value": "https://mockurl/featurecontrol/getSettings",
            }
        ]
    }
    resp = http_request("POST", payload=payload)
    CALLER_ID_MSG = "Exiting.. POST operation not allowed with unknown CallerID"   

    assert resp.status_code == 500, f"Expected HTTP 500 for POST without CallerID, got {resp.status_code}"
    assert (
        "POST Not Allowed without CallerID" in (resp.reason or "")
        or "POST Not Allowed without CallerID" in (resp.text or "")
    ), f"Expected CallerID rejection reason, got reason='{resp.reason}' body='{resp.text}'"
    assert CALLER_ID_MSG in grep_tr69hostiflogs(CALLER_ID_MSG)


@pytest.mark.run(order=74)
def test_HTTPServer_GET_Malformed_JSON_Body():
    resp = http_request("GET", raw_body="{bad json}")
    PARSE_ERROR_MSG = "Exiting.. Failed to parse JSON Message"

    assert resp.status_code == 400, f"Expected HTTP 400 for malformed JSON, got {resp.status_code}"
    assert (
        "Bad Request" in (resp.reason or "")
        or "Bad Request" in (resp.text or "")
    ), f"Expected 'Bad Request' message, got reason='{resp.reason}' body='{resp.text}'"
    assert PARSE_ERROR_MSG in grep_tr69hostiflogs(PARSE_ERROR_MSG)



@pytest.mark.run(order=75)
def test_HTTPServer_GET_Unknown_Parameter_NonZero_StatusCode():
    payload = {"names": ["Device.DoesNotExist.Param"]}
    resp = http_request("GET", payload=payload)
    INVALID_PARAM_MSG = "Invalid parameter name Device.DoesNotExist.Param: doesn't exist in data-model"

    body = _assert_json_http_200(resp)
    assert body["statusCode"] != 0, f"Expected non-zero statusCode for unknown parameter: {body}"
    assert "Invalid parameter name" in body.get("message", ""), (
        f"Expected 'Invalid parameter name' message for unknown parameter, got: {body}"
    )
    assert INVALID_PARAM_MSG in grep_tr69hostiflogs(INVALID_PARAM_MSG)


@pytest.mark.run(order=76)
def test_HTTPServer_GET_Empty_Body_Bad_Request():
    resp = http_request("GET")
    EMPTY_BODY_MSG =  "Exiting.. Failed due to no message data."

    assert resp.status_code == 400, f"Expected HTTP 400 for empty request body, got {resp.status_code}"
    assert (
        "No request data." in (resp.reason or "")
        or "No request data." in (resp.text or "")
    ), f"Expected 'No request data.' message, got reason='{resp.reason}' body='{resp.text}'"
    assert EMPTY_BODY_MSG in grep_tr69hostiflogs(EMPTY_BODY_MSG) 
    
