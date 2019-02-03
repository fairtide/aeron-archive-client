#
# Copyright 2018-2019 Fairtide Pte. Ltd.
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

# install Aeron
set(Aeron_VERSION "1.15.0" CACHE STRING "Aeron version")
set(Aeron_PREFIX ${THIRDPARTY_BINARY_DIR}/aeron)
set(Aeron_SOURCE_DIR ${Aeron_PREFIX}/src/aeron)
set(Aeron_CLIENT_LIB_PATH ${CMAKE_CFG_INTDIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}aeron_client${CMAKE_STATIC_LIBRARY_SUFFIX})

ExternalProject_Add(aeron_project
    GIT_REPOSITORY https://github.com/real-logic/aeron
    GIT_TAG ${Aeron_VERSION}
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    PREFIX "${Aeron_PREFIX}"
    CMAKE_ARGS
        -DAERON_TESTS=FALSE
        -DAERON_BUILD_SAMPLES=FALSE
        -DBUILD_AERON_DRIVER=OFF
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
        -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    BUILD_BYPRODUCTS "${Aeron_PREFIX}/src/aeron_project-build/${Aeron_CLIENT_LIB_PATH}"
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(aeron_project source_dir)
ExternalProject_Get_Property(aeron_project binary_dir)

set(Aeron_INCLUDE_DIR ${source_dir}/aeron-client/src/main/cpp CACHE STRING "Aeron include files")
set(Aeron_RESOURCES_DIR ${source_dir}/aeron-archive/src/main/resources CACHE STRING "Aeron resources")
set(Aeron_CLIENT_LIB ${binary_dir}/${Aeron_CLIENT_LIB_PATH} CACHE STRING "Aeron client library")

add_library(aeron INTERFACE)
add_dependencies(aeron aeron_project)
target_include_directories(aeron INTERFACE ${Aeron_INCLUDE_DIR})
target_link_libraries(aeron INTERFACE ${Aeron_CLIENT_LIB})
