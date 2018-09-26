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

#include "Configuration.h"

namespace aeron {
namespace archive {

class Context {
public:
    Context();
    explicit Context(const Configuration& cfg);

    Context & messageTimeoutNs(std::int64_t value);
    Context & controlChannel(const std::string & value);
    Context & controlStreamId(std::int32_t value);
    Context & localControlChannel(const std::string & value);
    Context & localControlStreamId(std::int32_t value);
    Context & controlResponseChannel(const std::string & value);
    Context & controlResponseStreamId(std::int32_t value);
    Context & recordingEventsChannel(const std::string & value);
    Context & recordingEventsStreamId(std::int32_t value);
    Context & controlTermBufferSparse(bool value);
    Context & controlTermBufferLength(std::int32_t value);
    Context & controlMtuLength(std::int32_t value);

    std::int64_t messageTimeoutNs() const;
    const std::string & controlChannel() const;
    std::int32_t controlStreamId() const;
    const std::string & localControlChannel() const;
    std::int32_t localControlStreamId() const;
    const std::string & controlResponseChannel() const;
    std::int32_t controlResponseStreamId() const;
    const std::string & recordingEventsChannel() const;
    std::int32_t recordingEventsStreamId() const;
    bool controlTermBufferSparse() const;
    std::int32_t controlTermBufferLength() const;
    std::int32_t controlMtuLength() const;

private:
    Configuration cfg_;
};

}
}

