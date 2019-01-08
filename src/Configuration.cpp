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

#include <cstdlib>

#include "util/PropertiesReader.h"
#include "Configuration.h"

#define DECLARE_PROPERTY(name, cap_name, type, key_name, value) \
    struct name { \
        static const char * key() { return key_name; }; \
        static type defaultValue() { return type(value); }; \
        static type getEnvVarOrDefault() { \
            char * evv = std::getenv("AERON_ARCHIVE_"#cap_name); \
            if (!evv) return defaultValue(); \
            return boost::lexical_cast<type>(evv); \
        }}; \
    static type get##name() { return name::getEnvVarOrDefault(); }

namespace {
// Timeout when waiting on a message to be sent or received.
DECLARE_PROPERTY(MessageTimeout, MESSAGE_TIMEOUT, std::int64_t, "aeron.archive.message.timeout", 5 * 1000000000L)

// Channel for sending control messages to an archive.
DECLARE_PROPERTY(ControlChannel, CONTROL_CHANNEL, std::string, "aeron.archive.control.channel", "aeron:udp?endpoint=localhost:8010")

// Stream id within a channel for sending control messages to an archive.
DECLARE_PROPERTY(ControlStreamId, CONTROL_STREAM_ID, std::int32_t, "aeron.archive.control.stream.id", 10)

// Channel for sending control messages to a driver local archive. Default to IPC.
DECLARE_PROPERTY(LocalControlChannel, LOCAL_CONTROL_CHANNEL, std::string, "aeron.archive.local.control.channel", "aeron:ipc")

// Stream id within a channel for sending control messages to a driver local archive.
DECLARE_PROPERTY(LocalControlStreamId, LOCAL_CONTROL_STREAM_ID, std::int32_t, "aeron.archive.local.control.stream.id", 11)

// Channel for receiving control response messages from an archive.
DECLARE_PROPERTY(ControlResponseChannel, CONTROL_RESPONSE_CHANNEL, std::string, "aeron.archive.control.response.channel", "aeron:udp?endpoint=localhost:8020")

// Stream id within a channel for receiving control messages from an archive.
DECLARE_PROPERTY(ControlResponseStreamId, CONTROL_RESPONSE_STREAM_ID, std::int32_t, "aeron.archive.control.response.stream.id", 20)

// Channel for receiving progress events of recordings from an archive.
// For production it is recommended that multicast or dynamic multi-destination-cast (MDC) is used to allow
// for dynamic subscribers.
DECLARE_PROPERTY(RecordingEventsChannel, RECORDING_EVENTS_CHANNEL, std::string, "aeron.archive.recording.events.channel", "aeron:udp?endpoint=localhost:8030")

// Stream id within a channel for receiving progress of recordings from an archive.
DECLARE_PROPERTY(RecordingEventsStreamId, RECORDING_EVENTS_STREAM_ID, std::int32_t, "aeron.archive.recording.events.stream.id", 30)

// Sparse term buffer indicator for control streams.
DECLARE_PROPERTY(ControlTermBufferSparse, CONTROL_TERM_BUFFER_SPARSE, bool, "aeron.archive.control.term.buffer.sparse", true)

// Term length for control streams.
// Low term length for control channel reflects expected low bandwidth usage.
DECLARE_PROPERTY(ControlTermBufferLength, CONTROL_TERM_BUFFER_LENGTH, std::int32_t, "aeron.archive.control.term.buffer.length", 64 * 1024)

// MTU length for control streams.
DECLARE_PROPERTY(ControlMtuLength, CONTROL_MTU_LENGTH, std::int32_t, "aeron.archive.control.mtu.length", 1408)
}  // namespace

namespace aeron {
namespace archive {

Configuration::Configuration() {
    messageTimeoutNs = getMessageTimeout();
    controlChannel = getControlChannel();
    controlStreamId = getControlStreamId();
    localControlChannel = getLocalControlChannel();
    localControlStreamId = getLocalControlStreamId();
    controlResponseChannel = getControlResponseChannel();
    controlResponseStreamId = getControlResponseStreamId();
    recordingEventsChannel = getRecordingEventsChannel();
    recordingEventsStreamId = getRecordingEventsStreamId();
    controlTermBufferSparse = getControlTermBufferSparse();
    controlTermBufferLength = getControlTermBufferLength();
    controlMtuLength = getControlMtuLength();
}

Configuration::Configuration(const std::string& filename) {
    util::PropertiesReader pr(filename, true);

    messageTimeoutNs = pr.get(MessageTimeout::key(), MessageTimeout::defaultValue());
    controlChannel = pr.get(ControlChannel::key(), ControlChannel::defaultValue());
    controlStreamId = pr.get(ControlStreamId::key(), ControlStreamId::defaultValue());
    localControlChannel = pr.get(LocalControlChannel::key(), LocalControlChannel::defaultValue());
    localControlStreamId = pr.get(LocalControlStreamId::key(), LocalControlStreamId::defaultValue());
    controlResponseChannel = pr.get(ControlResponseChannel::key(), ControlResponseChannel::defaultValue());
    controlResponseStreamId = pr.get(ControlResponseStreamId::key(), ControlResponseStreamId::defaultValue());
    recordingEventsChannel = pr.get(RecordingEventsChannel::key(), RecordingEventsChannel::defaultValue());
    recordingEventsStreamId = pr.get(RecordingEventsStreamId::key(), RecordingEventsStreamId::defaultValue());
    controlTermBufferSparse = pr.get(ControlTermBufferSparse::key(), ControlTermBufferSparse::defaultValue());
    controlTermBufferLength = pr.get(ControlTermBufferLength::key(), ControlTermBufferLength::defaultValue());
    controlMtuLength = pr.get(ControlMtuLength::key(), ControlMtuLength::defaultValue());
}

}  // namespace archive
}  // namespace aeron
