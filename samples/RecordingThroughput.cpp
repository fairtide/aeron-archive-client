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
#include <vector>

#include <boost/program_options.hpp>

#include <AeronArchive.h>
#include <RecordingEventsAdapter.h>
#include <concurrent/YieldingIdleStrategy.h>

namespace archive = aeron::archive;
namespace codecs = io::aeron::archive::codecs;
namespace po = boost::program_options;

namespace {
const int FRAGMENTS_LIMIT = 10;
const double MEGABYTE = 1024.0 * 1024.0;

std::atomic<bool> running{true};

void sigIntHandler(int) { running = false; }

auto now() { return std::chrono::high_resolution_clock::now(); }

}  // namespace

//
class RecordingThroughput {
public:
    RecordingThroughput(const archive::Configuration& cfg, const std::string& channel, std::int32_t streamId,
                        std::int32_t numberOfMessages, std::int32_t messageLength, bool disableRecording)
        : disableRecording_(disableRecording)
        , ctx_(cfg)
        , archive_(archive::AeronArchive::connect(ctx_))
        , aeron_(archive_->context().aeron())
        , channel_(channel)
        , streamId_(streamId)
        , numberOfMessages_(numberOfMessages)
        , messageLength_(messageLength)
        , recordingThread_([this] {
            if (!disableRecording_) {
                runRecordingEventsPoller();
            }
        }) {}

    ~RecordingThroughput() {
        recordingThread_.join();
        stopRecording();
    }

    void startRecording() {
        if (!disableRecording_) {
            archive_->startRecording(channel_, streamId_, codecs::SourceLocation::LOCAL);
            std::cout << "started recording of " << channel_ << ":" << streamId_ << '\n';
        }
    }

    void stopRecording() {
        if (!disableRecording_) {
            archive_->stopRecording(channel_, streamId_);
            std::cout << "stopped recording of " << channel_ << ":" << streamId_ << '\n';
        }
    }

    void streamMessagesForRecording() {
        std::shared_ptr<aeron::ExclusivePublication> publication;

        std::int64_t pubId = aeron_->addExclusivePublication(channel_, streamId_);
        while (!(publication = aeron_->findExclusivePublication(pubId))) {
            std::this_thread::yield();
        }

        isRecording_ = true;
        stopPosition_ = std::numeric_limits<std::int64_t>::max();
        recordingStartTime_ = now();

        std::vector<std::uint8_t> buffer(messageLength_);
        aeron::concurrent::AtomicBuffer bufferWrap(&buffer[0], messageLength_);

        std::cout << "publishing " << numberOfMessages_ << " messages...\n";

        auto start = now();

        for (std::int32_t i = 0; i < numberOfMessages_; ++i) {
            bufferWrap.putInt32(0, 'a' + i % ('z' - 'a'));
            while (publication->offer(bufferWrap, 0, messageLength_) < 0) {
                std::this_thread::yield();
            }
        }

        std::int64_t durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(now() - start).count();
        std::int64_t msgRate = (numberOfMessages_ / durationMs) * 1000;

        stopPosition_ = publication->position();

        std::cout << "all messages are published. Stop position: " << stopPosition_ << ", msg rate = " << msgRate
                  << " msg/sec\n";

        while (!disableRecording_ && isRecording_) {
            ::sleep(1);
        }
    }

private:
    void runRecordingEventsPoller() {
        std::cout << "polling thread started\n";

        try {
            std::int64_t id = aeron_->addSubscription(ctx_.recordingEventsChannel(), ctx_.recordingEventsStreamId());

            std::shared_ptr<aeron::Subscription> subscription;
            while (!(subscription = aeron_->findSubscription(id))) {
                std::this_thread::yield();
            }

            aeron::concurrent::YieldingIdleStrategy idleStrategy;
            archive::RecordingEventsAdapter adapter(subscription, FRAGMENTS_LIMIT, OnStartHandler(),
                                                    OnProgressHandler(), OnStopHandler());

            std::cout << "listening for recording events on " << ctx_.recordingEventsChannel() << ":"
                      << ctx_.recordingEventsStreamId() << '\n';

            while (running) {
                idleStrategy.idle(adapter.poll());
            }
        } catch (const archive::ArchiveException& e) {
            std::cerr << "aeron archive exception: " << e.what() << " (" << e.where() << ")\n" << '\n';
        } catch (const aeron::util::SourcedException& e) {
            std::cerr << "aeron exception: " << e.what() << " (" << e.where() << ")\n";
        } catch (const std::exception& e) {
            std::cerr << "exception: " << e.what() << '\n';
        } catch (...) {
            std::cerr << "unknown exception\n";
        }

        std::cout << "polling thread completed\n";
    }

