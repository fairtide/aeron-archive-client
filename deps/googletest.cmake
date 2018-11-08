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

# install GTEST
set(GTEST_VERSION "release-1.8.1" CACHE STRING "gtest version")
set(GTEST_PREFIX ${THIRDPARTY_BINARY_DIR}/googletest)
set(GTEST_SOURCE_DIR ${GTEST_PREFIX}/src/googletest)

ExternalProject_Add(googletest_project
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG ${GTEST_VERSION}
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    PREFIX "${GTEST_PREFIX}"
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
        -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    BUILD_BYPRODUCTS
        "${GTEST_PREFIX}/src/googletest_project-build/googlemock/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gmock${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "${GTEST_PREFIX}/src/googletest_project-build/googlemock/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gmock_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "${GTEST_PREFIX}/src/googletest_project-build/googlemock/gtest/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "${GTEST_PREFIX}/src/googletest_project-build/googlemock/gtest/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
     INSTALL_COMMAND ""
)

ExternalProject_Get_Property(googletest_project source_dir)

set(GTEST_INCLUDE_DIR ${source_dir}/googletest/include CACHE STRING "gtest include files")
set(GMOCK_INCLUDE_DIR ${source_dir}/googlemock/include CACHE STRING "gmock include files")

ExternalProject_Get_Property(googletest_project binary_dir)

set(GTEST_LIBS
    ${binary_dir}/googlemock/${CMAKE_CFG_INTDIR}/gtest/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${binary_dir}/googlemock/${CMAKE_CFG_INTDIR}/gtest/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}
    CACHE STRING "gtest libraries"
)

set(GMOCK_LIBS
    ${binary_dir}/googlemock/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gmock${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${binary_dir}/googlemock/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gmock_main${CMAKE_STATIC_LIBRARY_SUFFIX}
    CACHE STRING "gmock libraries"
)
