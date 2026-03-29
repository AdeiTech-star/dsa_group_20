cmake_minimum_required(VERSION 3.10)
project(UrbanIncidentSystem)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(googletest)

include_directories(include)

# Main code
add_library(core
    
)

# Tests
enable_testing()

add_executable(runTests

)

target_link_libraries(runTests core gtest gtest_main)

add_test(NAME runTests COMMAND runTests)