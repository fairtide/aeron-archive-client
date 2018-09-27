# install Aeron
set(Aeron_VERSION "1.11.1")
set(Aeron_PREFIX ${THIRDPARTY_BINARY_DIR}/aeron)
set(Aeron_SOURCE_DIR ${Aeron_PREFIX}/src/aeron)

ExternalProject_Add(aeron_project
    GIT_REPOSITORY https://github.com/real-logic/aeron
    GIT_TAG ${Aeron_VERSION}
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    PREFIX "${Aeron_PREFIX}"
    CMAKE_ARGS "-DAERON_TESTS=FALSE" "-DAERON_BUILD_SAMPLES=FALSE" "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
    INSTALL_COMMAND make install PREFIX=<INSTALL_DIR>
    COMMAND "cp" "-r" "<SOURCE_DIR>/aeron-archive/src/main/resources" "<INSTALL_DIR>"
)

ExternalProject_Get_Property(aeron_project install_dir)

set(Aeron_INCLUDE_DIR ${install_dir}/include CACHE STRING "Aeron include files")
message(STATUS "Aeron includes: ${Aeron_INCLUDE_DIR}")

set(Aeron_RESOURCES_DIR ${install_dir}/resources CACHE STRING "Aeron resources")
message(STATUS "Aeron resources: ${Aeron_RESOURCES_DIR}")

add_library(aeron_client STATIC IMPORTED GLOBAL)
add_dependencies(aeron_client aeron_project)
set_property(TARGET aeron_client PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libaeron_client.a)
#target_include_directories(aeron_client INTERFACE ${Aeron_INCLUDE_DIR})

