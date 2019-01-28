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

#include <iostream>

#include "SamplesUtil.h"

namespace aeron {
namespace archive {

RecordingData getLatestRecordingData(aeron::archive::AeronArchive& archive, const std::string& channel,
                                     std::int32_t streamId) {
    RecordingData result;
    result.recordingId = -1;

    auto consumer = [&](long controlSessionId, long correlationId, long recordingId, long startTimestamp,
                        long stopTimestamp, long startPosition, long stopPosition, int initialTermId,
                        int segmentFileLength, int termBufferLength, int mtuLength, int sessionId, int streamId,
                        const std::string& strippedChannel, const std::string& originalChannel,
                        const std::string& sourceIdentity) {
        result.recordingId = recordingId;
        result.stopPosition = stopPosition;
        result.initialTermId = initialTermId;
        result.termBufferLength = termBufferLength;
    };

    archive.listRecordingsForUri(0, 100, channel, streamId, consumer);
    return result;
}

std::int64_t findLatestRecordingId(aeron::archive::AeronArchive& archive, const std::string& channel,
                                   std::int32_t streamId) {
    std::int64_t lastRecordingId{-1};

    auto consumer = [&](long controlSessionId, long correlationId, long recordingId, long startTimestamp,
                        long stopTimestamp, long startPosition, long stopPosition, int initialTermId,
                        int segmentFileLength, int termBufferLength, int mtuLength, int sessionId, int streamId,
                        const std::string& strippedChannel, const std::string& originalChannel,
                        const std::string& sourceIdentity) {
        std::cout << "recId: " << recordingId << ", ts: [" << startTimestamp << ", " << stopTimestamp
                  << "], pos: [" << startPosition << ", " << stopPosition
                  << "], initialTermId: " << initialTermId << ", sessionId: " << sessionId << ", streamId: " << streamId
                  << ", strippedChannel: " << strippedChannel << ", originalChannel: " << originalChannel
                  << ", sourceIdentity: " << sourceIdentity << '\n';

        lastRecordingId = recordingId;
    };

    std::int32_t foundCount = archive.listRecordingsForUri(0, 100, channel, streamId, consumer);

    if (foundCount) {
        std::cout << "found " << foundCount << ", last recording id = " << lastRecordingId << '\n';
    }

    return lastRecordingId;
}

void findAllRecordingIds(aeron::archive::AeronArchive& archive) {
    auto consumer = [&](long controlSessionId, long correlationId, long recordingId, long startTimestamp,
                        long stopTimestamp, long startPosition, long stopPosition, int initialTermId,
                        int segmentFileLength, int termBufferLength, int mtuLength, int sessionId, int streamId,
                        const std::string& strippedChannel, const std::string& originalChannel,
                        const std::string& sourceIdentity) {
        std::cout << "recId: " << recordingId << ", ts: [" << startTimestamp << ", " << stopTimestamp
                  << "], pos: [" << startPosition << ", " << stopPosition
                  << "], initialTermId: " << initialTermId << ", sessionId: " << sessionId << ", streamId: " << streamId
                  << ", strippedChannel: " << strippedChannel << ", originalChannel: " << originalChannel
                  << ", sourceIdentity: " << sourceIdentity << '\n';
    };

    archive.listRecordings(0, 100, consumer);
}
}  // namespace archive
}  // namespace aeron
