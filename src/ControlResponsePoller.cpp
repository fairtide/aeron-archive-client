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

#include "ControlResponsePoller.h"

namespace codecs = io::aeron::archive::codecs;

namespace aeron {
namespace archive {

ControlResponsePoller::ControlResponsePoller(const std::shared_ptr<Subscription>& subscription,
                                             std::int32_t fragmentLimit)
    : subscription_(subscription)
    , fragmentLimit_(fragmentLimit)
    , fragmentAssembler_([this](concurrent::AtomicBuffer& buffer, util::index_t offset, util::index_t length,
                                Header& header) { return onFragment(buffer, offset, length, header); }) {}

const std::shared_ptr<Subscription> ControlResponsePoller::subscription() const { return subscription_; }

std::int64_t ControlResponsePoller::controlSessionId() const { return controlSessionId_; }

std::int64_t ControlResponsePoller::correlationId() const { return correlationId_; }

std::int64_t ControlResponsePoller::relevantId() const { return relevantId_; }

std::int64_t ControlResponsePoller::templateId() const { return templateId_; }

codecs::ControlResponseCode::Value ControlResponsePoller::code() const { return code_; }

const std::string& ControlResponsePoller::errorMessage() const { return errorMessage_; }

bool ControlResponsePoller::isPollComplete() const { return isPollComplete_; }

std::int32_t ControlResponsePoller::poll() {
    controlSessionId_ = -1;
    correlationId_ = -1;
    relevantId_ = -1;
    templateId_ = -1;
    isPollComplete_ = false;

    return subscription_->controlledPoll(fragmentAssembler_.handler(), fragmentLimit_);
}

ControlledPollAction ControlResponsePoller::onFragment(concurrent::AtomicBuffer& buffer, util::index_t offset,
                                                       util::index_t length, Header& header) {
    codecs::MessageHeader hdr;
    hdr.wrap((char*)buffer.buffer(), offset, 0, buffer.capacity());

    const std::uint16_t templateId = hdr.templateId();

    if (templateId == codecs::ControlResponse::sbeTemplateId()) {
        codecs::ControlResponse msg;
        msg.wrapForDecode((char*)buffer.buffer(), offset + hdr.encodedLength(), hdr.blockLength(), hdr.version(),
                          buffer.capacity());

        controlSessionId_ = msg.controlSessionId();
        correlationId_ = msg.correlationId();
        relevantId_ = msg.relevantId();
        templateId_ = templateId;
        code_ = msg.code();

        if (code_ == codecs::ControlResponseCode::ERROR) {
            errorMessage_ = msg.getErrorMessageAsString();
        } else {
            errorMessage_ = "";
        }
    } else if (templateId != codecs::RecordingDescriptor::sbeTemplateId()) {
        throw std::runtime_error("unknown template id: " + std::to_string(templateId));
    }

    isPollComplete_ = true;

    return ControlledPollAction::BREAK;
}

}  // namespace archive
}  // namespace aeron
