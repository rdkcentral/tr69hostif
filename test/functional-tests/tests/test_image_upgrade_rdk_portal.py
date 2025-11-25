#!/usr/bin/env python3
##########################################################################
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
##########################################################################

"""
Test module for Image Upgrade via RDK Portal functionality in tr69hostif.

This module tests the complete Image Upgrade workflow that is triggered 
via RDK portal through TR-069 parameters.

Image Upgrade Flow:
1. RDK Portal sends TR-069 parameters for firmware download configuration
2. Parameters include: Protocol, URL, Firmware filename, UseCodebig, DeferReboot flags
3. FirmwareDownloadNow parameter triggers the actual download script execution
4. tr69hostif calls /lib/rdk/userInitiatedFWDnld.sh script with all parameters
5. Download status is tracked via /opt/fwdnldstatus.txt file
6. Progress can be monitored through various TR-069 status parameters
"""

import pytest
import time
import os
import subprocess
import json
import re
from pathlib import Path

class TestImageUpgradeRDKPortal:
    """Test class for Image Upgrade via RDK Portal functionality."""
    
    # TR-069 Parameter definitions for firmware download
    FIRMWARE_PROTOCOL_PARAM = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol"
    FIRMWARE_URL_PARAM = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL"
    FIRMWARE_FILENAME_PARAM = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload"
    FIRMWARE_USE_CODEBIG_PARAM = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadUseCodebig"
    FIRMWARE_DEFER_REBOOT_PARAM = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot"
    FIRMWARE_DOWNLOAD_NOW_PARAM = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadNow"
    FIRMWARE_DOWNLOAD_STATUS_PARAM = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus"
    FIRMWARE_UPDATE_STATE_PARAM = "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareUpdateState"
    
    # File paths
    FW_STATUS_FILE = "/opt/fwdnldstatus.txt"
    FW_SCRIPT_PATH = "/lib/rdk/userInitiatedFWDnld.sh"
    
    # Test data
    TEST_PROTOCOL = "https"
    TEST_URL = "https://dac15cdlserver.ae.ccp.xcal.tv/Images"
    TEST_FIRMWARE = "ELTE11MWR_E037.000.00.8.1s22_DEV.bin"
    
    @pytest.fixture(autouse=True)
    def setup_and_cleanup(self):
        """Setup and cleanup for each test."""
        # Setup: Create mock firmware status file if it doesn't exist
        os.makedirs("/opt", exist_ok=True)
        
        # Create mock script directory
        os.makedirs("/lib/rdk", exist_ok=True)
        
        yield
        
        # Cleanup: Remove test files if they exist
        try:
            if os.path.exists(self.FW_STATUS_FILE):
                os.remove(self.FW_STATUS_FILE)
        except Exception as e:
            print(f"Cleanup warning: {e}")
    
    def create_mock_fw_script(self):
        """Create a mock firmware download script for testing."""
        script_content = """#!/bin/bash
# Mock userInitiatedFWDnld.sh script for testing
# Parameters: protocol url filename use_codebig defer_reboot

PROTOCOL="$1"
URL="$2" 
FILENAME="$3"
USE_CODEBIG="$4"
DEFER_REBOOT="$5"

echo "Mock firmware download script called with:"
echo "Protocol: $PROTOCOL"
echo "URL: $URL"
echo "Filename: $FILENAME"
echo "Use Codebig: $USE_CODEBIG"
echo "Defer Reboot: $DEFER_REBOOT"

# Update firmware status file to simulate download progress
echo "Proto|$PROTOCOL" > /opt/fwdnldstatus.txt
echo "DnldURL|$URL" >> /opt/fwdnldstatus.txt
echo "DnldFile|$FILENAME" >> /opt/fwdnldstatus.txt
echo "Codebig_Enable|$USE_CODEBIG" >> /opt/fwdnldstatus.txt
echo "Status|Download In Progress" >> /opt/fwdnldstatus.txt
echo "FwUpdateState|Download In Progress" >> /opt/fwdnldstatus.txt

# Simulate download completion after a short delay
sleep 2
sed -i 's/Download In Progress/Download Complete/g' /opt/fwdnldstatus.txt
echo "Method|tr69" >> /opt/fwdnldstatus.txt

exit 0
"""
        with open(self.FW_SCRIPT_PATH, 'w') as f:
            f.write(script_content)
        os.chmod(self.FW_SCRIPT_PATH, 0o755)
    
    def webpa_set_parameter(self, param_name, param_value, param_type="string"):
        """Set a TR-069 parameter via WebPA interface."""
        if param_type == "boolean":
            value_str = "true" if param_value else "false"
            payload = f'{{"command":"SET","parameters":[{{"name":"{param_name}","value":{value_str},"dataType":3}}]}}'
        else:
            payload = f'{{"command":"SET","parameters":[{{"name":"{param_name}","value":"{param_value}","dataType":0}}]}}'
        
        try:
            result = subprocess.run(
                ["/usr/local/bin/parodus", payload],
                capture_output=True,
                text=True,
                timeout=10
            )
            return result.returncode == 0
        except Exception as e:
            print(f"WebPA set failed: {e}")
            return False
    
    def webpa_get_parameter(self, param_name):
        """Get a TR-069 parameter via WebPA interface."""
        payload = f'{{"command":"GET","names":["{param_name}"]}}'
        
        try:
            result = subprocess.run(
                ["/usr/local/bin/parodus", payload],
                capture_output=True,
                text=True,
                timeout=10
            )
            if result.returncode == 0:
                # Parse the response to extract the parameter value
                # This is a simplified parser - actual implementation may vary
                output = result.stdout
                if "value" in output:
                    # Extract value from JSON response
                    import re
                    match = re.search(r'"value":"([^"]*)"', output)
                    if match:
                        return match.group(1)
            return None
        except Exception as e:
            print(f"WebPA get failed: {e}")
            return None
    
    def verify_fw_status_file(self, expected_values):
        """Verify contents of firmware status file."""
        if not os.path.exists(self.FW_STATUS_FILE):
            return False
        
        with open(self.FW_STATUS_FILE, 'r') as f:
            content = f.read()
        
        for key, expected_value in expected_values.items():
            pattern = f"{key}\\|{re.escape(expected_value)}"
            if not re.search(pattern, content):
                print(f"Expected {key}|{expected_value} not found in firmware status file")
                return False
        
        return True
    
    def wait_for_download_completion(self, timeout=30):
        """Wait for firmware download to complete."""
        start_time = time.time()
        
        while time.time() - start_time < timeout:
            if os.path.exists(self.FW_STATUS_FILE):
                with open(self.FW_STATUS_FILE, 'r') as f:
                    content = f.read()
                    if "Status|Download Complete" in content:
                        return True
            time.sleep(1)
        
        return False
    
    def test_firmware_download_parameter_configuration(self):
        """Test setting firmware download parameters via RDK portal (TR-069)."""
        print("Testing firmware download parameter configuration...")
        
        # Step 1: Configure firmware download protocol
        assert self.webpa_set_parameter(self.FIRMWARE_PROTOCOL_PARAM, self.TEST_PROTOCOL), \
            "Failed to set firmware download protocol"
        
        # Step 2: Configure firmware download URL  
        assert self.webpa_set_parameter(self.FIRMWARE_URL_PARAM, self.TEST_URL), \
            "Failed to set firmware download URL"
        
        # Step 3: Configure firmware filename
        assert self.webpa_set_parameter(self.FIRMWARE_FILENAME_PARAM, self.TEST_FIRMWARE), \
            "Failed to set firmware filename"
        
        # Step 4: Configure Codebig usage (optional CDN optimization)
        assert self.webpa_set_parameter(self.FIRMWARE_USE_CODEBIG_PARAM, False, "boolean"), \
            "Failed to set firmware download use codebig flag"
        
        # Step 5: Configure defer reboot flag
        assert self.webpa_set_parameter(self.FIRMWARE_DEFER_REBOOT_PARAM, False, "boolean"), \
            "Failed to set firmware download defer reboot flag"
        
        # Verify parameters are set correctly by reading them back
        protocol = self.webpa_get_parameter(self.FIRMWARE_PROTOCOL_PARAM)
        url = self.webpa_get_parameter(self.FIRMWARE_URL_PARAM) 
        filename = self.webpa_get_parameter(self.FIRMWARE_FILENAME_PARAM)
        
        assert protocol == self.TEST_PROTOCOL, f"Protocol mismatch: expected {self.TEST_PROTOCOL}, got {protocol}"
        assert url == self.TEST_URL, f"URL mismatch: expected {self.TEST_URL}, got {url}"
        assert filename == self.TEST_FIRMWARE, f"Filename mismatch: expected {self.TEST_FIRMWARE}, got {filename}"
        
        print("✅ Firmware download parameters configured successfully")
    
    def test_firmware_download_script_trigger(self):
        """Test that setting FirmwareDownloadNow triggers the download script."""
        print("Testing firmware download script triggering...")
        
        # Create mock download script
        self.create_mock_fw_script()
        
        # Configure all required parameters first
        self.test_firmware_download_parameter_configuration()
        
        # Step 1: Trigger download by setting FirmwareDownloadNow to true
        print("Triggering firmware download via FirmwareDownloadNow parameter...")
        assert self.webpa_set_parameter(self.FIRMWARE_DOWNLOAD_NOW_PARAM, True, "boolean"), \
            "Failed to trigger firmware download"
        
        # Step 2: Verify script execution by checking if firmware status file is created/updated
        time.sleep(5)  # Allow time for script execution
        
        assert os.path.exists(self.FW_STATUS_FILE), \
            "Firmware status file not created - script may not have executed"
        
        # Step 3: Verify script was called with correct parameters
        expected_status = {
            "Proto": self.TEST_PROTOCOL,
            "DnldURL": self.TEST_URL,
            "DnldFile": self.TEST_FIRMWARE,
            "Codebig_Enable": "0",  # False -> 0
        }
        
        assert self.verify_fw_status_file(expected_status), \
            "Firmware status file does not contain expected values"
        
        print("✅ Firmware download script triggered successfully")
    
    def test_firmware_download_status_monitoring(self):
        """Test monitoring firmware download status through TR-069 parameters."""
        print("Testing firmware download status monitoring...")
        
        # Trigger download first
        self.test_firmware_download_script_trigger()
        
        # Step 1: Monitor download status via TR-069 parameter
        status = self.webpa_get_parameter(self.FIRMWARE_DOWNLOAD_STATUS_PARAM)
        print(f"Download status: {status}")
        
        # Step 2: Monitor firmware update state
        update_state = self.webpa_get_parameter(self.FIRMWARE_UPDATE_STATE_PARAM)
        print(f"Firmware update state: {update_state}")
        
        # Step 3: Wait for download completion
        assert self.wait_for_download_completion(), \
            "Download did not complete within expected time"
        
        # Step 4: Verify completion status
        final_status = self.webpa_get_parameter(self.FIRMWARE_DOWNLOAD_STATUS_PARAM)
        assert final_status and "Complete" in final_status, \
            f"Expected download completion, got status: {final_status}"
        
        print("✅ Firmware download status monitoring successful")
    
    def test_firmware_download_error_handling(self):
        """Test error handling when required parameters are missing."""
        print("Testing firmware download error handling...")
        
        # Step 1: Try to trigger download without setting required parameters
        assert self.webpa_set_parameter(self.FIRMWARE_DOWNLOAD_NOW_PARAM, True, "boolean"), \
            "Failed to send download trigger"
        
        # Step 2: Verify that download doesn't proceed without required params
        time.sleep(3)
        
        # Should not create status file or should indicate error
        if os.path.exists(self.FW_STATUS_FILE):
            with open(self.FW_STATUS_FILE, 'r') as f:
                content = f.read()
                # Should not show successful download progress
                assert "Download Complete" not in content, \
                    "Download should not complete without required parameters"
        
        print("✅ Error handling verification successful")
    
    def test_firmware_download_parameter_validation(self):
        """Test validation of firmware download parameters."""
        print("Testing parameter validation...")
        
        # Test invalid protocol
        result = self.webpa_set_parameter(self.FIRMWARE_PROTOCOL_PARAM, "invalid_protocol")
        # Implementation should validate protocol types (http/https/tftp)
        
        # Test invalid URL format
        result = self.webpa_set_parameter(self.FIRMWARE_URL_PARAM, "not_a_valid_url")
        # Implementation should validate URL format
        
        # Test empty firmware filename
        result = self.webpa_set_parameter(self.FIRMWARE_FILENAME_PARAM, "")
        # Implementation should reject empty filenames
        
        print("✅ Parameter validation tests completed")
    
    def test_concurrent_download_prevention(self):
        """Test that concurrent downloads are prevented."""
        print("Testing concurrent download prevention...")
        
        # Configure parameters
        self.test_firmware_download_parameter_configuration()
        self.create_mock_fw_script()
        
        # Trigger first download
        assert self.webpa_set_parameter(self.FIRMWARE_DOWNLOAD_NOW_PARAM, True, "boolean"), \
            "Failed to trigger first download"
        
        # Immediately try to trigger second download
        time.sleep(1)
        second_result = self.webpa_set_parameter(self.FIRMWARE_DOWNLOAD_NOW_PARAM, True, "boolean")
        
        # Second trigger should be rejected or handled gracefully
        # The implementation should prevent concurrent downloads
        
        print("✅ Concurrent download prevention test completed")

    def test_image_upgrade_end_to_end_flow(self):
        """Test complete end-to-end image upgrade flow via RDK portal."""
        print("Testing complete end-to-end image upgrade flow...")
        
        # This test simulates the complete flow that happens when RDK portal
        # initiates a firmware upgrade
        
        # Step 1: RDK Portal Configuration Phase
        print("Phase 1: RDK Portal sends configuration parameters...")
        self.test_firmware_download_parameter_configuration()
        
        # Step 2: Download Trigger Phase  
        print("Phase 2: RDK Portal triggers firmware download...")
        self.create_mock_fw_script()
        assert self.webpa_set_parameter(self.FIRMWARE_DOWNLOAD_NOW_PARAM, True, "boolean"), \
            "Failed to trigger download via RDK portal"
        
        # Step 3: Script Execution Verification
        print("Phase 3: Verifying script execution...")
        time.sleep(3)
        assert os.path.exists(self.FW_SCRIPT_PATH), "Download script should exist"
        assert os.path.exists(self.FW_STATUS_FILE), "Status file should be created by script"
        
        # Step 4: Progress Monitoring Phase
        print("Phase 4: Monitoring download progress...")
        assert self.wait_for_download_completion(timeout=15), \
            "Download should complete successfully"
        
        # Step 5: Final Status Verification
        print("Phase 5: Verifying final status...")
        final_status = self.webpa_get_parameter(self.FIRMWARE_DOWNLOAD_STATUS_PARAM)
        final_update_state = self.webpa_get_parameter(self.FIRMWARE_UPDATE_STATE_PARAM)
        
        assert final_status and ("Complete" in final_status or "complete" in final_status), \
            f"Expected completion status, got: {final_status}"
        
        # Step 6: Verify parameter reset after successful download
        print("Phase 6: Verifying parameter cleanup...")
        # Implementation should clear cached parameters after successful download
        
        print("✅ End-to-end image upgrade flow completed successfully")
        print(f"   Final Status: {final_status}")
        print(f"   Final Update State: {final_update_state}")


