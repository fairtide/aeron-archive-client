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
#include <type_traits>

#include <boost/program_options.hpp>

#include <AeronArchive.h>
#include <ChannelUri.h>
#include <RecordingPos.h>

namespace po = boost::program_options;
using namespace aeron;

namespace {
std::atomic<bool> running{true};

void sigIntHandler(int) { running = false; }
}  // namespace

int main(int argc, char* argv[]) {
    ::signal(SIGINT, sigIntHandler);

    std::string channel;
    std::int32_t streamId;
    std::int32_t messagesCount;
    bool stopRecording;

    po::options_description desc("Options");
    desc.add_options()("help", "print help message")(
        "channel", po::value<std::string>(&channel)->default_value("aeron:udp?endpoint=localhost:40123"))(
        "stream-id", po::value<std::int32_t>(&streamId)->default_value(10))(
        "messages-count", po::value<std::int32_t>(&messagesCount)->default_value(1000000))(
        "stop-recording", po::value<bool>(&stopRecording)->default_value(false));

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';
            return 1;
        }

        std::cout << "Publishing to " << channel << " on stream id " << streamId << '\n';

        // TODO: archive::Context ctx;
        auto archive = archive::AeronArchive::connect();
        auto aeron = archive->context().aeron();

        if (stopRecording) {
            // stop recording from a previous run before start
            archive->stopRecording(channel, streamId);
        }

        archive->startRecording(channel, streamId, io::aeron::archive::codecs::SourceLocation::LOCAL);

        std::int64_t pubId = aeron->addPublication(channel, streamId);
        std::shared_ptr<Publication> publication;
        while (!(publication = aeron->findPublication(pubId))) {
            std::this_thread::yield();
        }

        // find an archiving counter
        auto& counters = aeron->countersReader();
        std::int32_t counterId = archive::RecordingPos::findCounterIdBySession(counters, publication->sessionId());
        while (-1 == counterId) {
            if (!running) {
                return 1;
            }

            std::this_thread::yield();
            counterId = archive::RecordingPos::findCounterIdBySession(counters, publication->sessionId());
        }

        // wait for recording to start
        std::int64_t recordingId = archive::RecordingPos::getRecordingId(counters, counterId);
        std::cout << "Recording started, recording id = " << recordingId << '\n';

        // publish messages
        std::array<std::uint8_t, 256> buffer;
        concurrent::AtomicBuffer srcBuffer(buffer);
        char message[256];

        for (std::int32_t i = 0; i < messagesCount && running; ++i) {
            int messageLen = ::snprintf(message, sizeof(message), "Hello World! %d", i);
            srcBuffer.putBytes(0, reinterpret_cast<std::uint8_t*>(message), messageLen);

            std::cout << "offering " << i << "/" << messagesCount << " - ";

            std::int64_t result = publication->offer(srcBuffer, 0, messageLen);

            if (result < 0) {
                if (BACK_PRESSURED == result) {
                    std::cout << "Offer failed due to back pressure\n";
                } else if (NOT_CONNECTED == result) {
                    std::cout << "Offer failed because publisher is not connected to subscriber\n";
                } else if (ADMIN_ACTION == result) {
                    std::cout << "Offer failed because of an administration action in the system\n";
                } else if (PUBLICATION_CLOSED == result) {
                    std::cout << "Offer failed publication is closed\n";
                } else {
                    std::cout << "Offer failed due to unknown reason" << result << std::endl;
                }
            } else {
                std::cout << "yay!\n";
            }

            //
            auto errorMessage = archive->pollForErrorResponse();
            if (errorMessage) {
                throw util::IllegalStateException(*errorMessage, SOURCEINFO);
            }

            ::sleep(1);
        }

        // wait for recording to complete
        while (counters.getCounterValue(counterId) < publication->position()) {
            if (!archive::RecordingPos::isActive(counters, counterId, recordingId)) {
                std::cerr << "recording has stopped unexpectedly: " << recordingId << '\n';
                break;
            }

            std::this_thread::yield();
        }

        archive->stopRecording(channel, streamId);

        std::cout << "Shutting down...\n";
    } catch (const archive::ArchiveException& e) {
        std::cerr << "aeron archive exception: " << e.what() << " (" << e.where() << ")\n" << e.stackTrace() << '\n';
        return 1;
    } catch (const util::SourcedException& e) {
        std::cerr << "aeron exception: " << e.what() << " (" << e.where() << ")\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "exception: " << e.what() << '\n';
        return 1;
    }

    std::cout << "done\n";

    return 0;
}
