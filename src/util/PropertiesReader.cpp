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

#include <fstream>

#include "PropertiesReader.h"

namespace {
inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

inline void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}

}  // namespace

namespace aeron {
namespace archive {
namespace util {

PropertiesReader::PropertiesReader(const std::string& filename)
    : PropertiesReader(filename, [](const std::exception& e) { throw e; }) {}

PropertiesReader::PropertiesReader(const std::string& filename, std::function<void(const std::exception&)>&& onError) {
    std::fstream ifs(filename, std::ios_base::in);

    for (std::string line; std::getline(ifs, line);) {
        try {
            properties_.emplace(parseLine(line));
        } catch (const std::exception& e) {
            onError(e);
        }
    }
}

std::pair<std::string, std::string> PropertiesReader::parseLine(const std::string& line) {
    enum { START, KEY, VALUE } state;
    state = START;

    std::pair<std::string, std::string> result;

    std::size_t i = 0, pos = 0;
    for (char c : line) {
        if (state == START) {
            if (!std::isspace((unsigned char)c)) {
                if (c == '#') {
                    return result;
                } else {
                    state = KEY;
                    pos = i;
                }
            }
        } else if (state == KEY) {
            if (c == '=') {
                state = VALUE;
                result.first = std::string(line, pos, i);
                rtrim(result.first);
                pos = i + 1;
            } else if (c == '#') {
                break;
            }
        } else if (state == VALUE) {
            if (c == '#') {
                break;
            }
        } else {
            throw std::runtime_error("unknown state: " + std::to_string(state));
        }
        ++i;
    }

    if (state == KEY) {
        throw std::runtime_error("no value: " + line);
    } else if (state == VALUE) {
        result.second = std::string(line, pos, i);
        trim(result.second);
    }

    return result;
}

// explicit specialization for bool
bool PropertiesReader::parse_value(const std::string& v, bool*) {
    std::string vu = v;
    std::transform(vu.begin(), vu.end(), vu.begin(), [](unsigned char c) { return std::toupper(c); });
    return (vu == "TRUE" || vu == "1" || vu == "Y");
}

}  // namespace util
}  // namespace archive
}  // namespace aeron
