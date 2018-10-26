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

add_library(sample_utils STATIC
    SamplesUtil.cpp)

target_link_libraries(sample_utils
    aeron_archive_client
    ${Aeron_CLIENT_LIB}
)

find_package(Boost REQUIRED
    COMPONENTS program_options)

function(aeron_archive_sample name file)
    add_executable(${name} ${file})
    target_link_libraries(${name}
        aeron_archive_client
        sample_utils
        Boost::program_options
        ${CMAKE_THREAD_LIBS_INIT}
    )
endfunction()

# sample apps
aeron_archive_sample(DriverControl DriverControl.cpp)
aeron_archive_sample(RecordedBasicPublisher RecordedBasicPublisher.cpp)
aeron_archive_sample(RecordingThroughput RecordingThroughput.cpp)
aeron_archive_sample(ReplayedBasicSubscriber ReplayedBasicSubscriber.cpp)

install(
    TARGETS
        DriverControl
        RecordedBasicPublisher
        RecordingThroughput
        ReplayedBasicSubscriber
    DESTINATION
        bin)

