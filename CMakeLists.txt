
cmake_minimum_required(VERSION 3.10)
project(UrbanIncidentResponseSystem VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# adding all headers
include_directories(include)

# ─── 1. CORE LIBRARIES ──────────────────────────────────────────
# We create a library for each data structure so they can be reused
add_library(graph_lib src/graph.cpp)
add_library(hash_table_lib src/hash_table.cpp)
add_library(min_heap_lib src/min_heap.cpp)

# ─── 2. MAIN EXECUTABLE ─────────────────────────────────────────
# This is the entry point for your actual application
add_executable(urban_irs src/main.cpp)
target_link_libraries(urban_irs graph_lib hash_table_lib min_heap_lib)

# ─── 3. TEST EXECUTABLES ────────────────────────────────────────
# We create separate executables for each test file

# Min-Heap Test
add_executable(test_min_heap tests/test_min_heap.cpp)
target_link_libraries(test_min_heap min_heap_lib)

# Graph Test (once you create it)
add_executable(test_graph tests/test_graph.cpp)
target_link_libraries(test_graph graph_lib)

# Hash Table Test (once you create it)
add_executable(test_hash_table tests/test_hash_table.cpp)
target_link_libraries(test_hash_table hash_table_lib)
