
cmake_minimum_required(VERSION 3.10)
project(UrbanIncidentResponseSystem VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# adding all headers
include_directories(include)

# the libraries for each data structure with reusability purpose
add_library(graph_lib src/graph.cpp)
add_library(hash_table_lib src/hash_table.cpp)
add_library(min_heap_lib src/min_heap.cpp)
add_library(avl_tree_lib src/avl_tree.cpp)

# the executables
# The entry point for actual program running
add_executable(urban_irs src/main.cpp)
target_link_libraries(urban_irs graph_lib hash_table_lib min_heap_lib)

#this is for all executables for each test file
# Min-Heap Test
add_executable(test_min_heap tests/test_min_heap.cpp)
target_link_libraries(test_min_heap min_heap_lib)

# Graph Test 
add_executable(test_graph tests/test_graph.cpp)
target_link_libraries(test_graph graph_lib)

# Hash Table Test 
add_executable(test_hash_table tests/test_hash_table.cpp)
target_link_libraries(test_hash_table hash_table_lib)

# AVL Tree Test
add_executable(test_avl_tree tests/test_avl_tree.cpp)
target_link_libraries(test_avl_tree avl_tree_lib)
