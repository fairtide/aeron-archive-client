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

#include "io_aeron_archive_codecs/ControlResponse.h"
#include "io_aeron_archive_codecs/RecordingDescriptor.h"

#include "ArchiveException.h"
#include "RecordingDescriptorPoller.h"

namespace codecs = io::aeron::archive::codecs;

namespace aeron {
namespace archive {

RecordingDescriptorPoller::RecordingDescriptorPoller(const std::shared_ptr<Subscription>& subscription,
                                                     std::int32_t fragmentLimit, std::int64_t controlSessionId)
    : subscription_(subscription)
    , fragmentLimit_(fragmentLimit)
    , controlSessionId_(controlSessionId)
    , fragmentAssembler_([this](concurrent::AtomicBuffer& buffer, util::index_t offset, util::index_t length,
                                Header& header) { return onFragment(buffer, offset, length, header); }) {}

std::int32_t RecordingDescriptorPoller::poll() {
    isDispatchComplete_ = false;
    return subscription_->controlledPoll(fragmentAssembler_.handler(), fragmentLimit_);
}

void RecordingDescriptorPoller::reset(std::int64_t correlationId, std::int32_t remainingRecordCount,
                                      RecordingDescriptorConsumer&& consumer) {
    correlationId_ = correlationId;
    consumer_ = consumer;
    remainingRecordCount_ = remainingRecordCount;
    isDispatchComplete_ = false;
}

const std::shared_ptr<Subscription>& RecordingDescriptorPoller::subscription() const { return subscription_; }

std::int32_t RecordingDescriptorPoller::remainingRecordCount() const { return remainingRecordCount_; }

bool RecordingDescriptorPoller::isDispatchComplete() const { return isDispatchComplete_; }

ControlledPollAction RecordingDescriptorPoller::onFragment(concurrent::AtomicBuffer& buffer, util::index_t offset,
                                                           util::index_t length, Header& header) {
    codecs::MessageHeader hdr;
    hdr.wrap((char*)buffer.buffer(), offset, 0, buffer.capacity());

    const std::uint16_t templateId = hdr.templateId();

    if (templateId == codecs::ControlResponse::sbeTemplateId()) {
        codecs::ControlResponse msg;
        msg.wrapForDecode((char*)buffer.buffer(), offset + hdr.encodedLength(), hdr.blockLength(), hdr.version(),
                          buffer.capacity());

        if (controlSessionId_ == msg.controlSessionId()) {
            auto code = msg.code();
            const std::int64_t correlationId = msg.correlationId();

            if (code == codecs::ControlResponseCode::RECORDING_UNKNOWN &&
                correlationId_ == correlationId) {
                isDispatchComplete_ = true;
                return ControlledPollAction::BREAK;
            } else if (code == codecs::ControlResponseCode::ERROR) {
                throw ArchiveException(
                    "response for correlationId=" + std::to_string(correlationId_) +
                    ", error: " + msg.getErrorMessageAsString(), SOURCEINFO);
            }
        }
    } else if (templateId == codecs::RecordingDescriptor::sbeTemplateId()) {
        codecs::RecordingDescriptor msg;
        msg.wrapForDecode((char*)buffer.buffer(), offset + hdr.encodedLength(), hdr.blockLength(), hdr.version(),
                          buffer.capacity());

        if (controlSessionId_ == msg.controlSessionId() && correlationId_ == msg.correlationId()) {
            consumer_(controlSessionId_, correlationId_, msg.recordingId(), msg.startTimestamp(), msg.stopTimestamp(),
                      msg.startPosition(), msg.stopPosition(), msg.initialTermId(), msg.segmentFileLength(),
                      msg.termBufferLength(), msg.mtuLength(), msg.sessionId(), msg.streamId(), msg.getStrippedChannelAsString(),
                      msg.getOriginalChannelAsString(), msg.getSourceIdentityAsString());

            if (--remainingRecordCount_ == 0) {
                isDispatchComplete_ = true;
                return ControlledPollAction::BREAK;
            }
        }
    } else {
        throw ArchiveException("unknown template id: " + std::to_string(templateId), SOURCEINFO);
    }

    return ControlledPollAction::CONTINUE;
}

}  // namespace archive
}  // namespace aeron
