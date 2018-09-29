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


#include <algorithm>
#include <sstream>

#include <util/Exceptions.h>

#include "ChannelUri.h"

namespace {

const std::string SPY_PREFIX = "aeron-spy:";
const std::string SPY_QUALIFIER = "aeron-spy";
const std::string AERON_SCHEME = "aeron";
const std::string AERON_PREFIX = AERON_SCHEME + ":";
const std::string SESSION_ID_PARAM_NAME = "session-id";

enum class State { MEDIA, PARAMS_KEY, PARAMS_VALUE };

bool startsWith(const std::string& input, std::int32_t position, const std::string& prefix) {
    if (input.size() < position + prefix.size())
        return false;

    return std::equal(prefix.begin(), prefix.end(), std::next(input.begin(), position));
}

bool startsWith(const std::string& input, const std::string& prefix) { return startsWith(input, 0, prefix); }

}  // namespace

namespace aeron {
namespace archive {

ChannelUri::ChannelUri(const std::string& prefix, const std::string& media, ParamsMap&& params)
    : prefix_(prefix)
    , media_(media)
    , params_(std::move(params)) {
    // TODO: tags are not parsed, however these are not needed for the archive client
}

ChannelUri::ChannelUri(const std::string& media, ParamsMap&& params)
    : ChannelUri("", media, std::move(params)) {}

const std::string& ChannelUri::scheme() const { return AERON_SCHEME; }

const std::string& ChannelUri::prefix() const { return prefix_; }

const std::string& ChannelUri::media() const { return media_; }

void ChannelUri::put(const std::string& key, const std::string& value) { params_[key] = value; }

boost::optional<std::string> ChannelUri::get(const std::string& key) {
    auto it = params_.find(key);
    if (it == params_.end()) {
        return {};
    }
    return it->second;
}

boost::optional<std::string> ChannelUri::get(const std::string& key, const std::string& defaultValue) {
    auto it = params_.find(key);
    if (it == params_.end()) {
        return defaultValue;
    }
    return it->second;
}

std::string ChannelUri::toString() const {
    std::ostringstream ss;

    if (!prefix_.empty()) {
        ss << prefix_;
        if (':' != *prefix_.rbegin()) {
            ss << ':';
        }
    }

    ss << AERON_PREFIX << media_;

    if (!params_.empty()) {
        ss << '?';

        std::size_t ps = params_.size();

        for (const auto& p : params_) {
            ss << p.first << '=' << p.second << (--ps ? "|" : "");
        }
    }

    return ss.str();
}

ChannelUri ChannelUri::parse(const std::string& str) {
    std::size_t position = 0;
    std::string prefix;

    if (startsWith(str, SPY_PREFIX)) {
        prefix = SPY_QUALIFIER;
        position += SPY_PREFIX.size();
    }

    if (!startsWith(str, position, AERON_PREFIX)) {
        throw aeron::util::IllegalArgumentException("Aeron URIs must start with 'aeron:', found: '" + str + "'",
                                                    SOURCEINFO);
    } else {
        position += AERON_PREFIX.size();
    }

    std::string media, key;
    State state = State::MEDIA;

    std::string buffer;
    buffer.reserve(str.size());

    ParamsMap params;

    while (position < str.size()) {
        char c = str[position++];

        switch (state) {
            case State::MEDIA:
                switch (c) {
                    case '?':
                        media = buffer;
                        buffer.clear();
                        state = State::PARAMS_KEY;
                        break;

                    case ':':
                        throw aeron::util::IllegalArgumentException("encountered ':' within media definition",
                                                                    SOURCEINFO);

                    default:
                        buffer.push_back(c);
                }

                break;

            case State::PARAMS_KEY:
                switch (c) {
                    case '=':
                        key = buffer;
                        buffer.clear();
                        state = State::PARAMS_VALUE;
                        break;

                    default:
                        buffer.push_back(c);
                }

                break;

            case State::PARAMS_VALUE:
                switch (c) {
                    case '|':
                        params[key] = buffer;
                        buffer.clear();
                        state = State::PARAMS_KEY;
                        break;

                    default:
                        buffer.push_back(c);
                }

                break;

            default:
                throw aeron::util::IllegalStateException("unexpected state=" + std::to_string(static_cast<int>(state)),
                                                         SOURCEINFO);
        }
    }

    switch (state) {
        case State::MEDIA:
            media = buffer;
            break;

        case State::PARAMS_VALUE:
            params[key] = buffer;
            break;

        default:
            throw aeron::util::IllegalArgumentException(
                "no more input found, state=" + std::to_string(static_cast<int>(state)), SOURCEINFO);
    }

    return ChannelUri(prefix, media, std::move(params));
}

std::string ChannelUri::addSessionId(const std::string& channel, std::int32_t sessionId) {
    ChannelUri channelUri = ChannelUri::parse(channel);
    channelUri.put(SESSION_ID_PARAM_NAME, std::to_string(sessionId));
    return channelUri.toString();
}

const std::string ChannelUri::AERON_SCHEME{"aeron"};

}  // namespace archive
}  // namespace aeron
