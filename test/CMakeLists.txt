function(aeron_archive_test name file)
    add_executable(${name} ${file})
    target_include_directories(${name}
        PRIVATE ${GTEST_INCLUDE_DIR} ${GMOCK_INCLUDE_DIR})
    target_link_libraries(${name}
        aeron_archive_client
        ${GTEST_LIBS} ${GMOCK_LIBS}
        ${CMAKE_THREAD_LIBS_INIT})
    add_dependencies(${name} googletest_project)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

# tests
aeron_archive_test(ChannelUriTest ChannelUriTest.cpp)
aeron_archive_test(ContextTest ContextTest.cpp)
