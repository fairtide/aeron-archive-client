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

#include "io_aeron_archive_codecs/RecordingProgress.h"
#include "io_aeron_archive_codecs/RecordingStarted.h"
#include "io_aeron_archive_codecs/RecordingStopped.h"

#include "ArchiveException.h"
#include "RecordingEventsAdapter.h"

namespace codecs = io::aeron::archive::codecs;

namespace aeron {
namespace archive {

RecordingEventsAdapter::RecordingEventsAdapter(const std::shared_ptr<aeron::Subscription>& subscription,
                                               std::int32_t fragmentLimit, OnStart&& onStart, OnProgress&& onProgress,
                                               OnStop&& onStop)
    : subscription_(subscription),
      fragmentLimit_(fragmentLimit),
      onStart_(onStart),
      onProgress_(onProgress),
      onStop_(onStop) {
    fragmentHandler_ = [this](aeron::AtomicBuffer& buffer, aeron::util::index_t offset, aeron::util::index_t length,
                              const aeron::Header& header) { fragmentHandler(buffer, offset, length, header); };
}

std::int32_t RecordingEventsAdapter::poll() { return subscription_->poll(fragmentHandler_, fragmentLimit_); }

void RecordingEventsAdapter::fragmentHandler(aeron::AtomicBuffer& buffer, aeron::util::index_t offset,
                                             aeron::util::index_t length, const aeron::Header& header) {
    codecs::MessageHeader hdr;
    hdr.wrap((char*)buffer.buffer(), offset, 0, buffer.capacity());

    const std::uint16_t templateId = hdr.templateId();

    if (templateId == codecs::RecordingStarted::sbeTemplateId()) {
        codecs::RecordingStarted msg;
        msg.wrapForDecode((char*)buffer.buffer(), offset + hdr.encodedLength(), hdr.blockLength(), hdr.version(),
                          buffer.capacity());

        onStart_(msg.recordingId(), msg.startPosition(), msg.sessionId(), msg.streamId(), msg.getChannelAsString(),
                 msg.getSourceIdentityAsString());
    } else if (templateId == codecs::RecordingProgress::sbeTemplateId()) {
        codecs::RecordingProgress msg;
        msg.wrapForDecode((char*)buffer.buffer(), offset + hdr.encodedLength(), hdr.blockLength(), hdr.version(),
                          buffer.capacity());

        onProgress_(msg.recordingId(), msg.startPosition(), msg.position());
    } else if (templateId == codecs::RecordingStopped::sbeTemplateId()) {
        codecs::RecordingStopped msg;
        msg.wrapForDecode((char*)buffer.buffer(), offset + hdr.encodedLength(), hdr.blockLength(), hdr.version(),
                          buffer.capacity());

        onStop_(msg.recordingId(), msg.startPosition(), msg.stopPosition());
    } else {
        throw ArchiveException("unknown template id: " + std::to_string(templateId), SOURCEINFO);
    }
}

}  // namespace archive
}  // namespace aeron
