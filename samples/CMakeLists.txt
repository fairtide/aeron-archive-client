find_package(Boost REQUIRED
    COMPONENTS program_options)

function(aeron_archive_sample name file)
    add_executable(${name} ${file})
    target_link_libraries(${name}
        aeron_archive_client
        Boost::program_options
        ${CMAKE_THREAD_LIBS_INIT}
    )
endfunction()

# sample apps
aeron_archive_sample(RecordedBasicPublisher RecordedBasicPublisher.cpp)
aeron_archive_sample(ReplayedBasicSubscriber ReplayedBasicSubscriber.cpp)

install(TARGETS RecordedBasicPublisher ReplayedBasicSubscriber
    DESTINATION bin)

