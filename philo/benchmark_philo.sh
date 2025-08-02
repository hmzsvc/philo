#!/bin/bash

# Philosophers Performance Benchmark Suite
# Tests performance across various scenarios and configurations
# Author: GitHub Copilot & mecavus

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

# Results storage
declare -A results

print_header() {
    echo -e "${BLUE}=================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}=================================${NC}"
}

print_benchmark() {
    echo -e "${CYAN}Benchmarking: $1${NC}"
}

# Benchmark function
benchmark_scenario() {
    local name="$1"
    local philos="$2"
    local die_time="$3"
    local eat_time="$4"
    local sleep_time="$5"
    local meals="$6"
    
    print_benchmark "$name"
    
    # Measure execution time
    start_time=$(date +%s%N)
    output=$(timeout 60s ./philo $philos $die_time $eat_time $sleep_time $meals 2>/dev/null)
    exit_code=$?
    end_time=$(date +%s%N)
    
    duration_ms=$(( (end_time - start_time) / 1000000 ))
    
    # Check for deaths
    deaths=$(echo "$output" | grep -c "died")
    
    # Count total meals
    total_meals=$(echo "$output" | grep -c "is eating")
    
    # Calculate expected meals
    expected_meals=$((philos * meals))
    
    # Calculate efficiency
    if [ $expected_meals -gt 0 ]; then
        efficiency=$((total_meals * 100 / expected_meals))
    else
        efficiency=0
    fi
    
    # Store results
    results["$name,duration"]=$duration_ms
    results["$name,deaths"]=$deaths
    results["$name,meals"]=$total_meals
    results["$name,expected"]=$expected_meals
    results["$name,efficiency"]=$efficiency
    results["$name,exit_code"]=$exit_code
    
    # Print immediate results
    if [ $exit_code -eq 124 ]; then
        echo -e "  ${YELLOW}⏱️  Timeout after 60s${NC}"
    elif [ $deaths -gt 0 ]; then
        echo -e "  ${RED}💀 $deaths deaths in ${duration_ms}ms${NC}"
    else
        echo -e "  ${GREEN}✅ Completed in ${duration_ms}ms${NC}"
    fi
    
    echo -e "  📊 Meals: $total_meals/$expected_meals (${efficiency}% efficiency)"
    echo ""
}

# Performance test suites
run_basic_benchmarks() {
    print_header "BASIC PERFORMANCE BENCHMARKS"
    
    benchmark_scenario "2_philos_basic" 2 1000 200 200 5
    benchmark_scenario "3_philos_basic" 3 800 200 200 5
    benchmark_scenario "4_philos_basic" 4 800 200 200 5
    benchmark_scenario "5_philos_basic" 5 800 200 200 5
    benchmark_scenario "10_philos_basic" 10 800 200 200 5
}

run_stress_benchmarks() {
    print_header "STRESS TEST BENCHMARKS"
    
    benchmark_scenario "50_philos_stress" 50 2000 200 200 10
    benchmark_scenario "100_philos_stress" 100 3000 200 200 5
    benchmark_scenario "200_philos_stress" 200 5000 200 200 3
}

run_timing_benchmarks() {
    print_header "TIMING VARIATION BENCHMARKS"
    
    benchmark_scenario "fast_timing" 5 500 50 50 10
    benchmark_scenario "medium_timing" 5 1000 200 200 10
    benchmark_scenario "slow_timing" 5 3000 500 500 10
    benchmark_scenario "very_fast" 3 300 10 10 20
    benchmark_scenario "asymmetric_timing" 4 2000 100 500 10
}

run_meal_benchmarks() {
    print_header "MEAL COUNT BENCHMARKS"
    
    benchmark_scenario "few_meals" 5 1000 200 200 3
    benchmark_scenario "many_meals" 5 2000 200 200 50
    benchmark_scenario "very_many_meals" 3 3000 200 200 100
}

