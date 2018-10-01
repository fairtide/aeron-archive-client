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

#include <boost/optional.hpp>

#include <io_aeron_archive_codecs/SourceLocation.h>

#include "ArchiveException.h"
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
    AeronArchive(const Context& ctx);
    AeronArchive(const Context& ctx, const ArchiveProxy& archiveProxy);

    // helper methods
    static std::shared_ptr<AeronArchive> connect();
    static std::shared_ptr<AeronArchive> connect(const Context& ctx);

    static std::shared_ptr<AeronArchive> asyncConnect();
    static std::shared_ptr<AeronArchive> asyncConnect(const Context& ctx);

    // getters
    const Context& context() const;

    //
    boost::optional<std::string> pollForErrorResponse();
    void checkForErrorResponse();

    std::shared_ptr<aeron::Publication> addRecordedPublication(const std::string& channel, std::int32_t streamId);
    std::shared_ptr<aeron::ExclusivePublication> addRecordedExclusivePublication(const std::string& channel,
                                                                                 std::int32_t streamId);

    std::int64_t startRecording(const std::string& channel, std::int32_t streamId,
                                io::aeron::archive::codecs::SourceLocation::Value sourceLocation);

    void stopRecording(const std::string& channel, std::int32_t streamId);

    void stopRecording(const aeron::Publication& publication);

    void stopRecording(std::int64_t subscriptionId);

    std::int64_t startReplay(std::int64_t recordingId, std::int64_t position, std::int64_t length,
                             const std::string& replayChannel, std::int32_t replayStreamId);

    void stopReplay(std::int64_t replaySessionId);

    std::shared_ptr<aeron::Subscription> replay(std::int64_t recordingId, std::int64_t position, std::int64_t length,
                                                const std::string& replayChannel, std::int32_t replayStreamId);

    std::shared_ptr<aeron::Subscription> replay(std::int64_t recordingId, std::int64_t position, std::int64_t length,
                                                const std::string& replayChannel, std::int32_t replayStreamId,
                                                aeron::on_available_image_t&& availableImageHandler,
                                                aeron::on_unavailable_image_t&& unavailableImageHandler);

    std::int32_t listRecordings(std::int64_t fromRecordingId, std::int32_t recordCount,
                                RecordingDescriptorConsumer&& consumer);

    std::int32_t listRecordingsForUri(std::int64_t fromRecordingId, std::int32_t recordCount,
                                      const std::string& channel, std::int32_t streamId,
                                      RecordingDescriptorConsumer&& consumer);

    std::int32_t listRecording(std::int64_t recordingId, RecordingDescriptorConsumer&& consumer);

    std::int64_t getRecordingPosition(std::int64_t recordingId);

    void truncateRecording(std::int64_t recordingId, std::int64_t position);

private:
    std::int64_t awaitSessionOpened(std::int64_t correlationId);
    void awaitConnection(const TimePoint& deadline);

    std::int64_t pollForResponse(std::int64_t correlationId);
    void pollNextResponse(std::int64_t correlationId, const TimePoint& deadline);
    std::int64_t pollForDescriptors(std::int64_t correlationId, std::int32_t recordCount,
                                    RecordingDescriptorConsumer&& consumer);

    std::int64_t callAndPollForResponse(std::function<bool(std::int64_t)>&& f, const char* request);
    std::int64_t callAndPollForDescriptors(std::function<bool(std::int64_t)>&& f, std::int32_t recordCount,
                                           RecordingDescriptorConsumer&& consumer, const char* request);

private:
    Context ctx_;
    std::unique_ptr<ArchiveProxy> archiveProxy_;
    std::unique_ptr<ControlResponsePoller> controlResponsePoller_;
    std::unique_ptr<RecordingDescriptorPoller> recordingDescriptorPoller_;

    std::shared_ptr<aeron::Aeron> aeron_;
    aeron::concurrent::YieldingIdleStrategy idleStrategy_;  // TODO: make it generic
    std::mutex lock_;                                       // TODO: make it generic to use no-op lock

    std::chrono::nanoseconds messageTimeoutNs_;
    std::int64_t controlSessionId_;
};

}  // namespace archive
}  // namespace aeron
