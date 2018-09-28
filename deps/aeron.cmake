# install Aeron
set(Aeron_VERSION "1.11.1")
set(Aeron_PREFIX ${THIRDPARTY_BINARY_DIR}/aeron)
set(Aeron_SOURCE_DIR ${Aeron_PREFIX}/src/aeron)
set(Aeron_CLIENT_LIB_PATH ${CMAKE_CFG_INTDIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}aeron_client${CMAKE_STATIC_LIBRARY_SUFFIX})

ExternalProject_Add(aeron_project
    GIT_REPOSITORY https://github.com/real-logic/aeron
    GIT_TAG ${Aeron_VERSION}
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    PREFIX "${Aeron_PREFIX}"
    CMAKE_ARGS "-DAERON_TESTS=FALSE" "-DAERON_BUILD_SAMPLES=FALSE"
    BUILD_BYPRODUCTS "${AERON_PREFIX}/src/aeron_project-build/${Aeron_CLIENT_LIB_PATH}"
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(aeron_project source_dir)
ExternalProject_Get_Property(aeron_project binary_dir)

set(Aeron_INCLUDE_DIR ${source_dir}/aeron-client/src/main/cpp CACHE STRING "Aeron include files")
set(Aeron_RESOURCES_DIR ${source_dir}/aeron-archive/src/main/resources CACHE STRING "Aeron resources")
set(Aeron_CLIENT_LIB ${binary_dir}/${Aeron_CLIENT_LIB_PATH} CACHE STRING "Aeron client library")

