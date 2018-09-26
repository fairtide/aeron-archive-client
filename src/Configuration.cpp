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


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <string>

#include "Configuration.h"

namespace {
// Timeout when waiting on a message to be sent or received.
static const std::string MESSAGE_TIMEOUT_PROP_NAME = "aeron.archive.message.timeout";

// Timeout when waiting on a message to be sent or received.
static const std::int64_t MESSAGE_TIMEOUT_DEFAULT_NS = 5 * 1000000000L;

// Channel for sending control messages to an archive.
static const std::string CONTROL_CHANNEL_PROP_NAME = "aeron.archive.control.channel";

// Channel for sending control messages to an archive.
static const std::string CONTROL_CHANNEL_DEFAULT = "aeron:udp?endpoint=localhost:8010";

// Stream id within a channel for sending control messages to an archive.
static const std::string CONTROL_STREAM_ID_PROP_NAME = "aeron.archive.control.stream.id";

// Stream id within a channel for sending control messages to an archive.
static const std::int32_t CONTROL_STREAM_ID_DEFAULT = 10;

// Channel for sending control messages to a driver local archive.
static const std::string LOCAL_CONTROL_CHANNEL_PROP_NAME = "aeron.archive.local.control.channel";

// Channel for sending control messages to a driver local archive. Default to IPC.
static const std::string LOCAL_CONTROL_CHANNEL_DEFAULT = "aeron:ipc";

// Stream id within a channel for sending control messages to a driver local archive.
static const std::string LOCAL_CONTROL_STREAM_ID_PROP_NAME = "aeron.archive.local.control.stream.id";

// Stream id within a channel for sending control messages to a driver local archive.
static const std::int32_t LOCAL_CONTROL_STREAM_ID_DEFAULT = 11;

// Channel for receiving control response messages from an archive.
static const std::string CONTROL_RESPONSE_CHANNEL_PROP_NAME = "aeron.archive.control.response.channel";

// Channel for receiving control response messages from an archive.
static const std::string CONTROL_RESPONSE_CHANNEL_DEFAULT = "aeron:udp?endpoint=localhost:8020";

// Stream id within a channel for receiving control messages from an archive.
static const std::string CONTROL_RESPONSE_STREAM_ID_PROP_NAME = "aeron.archive.control.response.stream.id";

// Stream id within a channel for receiving control messages from an archive.
static const std::int32_t CONTROL_RESPONSE_STREAM_ID_DEFAULT = 20;

// Channel for receiving progress events of recordings from an archive.
static const std::string RECORDING_EVENTS_CHANNEL_PROP_NAME = "aeron.archive.recording.events.channel";

// Channel for receiving progress events of recordings from an archive.
// For production it is recommended that multicast or dynamic multi-destination-cast (MDC) is used to allow
// for dynamic subscribers.
static const std::string RECORDING_EVENTS_CHANNEL_DEFAULT = "aeron:udp?endpoint=localhost:8030";

// Stream id within a channel for receiving progress of recordings from an archive.
static const std::string RECORDING_EVENTS_STREAM_ID_PROP_NAME = "aeron.archive.recording.events.stream.id";

// Stream id within a channel for receiving progress of recordings from an archive.
static const std::int32_t RECORDING_EVENTS_STREAM_ID_DEFAULT = 30;

// Sparse term buffer indicator for control streams.
static const std::string CONTROL_TERM_BUFFER_SPARSE_PARAM_NAME = "aeron.archive.control.term.buffer.sparse";

// Overrides {@link io.aeron.driver.Configuration#TERM_BUFFER_SPARSE_FILE_PROP_NAME} for if term buffer files
// are sparse on the control channel.
static const bool CONTROL_TERM_BUFFER_SPARSE_DEFAULT = true;

// Term length for control streams.
static const std::string CONTROL_TERM_BUFFER_LENGTH_PARAM_NAME = "aeron.archive.control.term.buffer.length";

// Low term length for control channel reflects expected low bandwidth usage.
static const std::int32_t CONTROL_TERM_BUFFER_LENGTH_DEFAULT = 64 * 1024;

// MTU length for control streams.
static const std::string CONTROL_MTU_LENGTH_PARAM_NAME = "aeron.archive.control.mtu.length";

// MTU to reflect default for the control streams.
static const std::int32_t CONTROL_MTU_LENGTH_DEFAULT = 1408;
}

