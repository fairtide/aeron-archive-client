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

namespace aeron {
namespace archive {

std::int32_t RecordingPos::findCounterIdBySession(aeron::concurrent::CountersReader& countersReader,
                                                  std::int32_t sessionId) {
    return -1;
}

std::int64_t RecordingPos::getRecordingId(aeron::concurrent::CountersReader& countersReader, std::int32_t counterId) {
    return -1;
}

bool RecordingPos::isActive(aeron::concurrent::CountersReader& countersReader, std::int32_t counterId,
                            std::int64_t recordingId) {
    return false;
}

}  // namespace archive
}  // namespace aeron
