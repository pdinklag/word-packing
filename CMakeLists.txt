cmake_minimum_required(VERSION 3.4.1 FATAL_ERROR)
project(word-packing)

# set C++ build flags
set(CXX_STANDARD c++20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=gnu++20 ${GCC_WARNINGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb")

# create interface library
add_library(word-packing INTERFACE)
target_include_directories(word-packing INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# provide tests if standalone
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    enable_testing()
    add_subdirectory(test)
endif()