    archive::RecordingEventsAdapter::OnStart OnStartHandler() {
        return [this](std::int64_t recordingId, std::int64_t startPosition, std::int32_t sessionId,
                      std::int32_t streamId, const std::string& channel, const std::string& sourceIdentity) {
            std::cout << "Recording #" << recordingId << " started for " << channel << ":" << streamId << ":"
                      << sessionId << " from " << startPosition << " position\n";
        };
    }

    archive::RecordingEventsAdapter::OnProgress OnProgressHandler() {
        return [this](std::int64_t recordingId, std::int64_t startPosition, std::int64_t position) {
            if (position >= stopPosition_) {
                std::int64_t durationMs =
                    std::chrono::duration_cast<std::chrono::milliseconds>(now() - recordingStartTime_).count();
                std::int64_t recordingLength = position - startPosition;
                double dataRate = (recordingLength * 1000.0 / durationMs) / MEGABYTE;
                double recordingMb = recordingLength / MEGABYTE;
                std::int64_t msgRate = (numberOfMessages_ / durationMs) * 1000;

                std::cout << "Published and recorded " << recordingMb << " MB @ " << dataRate << " MB/s - " << msgRate
                          << " msg/sec - " << messageLength_ << " byte payload + 32 byte header\n";

                isRecording_ = false;
            }
        };
    }

    archive::RecordingEventsAdapter::OnStop OnStopHandler() {
        return [this](std::int64_t recordingId, std::int64_t startPosition, std::int64_t position) {
            std::cout << "Recording #" << recordingId << " stopped at " << position << " position\n";
            isRecording_ = false;
            running = false;
        };
    }

private:
    bool disableRecording_;

    archive::Context ctx_;
    std::shared_ptr<archive::AeronArchive> archive_;
    std::shared_ptr<aeron::Aeron> aeron_;

    const std::string& channel_;
    const std::int32_t streamId_;
    const std::int32_t numberOfMessages_;
    const std::int32_t messageLength_;

    std::thread recordingThread_;

    std::int64_t stopPosition_;
    bool isRecording_{false};
    decltype(now()) recordingStartTime_;
};

//
int main(int argc, char* argv[]) {
    ::signal(SIGINT, sigIntHandler);

    std::string channel, configFile;
    std::int32_t streamId;
    std::int32_t numberOfMessages;
    std::int32_t messageLength;
    bool disableRecording;

    po::options_description desc("Options");
    desc.add_options()("help", "print help message")(
        "channel,c", po::value<std::string>(&channel)->default_value("aeron:udp?endpoint=localhost:40123"))(
        "stream-id,i", po::value<std::int32_t>(&streamId)->default_value(10))(
        "messages-count,m", po::value<std::int32_t>(&numberOfMessages)->default_value(1000000))(
        "message-length,l", po::value<std::int32_t>(&messageLength)->default_value(256))(
        "disable-recording,d", po::bool_switch(&disableRecording)->default_value(false))(
        "file,f", po::value<std::string>(&configFile));

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';
            return 1;
        }

        std::unique_ptr<archive::Configuration> cfg;

        if (!configFile.empty()) {
            cfg = std::make_unique<archive::Configuration>(configFile);
        } else {
            cfg = std::make_unique<archive::Configuration>();
        }

        RecordingThroughput test(*cfg, channel, streamId, numberOfMessages, messageLength, disableRecording);

        test.startRecording();

        test.streamMessagesForRecording();

        std::cout << "Shutting down...\n";
    } catch (const archive::ArchiveException& e) {
        std::cerr << "aeron archive exception: " << e.what() << " (" << e.where() << ")\n" << '\n';
        return 1;
    } catch (const aeron::util::SourcedException& e) {
        std::cerr << "aeron exception: " << e.what() << " (" << e.where() << ")\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "exception: " << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "unknown exception\n";
    }

    std::cout << "done\n";

    return 0;
}
