find_package(Boost REQUIRED)

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

# static library
set(SOURCE
    ArchiveProxy.cpp
    Configuration.cpp
    Context.cpp
)

set(HEADERS
    ArchiveProxy.h
    Configuration.h
    Context.h
)

#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DDISABLE_BOUNDS_CHECKS")

add_library(aeron_archive_client STATIC ${SOURCE} ${HEADERS})

add_dependencies(aeron_archive_client generated_sbe_code)

target_include_directories(aeron_archive_client
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${SBE_ARCHIVE_INCLUDE_DIR}
    ${Aeron_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
)

target_link_libraries(aeron_archive_client
    aeron
    sbe
)

install(TARGETS aeron_archive_client ARCHIVE DESTINATION lib)
install(DIRECTORY . DESTINATION include FILES_MATCHING PATTERN "*.h")