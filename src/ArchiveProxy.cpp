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

#include "io_aeron_archive_codecs/CloseSessionRequest.h"
#include "io_aeron_archive_codecs/ConnectRequest.h"
#include "io_aeron_archive_codecs/ExtendRecordingRequest.h"
#include "io_aeron_archive_codecs/FindLastMatchingRecordingRequest.h"
#include "io_aeron_archive_codecs/ListRecordingRequest.h"
#include "io_aeron_archive_codecs/ListRecordingsForUriRequest.h"
#include "io_aeron_archive_codecs/ListRecordingsRequest.h"
#include "io_aeron_archive_codecs/RecordingPositionRequest.h"
#include "io_aeron_archive_codecs/ReplayRequest.h"
#include "io_aeron_archive_codecs/StartRecordingRequest.h"
#include "io_aeron_archive_codecs/StopPositionRequest.h"
#include "io_aeron_archive_codecs/StopRecordingRequest.h"
#include "io_aeron_archive_codecs/StopRecordingSubscriptionRequest.h"
#include "io_aeron_archive_codecs/StopReplayRequest.h"
#include "io_aeron_archive_codecs/TruncateRecordingRequest.h"

#include "ArchiveException.h"
#include "ArchiveProxy.h"

namespace codecs = io::aeron::archive::codecs;