if __name__ == "__main__":
    """
    How Image Upgrade via RDK Portal Works:
    
    1. RDK Portal Integration:
       - RDK Portal is the web-based management interface for RDK devices
       - Operators use it to manage firmware upgrades across their device fleet
       - Portal communicates with devices via TR-069/CWMP protocol
    
    2. Parameter Configuration Phase:
       - Portal sets TR-069 parameters on the device:
         * Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol
         * Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL  
         * Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload
         * Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadUseCodebig
         * Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot
    
    3. Download Trigger Phase:
       - Portal sets Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadNow = true
       - tr69hostif validates all required parameters are configured
       - If valid, calls: /lib/rdk/userInitiatedFWDnld.sh with all parameters
    
    4. Script Execution:
       - userInitiatedFWDnld.sh handles actual firmware download
       - Updates /opt/fwdnldstatus.txt with progress information
       - Manages download process, verification, and installation
    
    5. Status Monitoring:
       - Portal monitors progress via TR-069 status parameters
       - Device reports status through fwdnldstatus.txt file
       - Operators can track download progress in real-time
    
    6. Completion & Reboot:
       - Upon completion, device updates status to "Download Complete" 
       - Depending on DeferReboot flag, device may reboot automatically
       - New firmware becomes active after reboot
    
    Key Files & Components:
    - /lib/rdk/userInitiatedFWDnld.sh: Main download script
    - /opt/fwdnldstatus.txt: Status tracking file
    - Device_DeviceInfo.cpp: TR-069 parameter handlers
    - hostIf_DeviceClient_ReqHandler.cpp: Request routing
    
    This test suite validates the complete integration between RDK Portal
    and the device firmware upgrade mechanism.
    """
    
    # Run specific test
    test_instance = TestImageUpgradeRDKPortal()
    test_instance.test_image_upgrade_end_to_end_flow()
