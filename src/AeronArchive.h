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

#include <chrono>
#include <mutex>

#include "ArchiveProxy.h"
#include "Context.h"
#include "ControlResponsePoller.h"
#include "RecordingDescriptorPoller.h"

namespace aeron {
namespace archive {

class AeronArchive {
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

public:
    AeronArchive(const Context & ctx);
    AeronArchive(const Context & ctx, const ArchiveProxy & archiveProxy);

private:
    std::int64_t awaitSessionOpened(std::int64_t correlationId);
    void awaitConnection(const TimePoint & deadline);

    std::int64_t pollForResponse(std::int64_t correlationId);
    void pollNextResponse(std::int64_t correlationId, const TimePoint& deadline);

private:
    Context ctx_;
    std::unique_ptr<ArchiveProxy> archiveProxy_;
    std::unique_ptr<ControlResponsePoller> controlResponsePoller_;
    std::unique_ptr<RecordingDescriptorPoller> recordingDescriptorPoller_;

    std::shared_ptr<aeron::Aeron> aeron_;
    aeron::concurrent::YieldingIdleStrategy idleStrategy_; // TODO: make it generic
    std::mutex lock_; // TODO: make it generic to use no-op lock

    std::chrono::nanoseconds messageTimeoutNs_;
    std::int64_t controlSessionId_;
};

}  // namespace archive
}  // namespace aeron
