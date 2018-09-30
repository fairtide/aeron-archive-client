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


#include "RecordingPos.h"

namespace {

constexpr std::int32_t SIZE_OF_INT = sizeof(std::int32_t);
constexpr std::int32_t SIZE_OF_LONG = sizeof(std::int64_t);

constexpr std::int32_t TYPE_ID_OFFSET = sizeof(std::int32_t);
constexpr std::int32_t RECORDING_POSITION_TYPE_ID = 100;
constexpr std::int32_t RECORDING_ID_OFFSET = 0;
constexpr std::int32_t SESSION_ID_OFFSET = RECORDING_ID_OFFSET + SIZE_OF_LONG;
constexpr std::int32_t SOURCE_IDENTITY_LENGTH_OFFSET = SESSION_ID_OFFSET + SIZE_OF_INT;
constexpr std::int32_t SOURCE_IDENTITY_OFFSET = SOURCE_IDENTITY_LENGTH_OFFSET + SIZE_OF_INT;
}  // namespace

namespace aeron {
namespace archive {

std::int32_t RecordingPos::findCounterIdBySession(concurrent::CountersReader& countersReader, std::int32_t sessionId) {
    auto buffer = countersReader.metaDataBuffer();

    for (int i = 0, size = countersReader.maxCounterId(); i < size; ++i) {
        if (countersReader.getCounterState(i) == concurrent::CountersReader::RECORD_ALLOCATED) {
            std::int32_t recordOffset = concurrent::CountersReader::metadataOffset(i);

            if (buffer.getInt32(recordOffset + TYPE_ID_OFFSET) == RECORDING_POSITION_TYPE_ID && buffer.getInt32(recordOffset + concurrent::CountersReader::KEY_OFFSET + SESSION_ID_OFFSET) == sessionId) {
                return i;
            }
        }
    }

    return -1;
}

std::int64_t RecordingPos::getRecordingId(concurrent::CountersReader& countersReader, std::int32_t counterId) {
    auto buffer = countersReader.metaDataBuffer();

    if (countersReader.getCounterState(counterId) == concurrent::CountersReader::RECORD_ALLOCATED) {
        std::int32_t recordOffset = concurrent::CountersReader::metadataOffset(counterId);

        if (buffer.getInt32(recordOffset + TYPE_ID_OFFSET) == RECORDING_POSITION_TYPE_ID) {
            return buffer.getInt64(recordOffset + concurrent::CountersReader::KEY_OFFSET + RECORDING_ID_OFFSET);
        }
    }

    return -1;
}

bool RecordingPos::isActive(concurrent::CountersReader& countersReader, std::int32_t counterId, std::int64_t recordingId) { return RecordingPos::getRecordingId(countersReader, counterId) == recordingId; }

}  // namespace archive
}  // namespace aeron
