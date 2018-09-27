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


#include <AeronArchive.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace {
std::int64_t findLatestRecordingId(aeron::archive::AeronArchive& archive, const std::string & channel, std::int32_t streamId) {
    std::int64_t lastRecordingId;

    auto consumer = [&](long controlSessionId, long correlationId, long recordingId, long startTimestamp, long stopTimestamp, long startPosition, long stopPosition, int initialTermId, int segmentFileLength, int termBufferLength,
                        int mtuLength, int sessionId, int streamId, const std::string& strippedChannel, const std::string& originalChannel, const std::string& sourceIdentity)
    {
        lastRecordingId = recordingId;
    };

    std::int32_t foundCount = archive.listRecordingsForUri(0, 100, channel, streamId, consumer);

    if (!foundCount)
    {
        throw std::runtime_error("no recordings found");
    }

    return lastRecordingId;
}
}  // namespace

int main(int argc, char* argv[]) {
    std::string channel;
    std::int32_t streamId;
    std::int32_t frameCountLimit;

    po::options_description desc("Options");
    desc.add_options()
        ("help", "print help message")
        ("channel", po::value<std::string>(&channel)->default_value("aeron:udp?endpoint=localhost:40123"))
        ("stream-id", po::value<std::int32_t>(&streamId)->default_value(10))
        ("frame-count-limit", po::value<std::int32_t>(&frameCountLimit)->default_value(20));

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';
            return 1;
        }

        std::int32_t replayStreamId = streamId + 1;

        std::cout << "Subscribing to " << channel << " on stream id " << streamId << '\n';

        // TODO: aeron::archive::Context ctx;
        auto archive = aeron::archive::AeronArchive::connect();

        std::int64_t recordingId = findLatestRecordingId(*archive, channel, streamId);
        std::int64_t sessionId = archive->startReplay(recordingId, 0, std::numeric_limits<std::int64_t>::max(),
                channel, replayStreamId);

        // TODO: implement ChannelUri, we gonna need it
        std::string channel = "ChannelUri::addSessionId" + std::to_string(sessionId);

        std::int64_t subId = archive->context().aeron()->addSubscription(channel, replayStreamId);
        std::shared_ptr<aeron::Subscription> subscription;
        while (!(subscription = archive->context().aeron()->findSubscription(subId)))
        {
            std::this_thread::yield();
        }

        // TODO: implement subscriber loop
    } catch (const std::exception& e) {
        std::cerr << "exception: " << e.what() << '\n';
        return 1;
    }

    std::cout << "exiting...\n";

    return 0;
}
