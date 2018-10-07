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

#include <boost/program_options.hpp>

#include <AeronArchive.h>

#include "SamplesUtil.h"

namespace po = boost::program_options;
namespace codecs = io::aeron::archive::codecs;

using namespace aeron;

int main(int argc, char* argv[]) {
    std::string channel, configFile;
    std::int32_t streamId;
    bool remoteLocation;
    std::string command;
    std::int64_t recId;

    po::options_description desc("Options");
    desc.add_options()("help", "print help message")(
        "channel,c", po::value<std::string>(&channel)->default_value("aeron:udp?endpoint=localhost:40123"))(
        "stream-id,i", po::value<std::int32_t>(&streamId)->default_value(10))(
        "remote,r", po::bool_switch(&remoteLocation)->default_value(false))("command", po::value<std::string>(&command),
                                                                            "values: start, stop, list, delete")(
        "recId", po::value<std::int64_t>(&recId)->default_value(-1))("file,f", po::value<std::string>(&configFile));

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help") || command.empty()) {
            std::cout << desc << '\n';
            return 1;
        }

        std::unique_ptr<aeron::archive::Configuration> cfg;

        if (!configFile.empty()) {
            cfg = std::make_unique<aeron::archive::Configuration>(configFile);
        } else {
            cfg = std::make_unique<aeron::archive::Configuration>(configFile);
        }

        aeron::archive::Context ctx(*cfg);
        auto archive = aeron::archive::AeronArchive::connect(ctx);
        auto aeron = archive->context().aeron();

        auto location = remoteLocation ? codecs::SourceLocation::REMOTE : codecs::SourceLocation::LOCAL;

        if ("start" == command) {
            std::cout << "starting recording of " << channel << " on stream id " << streamId
                      << (remoteLocation ? " remote" : " local") << " location" << '\n';
            archive->startRecording(channel, streamId, location);
        } else if ("stop" == command) {
            std::cout << "stopping recording of " << channel << " on stream id " << streamId << '\n';
            archive->stopRecording(channel, streamId);
        } else if ("list" == command) {
            aeron::archive::findLatestRecordingId(*archive, channel, streamId);
        } else if ("delete" == command) {
            if (recId != -1) {
                std::cout << "truncating recording " << recId << " of " << channel << " on stream id " << streamId
                          << " to 0 position\n";
                archive->truncateRecording(recId, 0);
            } else {
                std::cerr << "recId is not correct\n";
            }
        } else {
            std::cerr << "unknown command: " << command << '\n';
        }
    } catch (const archive::ArchiveException& e) {
        std::cerr << "aeron archive exception: " << e.what() << " (" << e.where() << ")\n" << '\n';
        return 1;
    } catch (const util::SourcedException& e) {
        std::cerr << "aeron exception: " << e.what() << " (" << e.where() << ")\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "exception: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
