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

Feature: tr69hostif Account Thunder plugin handlers

  Scenario: thunder plugin hotel checkout last reset time get handler
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And when the tr69hostif is initialized successfully
    Then the tr69hostif validation is done for Thunder plugin HotelCheckout LastResetTime get handler

  Scenario: thunder plugin hotel checkout status get handler
    Given When the tr69hostif binary is invoked
    Then the tr69hostif should be running as a daemon
    And when the tr69hostif is initialized successfully
    Then the tr69hostif validation is done for Thunder plugin HotelCheckout Status get handler
