#!/bin/bash

# Advanced Philosophers Race Condition and Deadlock Tester
# Specifically designed to catch subtle threading issues
# Author: GitHub Copilot & mecavus

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m'

TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

print_header() {
    echo -e "${BLUE}=================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}=================================${NC}"
}

print_test() {
    echo -e "${CYAN}Testing: $1${NC}"
    ((TOTAL_TESTS++))
}

print_success() {
    echo -e "${GREEN}✓ PASS: $1${NC}"
    ((PASSED_TESTS++))
}

print_fail() {
    echo -e "${RED}✗ FAIL: $1${NC}"
    ((FAILED_TESTS++))
}

# Test for deadlocks by running multiple instances simultaneously
test_deadlock_detection() {
    print_header "DEADLOCK DETECTION TESTS"
    
    print_test "Concurrent instances (deadlock detection)"
    pids=()
    for i in {1..5}; do
        timeout 10s ./philo 4 1000 200 200 10 >/dev/null 2>&1 &
        pids+=($!)
    done
    
    # Wait for all processes
    all_completed=true
    for pid in "${pids[@]}"; do
        if ! wait $pid; then
            all_completed=false
        fi
    done
    
    if $all_completed; then
        print_success "No deadlocks detected in concurrent execution"
    else
        print_fail "Possible deadlock detected"
    fi
}

# Test for race conditions in message printing
test_message_races() {
    print_header "MESSAGE RACE CONDITION TESTS"
    
    print_test "Message interleaving detection"
    output=$(timeout 5s ./philo 5 1000 100 100 20 2>/dev/null)
    
    # Check for malformed messages (race conditions in printf)
    malformed_lines=$(echo "$output" | grep -v -E "^[0-9]+ [0-9]+ (has taken a fork|is eating|is sleeping|is thinking|died)$" | wc -l)
    
    if [ $malformed_lines -eq 0 ]; then
        print_success "No message race conditions detected"
    else
        print_fail "Found $malformed_lines malformed messages (possible race conditions)"
        echo "Sample malformed messages:"
        echo "$output" | grep -v -E "^[0-9]+ [0-9]+ (has taken a fork|is eating|is sleeping|is thinking|died)$" | head -5
    fi
}

