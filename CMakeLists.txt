cmake_minimum_required(VERSION 3.5)
project( DSA_GROUP_20 )
find_package(PkgConfig REQUIRED)

add_executable(DSA_GROUP_20
    src/main.cpp
    src/hash_table.cpp
    src/graph.cpp
    src/union_find.cpp
    src/min_heap.cpp
    src/avl_tree.cpp
    src/segment_tree.cpp
    src/trie.cpp
    src/dispatcher.cpp
    
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
target_link_libraries(DSA_GROUP_20)