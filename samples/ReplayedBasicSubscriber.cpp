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

#include <signal.h>
#include <atomic>
#include <chrono>

#include <boost/program_options.hpp>

#include <AeronArchive.h>
#include <ChannelUri.h>

#include "SamplesUtil.h"

namespace po = boost::program_options;

namespace {
const std::chrono::duration<long, std::milli> IDLE_SLEEP_MS(1);
const int FRAGMENTS_LIMIT = 10;

std::atomic<bool> running{true};

void sigIntHandler(int) { running = false; }

aeron::fragment_handler_t printStringMessage() {
    return [&](const aeron::AtomicBuffer& buffer, aeron::util::index_t offset, aeron::util::index_t length,
               const aeron::Header& header) {
        std::cout << "Message to stream " << header.streamId() << " from session " << header.sessionId() << ", stream: " << header.streamId();
        std::cout << "(" << length << "@" << offset << "@" << header.position() << ") <<";
        std::cout << std::string(reinterpret_cast<const char*>(buffer.buffer()) + offset,
                                 static_cast<std::size_t>(length))
                  << ">>" << std::endl;
    };
}

}  // namespace

int main(int argc, char* argv[]) {
    ::signal(SIGINT, sigIntHandler);

    std::string channel, configFile;
    std::int32_t streamId;
    std::int32_t frameCountLimit;
    std::int64_t recId;
    std::int64_t position;

    po::options_description desc("Options");
    desc.add_options()("help", "print help message")(
        "channel,c", po::value<std::string>(&channel)->default_value("aeron:udp?endpoint=localhost:40123"))(
        "stream-id,i", po::value<std::int32_t>(&streamId)->default_value(10))(
        "frame-count-limit", po::value<std::int32_t>(&frameCountLimit)->default_value(20))(
        "recId", po::value<std::int64_t>(&recId)->default_value(-1))(
        "position", po::value<std::int64_t>(&position)->default_value(0))(
        "file,f", po::value<std::string>(&configFile));

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

        std::unique_ptr<aeron::archive::Configuration> cfg;

        if (!configFile.empty()) {
            cfg = std::make_unique<aeron::archive::Configuration>(configFile);
        } else {
            cfg = std::make_unique<aeron::archive::Configuration>();
        }

        aeron::archive::Context ctx(*cfg);
        auto archive = aeron::archive::AeronArchive::connect(ctx);

        std::int64_t recordingId =
            (recId != -1) ? recId : aeron::archive::findLatestRecordingId(*archive, channel, streamId);
        auto subscription =
            archive->replay(recordingId, position, std::numeric_limits<std::int64_t>::max(), channel, replayStreamId,
                    [](aeron::Image& image) {
                        std::cout << "onAvailableImage: sourceIdty: " << image.sourceIdentity()
                            << ", session: " << image.sessionId() << ", joinPos: " << image.joinPosition()
                            << ", pos: " << image.position()
                            << ", isEos: " << image.isEndOfStream()
                            << ", isClosed: " << image.isClosed() << '\n';
                    },
                    [](aeron::Image& image) {
                        std::cout << "onUnavailableImage: sourceIdty: " << image.sourceIdentity()
                            << ", session: " << image.sessionId() << ", joinPos: " << image.joinPosition()
                            << ", pos: " << image.position()
                            << ", isEos: " << image.isEndOfStream()
                            << ", isClosed: " << image.isClosed() << '\n';
                    });

        // polling loop
        auto handler = printStringMessage();
        aeron::concurrent::SleepingIdleStrategy idleStrategy(IDLE_SLEEP_MS);
        bool reachedEos{false};

        while (running && !reachedEos) {
            const int fragmentsRead = subscription->poll(handler, FRAGMENTS_LIMIT);

            if (0 == fragmentsRead) {
                if (subscription->pollEndOfStreams([](aeron::Image& image) {
                        std::cout << "EOS image correlationId=" << image.correlationId()
                                  << " sessionId=" << image.sessionId() << " from " << image.sourceIdentity()
                                  << " position=" << image.position() << '\n';
                    })) {
                    reachedEos = true;
                }
            }

            idleStrategy.idle(fragmentsRead);
        }

        std::cout << "Shutting down...\n";
    } catch (const aeron::archive::ArchiveException& e) {
        std::cerr << "aeron archive exception: " << e.what() << " (" << e.where() << ")\n" << '\n';
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
