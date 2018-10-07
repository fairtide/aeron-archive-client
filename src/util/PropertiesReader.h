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

#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>

#include <boost/lexical_cast.hpp>

namespace aeron {
namespace archive {
namespace util {

class PropertiesReader {
public:
    explicit PropertiesReader(const std::string& filename);
    PropertiesReader(const std::string& filename, std::function<void(const std::exception&)>&& onError);
    ~PropertiesReader() = default;

    template <typename T>
    T get(const std::string& key) {
        auto it = properties_.find(key);
        if (it == properties_.end()) {
            throw std::runtime_error("key not found: " + key);
        }

        return parse_value(it->second, (T*)nullptr);
    }

    template <typename T>
    T get(const std::string& key, const T& def) {
        auto it = properties_.find(key);
        if (it == properties_.end()) {
            return def;
        }

        return parse_value(it->second, (T*)nullptr);
    }

private:
    std::pair<std::string, std::string> parseLine(const std::string& line);

    template <typename T>
    T parse_value(const std::string& v, T*) {
        return boost::lexical_cast<T>(v);
    }

    bool parse_value(const std::string& v, bool*);

private:
    std::unordered_map<std::string, std::string> properties_;
};

}  // namespace util
}  // namespace archive
}  // namespace aeron