namespace aeron {
namespace archive {

ArchiveProxy::ArchiveProxy(const std::shared_ptr<aeron::Publication>& publication, std::int64_t connectTimeoutNs,
                           std::int32_t retryAttempts)
    : publication_(publication)
    , buffer_(&underlyingBuffer_[0], underlyingBuffer_.size())
    , connectTimeoutNs_(connectTimeoutNs)
    , retryAttempts_(retryAttempts) {}

bool ArchiveProxy::connect(const std::string& responseChannel, std::int32_t responseStreamId,
                           std::int64_t correlationId) {
    codecs::ConnectRequest msg;

    wrapAndApplyHeader(msg)
        .correlationId(correlationId)
        .responseStreamId(responseStreamId)
        .putResponseChannel(responseChannel);

    return offerWithTimeout(msg.encodedLength(), nullptr);
}

bool ArchiveProxy::tryConnect(const std::string& responseChannel, std::int32_t responseStreamId,
                              std::int64_t correlationId) {
    codecs::ConnectRequest msg;

    wrapAndApplyHeader(msg)
        .correlationId(correlationId)
        .responseStreamId(responseStreamId)
        .putResponseChannel(responseChannel);

    return publication_->offer(buffer_, 0, codecs::MessageHeader::encodedLength() + msg.encodedLength()) > 0;
}

bool ArchiveProxy::connect(const std::string& responseChannel, std::int32_t responseStreamId,
                           std::int64_t correlationId, AgentInvoker<ClientConductor>& agentInvoker) {
    codecs::ConnectRequest msg;

    wrapAndApplyHeader(msg)
        .correlationId(correlationId)
        .responseStreamId(responseStreamId)
        .putResponseChannel(responseChannel);

    return offerWithTimeout(msg.encodedLength(), &agentInvoker);
}

bool ArchiveProxy::closeSession(std::int64_t controlSessionId) {
    codecs::CloseSessionRequest msg;

    wrapAndApplyHeader(msg).controlSessionId(controlSessionId);

    return offer(msg.encodedLength());
}

bool ArchiveProxy::startRecording(const std::string& channel, std::int32_t streamId,
                                  codecs::SourceLocation::Value sourceLocation, std::int64_t correlationId,
                                  std::int64_t controlSessionId) {
    codecs::StartRecordingRequest msg;

    wrapAndApplyHeader(msg)
        .controlSessionId(controlSessionId)
        .correlationId(correlationId)
        .streamId(streamId)
        .sourceLocation(sourceLocation)
        .putChannel(channel);

    return offer(msg.encodedLength());
}

bool ArchiveProxy::stopRecording(const std::string& channel, std::int32_t streamId, std::int64_t correlationId,
                                 std::int64_t controlSessionId) {
    codecs::StopRecordingRequest msg;

    wrapAndApplyHeader(msg)
        .controlSessionId(controlSessionId)
        .correlationId(correlationId)
        .streamId(streamId)
        .putChannel(channel);

    return offer(msg.encodedLength());
}

bool ArchiveProxy::stopRecording(std::int64_t subscriptionId, std::int64_t correlationId,
                                 std::int64_t controlSessionId) {
    codecs::StopRecordingSubscriptionRequest msg;

    wrapAndApplyHeader(msg)
        .controlSessionId(controlSessionId)
        .correlationId(correlationId)
        .subscriptionId(subscriptionId);

    return offer(msg.encodedLength());
}

bool ArchiveProxy::replay(std::int64_t recordingId, std::int64_t position, std::int64_t length,
                          const std::string& replayChannel, std::int32_t replayStreamId, std::int64_t correlationId,
                          std::int64_t controlSessionId) {
    codecs::ReplayRequest msg;

    wrapAndApplyHeader(msg)
        .controlSessionId(controlSessionId)
        .correlationId(correlationId)
        .recordingId(recordingId)
        .position(position)
        .length(length)
        .replayStreamId(replayStreamId)
        .putReplayChannel(replayChannel);

    return offer(msg.encodedLength());
}

bool ArchiveProxy::stopReplay(std::int64_t replaySessionId, std::int64_t correlationId, std::int64_t controlSessionId) {
    codecs::StopReplayRequest msg;

    wrapAndApplyHeader(msg)
        .controlSessionId(controlSessionId)
        .correlationId(correlationId)
        .replaySessionId(replaySessionId);

    return offer(msg.encodedLength());
}

bool ArchiveProxy::listRecordings(std::int64_t fromRecordingId, std::int32_t recordCount, std::int64_t correlationId,
                                  std::int64_t controlSessionId) {
    codecs::ListRecordingsRequest msg;

    wrapAndApplyHeader(msg)
        .controlSessionId(controlSessionId)
        .correlationId(correlationId)
        .fromRecordingId(fromRecordingId)
        .recordCount(recordCount);

    return offer(msg.encodedLength());
}

bool ArchiveProxy::listRecordingsForUri(std::int64_t fromRecordingId, std::int32_t recordCount,
                                        const std::string& channel, std::int32_t streamId, std::int64_t correlationId,
                                        std::int64_t controlSessionId) {
    codecs::ListRecordingsForUriRequest msg;

    wrapAndApplyHeader(msg)
        .controlSessionId(controlSessionId)
        .correlationId(correlationId)
        .fromRecordingId(fromRecordingId)
        .recordCount(recordCount)
        .streamId(streamId)
        .putChannel(channel);

    return offer(msg.encodedLength());
}

bool ArchiveProxy::listRecording(std::int64_t recordingId, std::int64_t correlationId, std::int64_t controlSessionId) {
    codecs::ListRecordingRequest msg;

    wrapAndApplyHeader(msg).controlSessionId(controlSessionId).correlationId(correlationId).recordingId(recordingId);

    return offer(msg.encodedLength());
}

bool ArchiveProxy::extendRecording(const std::string& channel, std::int32_t streamId,
                                   codecs::SourceLocation::Value sourceLocation, std::int64_t recordingId,
                                   std::int64_t correlationId, std::int64_t controlSessionId) {
    codecs::ExtendRecordingRequest msg;

    wrapAndApplyHeader(msg)
        .controlSessionId(controlSessionId)
        .correlationId(correlationId)
        .recordingId(recordingId)
        .streamId(streamId)
        .sourceLocation(sourceLocation)
        .putChannel(channel);

    return offer(msg.encodedLength());
}

bool ArchiveProxy::getRecordingPosition(std::int64_t recordingId, std::int64_t correlationId,
                                        std::int64_t controlSessionId) {
    codecs::RecordingPositionRequest msg;

    wrapAndApplyHeader(msg).controlSessionId(controlSessionId).correlationId(correlationId).recordingId(recordingId);

    return offer(msg.encodedLength());
}

bool ArchiveProxy::truncateRecording(std::int64_t recordingId, std::int64_t position, std::int64_t correlationId,
                                     std::int64_t controlSessionId) {
    codecs::TruncateRecordingRequest msg;

    wrapAndApplyHeader(msg)
        .controlSessionId(controlSessionId)
        .correlationId(correlationId)
        .recordingId(recordingId)
        .position(position);

    return offer(msg.encodedLength());
}

bool ArchiveProxy::getStopPosition(std::int64_t recordingId, std::int64_t correlationId, std::int64_t controlSessionId)
{
    codecs::StopPositionRequest msg;

    wrapAndApplyHeader(msg)
        .controlSessionId(controlSessionId)
        .correlationId(correlationId)
        .recordingId(recordingId);

    return offer(msg.encodedLength());
}

bool ArchiveProxy::findLastMatchingRecording(std::int64_t minRecordingId, const std::string& channel, std::int32_t streamId,
                                   std::int32_t sessionId, std::int64_t correlationId, std::int64_t controlSessionId)
{
    codecs::FindLastMatchingRecordingRequest msg;

    wrapAndApplyHeader(msg)
        .controlSessionId(controlSessionId)
        .correlationId(correlationId)
        .minRecordingId(minRecordingId)
        .sessionId(sessionId)
        .streamId(streamId)
        .putChannel(channel);

    return offer(msg.encodedLength());
}

bool ArchiveProxy::offer(std::int32_t length) {
    std::int32_t attempts = retryAttempts_;
    while (true) {
        std::int64_t result = publication_->offer(buffer_, 0, codecs::MessageHeader::encodedLength() + length);
        if (result > 0) {
            return true;
        }

        if (result == aeron::PUBLICATION_CLOSED) {
            throw ArchiveException("connection to the archive has been closed", SOURCEINFO);
        } else if (result == aeron::NOT_CONNECTED) {
            throw ArchiveException("connection to the archive is no longer available", SOURCEINFO);
        } else if (result == aeron::MAX_POSITION_EXCEEDED) {
            throw ArchiveException("offer failed due to max position being reached", SOURCEINFO);
        }

        if (--attempts <= 0) {
            return false;
        }

        idle_.idle();
    }
}

bool ArchiveProxy::offerWithTimeout(std::int32_t length,
                                    aeron::AgentInvoker<aeron::ClientConductor>* aeronClientInvoker) {
    auto deadline = std::chrono::high_resolution_clock::now() + connectTimeoutNs_;
    while (true) {
        std::int64_t result = publication_->offer(buffer_, 0, codecs::MessageHeader::encodedLength() + length);
        if (result > 0) {
            return true;
        }

        if (result == aeron::PUBLICATION_CLOSED) {
            throw ArchiveException("connection to the archive has been closed", SOURCEINFO);
        } else if (result == aeron::MAX_POSITION_EXCEEDED) {
            throw ArchiveException("offer failed due to max position being reached", SOURCEINFO);
        }

        if (std::chrono::high_resolution_clock::now() > deadline) {
            return false;
        }

        if (aeronClientInvoker) {
            aeronClientInvoker->invoke();
        }

        idle_.idle();
    }
}

}  // namespace archive
}  // namespace aeron
