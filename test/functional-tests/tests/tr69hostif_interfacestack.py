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


import pytest

from helper_functions import *


@pytest.mark.run(order=523)
def test_InterfaceStack_NumberOfEntries_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.InterfaceStackNumberOfEntries"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING not in rstdout


@pytest.mark.run(order=524)
def test_InterfaceStack_HigherLayer_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.InterfaceStack.1.HigherLayer"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout


@pytest.mark.run(order=525)
def test_InterfaceStack_LowerLayer_Get_Handler():
    #clear_tr69hostiflogs()
    DATA_ELEMENT_NAME = "Device.InterfaceStack.1.LowerLayer"
    rstdout = rbus_get_data(DATA_ELEMENT_NAME)
    assert RBUS_EXCEPTION_STRING in rstdout
