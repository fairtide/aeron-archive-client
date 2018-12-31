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

# install SBE
set(SBE_VERSION "1.10.1" CACHE STRING "SBE version")
set(SBE_PREFIX ${THIRDPARTY_BINARY_DIR}/sbe)
set(SBE_SOURCE_DIR ${SBE_PREFIX}/src/sbe)

ExternalProject_Add(sbe_project
    GIT_REPOSITORY https://github.com/real-logic/simple-binary-encoding
    GIT_TAG ${SBE_VERSION}
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    PREFIX "${SBE_PREFIX}"
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
        -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    CONFIGURE_COMMAND ""
    BUILD_COMMAND "<SOURCE_DIR>/gradlew" ":sbe-all:assemble"
    BUILD_IN_SOURCE TRUE
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(sbe_project source_dir)

set(SBE_INCLUDE_DIR ${source_dir}/sbe-tool/src/main/cpp CACHE STRING "SBE include files")
message(STATUS "SBE includes: ${SBE_INCLUDE_DIR}")

set(SBE_TOOL_JAR "${source_dir}/sbe-all/build/libs/sbe-all-${SBE_VERSION}.jar" CACHE STRING "SBE Tool")
message(STATUS "SBE tool JAR: ${SBE_TOOL_JAR}")

add_library(sbe INTERFACE)
add_dependencies(sbe sbe_project)
target_include_directories(sbe INTERFACE ${SBE_INCLUDE_DIR})

