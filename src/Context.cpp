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


#include "Context.h"

namespace aeron {
namespace archive {

Context::Context() {}

Context::Context(const Configuration& cfg) : cfg_(cfg) {}

Context & Context::messageTimeoutNs(std::int64_t value) { cfg_.messageTimeoutNs = value; return *this; }
Context & Context::controlChannel(const std::string & value) { cfg_.controlChannel = value; return *this; }
Context & Context::controlStreamId(std::int32_t value) { cfg_.controlStreamId = value; return *this; }
Context & Context::localControlChannel(const std::string & value) { cfg_.localControlChannel = value; return *this; }
Context & Context::localControlStreamId(std::int32_t value) { cfg_.localControlStreamId = value; return *this; }
Context & Context::controlResponseChannel(const std::string & value) { cfg_.controlResponseChannel = value; return *this; }
Context & Context::controlResponseStreamId(std::int32_t value) { cfg_.controlResponseStreamId = value; return *this; }
Context & Context::recordingEventsChannel(const std::string & value) { cfg_.recordingEventsChannel = value; return *this; }
Context & Context::recordingEventsStreamId(std::int32_t value) { cfg_.recordingEventsStreamId = value; return *this; }
Context & Context::controlTermBufferSparse(bool value) { cfg_.controlTermBufferSparse = value; return *this; }
Context & Context::controlTermBufferLength(std::int32_t value) { cfg_.controlTermBufferLength = value; return *this; }
Context & Context::controlMtuLength(std::int32_t value) { cfg_.controlMtuLength = value; return *this; }

std::int64_t Context::messageTimeoutNs() const { return cfg_.messageTimeoutNs; }
const std::string & Context::controlChannel() const { return cfg_.controlChannel; }
std::int32_t Context::controlStreamId() const { return cfg_.controlStreamId; }
const std::string & Context::localControlChannel() const { return cfg_.localControlChannel; }
std::int32_t Context::localControlStreamId() const { return cfg_.localControlStreamId; }
const std::string & Context::controlResponseChannel() const { return cfg_.controlResponseChannel; }
std::int32_t Context::controlResponseStreamId() const { return cfg_.controlResponseStreamId; }
const std::string & Context::recordingEventsChannel() const { return cfg_.recordingEventsChannel; }
std::int32_t Context::recordingEventsStreamId() const { return cfg_.recordingEventsStreamId; }
bool Context::controlTermBufferSparse() const { return cfg_.controlTermBufferSparse; }
std::int32_t Context::controlTermBufferLength() const { return cfg_.controlTermBufferLength; }
std::int32_t Context::controlMtuLength() const { return cfg_.controlMtuLength; }

}
}
