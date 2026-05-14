#!/bin/sh

####################################################################################
# If not stated otherwise in this file or this component's LICENSE file the
# following copyright and licenses apply:
#
# Copyright 2023 RDK Management
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

# Script to generate test gap analysis for automatics tests
# Usage: ./automatics_test_gap.sh

echo "Generating test gap analysis for automatics tests..."
# Create virtual environment
python3 -m venv venv
# Activate virtual environment
source venv/bin/activate
# Install dependencies
pip install -r requirements.txt
# Format the .xls file (merge cells, add serial numbers)
python3 format_xls.py
# Convert formatted .xls to grouped markdown
python3 xls_to_markdown.py
# Generate test gap analysis report (compare against docs/features)
python3 generate_test_gap_report.py

echo "Test gap analysis generated successfully!"
