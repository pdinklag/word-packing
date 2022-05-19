if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    message(INFO "CMAKE_BUILD_TYPE not set, defaulting to Release for benchmark.")
elseif(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    message(WARNING "Building the benchmark in non-release mode (CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}), this will considerably worsen performance!")
endif()

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark word-packing)
