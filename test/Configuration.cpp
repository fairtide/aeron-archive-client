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

#include <gtest/gtest.h>
#include <chrono>
#include <fstream>

#include <Configuration.h>

namespace {
const std::string propertyFileContent = R"#(
aeron.archive.control.channel=aaaBBBwwwQQQ
aeron.archive.control.mtu.length=2048
aeron.archive.control.response.channel=ctrRespChannel
aeron.archive.control.response.stream.id=55
aeron.archive.control.stream.id=42
aeron.archive.control.term.buffer.length=4096
aeron.archive.control.term.buffer.sparse=0
aeron.archive.local.control.channel=aeron:opc
aeron.archive.local.control.stream.id=33
aeron.archive.message.timeout=1234567
aeron.archive.recording.events.channel=recEvtsChannel
aeron.archive.recording.events.stream.id=22
)#";
}

void SetDefaultEnvs() {
}

class ConfigurationTest : public ::testing::Test {
protected:
    void SetUp() override {
        using namespace std::chrono;

        // generate an almost unique filename
        std::uint64_t ts = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();
        filename = "ConfigurationTest." + std::to_string(ts);

        std::ofstream of(filename);
        if (!of.is_open()) {
            throw std::runtime_error("Can't create a temporary file: " + filename);
        }

        of << propertyFileContent;
        of.close();

        // set env variables
        setenv("AERON_ARCHIVE_MESSAGE_TIMEOUT", "123456", 1);
        setenv("AERON_ARCHIVE_CONTROL_CHANNEL", "qqqwwweee", 1);
        setenv("AERON_ARCHIVE_CONTROL_STREAM_ID", "42", 1);
        setenv("AERON_ARCHIVE_LOCAL_CONTROL_CHANNEL", "ipc:aeron", 1);
        setenv("AERON_ARCHIVE_LOCAL_CONTROL_STREAM_ID", "314", 1);
        setenv("AERON_ARCHIVE_CONTROL_RESPONSE_CHANNEL", "ResponsEconTrol", 1);
        setenv("AERON_ARCHIVE_CONTROL_RESPONSE_STREAM_ID", "271", 1);
        setenv("AERON_ARCHIVE_RECORDING_EVENTS_CHANNEL", "aaaaassss", 1);
        setenv("AERON_ARCHIVE_RECORDING_EVENTS_STREAM_ID", "144", 1);
        setenv("AERON_ARCHIVE_CONTROL_TERM_BUFFER_SPARSE", "0", 1);
        setenv("AERON_ARCHIVE_CONTROL_TERM_BUFFER_LENGTH", "1024", 1);
        setenv("AERON_ARCHIVE_CONTROL_MTU_LENGTH", "1812", 1);
    }

    void TearDown() override { std::remove(filename.c_str()); }

    std::string filename;
};

TEST_F(ConfigurationTest, shouldReadConfigFromEnvVariables) {
    aeron::archive::Configuration cfg;

    EXPECT_EQ(123456, cfg.messageTimeoutNs);
    EXPECT_EQ("qqqwwweee", cfg.controlChannel);
    EXPECT_EQ(42, cfg.controlStreamId);
    EXPECT_EQ("ipc:aeron", cfg.localControlChannel);
    EXPECT_EQ(314, cfg.localControlStreamId);
    EXPECT_EQ("ResponsEconTrol", cfg.controlResponseChannel);
    EXPECT_EQ(271, cfg.controlResponseStreamId);
    EXPECT_EQ("aaaaassss", cfg.recordingEventsChannel);
    EXPECT_EQ(144, cfg.recordingEventsStreamId);
    EXPECT_EQ(false, cfg.controlTermBufferSparse);
    EXPECT_EQ(1024, cfg.controlTermBufferLength);
    EXPECT_EQ(1812, cfg.controlMtuLength);
}

TEST_F(ConfigurationTest, shouldReadConfigFromPropertyFile) {
    aeron::archive::Configuration cfg(filename);

    EXPECT_EQ(1234567, cfg.messageTimeoutNs);
    EXPECT_EQ("aaaBBBwwwQQQ", cfg.controlChannel);
    EXPECT_EQ(42, cfg.controlStreamId);
    EXPECT_EQ("aeron:opc", cfg.localControlChannel);
    EXPECT_EQ(33, cfg.localControlStreamId);
    EXPECT_EQ("ctrRespChannel", cfg.controlResponseChannel);
    EXPECT_EQ(55, cfg.controlResponseStreamId);
    EXPECT_EQ("recEvtsChannel", cfg.recordingEventsChannel);
    EXPECT_EQ(22, cfg.recordingEventsStreamId);
    EXPECT_EQ(false, cfg.controlTermBufferSparse);
    EXPECT_EQ(4096, cfg.controlTermBufferLength);
    EXPECT_EQ(2048, cfg.controlMtuLength);
}


TEST_F(ConfigurationTest, shouldThrowExceptionOnIncorrectFilename) {
    EXPECT_ANY_THROW(aeron::archive::Configuration cfg("I tHinK this FileNAmE shOulD n0t exIst"));
}
