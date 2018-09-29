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


#include <boost/stacktrace.hpp>

#include <sstream>

#include "ArchiveException.h"

namespace aeron {
namespace archive {

ArchiveException::ArchiveException(const std::string& what, const std::string& function, const std::string& where)
    : aeron::util::SourcedException(what, function, where) {
    std::ostringstream ss;
    ss << boost::stacktrace::stacktrace();
    stackTrace_ = ss.str();
}

const std::string& ArchiveException::stackTrace() const { return stackTrace_; }

}  // namespace archive
}  // namespace aeron
