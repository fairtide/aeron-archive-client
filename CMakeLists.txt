cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
cmake_policy(VERSION 3.0.2)

find_package(Threads)
include(CTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Wno-unused-parameter -std=c++14 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DDISABLE_BOUNDS_CHECKS")

add_subdirectory(deps)
add_subdirectory(src)
add_subdirectory(test)

