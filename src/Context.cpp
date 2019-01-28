/*
 * Copyright 2018-2019 Fairtide Pte. Ltd.
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
#include "ChannelUri.h"

namespace {
// TODO: CommonContext
const std::string TERM_LENGTH_PARAM_NAME = "term-length";
const std::string MTU_LENGTH_PARAM_NAME = "mtu";
const std::string SPARSE_PARAM_NAME = "sparse";
}  // namespace

namespace aeron {
namespace archive {

Context::Context() { controlRequestChannel(cfg_.controlChannel); }

Context::Context(const Configuration& cfg)
    : cfg_(cfg) {
    controlRequestChannel(cfg_.controlChannel);
}

void Context::conclude() {
    if (aeronDirectoryName_.empty()) {
        aeronDirectoryName_ = aeron::Context().defaultAeronPath();
    }

    if (!aeron_) {
        aeronContext_ = std::make_shared<aeron::Context>(aeron::Context());
        aeronContext_->aeronDir(aeronDirectoryName_);
        aeron_ = aeron::Aeron::connect(*aeronContext_);
    }
}

Context& Context::messageTimeoutNs(std::int64_t value) {
    cfg_.messageTimeoutNs = value;
    return *this;
}

Context& Context::controlRequestStreamId(std::int32_t value) {
    cfg_.controlStreamId = value;
    return *this;
}

Context& Context::localControlChannel(const std::string& value) {
    cfg_.localControlChannel = value;
    return *this;
}

Context& Context::localControlStreamId(std::int32_t value) {
    cfg_.localControlStreamId = value;
    return *this;
}

Context& Context::controlResponseChannel(const std::string& value) {
    cfg_.controlResponseChannel = value;
    return *this;
}

Context& Context::controlResponseStreamId(std::int32_t value) {
    cfg_.controlResponseStreamId = value;
    return *this;
}

Context& Context::recordingEventsChannel(const std::string& value) {
    cfg_.recordingEventsChannel = value;
    return *this;
}

Context& Context::recordingEventsStreamId(std::int32_t value) {
    cfg_.recordingEventsStreamId = value;
    return *this;
}

Context& Context::controlTermBufferSparse(bool value) {
    cfg_.controlTermBufferSparse = value;
    return *this;
}

Context& Context::controlTermBufferLength(std::int32_t value) {
    cfg_.controlTermBufferLength = value;
    return *this;
}

Context& Context::controlMtuLength(std::int32_t value) {
    cfg_.controlMtuLength = value;
    return *this;
}

Context& Context::aeronDirectoryName(const std::string& value) {
    aeronDirectoryName_ = value;
    return *this;
}

Context& Context::aeron(const std::shared_ptr<aeron::Aeron>& value) {
    aeron_ = value;
    return *this;
}

Context& Context::controlRequestChannel(const std::string& value) {
    ChannelUri uri = ChannelUri::parse(value);
    uri.put(TERM_LENGTH_PARAM_NAME, std::to_string(cfg_.controlTermBufferLength));
    uri.put(MTU_LENGTH_PARAM_NAME, std::to_string(cfg_.controlMtuLength));
    uri.put(SPARSE_PARAM_NAME, std::to_string(cfg_.controlTermBufferSparse));
    controlRequestChannel_ = uri.toString();
    return *this;
}

std::int64_t Context::messageTimeoutNs() const { return cfg_.messageTimeoutNs; }
std::int32_t Context::controlRequestStreamId() const { return cfg_.controlStreamId; }
const std::string& Context::localControlChannel() const { return cfg_.localControlChannel; }
std::int32_t Context::localControlStreamId() const { return cfg_.localControlStreamId; }
const std::string& Context::controlResponseChannel() const { return cfg_.controlResponseChannel; }
std::int32_t Context::controlResponseStreamId() const { return cfg_.controlResponseStreamId; }
const std::string& Context::recordingEventsChannel() const { return cfg_.recordingEventsChannel; }
std::int32_t Context::recordingEventsStreamId() const { return cfg_.recordingEventsStreamId; }
bool Context::controlTermBufferSparse() const { return cfg_.controlTermBufferSparse; }
std::int32_t Context::controlTermBufferLength() const { return cfg_.controlTermBufferLength; }
std::int32_t Context::controlMtuLength() const { return cfg_.controlMtuLength; }

const std::string& Context::aeronDirectoryName() const { return aeronDirectoryName_; }
const std::shared_ptr<aeron::Aeron>& Context::aeron() const { return aeron_; }
const std::string& Context::controlRequestChannel() const { return controlRequestChannel_; }

}  // namespace archive
}  // namespace aeron
