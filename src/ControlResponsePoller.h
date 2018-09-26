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

#include <Aeron.h>
#include <ControlledFragmentAssembler.h>

#include <io_aeron_archive_codecs/ControlResponseCode.h>

namespace aeron {
namespace archive {

class ControlResponsePoller {
public:
    ControlResponsePoller(const std::shared_ptr<aeron::Subscription>& subscription, std::int32_t fragmentLimit);

    std::int32_t poll();

private:
    aeron::ControlledPollAction onFragment(aeron::concurrent::AtomicBuffer& buffer, aeron::util::index_t offset,
                                           aeron::util::index_t length, aeron::Header& header);

private:
    std::shared_ptr<aeron::Subscription> subscription_;
    std::int32_t fragmentLimit_;
    aeron::ControlledFragmentAssembler fragmentAssembler_;

    std::int64_t controlSessionId_;
    std::int64_t correlationId_;
    std::int64_t relevantId_;
    std::int64_t templateId_;
    io::aeron::archive::codecs::ControlResponseCode::Value code_;
    std::string errorMessage_;
    bool isPollComplete_{false};
};

}  // namespace archive
}  // namespace aeron
