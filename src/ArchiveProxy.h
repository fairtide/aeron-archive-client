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

#include <Aeron.h>
#include <concurrent/YieldingIdleStrategy.h>

#include "io_aeron_archive_codecs/MessageHeader.h"
#include "io_aeron_archive_codecs/SourceLocation.h"

namespace aeron {
namespace archive {

class ArchiveProxy {
public:
    ArchiveProxy(const std::shared_ptr<aeron::ExclusivePublication>& publication, std::int64_t connectTimeoutNs,
                 std::int32_t retryAttempts);

    bool connect(const std::string& responseChannel, std::int32_t responseStreamId, std::int64_t correlationId);

    bool tryConnect(const std::string& responseChannel, std::int32_t responseStreamId, std::int64_t correlationId);

    bool connect(const std::string& responseChannel, std::int32_t responseStreamId, std::int64_t correlationId,
                 AgentInvoker<ClientConductor>& agentInvoker);

    bool closeSession(std::int64_t controlSessionId);

    bool startRecording(const std::string& channel, std::int32_t streamId,
                        io::aeron::archive::codecs::SourceLocation::Value sourceLocation, std::int64_t correlationId,
                        std::int64_t controlSessionId);

    bool stopRecording(const std::string& channel, std::int32_t streamId, std::int64_t correlationId,
                       std::int64_t controlSessionId);

    bool stopRecording(std::int64_t subscriptionId, std::int64_t correlationId, std::int64_t controlSessionId);

    bool replay(std::int64_t recordingId, std::int64_t position, std::int64_t length, const std::string& replayChannel,
                std::int32_t replayStreamId, std::int64_t correlationId, std::int64_t controlSessionId);

    bool stopReplay(std::int64_t replaySessionId, std::int64_t correlationId, std::int64_t controlSessionId);

    bool listRecordings(std::int64_t fromRecordingId, std::int32_t recordCount, std::int64_t correlationId,
                        std::int64_t controlSessionId);

    bool listRecordingsForUri(std::int64_t fromRecordingId, std::int32_t recordCount, const std::string& channelFragment,
                              std::int32_t streamId, std::int64_t correlationId, std::int64_t controlSessionId);

    bool listRecording(std::int64_t recordingId, std::int64_t correlationId, std::int64_t controlSessionId);

    bool extendRecording(const std::string& channel, std::int32_t streamId,
                         io::aeron::archive::codecs::SourceLocation::Value sourceLocation, std::int64_t recordingId,
                         std::int64_t correlationId, std::int64_t controlSessionId);

    bool getRecordingPosition(std::int64_t recordingId, std::int64_t correlationId, std::int64_t controlSessionId);

    bool truncateRecording(std::int64_t recordingId, std::int64_t position, std::int64_t correlationId,
                           std::int64_t controlSessionId);

    bool getStopPosition(std::int64_t recordingId, std::int64_t correlationId, std::int64_t controlSessionId);

    bool findLastMatchingRecording(std::int64_t minRecordingId, const std::string& channelFragment, std::int32_t streamId,
                                   std::int32_t sessionId, std::int64_t correlationId, std::int64_t controlSessionId);

private:
    template <typename T>
    T& wrapAndApplyHeader(T& msg) {
        constexpr std::uint64_t offset{0};
        io::aeron::archive::codecs::MessageHeader hdr;

        hdr.wrap((char*)buffer_.buffer(), offset, 0, buffer_.capacity())
            .blockLength(T::sbeBlockLength())
            .templateId(T::sbeTemplateId())
            .schemaId(T::sbeSchemaId())
            .version(T::sbeSchemaVersion());

        return msg.wrapForEncode((char*)buffer_.buffer(), offset + hdr.encodedLength(), buffer_.capacity());
    }

    bool offer(std::int32_t length);
    bool offerWithTimeout(std::int32_t length, aeron::AgentInvoker<aeron::ClientConductor>* aeronClientInvoker);

private:
    std::shared_ptr<aeron::ExclusivePublication> publication_;
    concurrent::YieldingIdleStrategy idle_;
    // TODO: size of the buffer - should it be configurable?
    // should it be equal to the maximum size of all SBE messages?
    std::array<std::uint8_t, 4096> underlyingBuffer_;
    concurrent::AtomicBuffer buffer_;
    const std::chrono::nanoseconds connectTimeoutNs_;
    const std::int32_t retryAttempts_;
};

}  // namespace archive
}  // namespace aeron
