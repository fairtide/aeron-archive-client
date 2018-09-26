# install SBE
set(SBE_VERSION "1.8.8")
set(SBE_PREFIX ${THIRDPARTY_BINARY_DIR}/sbe)
set(SBE_SOURCE_DIR ${SBE_PREFIX}/src/sbe)

ExternalProject_Add(sbe_project
    GIT_REPOSITORY https://github.com/real-logic/simple-binary-encoding
    GIT_TAG ${SBE_VERSION}
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    PREFIX "${SBE_PREFIX}"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND "<SOURCE_DIR>/gradlew" ":sbe-all:assemble"
    BUILD_IN_SOURCE TRUE
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(sbe_project source_dir)

set(SBE_INCLUDE_DIR ${source_dir}/sbe-tool/src/main/cpp CACHE STRING "SBE include files")
message(STATUS "SBE includes: ${SBE_INCLUDE_DIR}")

set(SBE_TOOL_JAR "${source_dir}/sbe-all/build/libs/sbe-all-${SBE_VERSION}.jar" CACHE STRING "SBE Tool")
message(STATUS "SBE tool JAR: ${SBE_TOOL_JAR}")

add_library(sbe INTERFACE)
add_dependencies(sbe sbe_project)
target_include_directories(sbe INTERFACE ${SBE_INCLUDE_DIR})

