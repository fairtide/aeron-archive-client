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

#pragma once

#include <Aeron.h>

namespace aeron {
namespace archive {

class RecordingEventsAdapter {
public:
    using OnStart =
        std::function<void(std::int64_t recordingId, std::int64_t startPosition, std::int32_t sessionId,
                           std::int32_t streamId, const std::string& channel, const std::string& sourceIdentity)>;
    using OnProgress = std::function<void(std::int64_t recordingId, std::int64_t startPosition, std::int64_t position)>;
    using OnStop = std::function<void(std::int64_t recordingId, std::int64_t startPosition, std::int64_t position)>;

    RecordingEventsAdapter(const std::shared_ptr<aeron::Subscription>& subscription, std::int32_t fragmentLimit,
                           OnStart&& onStart, OnProgress&& onProgress, OnStop&& onStop);

    std::int32_t poll();

private:
    void fragmentHandler(aeron::AtomicBuffer& buffer, aeron::util::index_t offset, aeron::util::index_t length,
                         const aeron::Header& header);

private:
    std::shared_ptr<aeron::Subscription> subscription_;
    const std::int32_t fragmentLimit_;

    OnStart onStart_;
    OnProgress onProgress_;
    OnStop onStop_;

    aeron::fragment_handler_t fragmentHandler_;
};

}  // namespace archive
}  // namespace aeron
