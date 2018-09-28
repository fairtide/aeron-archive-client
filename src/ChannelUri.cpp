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


#include "ChannelUri.h"

namespace aeron {
namespace archive {

ChannelUri::ChannelUri() {}

const std::string & ChannelUri::scheme() const { return AERON_SCHEME; }
const std::string & ChannelUri::prefix() const { return prefix_; }
const std::string & ChannelUri::media() const { return media_; }

void ChannelUri::put(const std::string & key, const std::string & value) {
    params_[key] = value;
}

boost::optional<std::string> ChannelUri::get(const std::string & key) {
    auto it = params_.find(key);
    if (it == params_.end()) {
        return {};
    }

    return it->second;
}

boost::optional<std::string> ChannelUri::get(const std::string & key, const std::string & defaultValue) {
    auto it = params_.find(key);
    if (it == params_.end()) {
        return defaultValue;
    }

    return it->second;
}

std::string ChannelUri::toString() const { return ""; }

ChannelUri ChannelUri::parse(const std::string & channel) { return ChannelUri(); }

std::string ChannelUri::addSessionId(const std::string & channel, std::int32_t sessionId)
{
    ChannelUri channelUri = ChannelUri::parse(channel);
    channelUri.put("aaa", "bbb");
    return channelUri.toString();
}

const std::string ChannelUri::AERON_SCHEME { "aeron" };

}
}
