option(BUILD_TESTS "Enables the unit tests." OFF)

if (NOT BUILD_TESTS)
    return()
endif ()

enable_testing()

include(FetchContent)
include(GoogleTest)

Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG f8d7d77c06936315286eb55f8de22cd23c188571 # release-1.14.0
    FIND_PACKAGE_ARGS NAMES GTest
)

FetchContent_MakeAvailable(googletest)

add_executable(probfd_tests test_main.cc)
target_link_libraries(probfd_tests PRIVATE GTest::gtest)

include(TestFiles)

gtest_discover_tests(
    probfd_tests
    DISCOVERY_MODE PRE_TEST
)