run_edge_case_benchmarks() {
    print_header "EDGE CASE BENCHMARKS"
    
    benchmark_scenario "tight_timing_610" 3 610 200 100 10
    benchmark_scenario "tight_timing_410" 3 410 200 50 5
    benchmark_scenario "single_philo" 1 1000 200 200 1
    benchmark_scenario "minimal_times" 4 100 10 10 5
}

run_endurance_benchmarks() {
    print_header "ENDURANCE BENCHMARKS"
    
    benchmark_scenario "long_running" 5 10000 200 200 100
    benchmark_scenario "marathon" 3 20000 500 500 200
}

# System resource monitoring
monitor_resources() {
    local name="$1"
    shift
    local cmd="$@"
    
    # Start resource monitoring
    echo "Monitoring resources for: $name"
    
    # Run the command in background
    eval "$cmd" &
    local pid=$!
    
    # Monitor resources
    max_cpu=0
    max_mem=0
    samples=0
    
    while kill -0 $pid 2>/dev/null; do
        if ps -p $pid >/dev/null 2>&1; then
            cpu=$(ps -p $pid -o %cpu --no-headers | tr -d ' ')
            mem=$(ps -p $pid -o %mem --no-headers | tr -d ' ')
            
            if (( $(echo "$cpu > $max_cpu" | bc -l) )); then
                max_cpu=$cpu
            fi
            
            if (( $(echo "$mem > $max_mem" | bc -l) )); then
                max_mem=$mem
            fi
            
            ((samples++))
        fi
        sleep 0.1
    done
    
    wait $pid
    local exit_code=$?
    
    echo "  Max CPU: ${max_cpu}%"
    echo "  Max Memory: ${max_mem}%"
    echo "  Samples: $samples"
    echo "  Exit Code: $exit_code"
    echo ""
    
    return $exit_code
}

