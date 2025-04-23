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


Feature: tr69hostif runs as daemon to collect data

  Scenario: tr69hostif runs as daemon
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And when the tr69hostif is initialized successfully
    Then the tr69hostif Profile Initialization is done for Device.DeviceInfo
    Then the tr69hostif validation is done for Device.DeviceInfo get/set handlers
    Then the tr69hostif validation is done for Device.IP get/set handlers
    Then the tr69hostif validation is done for Device.Services get/set handlers
