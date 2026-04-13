cmake_minimum_required(VERSION 3.10)
project(project)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# Hash table library
add_library(hash_table src/hash_table.cpp)

# Hash table test executable
add_executable(test_hash_table tests/test_hash_table.cpp)
target_link_libraries(test_hash_table hash_table)