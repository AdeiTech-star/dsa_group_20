#!/usr/bin/env bash
# run_tests.sh — build every test target and save output to build/test_results/
# Usage: bash run_tests.sh  (from project root)

set -euo pipefail

BUILD_DIR="build"
RESULTS_DIR="$BUILD_DIR/test_results"

# Build all targets
cmake --build "$BUILD_DIR" 2>&1

mkdir -p "$RESULTS_DIR"

TESTS=(
    test_graph
    test_min_heap
    test_hash_table
    test_avl_tree
    test_segment_tree
    test_union_find
    test_trie
    test_dijkstra
    test_dispatcher
)

overall_pass=0
overall_fail=0

for t in "${TESTS[@]}"; do
    log="$RESULTS_DIR/${t}.log"
    printf "Running %-22s ... " "$t"
    if ./"$BUILD_DIR/$t" > "$log" 2>&1; then
        echo "PASS  (log: $log)"
        overall_pass=$(( overall_pass + 1 ))
    else
        echo "FAIL  (log: $log)"
        overall_fail=$(( overall_fail + 1 ))
    fi
done

echo ""
echo "Results: $overall_pass passed, $overall_fail failed."
[ "$overall_fail" -eq 0 ] && exit 0 || exit 1
