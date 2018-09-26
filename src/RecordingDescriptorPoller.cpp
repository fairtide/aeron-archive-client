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


#include <io_aeron_archive_codecs/ControlResponse.h>
#include <io_aeron_archive_codecs/RecordingDescriptor.h>

#include "RecordingDescriptorPoller.h"

namespace codecs = io::aeron::archive::codecs;

namespace aeron {
namespace archive {

RecordingDescriptorPoller::RecordingDescriptorPoller(const std::shared_ptr<Subscription>& subscription,
                                                     std::int32_t fragmentLimit, std::uint64_t controlSessionId)
    : subscription_(subscription)
    , fragmentLimit_(fragmentLimit)
    , controlSessionId_(controlSessionId)
    , fragmentAssembler_([this](concurrent::AtomicBuffer& buffer, util::index_t offset, util::index_t length,
                                Header& header) { return onFragment(buffer, offset, length, header); }) {
    //
}

std::int32_t RecordingDescriptorPoller::poll() {
    isDispatchComplete_ = false;
    return subscription_->controlledPoll(fragmentAssembler_.handler(), fragmentLimit_);
}

void RecordingDescriptorPoller::reset(std::int64_t expectedCorrelationId, std::int32_t remainingRecordCount,
                                      RecordingDescriptorConsumer&& consumer) {
    expectedCorrelationId_ = expectedCorrelationId;
    consumer_ = consumer;
    remainingRecordCount_ = remainingRecordCount;
    isDispatchComplete_ = false;
}

ControlledPollAction RecordingDescriptorPoller::onFragment(concurrent::AtomicBuffer& buffer, util::index_t offset,
                                                           util::index_t length, Header& header) {
    codecs::MessageHeader hdr;
    hdr.wrap((char*)buffer.buffer(), offset, 0 /* TODO */, buffer.capacity());

    const std::uint16_t templateId = hdr.templateId();

    if (templateId == codecs::ControlResponse::sbeTemplateId()) {
        codecs::ControlResponse msg;
        msg.wrapForDecode((char*)buffer.buffer(), offset + hdr.encodedLength(), hdr.blockLength(), hdr.version(),
                          buffer.capacity());

        if (controlSessionId_ == msg.controlSessionId()) {
            auto code = msg.code();

            if (code == codecs::ControlResponseCode::RECORDING_UNKNOWN) {
                isDispatchComplete_ = true;
                return ControlledPollAction::BREAK;
            } else if (code == codecs::ControlResponseCode::ERROR) {
                throw std::runtime_error(
                    "response for expectedCorrelationId=" + std::to_string(expectedCorrelationId_) +
                    ", error: " + msg.getErrorMessageAsString());
            }
        }
    } else if (templateId == codecs::RecordingDescriptor::sbeTemplateId()) {
        codecs::RecordingDescriptor msg;
        msg.wrapForDecode((char*)buffer.buffer(), offset + hdr.encodedLength(), hdr.blockLength(), hdr.version(),
                          buffer.capacity());

        const std::int64_t correlationId = msg.correlationId();
        if (controlSessionId_ == msg.controlSessionId() && correlationId == expectedCorrelationId_) {
            consumer_(controlSessionId_, correlationId, msg.recordingId(), msg.startTimestamp(), msg.stopTimestamp(),
                      msg.startPosition(), msg.stopPosition(), msg.initialTermId(), msg.segmentFileLength(),
                      msg.termBufferLength(), msg.mtuLength(), msg.sessionId(), msg.streamId(), msg.strippedChannel(),
                      msg.originalChannel(), msg.sourceIdentity());

            if (--remainingRecordCount_ == 0) {
                isDispatchComplete_ = true;
            }
        }
    } else {
        throw std::runtime_error("unknown template id: " + std::to_string(templateId));
    }

    return ControlledPollAction::CONTINUE;
}

}  // namespace archive
}  // namespace aeron
