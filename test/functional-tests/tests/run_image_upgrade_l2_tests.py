#!/usr/bin/env python3
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

"""
Simple test runner for Image Upgrade RDK Portal tests.
Follows the existing tr69hostif testing pattern - no Makefile needed.
"""

import subprocess
import sys
import os

def run_image_upgrade_tests():
    """Run Image Upgrade tests following existing pattern."""
    
    print("🔍 Running Image Upgrade via RDK Portal Tests")
    print("=" * 50)
    
    # Test files to run (following existing pattern in run_l2.sh)
    test_files = [
        "test/functional-tests/tests/validate_image_upgrade_trigger.py",
        "test/functional-tests/tests/test_image_upgrade_rdk_portal.py"
    ]
    
    results = []
    result_dir = "/tmp/l2_test_report"
    
    # Create result directory
    os.makedirs(result_dir, exist_ok=True)
    
    for test_file in test_files:
        test_name = os.path.basename(test_file).replace('.py', '')
        json_report = f"{result_dir}/{test_name}.json"
        
        print(f"\n🧪 Running: {test_file}")
        
        if test_file.endswith("validate_image_upgrade_trigger.py"):
            # Run validation script directly (not a pytest)
            try:
                result = subprocess.run(
                    ["python3", test_file], 
                    capture_output=True, 
                    text=True,
                    timeout=60
                )
                print(result.stdout)
                if result.stderr:
                    print("Errors:", result.stderr)
                results.append((test_name, result.returncode == 0))
            except Exception as e:
                print(f"❌ Failed to run {test_file}: {e}")
                results.append((test_name, False))
        else:
            # Run pytest tests (following existing pattern)
            try:
                cmd = [
                    "pytest", 
                    "--json-report", 
                    "--json-report-summary",
                    f"--json-report-file={json_report}",
                    test_file
                ]
                result = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
                print(result.stdout)
                if result.stderr:
                    print("Errors:", result.stderr)
                results.append((test_name, result.returncode == 0))
            except Exception as e:
                print(f"❌ Failed to run {test_file}: {e}")
                results.append((test_name, False))
    
    # Summary
    print(f"\n📋 Test Results Summary")
    print("=" * 30)
    
    passed = 0
    for test_name, success in results:
        status = "✅ PASS" if success else "❌ FAIL"
        print(f"{test_name}: {status}")
        if success:
            passed += 1
    
    print(f"\nOverall: {passed}/{len(results)} tests passed")
    print(f"Results saved to: {result_dir}/")
    
    return passed == len(results)

if __name__ == "__main__":
    """
    Usage (from project root):
    python3 test/functional-tests/tests/run_image_upgrade_l2_tests.py
    
    Or add to run_l2.sh:
    python3 test/functional-tests/tests/run_image_upgrade_l2_tests.py
    """
    success = run_image_upgrade_tests()
    sys.exit(0 if success else 1)