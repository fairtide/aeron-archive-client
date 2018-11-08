#
# Copyright 2018 Fairtide Pte. Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
cmake_policy(VERSION 3.0.2)

file(STRINGS version.txt VERSION_TXT LIMIT_COUNT 1 REGEX "^[0-9]+(\\.[0-9]+)+")
string(REGEX REPLACE "^([0-9]+(\\.[0-9]+)+).*$" "\\1" VERSION_FROM_FILE "${VERSION_TXT}")

project("aeron-archive-client" VERSION "${VERSION_FROM_FILE}")

option(SANITISE_BUILD "Enable sanitise options" OFF)
option(COVERAGE_BUILD "Enable code coverage" OFF)

find_package(Threads)

find_package(Doxygen)

enable_testing()
include(CTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Wno-unused-parameter -std=c++14 -g")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -DDISABLE_BOUNDS_CHECKS")
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
endif()


if(SANITISE_BUILD)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined")
endif(SANITISE_BUILD)

if (COVERAGE_BUILD)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -fno-inline --coverage -g")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fno-inline --coverage -g")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif(COVERAGE_BUILD)

if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

    add_custom_target(
        doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )

    # install the doc if it has been built
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc DESTINATION share OPTIONAL)
endif(DOXYGEN_FOUND)

add_subdirectory(deps)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(samples)

