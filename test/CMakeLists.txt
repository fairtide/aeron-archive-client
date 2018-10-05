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

function(aeron_archive_test name file)
    add_executable(${name} ${file})
    target_include_directories(${name}
        PRIVATE ${GTEST_INCLUDE_DIR} ${GMOCK_INCLUDE_DIR})
    target_link_libraries(${name}
        aeron_archive_client
        ${GTEST_LIBS} ${GMOCK_LIBS}
        ${CMAKE_THREAD_LIBS_INIT})
    add_dependencies(${name} googletest_project)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

# tests
aeron_archive_test(ChannelUriTest ChannelUriTest.cpp)
aeron_archive_test(Configuration Configuration.cpp)
aeron_archive_test(ContextTest ContextTest.cpp)
