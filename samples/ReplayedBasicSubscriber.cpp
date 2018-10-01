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


#include <signal.h>
#include <atomic>
#include <chrono>

#include <boost/program_options.hpp>

#include <AeronArchive.h>
#include <ChannelUri.h>

namespace po = boost::program_options;

namespace {
const std::chrono::duration<long, std::milli> IDLE_SLEEP_MS(1);
const int FRAGMENTS_LIMIT = 10;

std::atomic<bool> running{true};

void sigIntHandler(int) { running = false; }

std::int64_t findLatestRecordingId(aeron::archive::AeronArchive& archive, const std::string& channel,
                                   std::int32_t streamId) {
    std::int64_t lastRecordingId{-1};

    auto consumer = [&](long controlSessionId, long correlationId, long recordingId, long startTimestamp,
                        long stopTimestamp, long startPosition, long stopPosition, int initialTermId,
                        int segmentFileLength, int termBufferLength, int mtuLength, int sessionId, int streamId,
                        const std::string& strippedChannel, const std::string& originalChannel,
                        const std::string& sourceIdentity) { lastRecordingId = recordingId; };

    std::int32_t foundCount = archive.listRecordingsForUri(0, 100, channel, streamId, consumer);

    if (!foundCount) {
        throw std::runtime_error("no recordings found");
    }

    std::cout << "found " << foundCount << ", last recording id = " << lastRecordingId << '\n';

    return lastRecordingId;
}

aeron::fragment_handler_t printStringMessage() {
    return [&](const aeron::AtomicBuffer& buffer, aeron::util::index_t offset, aeron::util::index_t length,
               const aeron::Header& header) {
        std::cout << "Message to stream " << header.streamId() << " from session " << header.sessionId();
        std::cout << "(" << length << "@" << offset << ") <<";
        std::cout << std::string(reinterpret_cast<const char*>(buffer.buffer()) + offset,
                                 static_cast<std::size_t>(length))
                  << ">>" << std::endl;
    };
}

}  // namespace

int main(int argc, char* argv[]) {
    ::signal(SIGINT, sigIntHandler);

    std::string channel;
    std::int32_t streamId;
    std::int32_t frameCountLimit;

    po::options_description desc("Options");
    desc.add_options()("help", "print help message")(
        "channel", po::value<std::string>(&channel)->default_value("aeron:udp?endpoint=localhost:40123"))(
        "stream-id", po::value<std::int32_t>(&streamId)->default_value(10))(
        "frame-count-limit", po::value<std::int32_t>(&frameCountLimit)->default_value(20));

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
        std::int64_t sessionId =
            archive->startReplay(recordingId, 0, std::numeric_limits<std::int64_t>::max(), channel, replayStreamId);

        std::string replayChannel = aeron::archive::ChannelUri::addSessionId(channel, sessionId);

        std::int64_t subId = archive->context().aeron()->addSubscription(replayChannel, replayStreamId);
        std::shared_ptr<aeron::Subscription> subscription;
        while (!(subscription = archive->context().aeron()->findSubscription(subId))) {
            std::this_thread::yield();
        }

        // polling loop
        auto handler = printStringMessage();
        aeron::concurrent::SleepingIdleStrategy idleStrategy(IDLE_SLEEP_MS);
        bool reachedEos{false};

        while (running && !reachedEos) {
            const int fragmentsRead = subscription->poll(handler, FRAGMENTS_LIMIT);

            if (0 == fragmentsRead) {
                if (subscription->pollEndOfStreams([](aeron::Image& image) {
                        std::cout << "EOS image correlationId=" << image.correlationId()
                                  << " sessionId=" << image.sessionId() << " from " << image.sourceIdentity() << '\n';
                    })) {
                    reachedEos = true;
                }
            }

            idleStrategy.idle(fragmentsRead);
        }

        std::cout << "Shutting down...\n";
    } catch (const aeron::archive::ArchiveException& e) {
        std::cerr << "aeron archive exception: " << e.what() << " (" << e.where() << ")\n" << e.stackTrace() << '\n';
        return 1;
    } catch (const aeron::util::SourcedException& e) {
        std::cerr << "aeron exception: " << e.what() << " (" << e.where() << ")\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "exception: " << e.what() << '\n';
        return 1;
    }

    std::cout << "done\n";

    return 0;
}