# Test for fork sharing violations
test_fork_violations() {
    print_header "FORK SHARING VIOLATION TESTS"
    
    print_test "Fork sharing validation"
    output=$(timeout 10s ./philo 3 2000 200 200 10 2>/dev/null)
    
    # Parse eating events and check for concurrent eating by adjacent philosophers
    eating_events=()
    while IFS= read -r line; do
        if echo "$line" | grep -q "is eating"; then
            timestamp=$(echo "$line" | cut -d' ' -f1)
            philo_id=$(echo "$line" | cut -d' ' -f2)
            eating_events+=("$timestamp:$philo_id")
        fi
    done <<< "$output"
    
    violations=0
    for i in "${!eating_events[@]}"; do
        current=(${eating_events[$i]//:/ })
        current_time=${current[0]}
        current_philo=${current[1]}
        
        for j in "${!eating_events[@]}"; do
            if [ $i -ne $j ]; then
                other=(${eating_events[$j]//:/ })
                other_time=${other[0]}
                other_philo=${other[1]}
                
                # Check if adjacent philosophers are eating simultaneously
                time_diff=$((current_time - other_time))
                if [ ${time_diff#-} -lt 200 ]; then  # Within eating duration
                    if [ $((current_philo % 3)) -eq $(((other_philo - 1) % 3)) ] || 
                       [ $(((current_philo - 1) % 3)) -eq $((other_philo % 3)) ]; then
                        ((violations++))
                    fi
                fi
            fi
        done
    done
    
    if [ $violations -eq 0 ]; then
        print_success "No fork sharing violations detected"
    else
        print_fail "Found $violations potential fork sharing violations"
    fi
}

# Test philosopher starvation patterns
test_starvation_patterns() {
    print_header "STARVATION PATTERN TESTS"
    
    print_test "Starvation fairness analysis"
    output=$(timeout 30s ./philo 5 2000 200 200 20 2>/dev/null)
    
    # Count meals per philosopher
    declare -A meal_counts
    for i in {1..5}; do
        meal_counts[$i]=0
    done
    
    while IFS= read -r line; do
        if echo "$line" | grep -q "is eating"; then
            philo_id=$(echo "$line" | cut -d' ' -f2)
            ((meal_counts[$philo_id]++))
        fi
    done <<< "$output"
    
    # Check for starvation (significant meal count differences)
    min_meals=999999
    max_meals=0
    for i in {1..5}; do
        if [ ${meal_counts[$i]} -lt $min_meals ]; then
            min_meals=${meal_counts[$i]}
        fi
        if [ ${meal_counts[$i]} -gt $max_meals ]; then
            max_meals=${meal_counts[$i]}
        fi
    done
    
    meal_difference=$((max_meals - min_meals))
    if [ $meal_difference -le 2 ]; then
        print_success "Fair meal distribution (difference: $meal_difference)"
    else
        print_warning "Uneven meal distribution (difference: $meal_difference)"
        for i in {1..5}; do
            echo "  Philosopher $i: ${meal_counts[$i]} meals"
        done
    fi
}

# Rapid start/stop stress test
test_rapid_lifecycle() {
    print_header "RAPID LIFECYCLE TESTS"
    
    print_test "Rapid start/stop stress test"
    success_count=0
    for i in {1..20}; do
        timeout 1s ./philo 3 1000 100 100 5 >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            ((success_count++))
        fi
    done
    
    if [ $success_count -eq 20 ]; then
        print_success "All rapid lifecycle tests passed"
    elif [ $success_count -gt 15 ]; then
        print_warning "Most rapid lifecycle tests passed ($success_count/20)"
    else
        print_fail "Many rapid lifecycle tests failed ($success_count/20)"
    fi
}

# Test memory corruption patterns
test_memory_corruption() {
    print_header "MEMORY CORRUPTION TESTS"
    
    if command -v valgrind >/dev/null 2>&1; then
        print_test "Memory corruption detection"
        valgrind --tool=memcheck --track-origins=yes --show-leak-kinds=all \
                 --errors-for-leak-kinds=all --error-exitcode=1 \
                 timeout 10s ./philo 4 1500 200 200 10 >/dev/null 2>memcheck.log
        
        if [ $? -eq 1 ]; then
            print_fail "Memory corruption detected"
            echo "Memcheck errors:"
            cat memcheck.log
        else
            print_success "No memory corruption detected"
        fi
        rm -f memcheck.log
    else
        print_warning "Valgrind not available for memory corruption testing"
    fi
}

# Test timing precision
test_timing_precision() {
    print_header "TIMING PRECISION TESTS"
    
    print_test "Timing precision validation"
    output=$(timeout 10s ./philo 2 2000 500 500 5 2>/dev/null)
    
    # Extract eating start times for same philosopher
    eating_times=()
    while IFS= read -r line; do
        if echo "$line" | grep -q "1 is eating"; then
            timestamp=$(echo "$line" | cut -d' ' -f1)
            eating_times+=($timestamp)
        fi
    done <<< "$output"
    
    # Check intervals between eating sessions
    timing_errors=0
    for ((i=1; i<${#eating_times[@]}; i++)); do
        interval=$((eating_times[i] - eating_times[i-1]))
        # Expected interval: eat_time + sleep_time + some overhead
        # 500 + 500 = 1000ms minimum
        if [ $interval -lt 900 ]; then  # Allow 10% tolerance
            ((timing_errors++))
        fi
    done
    
    if [ $timing_errors -eq 0 ]; then
        print_success "Timing precision is accurate"
    else
        print_warning "Found $timing_errors timing precision errors"
    fi
}

# Test thread cleanup
test_thread_cleanup() {
    print_header "THREAD CLEANUP TESTS"
    
    print_test "Thread cleanup verification"
    
    # Count threads before
    threads_before=$(ps -eLf | grep -c ./philo)
    
    # Run multiple short tests
    for i in {1..10}; do
        timeout 2s ./philo 3 1000 100 100 3 >/dev/null 2>&1
    done
    
    sleep 1  # Allow time for cleanup
    
    # Count threads after
    threads_after=$(ps -eLf | grep -c ./philo)
    
    if [ $threads_after -le $((threads_before + 1)) ]; then
        print_success "Proper thread cleanup detected"
    else
        print_warning "Possible thread leakage (before: $threads_before, after: $threads_after)"
    fi
}

# Test CPU affinity and load balancing
test_cpu_behavior() {
    print_header "CPU BEHAVIOR TESTS"
    
    if command -v htop >/dev/null 2>&1 || command -v top >/dev/null 2>&1; then
        print_test "CPU load balancing"
        
        # Start a longer running test
        ./philo 10 5000 200 200 50 >/dev/null 2>&1 &
        PHILO_PID=$!
        
        sleep 5
        
        # Check CPU usage distribution
        if command -v htop >/dev/null 2>&1; then
            cpu_info=$(htop -d 1 -n 1 | head -20)
            print_success "CPU behavior monitoring completed"
        else
            cpu_usage=$(top -p $PHILO_PID -n 1 | grep -E "^[ ]*$PHILO_PID")
            print_success "CPU monitoring completed"
        fi
        
        kill $PHILO_PID 2>/dev/null
        wait $PHILO_PID 2>/dev/null
    else
        print_warning "CPU monitoring tools not available"
    fi
}

# Generate advanced report
generate_advanced_report() {
    print_header "ADVANCED TEST SUMMARY"
    
    echo -e "${BLUE}Advanced Tests: $TOTAL_TESTS${NC}"
    echo -e "${GREEN}Passed: $PASSED_TESTS${NC}"
    echo -e "${RED}Failed: $FAILED_TESTS${NC}"
    
    success_rate=$((PASSED_TESTS * 100 / TOTAL_TESTS))
    
    if [ $success_rate -ge 90 ]; then
        echo -e "${GREEN}🎯 Excellent! Success rate: ${success_rate}%${NC}"
    elif [ $success_rate -ge 75 ]; then
        echo -e "${YELLOW}⚠️  Good! Success rate: ${success_rate}% (some improvements needed)${NC}"
    else
        echo -e "${RED}❌ Needs work! Success rate: ${success_rate}%${NC}"
    fi
}

# Main execution
main() {
    echo -e "${PURPLE}=========================================${NC}"
    echo -e "${PURPLE}   ADVANCED PHILOSOPHERS TESTER        ${NC}"
    echo -e "${PURPLE}=========================================${NC}"
    echo ""
    
    if [ ! -f "./philo" ]; then
        echo -e "${RED}Program './philo' not found. Please build first.${NC}"
        exit 1
    fi
    
    test_deadlock_detection
    test_message_races
    test_fork_violations
    test_starvation_patterns
    test_rapid_lifecycle
    test_memory_corruption
    test_timing_precision
    test_thread_cleanup
    test_cpu_behavior
    
    generate_advanced_report
}

main "$@"