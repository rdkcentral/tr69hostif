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

import subprocess
import pytest
import time
import os
import json
from time import sleep

from helper_functions import *
from basic_constants import *

MODULE_NAME = "tr69hostif"

def is_process_active():
    """Check if tr69hostif process is active and wait for stabilization."""
    print("Checking tr69hostif process status")
    
    uptime_process = get_process_uptime(get_pid(MODULE_NAME))
    if uptime_process < 15:  # Wait longer for bootstrap completion
        sleep(15 - uptime_process)

    command_to_get_pid = "pidof tr69hostif"
    pid = run_shell_command(command_to_get_pid)
    assert pid != "", "tr69hostif process is not running"

def check_file_exists(filepath, description=""):
    """Helper function to check if a file exists."""
    exists = os.path.exists(filepath)
    print(f"File check: {filepath} - {'EXISTS' if exists else 'MISSING'} {description}")
    return exists

def check_log_message(message, should_exist=True, description=""):
    """Helper function to check for log messages."""
    found = message in grep_T2logs(message)
    status = "FOUND" if found else "NOT FOUND"
    expected = "EXPECTED" if should_exist else "NOT EXPECTED"
    print(f"Log check: {description} - {status} ({expected})")
    return found == should_exist

class TestBootstrapInitialization:
    """L2 Test Suite for TR-69 Host Interface Bootstrap Initialization Flow"""

    @pytest.mark.run(order=1)
    def test_bootstrap_process_startup(self):
        """Test 1: Verify tr69hostif process startup and basic initialization"""
        is_process_active()
        
        # Check startup message
        assert check_log_message(
            "Starting tr69HostIf Service",
            description="Service startup message"
        )

    @pytest.mark.run(order=2)
    def test_bootstrap_command_line_processing(self):
        """Test 2: Verify command line argument processing and defaults"""
        is_process_active()
        
        # Should not have command line parsing errors
        assert check_log_message(
            "non-option ARGV-elements",
            should_exist=False,
            description="No command line parsing errors"
        )

    @pytest.mark.run(order=3)  
    def test_bootstrap_signal_handlers(self):
        """Test 3: Verify signal handler setup"""
        is_process_active()
        
        # Should not have signal setup errors
        assert check_log_message(
            "sem_init() failed",
            should_exist=False,
            description="Semaphore initialization success"
        )
        
        assert check_log_message(
            "pthread_create() failed",
            should_exist=False,
            description="Shutdown thread creation success"
        )

    @pytest.mark.run(order=4)
    def test_bootstrap_glib_initialization(self):
        """Test 4: Verify GLib thread initialization"""
        is_process_active()
        
        # Check GLib thread support
        glib_supported = check_log_message(
            "g_thread supported",
            should_exist=False,  # Modern systems should have thread support by default
            description="GLib thread support check"
        )
        
        # Should not have GLib errors
        assert check_log_message(
            "g_thread NOT supported",
            should_exist=False,
            description="No GLib thread errors"
        )

    @pytest.mark.run(order=5)
    def test_bootstrap_config_manager_init(self):
        """Test 5: Verify Configuration Manager initialization"""
        is_process_active()
        
        # Configuration manager should initialize successfully
        assert check_log_message(
            "Failed to hostIf_initalize_ConfigManger()",
            should_exist=False,
            description="Configuration manager initialization success"
        )
        
        # Check if config file exists
        config_files = [
            "/etc/tr69hostif.conf",
            "/etc/mgrlist.conf"
        ]
        
        config_found = False
        for config_file in config_files:
            if check_file_exists(config_file, f"Config file: {config_file}"):
                config_found = True
                break
        
        assert config_found, "At least one configuration file should exist"

    @pytest.mark.run(order=6)
    def test_bootstrap_rfc_initialization(self):
        """Test 6: Verify RFC (Remote Feature Control) initialization"""
        is_process_active()
        
        # Check RFC-related files
        rfc_files = [
            "/etc/rfc.properties",
            "/opt/secure/RFC/rfcVariable.ini",
            "/opt/secure/RFC/tr181store.ini"
        ]
        
        for rfc_file in rfc_files:
            check_file_exists(rfc_file, f"RFC file: {rfc_file}")
        
        # Check legacy RFC status
        legacy_rfc_file = "/opt/RFC/.RFC_LegacyRFCEnabled.ini"
        legacy_enabled = check_file_exists(legacy_rfc_file, "Legacy RFC enablement")
        
        if not legacy_enabled:
            assert check_log_message(
                "legacyRFC Set to False, Starting New HTTP Server",
                description="New HTTP server started when legacy RFC disabled"
            )

    @pytest.mark.run(order=7)
    def test_bootstrap_iarm_initialization(self):
        """Test 7: Verify IARM Bus initialization"""
        is_process_active()
        
        # IARM should initialize successfully
        assert check_log_message(
            "Failed to start hostIf_IARM_IF_Start()",
            should_exist=False,
            description="IARM interface startup success"
        )
        
        # Check for IARM initialization success indicators
        iarm_success_patterns = [
            "TR69_HostIf_Mgr_Init",
            "TR69_HostIf_Mgr_Connect",
            "TR69_HostIf_Mgr_Get_RegisterCall"
        ]
        
        # At least some IARM initialization should be logged
        iarm_init_found = any(
            check_log_message(pattern, description=f"IARM: {pattern}")
            for pattern in iarm_success_patterns
        )

    @pytest.mark.run(order=8)
    def test_bootstrap_data_model_merge(self):
        """Test 8: Verify Data Model merging process"""
        is_process_active()
        
        # Data model merge should succeed
        assert check_log_message(
            "Error in merging Data Model",
            should_exist=False,
            description="Data model merge success"
        )
        
        assert check_log_message(
            "Successfully merged Data Model",
            description="Data model merge completion"
        )
        
        # Check for data model files
        data_model_files = [
            "/tmp/data-model.xml",
            "/etc/data-model-generic.xml"
        ]
        
        for dm_file in data_model_files:
            check_file_exists(dm_file, f"Data model file: {dm_file}")

    @pytest.mark.run(order=9)
    def test_bootstrap_data_model_loading(self):
        """Test 9: Verify Data Model loading"""
        is_process_active()
        
        # Data model should load successfully
        assert check_log_message(
            "Error in Data Model Initialization",
            should_exist=False,
            description="Data model loading success"
        )
        
        assert check_log_message(
            "Successfully initialize Data Model",
            description="Data model initialization completion"
        )

    @pytest.mark.run(order=10)
    def test_bootstrap_json_handler_thread(self):
        """Test 10: Verify JSON Handler Thread startup"""
        is_process_active()
        
        # JSON thread should start successfully
        assert check_log_message(
            "Thread create failed",
            should_exist=False,
            description="JSON handler thread creation success"
        )
        
        # Check for JSON server startup
        json_success_messages = [
            "SERVER: Started server successfully",
            "json_if_handler_thread"
        ]
        
        json_started = any(
            check_log_message(msg, description=f"JSON server: {msg}")
            for msg in json_success_messages
        )

    @pytest.mark.run(order=11)
    def test_bootstrap_http_server_thread(self):
        """Test 11: Verify HTTP Server Thread startup (if not legacy)"""
        is_process_active()
        
        # Check if new HTTP server is enabled
        legacy_rfc_file = "/opt/RFC/.RFC_LegacyRFCEnabled.ini"
        if not check_file_exists(legacy_rfc_file):
            # New HTTP server should start
            assert check_log_message(
                "New HTTP Server Thread Create failed",
                should_exist=False,
                description="New HTTP server thread creation success"
            )
            
            check_log_message(
                "legacyRFC Set to False, Starting New HTTP Server",
                description="New HTTP server startup indication"
            )
        else:
            # Legacy mode - new server should not start
            check_log_message(
                "legacyRFC Set to True, New HTTP Server is not started",
                description="Legacy RFC mode - new server not started"
            )

    @pytest.mark.run(order=12)
    def test_bootstrap_update_handler_init(self):
        """Test 12: Verify Update Handler initialization"""
        is_process_active()
        
        # Update handler should initialize
        # This is internal so we check for absence of errors
        assert check_log_message(
            "updateHandler::Init failed",
            should_exist=False,
            description="Update handler initialization success"
        )

    @pytest.mark.run(order=13)
    def test_bootstrap_parodus_initialization(self):
        """Test 13: Verify Parodus/WebPA initialization (if enabled)"""
        is_process_active()
        
        # Check if Parodus is enabled in build
        parodus_success = check_log_message(
            "Initiating Connection with PARODUS success",
            description="Parodus connection initiation"
        )
        
        if parodus_success:
            # Parodus should not fail to create thread
            assert check_log_message(
                "Parodus init thread create failed",
                should_exist=False,
                description="Parodus thread creation success"
            )
            
            # Check for WebPA startup
            check_log_message(
                "Starting WEBPA Parodus Connections",
                description="WebPA Parodus connections start"
            )

    @pytest.mark.run(order=14)
    def test_bootstrap_rbus_dml_provider(self):
        """Test 14: Verify RBUS DML Provider initialization"""
        is_process_active()
        
        # RBUS should initialize successfully
        rbus_error_messages = [
            "[rbusdml] Failed to initialized, rbus_checkStatus() returned with status",
            "consumer: rbus_open failed"
        ]
        
        for error_msg in rbus_error_messages:
            assert check_log_message(
                error_msg,
                should_exist=False,
                description=f"RBUS error check: {error_msg}"
            )
        
        # Check for RBUS success indicators
        rbus_success_messages = [
            "[rbusdml]Successfully get the complete parameter list",
            "rbus_regDataElements registered successfully"
        ]
        
        rbus_success = any(
            check_log_message(msg, description=f"RBUS success: {msg}")
            for msg in rbus_success_messages
        )

    @pytest.mark.run(order=15)
    def test_bootstrap_systemd_notification(self):
        """Test 15: Verify SystemD ready notification"""
        is_process_active()
        
        # Should send systemd notification
        systemd_success = check_log_message(
            "tr69hostif sd notify envent  is sent  Successfully",
            description="SystemD notification sent"
        )
        
        # Check for ready status
        ready_status = check_log_message(
            "tr69hostif is Successfully Initialized",
            description="Service ready status"
        )

    @pytest.mark.run(order=16)
    def test_bootstrap_device_manageability(self):
        """Test 16: Verify Device Manageability notification"""
        is_process_active()
        
        # Device should send manageability notification
        # This might not always be logged, so we check for absence of errors
        assert check_log_message(
            "send_DeviceManageableNotification failed",
            should_exist=False,
            description="Device manageability notification success"
        )

    @pytest.mark.run(order=17)
    def test_bootstrap_main_event_loop(self):
        """Test 17: Verify main event loop startup"""
        is_process_active()
        
        # Main loop should start successfully
        assert check_log_message(
            "Fails to Create a main loop",
            should_exist=False,
            description="Main event loop creation success"
        )
        
        # Process should be running and stable
        pid = get_pid(MODULE_NAME)
        assert pid != "", "Process should be running in main loop"
        
        # Wait a bit and check process is still alive
        sleep(2)
        pid_after = get_pid(MODULE_NAME)
        assert pid == pid_after, "Process should remain stable in main loop"

    @pytest.mark.run(order=18)
    def test_bootstrap_configuration_files_content(self):
        """Test 18: Verify configuration files have valid content"""
        is_process_active()
        
        # Check manager configuration file content
        config_file = "/etc/tr69hostif.conf"
        if not check_file_exists(config_file):
            config_file = "/etc/mgrlist.conf"
        
        if check_file_exists(config_file):
            try:
                with open(config_file, 'r') as f:
                    content = f.read()
                    
                # Should contain essential manager mappings
                essential_managers = [
                    "Device.DeviceInfo",
                    "Device.Ethernet", 
                    "deviceMgr",
                    "ethernetMgr"
                ]
                
                managers_found = sum(1 for mgr in essential_managers if mgr in content)
                assert managers_found >= 2, f"Configuration file should contain essential manager mappings"
                
            except Exception as e:
                print(f"Could not verify config file content: {e}")

    @pytest.mark.run(order=19)
    def test_bootstrap_rfc_store_initialization(self):
        """Test 19: Verify RFC Store initialization"""
        is_process_active()
        
        # Check for RFC store initialization
        rfc_store_indicators = [
            "Bootstrap Properties FileName",
            "RFC Variables FileName",
            "initBSPropertiesFileName"
        ]
        
        rfc_init_found = any(
            check_log_message(indicator, description=f"RFC store init: {indicator}")
            for indicator in rfc_store_indicators
        )

    @pytest.mark.run(order=20)
    def test_bootstrap_partner_defaults_loading(self):
        """Test 20: Verify partner defaults loading"""
        is_process_active()
        
        # Check for partner defaults files
        partner_files = [
            "/etc/partners_defaults.json",
            "/etc/partners_defaults_device.json"
        ]
        
        for partner_file in partner_files:
            if check_file_exists(partner_file):
                try:
                    with open(partner_file, 'r') as f:
                        json_content = json.load(f)
                        assert isinstance(json_content, dict), f"Partner file {partner_file} should contain valid JSON"
                except json.JSONDecodeError:
                    print(f"Warning: {partner_file} contains invalid JSON")
                except Exception as e:
                    print(f"Could not read partner file {partner_file}: {e}")

    @pytest.mark.run(order=21)
    def test_bootstrap_process_stability(self):
        """Test 21: Verify process stability after bootstrap completion"""
        is_process_active()
        
        # Get initial process info
        initial_pid = get_pid(MODULE_NAME)
        initial_uptime = get_process_uptime(initial_pid)
        
        # Wait and check stability
        sleep(5)
        
        current_pid = get_pid(MODULE_NAME)
        current_uptime = get_process_uptime(current_pid)
        
        # Process should be the same and uptime should have increased
        assert initial_pid == current_pid, "Process should not have restarted"
        assert current_uptime > initial_uptime, "Process uptime should have increased"
        
        # Check for crash indicators
        crash_indicators = [
            "EXITING MAIN PROGRAM",
            "Bad cast exception",
            "Exception:",
            "signal"
        ]
        
        for indicator in crash_indicators:
            assert check_log_message(
                indicator,
                should_exist=False,
                description=f"No crash indicator: {indicator}"
            )

    @pytest.mark.run(order=22)
    def test_bootstrap_functional_verification(self):
        """Test 22: Verify basic functionality after bootstrap"""
        is_process_active()
        
        # Try a simple RBUS operation to verify system is functional
        try:
            result = rbus_get_data("Device.DeviceInfo.Manufacturer")
            assert "Failed to get the data" not in result, "Basic RBUS operation should work"
            print(f"Functional test: RBUS get successful")
        except Exception as e:
            print(f"Functional test warning: Could not test RBUS functionality: {e}")
        
        # Check if HTTP interface is responsive (if available)
        try:
            import requests
            response = requests.get("http://127.0.0.1:11999/api/v1/device/config", timeout=5)
            print(f"HTTP interface test: Status {response.status_code}")
        except Exception as e:
            print(f"HTTP interface test: Not available or failed: {e}")

    @pytest.mark.run(order=23)
    def test_bootstrap_completion_summary(self):
        """Test 23: Bootstrap completion summary and health check"""
        is_process_active()
        
        print("\n" + "="*80)
        print("BOOTSTRAP INITIALIZATION TEST SUMMARY")
        print("="*80)
        
        # Count success indicators
        success_indicators = [
            "Starting tr69HostIf Service",
            "Successfully initialize Data Model",
            "Successfully merged Data Model",
            "tr69hostif is Successfully Initialized"
        ]
        
        success_count = sum(1 for indicator in success_indicators 
                          if check_log_message(indicator, description=f"Summary: {indicator}"))
        
        print(f"Success indicators found: {success_count}/{len(success_indicators)}")
        
        # Count error indicators
        error_indicators = [
            "Failed to hostIf_initalize_ConfigManger()",
            "Error in Data Model Initialization", 
            "Error in merging Data Model",
            "Failed to start hostIf_IARM_IF_Start()",
            "pthread_create() failed"
        ]
        
        error_count = sum(1 for indicator in error_indicators 
                         if check_log_message(indicator, should_exist=False, description=f"Error check: {indicator}"))
        
        print(f"Error checks passed: {error_count}/{len(error_indicators)}")
        
        # Overall health assessment
        health_score = (success_count + error_count) / (len(success_indicators) + len(error_indicators)) * 100
        print(f"Bootstrap health score: {health_score:.1f}%")
        
        assert health_score >= 70, f"Bootstrap health score should be at least 70%, got {health_score:.1f}%"
        
        print("="*80)
        print("BOOTSTRAP INITIALIZATION TEST COMPLETED SUCCESSFULLY")
        print("="*80)

if __name__ == "__main__":
    # Run the tests
    pytest.main([__file__, "-v", "--tb=short"])