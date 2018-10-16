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

namespace aeron {
namespace archive {

using RecordingDescriptorConsumer = std::function<void(
    std::int64_t controlSessionId, std::int64_t correlationId, std::int64_t recordingId, std::int64_t startTimestamp, std::int64_t stopTimestamp,
    std::int64_t startPosition, std::int64_t stopPosition, std::int32_t initialTermId, std::int32_t segmentFileLength, std::int32_t termBufferLength,
    std::int32_t mtuLength, std::int32_t sessionId, std::int32_t streamId, const std::string& strippedChannel, const std::string& originalChannel,
    const std::string& sourceIdentity)>;

class RecordingDescriptorPoller {
public:
    RecordingDescriptorPoller(const std::shared_ptr<aeron::Subscription>& subscription, std::int32_t fragmentLimit,
                              std::int64_t controlSessionId);

    std::int32_t poll();
    void reset(std::int64_t expectedCorrelationId, std::int32_t remainingRecordCount,
               RecordingDescriptorConsumer&& consumer);

    const std::shared_ptr<aeron::Subscription>& subscription() const;
    std::int32_t remainingRecordCount() const;
    bool isDispatchComplete() const;

private:
    aeron::ControlledPollAction onFragment(aeron::concurrent::AtomicBuffer& buffer, aeron::util::index_t offset,
                                           aeron::util::index_t length, aeron::Header& header);

private:
    std::shared_ptr<aeron::Subscription> subscription_;
    std::int32_t fragmentLimit_;
    std::int64_t controlSessionId_;
    aeron::ControlledFragmentAssembler fragmentAssembler_;

    std::int64_t expectedCorrelationId_;
    std::int32_t remainingRecordCount_;
    RecordingDescriptorConsumer consumer_;
    bool isDispatchComplete_{false};
};

}  // namespace archive
}  // namespace aeron
