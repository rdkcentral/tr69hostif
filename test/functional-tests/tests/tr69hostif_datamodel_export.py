####################################################################################
# If not stated otherwise in this file or this component's Licenses file the
# following copyright and licenses apply:
#
# Copyright 2026 RDK Management
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

import csv
from pathlib import Path
import shutil
import stat
from time import monotonic, sleep

import pytest

from helper_functions import *

GET_DATA_MODEL_LIST_PARAM = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.GetDataModelList"


@pytest.fixture
def data_model_export_path(tmp_path):
    export_path = Path("/opt/secure/RFC/supported_datamodel_list")
    backup_path = tmp_path / "datamodel.list"
    if export_path.exists():
        shutil.copy2(export_path, backup_path)
        export_path.unlink()
    try:
        yield export_path
    finally:
        try:
            result = rbus_set_data(GET_DATA_MODEL_LIST_PARAM, "boolean", "false")
            assert RBUS_SUCCESS_STRING in result, f"Failed to reset export trigger: {result}"
        finally:
            if backup_path.exists():
                shutil.copy2(backup_path, export_path)
            elif export_path.exists():
                export_path.unlink()


@pytest.mark.run(order=393)
@pytest.mark.parametrize("existing_export", [False, True], ids=["absent", "existing"])
def test_DeviceInfo_GetDataModelList_False_Does_Not_Export(
        data_model_export_path, existing_export):
    export_path = data_model_export_path
    original_content = b"existing data model export\n"
    if existing_export:
        export_path.write_bytes(original_content)
        original_stat = export_path.stat()

    result = rbus_set_data(GET_DATA_MODEL_LIST_PARAM, "boolean", "false")
    assert RBUS_SUCCESS_STRING in result, f"SET false failed: {result}"
    assert rbus_get_value(GET_DATA_MODEL_LIST_PARAM).lower() == "false"

    deadline = monotonic() + 1
    while True:
        if existing_export:
            assert export_path.read_bytes() == original_content, "SET false changed the export"
            current_stat = export_path.stat()
            assert (current_stat.st_ino, current_stat.st_mtime_ns) == (
                original_stat.st_ino, original_stat.st_mtime_ns
            ), "SET false replaced or rewrote the export"
        else:
            assert not export_path.exists(), "SET false created an export"
        if monotonic() >= deadline:
            break
        sleep(0.05)


@pytest.mark.run(order=394)
def test_DeviceInfo_GetDataModelList_True_Exports_Loaded_Model(data_model_export_path):
    export_path = data_model_export_path
    for attempt in range(2):
        result = rbus_set_data(GET_DATA_MODEL_LIST_PARAM, "boolean", "true")
        assert RBUS_SUCCESS_STRING in result, f"SET true failed on attempt {attempt + 1}: {result}"
        assert rbus_get_value(GET_DATA_MODEL_LIST_PARAM).lower() == "true"

        deadline = monotonic() + 10
        while not export_path.exists() and monotonic() < deadline:
            sleep(0.05)
        assert export_path.exists(), "SET true did not generate /opt/secure/RFC/supported_datamodel_list within 10 seconds"

        with export_path.open(newline="", encoding="utf-8") as export_file:
            rows = list(csv.reader(
                export_file, delimiter="|", escapechar="\\", quoting=csv.QUOTE_NONE, strict=True
            ))
        assert rows, "Export is empty"
        assert rows[0] == ["name", "dataType", "defaultValue", "access", "description"]
        parameters = rows[1:]
        assert parameters, "Export contains no parameters"
        assert all(len(row) == 5 for row in parameters), "Export contains a row without five fields"
        names = [row[0] for row in parameters]
        assert names == sorted(names), "Exported parameter names are not sorted"
        assert any("{i}" in name for name in names), "Export is missing table templates"
        assert [
            GET_DATA_MODEL_LIST_PARAM, "boolean", "false", "readWrite",
            "Generate an export of the loaded TR-181 data model."
        ] in parameters, "Export is missing RFC trigger metadata"
        assert stat.S_IMODE(export_path.stat().st_mode) == 0o644
        export_path.unlink()
