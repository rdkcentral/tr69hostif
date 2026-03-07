#!/bin/bash

####################################################################################
# Bootstrap Initialization Test Runner
# Comprehensive L2 testing for TR-69 Host Interface Bootstrap Initialization
####################################################################################

# Test configuration
TEST_DIR="$(dirname "$0")"
RESULT_DIR="/tmp/tr69hostif_test_results"
LOG_DIR="/opt/logs"
BOOTSTRAP_TEST_RESULT="$RESULT_DIR/bootstrap_initialization.json"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging function
log_message() {
    echo -e "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Create result directory
mkdir -p "$RESULT_DIR"

log_message "${BLUE}Starting TR-69 Host Interface Bootstrap Initialization Tests${NC}"

# Pre-test setup
log_message "${YELLOW}Setting up test environment...${NC}"

# Stop any existing tr69hostif process
if pgrep tr69hostif > /dev/null; then
    log_message "Stopping existing tr69hostif process..."
    pkill -TERM tr69hostif
    sleep 3
    if pgrep tr69hostif > /dev/null; then
        pkill -KILL tr69hostif
        sleep 2
    fi
fi

# Clear logs for clean test run
if [ -f "$LOG_DIR/tr69hostif.log.0" ]; then
    log_message "Clearing existing logs..."
    echo "" > "$LOG_DIR/tr69hostif.log.0"
fi

# Verify required files exist
log_message "Verifying test prerequisites..."

required_files=(
    "/usr/local/bin/tr69hostif"
    "/etc/tr69hostif.conf"
    "/etc/mgrlist.conf" 
)

missing_files=0
for file in "${required_files[@]}"; do
    if [ ! -f "$file" ] && [ ! -f "${file##*/}" ]; then
        log_message "${RED}Warning: Required file $file not found${NC}"
        ((missing_files++))
    fi
done

if [ $missing_files -gt 0 ]; then
    log_message "${YELLOW}Some required files are missing, but continuing with tests...${NC}"
fi

# Start tr69hostif in background for testing
log_message "${YELLOW}Starting tr69hostif for bootstrap testing...${NC}"

# Try different possible locations for tr69hostif binary
tr69hostif_binary=""
possible_paths=(
    "/usr/local/bin/tr69hostif"
    "/usr/bin/tr69hostif"
    "./tr69hostif"
    "../src/hostif/src/tr69hostif"
)

for path in "${possible_paths[@]}"; do
    if [ -x "$path" ]; then
        tr69hostif_binary="$path"
        break
    fi
done

if [ -z "$tr69hostif_binary" ]; then
    log_message "${RED}Error: tr69hostif binary not found in expected locations${NC}"
    exit 1
fi

log_message "Starting tr69hostif from: $tr69hostif_binary"

# Start tr69hostif with appropriate configuration
if [ -f "/etc/tr69hostif.conf" ]; then
    $tr69hostif_binary --conffile /etc/tr69hostif.conf &
elif [ -f "/etc/mgrlist.conf" ]; then
    $tr69hostif_binary --conffile /etc/mgrlist.conf &
else
    $tr69hostif_binary &
fi

TR69_PID=$!
log_message "Started tr69hostif with PID: $TR69_PID"

# Wait for process to initialize
log_message "Waiting for bootstrap initialization..."
sleep 5

# Verify process is running
if ! pgrep tr69hostif > /dev/null; then
    log_message "${RED}Error: tr69hostif process failed to start${NC}"
    exit 1
fi

# Run bootstrap initialization tests
log_message "${BLUE}Running Bootstrap Initialization Tests...${NC}"

# Execute the main bootstrap test
cd "$TEST_DIR"
python3 -m pytest test_bootstrap_initialization.py \
    --json-report \
    --json-report-summary \
    --json-report-file="$BOOTSTRAP_TEST_RESULT" \
    -v \
    --tb=short \
    --durations=10

test_exit_code=$?

# Capture test results
if [ $test_exit_code -eq 0 ]; then
    log_message "${GREEN}Bootstrap Initialization Tests PASSED${NC}"
else
    log_message "${RED}Bootstrap Initialization Tests FAILED (exit code: $test_exit_code)${NC}"
fi

# Generate summary report
log_message "${BLUE}Generating test summary...${NC}"

if [ -f "$BOOTSTRAP_TEST_RESULT" ]; then
    # Extract key metrics from JSON report
    total_tests=$(jq -r '.summary.total // 0' "$BOOTSTRAP_TEST_RESULT")
    passed_tests=$(jq -r '.summary.passed // 0' "$BOOTSTRAP_TEST_RESULT") 
    failed_tests=$(jq -r '.summary.failed // 0' "$BOOTSTRAP_TEST_RESULT")
    duration=$(jq -r '.duration // 0' "$BOOTSTRAP_TEST_RESULT")
    
    log_message "Test Summary:"
    log_message "  Total Tests: $total_tests"
    log_message "  Passed: ${GREEN}$passed_tests${NC}"
    log_message "  Failed: ${RED}$failed_tests${NC}"
    log_message "  Duration: ${duration}s"
    
    # Calculate success rate
    if [ "$total_tests" -gt 0 ]; then
        success_rate=$(echo "scale=1; $passed_tests * 100 / $total_tests" | bc -l)
        log_message "  Success Rate: ${success_rate}%"
    fi
else
    log_message "${YELLOW}Warning: Test result file not found${NC}"
fi

# Generate detailed HTML report if possible
if command -v pytest-html &> /dev/null; then
    log_message "Generating HTML report..."
    python3 -m pytest test_bootstrap_initialization.py \
        --html="$RESULT_DIR/bootstrap_report.html" \
        --self-contained-html \
        --tb=short > /dev/null 2>&1
fi

# Collect system information for debugging
log_message "${BLUE}Collecting system information...${NC}"

cat > "$RESULT_DIR/system_info.txt" << EOF
Bootstrap Initialization Test - System Information
Generated: $(date)

System Information:
$(uname -a)

Memory Usage:
$(free -h)

Disk Usage:
$(df -h)

Process Information:
$(ps aux | grep tr69hostif)

Network Interfaces:
$(ip addr show)

Configuration Files:
EOF

# Check configuration files
config_files=(
    "/etc/tr69hostif.conf"
    "/etc/mgrlist.conf"
    "/etc/rfc.properties"
    "/etc/partners_defaults.json"
)

for config_file in "${config_files[@]}"; do
    if [ -f "$config_file" ]; then
        echo "=== $config_file ===" >> "$RESULT_DIR/system_info.txt"
        head -20 "$config_file" >> "$RESULT_DIR/system_info.txt"
        echo "" >> "$RESULT_DIR/system_info.txt"
    else
        echo "$config_file: NOT FOUND" >> "$RESULT_DIR/system_info.txt"
    fi
done

# Collect relevant log snippets
if [ -f "$LOG_DIR/tr69hostif.log.0" ]; then
    log_message "Collecting log snippets..."
    
    # Extract bootstrap-related log messages
    grep -i "bootstrap\|init\|start\|success\|error\|fail" "$LOG_DIR/tr69hostif.log.0" | tail -100 > "$RESULT_DIR/bootstrap_logs.txt"
    
    # Extract error messages
    grep -i "error\|fail\|exception" "$LOG_DIR/tr69hostif.log.0" | tail -50 > "$RESULT_DIR/error_logs.txt"
fi

# Performance analysis
log_message "${BLUE}Performing bootstrap performance analysis...${NC}"

if pgrep tr69hostif > /dev/null; then
    tr69_pid=$(pgrep tr69hostif)
    
    # Memory usage
    memory_usage=$(ps -p "$tr69_pid" -o rss= 2>/dev/null)
    if [ -n "$memory_usage" ]; then
        memory_mb=$(echo "scale=2; $memory_usage / 1024" | bc -l)
        log_message "Current memory usage: ${memory_mb} MB"
    fi
    
    # CPU usage over 10 seconds
    cpu_usage=$(top -p "$tr69_pid" -b -n 2 -d 5 | grep tr69hostif | tail -1 | awk '{print $9}' 2>/dev/null)
    if [ -n "$cpu_usage" ]; then
        log_message "CPU usage: ${cpu_usage}%"
    fi
    
    # File descriptors
    fd_count=$(ls -1 /proc/"$tr69_pid"/fd 2>/dev/null | wc -l)
    log_message "Open file descriptors: $fd_count"
    
    # Threads
    thread_count=$(ls -1 /proc/"$tr69_pid"/task 2>/dev/null | wc -l)
    log_message "Thread count: $thread_count"
fi

# Cleanup
log_message "${YELLOW}Cleaning up test environment...${NC}"

# Stop tr69hostif process
if pgrep tr69hostif > /dev/null; then
    log_message "Stopping tr69hostif process..."
    pkill -TERM tr69hostif
    sleep 3
    if pgrep tr69hostif > /dev/null; then
        pkill -KILL tr69hostif
    fi
fi

# Final results
log_message "${BLUE}Bootstrap Initialization Test Complete${NC}"
log_message "Results saved to: $RESULT_DIR"

if [ $test_exit_code -eq 0 ]; then
    log_message "${GREEN}Overall Result: PASS${NC}"
    exit 0
else
    log_message "${RED}Overall Result: FAIL${NC}"
    exit 1
fi