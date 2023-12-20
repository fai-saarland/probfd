
include(FetchContent)
include(GoogleTest)

Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        f8d7d77c06936315286eb55f8de22cd23c188571 # release-1.14.0
)

FetchContent_MakeAvailable(googletest)

add_library(test_cxx_flags INTERFACE)
target_link_libraries(test_cxx_flags INTERFACE common_cxx_flags gtest)

add_executable(probfd_tests test_main.cc)
target_link_libraries(probfd_tests PRIVATE test_cxx_flags)

include(TestFiles)

gtest_discover_tests(
    probfd_tests
    DISCOVERY_MODE PRE_TEST
)