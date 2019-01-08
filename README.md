# aeron-archive-client

C++ implementation of Aeron Archive client.

Current version (v0.2.8) implements all commands from the Aeron Archive protocol. The C++ library interface is almost the same as the Java client library one originally created by the [Aeron](https://github.com/real-logic/aeron) team.

More details about persistent streams and Aeron Archive are available in the [real-logic/aeron wiki](https://github.com/real-logic/aeron/wiki/Aeron-Archive).

## Build

You require the following to build the C++ API for Aeron Archive Client:

* [CMake](http://www.cmake.org/) - 3.0.2 or higher
* C++14 compiler for the supported platform
* Recent Boost library
* Java JDK to build Aeron & SBE dependencies

The project has internal dependencies on [Aeron (v. 1.14.0)](https://github.com/real-logic/aeron), [simple-binary-encoding (v. 1.10.2)](https://github.com/real-logic/simple-binary-encoding) & [Google Test (v. 1.8.1)](https://github.com/google/googletest) libraries which are automatically downloaded from Github by the build system

To run the full build with tests and samples:

```shell
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ctest .
```

## Running samples

There are some samples provided:

* **RecordedBasicPublisher** - a simple publisher which creates a recording session with the archiving media driver
* **ReplayedBasicSubscriber** - a simple consumer of archived data
* **RecordingThroughput** - a tool to measure recording throughput
* **DriverControl** - a tool to control the archiving media driver (start/stop/delete/list recordings on a channel/stream id)

To run these samples one need to start the archiving media driver first:

```shell
$ java -cp <path-to>/aeron-all-1.14.0.jar io.aeron.archive.ArchivingMediaDriver
```

Then start **RecordedBasicPublisher** and some consumer which will subscribe to the publisher. The consumer is required for the driver to start recording. One could use a basic subscriber from Aeron samples

```shell
$ java -cp <path-to>/aeron-all-1.14.0.jar io.aeron.samples.BasicSubscriber
```

The **ReplayedBasicSubscriber** will replay the  recorded stream.

## Configuration

Fine tuning of the client can be done via a property file or via environment variables

| Environment variable | Corresponding Aeron property | Default value |
| ----------- | ----------- | ----------- |
| AERON_ARCHIVE_MESSAGE_TIMEOUT | aeron.archive.message.timeout | 5000000000 |
| AERON_ARCHIVE_CONTROL_CHANNEL | aeron.archive.control.channel | aeron:udp?endpoint=localhost:8010 |
| AERON_ARCHIVE_CONTROL_STREAM_ID | aeron.archive.control.stream.id | 10 |
| AERON_ARCHIVE_LOCAL_CONTROL_CHANNEL | aeron.archive.local.control.channel | aeron:ipc |
| AERON_ARCHIVE_LOCAL_CONTROL_STREAM_ID | aeron.archive.local.control.stream.id | 11 |
| AERON_ARCHIVE_CONTROL_RESPONSE_CHANNEL | aeron.archive.control.response.channel | aeron:udp?endpoint=localhost:8020 |
| AERON_ARCHIVE_CONTROL_RESPONSE_STREAM_ID | aeron.archive.control.response.stream.id | 20 |
| AERON_ARCHIVE_RECORDING_EVENTS_CHANNEL | aeron.archive.recording.events.channel | aeron:udp?endpoint=localhost:8030 |
| AERON_ARCHIVE_RECORDING_EVENTS_STREAM_ID | aeron.archive.recording.events.stream.id | 30 |
| AERON_ARCHIVE_CONTROL_TERM_BUFFER_SPARSE | aeron.archive.control.term.buffer.sparse | true |
| AERON_ARCHIVE_CONTROL_TERM_BUFFER_LENGTH | aeron.archive.control.term.buffer.length | 65536 |
| AERON_ARCHIVE_CONTROL_MTU_LENGTH | aeron.archive.control.mtu.length | 1408 |

## Licence (See LICENSE file for full license)
Copyright 2018-2019 Fairtide Pte. Ltd.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
