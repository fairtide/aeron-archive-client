# install GTEST
set(GTEST_VERSION "release-1.8.1")
set(GTEST_PREFIX ${THIRDPARTY_BINARY_DIR}/googletest)
set(GTEST_SOURCE_DIR ${GTEST_PREFIX}/src/googletest)

ExternalProject_Add(googletest_project
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG ${GTEST_VERSION}
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    PREFIX "${GTEST_PREFIX}"
    BUILD_BYPRODUCTS "\
        ${GTEST_PREFIX}/src/googletest_project-build/googlemock/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gmock${CMAKE_STATIC_LIBRARY_SUFFIX};\
        ${GTEST_PREFIX}/src/googletest_project-build/googlemock/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gmock_main${CMAKE_STATIC_LIBRARY_SUFFIX};\
        ${GTEST_PREFIX}/src/googletest_project-build/googlemock/gtest/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX};\
        ${GTEST_PREFIX}/src/googletest_project-build/googlemock/gtest/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
     INSTALL_COMMAND ""
)

ExternalProject_Get_Property(googletest_project source_dir)

set(GTEST_INCLUDE_DIR ${source_dir}/googletest/include CACHE STRING "gtest include files")
set(GMOCK_INCLUDE_DIR ${source_dir}/googlemock/include CACHE STRING "gmock include files")

ExternalProject_Get_Property(googletest_project binary_dir)

set(GTEST_LIBS
    ${binary_dir}/googlemock/${CMAKE_CFG_INTDIR}/gtest/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${binary_dir}/googlemock/${CMAKE_CFG_INTDIR}/gtest/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}
    CACHE STRING "gtest libraries"
)

set(GMOCK_LIBS
    ${binary_dir}/googlemock/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gmock${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${binary_dir}/googlemock/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gmock_main${CMAKE_STATIC_LIBRARY_SUFFIX}
    CACHE STRING "gmock libraries"
)
