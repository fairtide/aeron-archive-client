find_package(Boost REQUIRED stacktrace_basic)

# generation of SBE C++ wrappers for the Aeron Archive RPC schema
set(SBE_ARCHIVE_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/sbe")

add_custom_target(generated_sbe_code
    java
    "-Dsbe.output.dir=${SBE_ARCHIVE_INCLUDE_DIR}"
    "-Dsbe.target.language=cpp"
    "-jar" "${SBE_TOOL_JAR}"
    "${Aeron_RESOURCES_DIR}/aeron-archive-codecs.xml"
    COMMENT "Generating SBE stubs"
)

add_dependencies(generated_sbe_code sbe)

# static library
set(SOURCE
    AeronArchive.cpp
    ArchiveException.cpp
    ArchiveProxy.cpp
    ChannelUri.cpp
    Configuration.cpp
    Context.cpp
    ControlResponsePoller.cpp
    RecordingDescriptorPoller.cpp
    RecordingPos.cpp
)

set(HEADERS
    AeronArchive.h
    ArchiveException.h
    ArchiveProxy.h
    ChannelUri.h
    Configuration.h
    Context.h
    ControlResponsePoller.h
    RecordingDescriptorPoller.h
    RecordingPos.h
)

add_library(aeron_archive_client STATIC ${SOURCE} ${HEADERS})

add_dependencies(aeron_archive_client generated_sbe_code)

target_include_directories(aeron_archive_client
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${SBE_ARCHIVE_INCLUDE_DIR}
    ${Aeron_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
)

target_link_libraries(aeron_archive_client
    Boost::stacktrace_basic
    dl
    ${Aeron_CLIENT_LIB}
    sbe
    ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS aeron_archive_client ARCHIVE DESTINATION lib)
install(DIRECTORY . DESTINATION include FILES_MATCHING PATTERN "*.h")
