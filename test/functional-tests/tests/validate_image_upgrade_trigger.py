#!/usr/bin/env python3

"""
Image Upgrade Script Trigger Validation

This script validates that the Image Upgrade via RDK portal functionality
is properly implemented and the script triggering mechanism works correctly.
"""

import os
import sys
import subprocess
import time
import json

class ImageUpgradeValidator:
    """Validator for Image Upgrade script triggering mechanism."""
    
    def __init__(self):
        self.fw_status_file = "/opt/fwdnldstatus.txt"
        self.fw_script_path = "/lib/rdk/userInitiatedFWDnld.sh"
        self.tr69hostif_binary = "/usr/local/bin/tr69hostif"
        
    def check_tr69hostif_running(self):
        """Check if tr69hostif process is running."""
        try:
            result = subprocess.run(["pgrep", "tr69hostif"], capture_output=True, text=True)
            return result.returncode == 0
        except Exception:
            return False
    
    def check_script_exists(self):
        """Check if firmware download script exists."""
        return os.path.exists(self.fw_script_path)
    
    def create_mock_script(self):
        """Create a mock firmware download script for testing."""
        os.makedirs("/lib/rdk", exist_ok=True)
        
        script_content = '''#!/bin/bash
# Mock userInitiatedFWDnld.sh for validation
echo "Script triggered with parameters: $*" > /tmp/fw_script_test.log
echo "Protocol: $1" >> /tmp/fw_script_test.log
echo "URL: $2" >> /tmp/fw_script_test.log  
echo "Filename: $3" >> /tmp/fw_script_test.log
echo "UseCodebig: $4" >> /tmp/fw_script_test.log
echo "DeferReboot: $5" >> /tmp/fw_script_test.log

# Create status file to simulate script execution
mkdir -p /opt
echo "Proto|$1" > /opt/fwdnldstatus.txt
echo "DnldURL|$2" >> /opt/fwdnldstatus.txt
echo "DnldFile|$3" >> /opt/fwdnldstatus.txt
echo "Codebig_Enable|$4" >> /opt/fwdnldstatus.txt
echo "Status|Download In Progress" >> /opt/fwdnldstatus.txt
echo "Method|tr69" >> /opt/fwdnldstatus.txt

sleep 2
sed -i 's/Download In Progress/Download Complete/g' /opt/fwdnldstatus.txt
'''
        
        with open(self.fw_script_path, 'w') as f:
            f.write(script_content)
        os.chmod(self.fw_script_path, 0o755)
        print(f"✅ Created mock script: {self.fw_script_path}")
    
    def set_tr181_parameter(self, param, value, param_type="string"):
        """Set TR-181 parameter using tr181Set command."""
        try:
            if param_type == "boolean":
                value = "true" if value else "false"
            
            cmd = ["tr181Set", param, param_type, str(value)]
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=10)
            
            if result.returncode == 0:
                print(f"✅ Set {param} = {value}")
                return True
            else:
                print(f"❌ Failed to set {param}: {result.stderr}")
                return False
                
        except Exception as e:
            print(f"❌ Error setting {param}: {e}")
            return False
    
    def get_tr181_parameter(self, param):
        """Get TR-181 parameter using tr181Get command."""
        try:
            cmd = ["tr181Get", param]
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=10)
            
            if result.returncode == 0:
                # Parse output to extract value
                output = result.stdout.strip()
                if "Value:" in output:
                    value = output.split("Value:")[-1].strip()
                    return value
                return output
            else:
                print(f"❌ Failed to get {param}: {result.stderr}")
                return None
                
        except Exception as e:
            print(f"❌ Error getting {param}: {e}")
            return None
    
    def validate_parameter_configuration(self):
        """Validate TR-181 parameter configuration."""
        print("\n🔧 Testing Parameter Configuration...")
        
        params = {
            "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol": ("https", "string"),
            "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL": ("https://test.server.com/firmware", "string"),
            "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload": ("test_firmware.bin", "string"),
            "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadUseCodebig": (False, "boolean"),
            "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot": (False, "boolean")
        }
        
        success_count = 0
        for param, (value, param_type) in params.items():
            if self.set_tr181_parameter(param, value, param_type):
                success_count += 1
            time.sleep(0.5)
        
        print(f"📊 Parameter configuration: {success_count}/{len(params)} successful")
        return success_count == len(params)
    
    def validate_script_triggering(self):
        """Validate that setting FirmwareDownloadNow triggers the script."""
        print("\n🚀 Testing Script Triggering...")
        
        # Clean up any existing files
        if os.path.exists(self.fw_status_file):
            os.remove(self.fw_status_file)
        if os.path.exists("/tmp/fw_script_test.log"):
            os.remove("/tmp/fw_script_test.log")
        
        # Trigger download
        success = self.set_tr181_parameter(
            "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadNow", 
            True, 
            "boolean"
        )
        
        if not success:
            print("❌ Failed to trigger firmware download")
            return False
        
        # Wait for script execution
        print("⏳ Waiting for script execution...")
        time.sleep(5)
        
        # Check if script was executed
        script_executed = os.path.exists("/tmp/fw_script_test.log")
        status_file_created = os.path.exists(self.fw_status_file)
        
        if script_executed:
            print("✅ Script execution log found")
            with open("/tmp/fw_script_test.log", 'r') as f:
                print(f"📝 Script log:\n{f.read()}")
        else:
            print("❌ Script execution log not found")
        
        if status_file_created:
            print("✅ Status file created")
            with open(self.fw_status_file, 'r') as f:
                print(f"📄 Status file content:\n{f.read()}")
        else:
            print("❌ Status file not created")
        
        return script_executed and status_file_created
    
    def validate_status_monitoring(self):
        """Validate status monitoring through TR-181 parameters."""
        print("\n📊 Testing Status Monitoring...")
        
        status_params = [
            "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus",
            "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareUpdateState"
        ]
        
        success_count = 0
        for param in status_params:
            value = self.get_tr181_parameter(param)
            if value is not None:
                print(f"✅ {param}: {value}")
                success_count += 1
            else:
                print(f"❌ Failed to read {param}")
            time.sleep(0.5)
        
        return success_count == len(status_params)
    
    def validate_error_handling(self):
        """Validate error handling when parameters are missing."""
        print("\n🛡️ Testing Error Handling...")
        
        # Clear all parameters first
        clear_params = [
            "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol",
            "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL", 
            "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload"
        ]
        
        for param in clear_params:
            self.set_tr181_parameter(param, "", "string")
        
        # Try to trigger download without parameters
        if os.path.exists("/tmp/fw_script_test.log"):
            os.remove("/tmp/fw_script_test.log")
        
        success = self.set_tr181_parameter(
            "Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadNow",
            True,
            "boolean"
        )
        
        time.sleep(3)
        
        # Should not execute script without required parameters
        script_executed = os.path.exists("/tmp/fw_script_test.log")
        
        if not script_executed:
            print("✅ Error handling working - script not executed without parameters")
            return True
        else:
            print("❌ Error handling failed - script executed without required parameters")
            return False
    
    def run_validation(self):
        """Run complete validation of Image Upgrade functionality."""
        print("🔍 Image Upgrade Script Trigger Validation")
        print("=" * 50)
        
        # Pre-requisite checks
        print("\n📋 Pre-requisite Checks...")
        
        if not self.check_tr69hostif_running():
            print("⚠️  tr69hostif not running - some tests may fail")
        else:
            print("✅ tr69hostif is running")
        
        # Create mock script if it doesn't exist
        if not self.check_script_exists():
            print("📝 Creating mock firmware download script...")
            self.create_mock_script()
        else:
            print("✅ Firmware download script exists")
        
        # Run validation tests
        tests = [
            ("Parameter Configuration", self.validate_parameter_configuration),
            ("Script Triggering", self.validate_script_triggering), 
            ("Status Monitoring", self.validate_status_monitoring),
            ("Error Handling", self.validate_error_handling)
        ]
        
        results = {}
        for test_name, test_func in tests:
            try:
                results[test_name] = test_func()
            except Exception as e:
                print(f"❌ {test_name} failed with exception: {e}")
                results[test_name] = False
        
        # Summary
        print("\n📋 Validation Summary")
        print("=" * 30)
        
        passed = sum(results.values())
        total = len(results)
        
        for test_name, result in results.items():
            status = "✅ PASS" if result else "❌ FAIL"
            print(f"{test_name}: {status}")
        
        print(f"\nOverall: {passed}/{total} tests passed")
        
        if passed == total:
            print("🎉 All validations passed! Image Upgrade script triggering is working correctly.")
            return True
        else:
            print("⚠️  Some validations failed. Check the implementation.")
            return False

def main():
    """Main entry point."""
    validator = ImageUpgradeValidator()
    success = validator.run_validation()
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()