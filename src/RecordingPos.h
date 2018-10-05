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

#include <concurrent/CountersReader.h>

namespace aeron {
namespace archive {

struct RecordingPos {
    static std::int32_t findCounterIdByRecording(aeron::concurrent::CountersReader& countersReader,
                                               std::int64_t recordingId);

    static std::int32_t findCounterIdBySession(aeron::concurrent::CountersReader& countersReader,
                                               std::int32_t sessionId);

    static std::int64_t getRecordingId(aeron::concurrent::CountersReader& countersReader, std::int32_t counterId);

    static bool isActive(aeron::concurrent::CountersReader& countersReader, std::int32_t counterId,
                         std::int64_t recordingId);
};
}  // namespace archive
}  // namespace aeron
