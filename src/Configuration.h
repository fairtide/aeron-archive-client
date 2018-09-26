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

class Configuration
{
public:
    Configuration();
    Configuration(const std::string & filename);

    std::int64_t messageTimeoutNs() const { return messageTimeoutNs_; }
    const std::string & controlChannel() const { return controlChannel_; }
    std::int32_t controlStreamId() const { return controlStreamId_; }
    const std::string & localControlChannel() const { return localControlChannel_; }
    std::int32_t localControlStreamId() const { return localControlStreamId_; }
    const std::string & controlResponseChannel() const { return controlResponseChannel_; }
    std::int32_t controlResponseStreamId() const { return controlResponseStreamId_; }
    const std::string & recordingEventsChannel() const { return recordingEventsChannel_; }
    std::int32_t recordingEventsStreamId() const { return recordingEventsStreamId_; }
    bool controlTermBufferSparse() const { return controlTermBufferSparse_; }
    std::int32_t controlTermBufferLength() const { return controlTermBufferLength_; }
    std::int32_t controlMtuLength() const { return controlMtuLength_; }

private:
    std::int64_t messageTimeoutNs_;
    std::string controlChannel_;
    std::int32_t controlStreamId_;
    std::string localControlChannel_;
    std::int32_t localControlStreamId_;
    std::string controlResponseChannel_;
    std::int32_t controlResponseStreamId_;
    std::string recordingEventsChannel_;
    std::int32_t recordingEventsStreamId_;
    bool controlTermBufferSparse_;
    std::int32_t controlTermBufferLength_;
    std::int32_t controlMtuLength_;
};

}
}