# Generate performance report
generate_performance_report() {
    print_header "PERFORMANCE ANALYSIS REPORT"
    
    echo -e "${PURPLE}📈 EXECUTION TIME ANALYSIS${NC}"
    echo "================================"
    
    # Sort by execution time
    for scenario in "2_philos_basic" "3_philos_basic" "4_philos_basic" "5_philos_basic" "10_philos_basic"; do
        if [[ -v results["$scenario,duration"] ]]; then
            duration=${results["$scenario,duration"]}
            efficiency=${results["$scenario,efficiency"]}
            deaths=${results["$scenario,deaths"]}
            
            if [ $deaths -eq 0 ]; then
                status="✅"
            else
                status="❌"
            fi
            
            printf "%-20s: %6dms (%3d%% eff) %s\n" "$scenario" "$duration" "$efficiency" "$status"
        fi
    done
    
    echo ""
    echo -e "${PURPLE}🔥 STRESS TEST RESULTS${NC}"
    echo "================================"
    
    for scenario in "50_philos_stress" "100_philos_stress" "200_philos_stress"; do
        if [[ -v results["$scenario,duration"] ]]; then
            duration=${results["$scenario,duration"]}
            efficiency=${results["$scenario,efficiency"]}
            deaths=${results["$scenario,deaths"]}
            exit_code=${results["$scenario,exit_code"]}
            
            if [ $exit_code -eq 124 ]; then
                status="⏱️"
            elif [ $deaths -eq 0 ]; then
                status="✅"
            else
                status="❌"
            fi
            
            printf "%-20s: %6dms (%3d%% eff) %s\n" "$scenario" "$duration" "$efficiency" "$status"
        fi
    done
    
    echo ""
    echo -e "${PURPLE}⚡ TIMING SENSITIVITY${NC}"
    echo "================================"
    
    for scenario in "fast_timing" "medium_timing" "slow_timing" "very_fast"; do
        if [[ -v results["$scenario,duration"] ]]; then
            duration=${results["$scenario,duration"]}
            efficiency=${results["$scenario,efficiency"]}
            deaths=${results["$scenario,deaths"]}
            
            if [ $deaths -eq 0 ]; then
                status="✅"
            else
                status="❌"
            fi
            
            printf "%-20s: %6dms (%3d%% eff) %s\n" "$scenario" "$duration" "$efficiency" "$status"
        fi
    done
    
    echo ""
    echo -e "${PURPLE}🎯 EDGE CASE PERFORMANCE${NC}"
    echo "================================"
    
    for scenario in "tight_timing_610" "tight_timing_410" "single_philo" "minimal_times"; do
        if [[ -v results["$scenario,duration"] ]]; then
            duration=${results["$scenario,duration"]}
            efficiency=${results["$scenario,efficiency"]}
            deaths=${results["$scenario,deaths"]}
            
            if [ $deaths -eq 0 ]; then
                status="✅"
            else
                status="❌"
            fi
            
            printf "%-20s: %6dms (%3d%% eff) %s\n" "$scenario" "$duration" "$efficiency" "$status"
        fi
    done
    
    # Calculate overall statistics
    total_scenarios=0
    successful_scenarios=0
    total_duration=0
    
    for key in "${!results[@]}"; do
        if [[ $key == *",deaths" ]]; then
            total_scenarios=$((total_scenarios + 1))
            deaths=${results[$key]}
            scenario_name=${key%,deaths}
            duration=${results["$scenario_name,duration"]}
            total_duration=$((total_duration + duration))
            
            if [ $deaths -eq 0 ]; then
                successful_scenarios=$((successful_scenarios + 1))
            fi
        fi
    done
    
    if [ $total_scenarios -gt 0 ]; then
        success_rate=$((successful_scenarios * 100 / total_scenarios))
        avg_duration=$((total_duration / total_scenarios))
        
        echo ""
        echo -e "${PURPLE}📊 OVERALL STATISTICS${NC}"
        echo "================================"
        echo "Total Scenarios: $total_scenarios"
        echo "Successful: $successful_scenarios"
        echo "Success Rate: ${success_rate}%"
        echo "Average Duration: ${avg_duration}ms"
        
        if [ $success_rate -ge 90 ]; then
            echo -e "${GREEN}🎉 Excellent performance!${NC}"
        elif [ $success_rate -ge 75 ]; then
            echo -e "${YELLOW}⚠️  Good performance with room for improvement${NC}"
        else
            echo -e "${RED}❌ Performance needs significant improvement${NC}"
        fi
    fi
}

# Main execution
main() {
    echo -e "${PURPLE}=========================================${NC}"
    echo -e "${PURPLE}   PHILOSOPHERS BENCHMARK SUITE         ${NC}"
    echo -e "${PURPLE}=========================================${NC}"
    echo ""
    
    if [ ! -f "./philo" ]; then
        echo -e "${RED}Program './philo' not found. Please build first.${NC}"
        exit 1
    fi
    
    # Check if bc is available for floating point calculations
    if ! command -v bc >/dev/null 2>&1; then
        echo -e "${YELLOW}Warning: 'bc' not found. Some calculations may be limited.${NC}"
    fi
    
    start_time=$(date +%s)
    
    run_basic_benchmarks
    run_stress_benchmarks
    run_timing_benchmarks
    run_meal_benchmarks
    run_edge_case_benchmarks
    
    # Only run endurance tests if requested
    if [[ "$1" == "--endurance" ]]; then
        run_endurance_benchmarks
    fi
    
    end_time=$(date +%s)
    total_time=$((end_time - start_time))
    
    generate_performance_report
    
    echo ""
    echo -e "${BLUE}Total benchmark time: ${total_time} seconds${NC}"
}

# Show usage if help requested
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    echo "Philosophers Benchmark Suite"
    echo ""
    echo "Usage: $0 [--endurance] [--help]"
    echo ""
    echo "Options:"
    echo "  --endurance    Include long-running endurance tests"
    echo "  --help, -h     Show this help message"
    echo ""
    echo "This script runs comprehensive performance benchmarks on the philosophers program."
    exit 0
fi

main "$@"