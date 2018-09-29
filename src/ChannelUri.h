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

#include <boost/optional.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace aeron {
namespace archive {

class ChannelUri {
    using ParamsMap = std::unordered_map<std::string, std::string>;

public:
    ChannelUri(const std::string& prefix, const std::string& media, ParamsMap&& params);
    ChannelUri(const std::string& media, ParamsMap&& params);

    // getters
    const std::string& scheme() const;
    const std::string& prefix() const;
    const std::string& media() const;

    void put(const std::string& key, const std::string& value);
    boost::optional<std::string> get(const std::string& key);
    boost::optional<std::string> get(const std::string& key, const std::string& defaultValue);

    std::string toString() const;

    static ChannelUri parse(const std::string& channel);
    static std::string addSessionId(const std::string& channel, std::int32_t sessionId);

    // statics
    static const std::string AERON_SCHEME;

private:
    std::string prefix_;
    std::string media_;
    ParamsMap params_;
    std::vector<std::string> tags_;
};

}  // namespace archive
}  // namespace aeron