namespace aeron {
namespace archive {

Configuration::Configuration() {
    messageTimeoutNs_ = MESSAGE_TIMEOUT_DEFAULT_NS;
    controlChannel_ = CONTROL_CHANNEL_DEFAULT;
    controlStreamId_ = CONTROL_STREAM_ID_DEFAULT;
    localControlChannel_ = LOCAL_CONTROL_CHANNEL_DEFAULT;
    localControlStreamId_ = LOCAL_CONTROL_STREAM_ID_DEFAULT;
    controlResponseChannel_ = CONTROL_RESPONSE_CHANNEL_DEFAULT;
    controlResponseStreamId_ = CONTROL_RESPONSE_STREAM_ID_DEFAULT;
    recordingEventsChannel_ = RECORDING_EVENTS_CHANNEL_DEFAULT;
    recordingEventsStreamId_ = RECORDING_EVENTS_STREAM_ID_DEFAULT;
    controlTermBufferSparse_ = CONTROL_TERM_BUFFER_SPARSE_DEFAULT;
    controlTermBufferLength_ = CONTROL_TERM_BUFFER_LENGTH_DEFAULT;
    controlMtuLength_ = CONTROL_MTU_LENGTH_DEFAULT;
}

Configuration::Configuration(const std::string & filename) {
    using namespace boost::property_tree;

    ptree pt;
    ini_parser::read_ini(filename, pt);

    messageTimeoutNs_ = pt.get(MESSAGE_TIMEOUT_PROP_NAME, MESSAGE_TIMEOUT_DEFAULT_NS);
    controlChannel_ = pt.get(CONTROL_CHANNEL_PROP_NAME, CONTROL_CHANNEL_DEFAULT);
    controlStreamId_ = pt.get(CONTROL_STREAM_ID_PROP_NAME, CONTROL_STREAM_ID_DEFAULT);
    localControlChannel_ = pt.get(LOCAL_CONTROL_CHANNEL_PROP_NAME, LOCAL_CONTROL_CHANNEL_DEFAULT);
    localControlStreamId_ = pt.get(LOCAL_CONTROL_STREAM_ID_PROP_NAME, LOCAL_CONTROL_STREAM_ID_DEFAULT);
    controlResponseChannel_ = pt.get(CONTROL_RESPONSE_CHANNEL_PROP_NAME, CONTROL_RESPONSE_CHANNEL_DEFAULT);
    controlResponseStreamId_ = pt.get(CONTROL_RESPONSE_STREAM_ID_PROP_NAME, CONTROL_RESPONSE_STREAM_ID_DEFAULT);
    recordingEventsChannel_ = pt.get(RECORDING_EVENTS_CHANNEL_PROP_NAME, RECORDING_EVENTS_CHANNEL_DEFAULT);
    recordingEventsStreamId_ = pt.get(RECORDING_EVENTS_STREAM_ID_PROP_NAME, RECORDING_EVENTS_STREAM_ID_DEFAULT);
    controlTermBufferSparse_ = pt.get(CONTROL_TERM_BUFFER_SPARSE_PARAM_NAME, CONTROL_TERM_BUFFER_SPARSE_DEFAULT);
    controlTermBufferLength_ = pt.get(CONTROL_TERM_BUFFER_LENGTH_PARAM_NAME, CONTROL_TERM_BUFFER_LENGTH_DEFAULT);
    controlMtuLength_ = pt.get(CONTROL_MTU_LENGTH_PARAM_NAME, CONTROL_MTU_LENGTH_DEFAULT);
}

}
}

