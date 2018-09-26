/*
 * Copyright 2018 Fairtide Pte. Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#pragma once

#include <string>

namespace aeron {
namespace archive {

struct Configuration {
public:
    // constructors
    Configuration();
    explicit Configuration(const std::string& filename);

    Configuration(const Configuration&) = default;
    Configuration(Configuration&&) = default;

    // parameters
    std::int64_t messageTimeoutNs;
    std::string controlChannel;
    std::int32_t controlStreamId;
    std::string localControlChannel;
    std::int32_t localControlStreamId;
    std::string controlResponseChannel;
    std::int32_t controlResponseStreamId;
    std::string recordingEventsChannel;
    std::int32_t recordingEventsStreamId;
    bool controlTermBufferSparse;
    std::int32_t controlTermBufferLength;
    std::int32_t controlMtuLength;
};

}  // namespace archive
}  // namespace aeron
