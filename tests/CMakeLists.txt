include(common_cxx_flags)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        f8d7d77c06936315286eb55f8de22cd23c188571 # release-1.14.0
)

FetchContent_MakeAvailable(googletest)

add_executable(
    probfd_tests
    test_main.cpp
    fp_tests.cpp
    interval_tests.cpp
    engine_tests.cpp
    heuristics/cartesian_tests.cpp
    task_tests.cpp
    test_main.cpp
    tasks/blocksworld.cpp
    verification/policy_verification.h)

target_link_libraries(probfd_tests downward_lib gtest_main)

# Get probfd include directory
get_target_property(
    DOWNWARD_INCLUDES
    downward_lib
    INTERFACE_INCLUDE_DIRECTORIES)
    
target_include_directories(
    probfd_tests
    PUBLIC
    ${DOWNWARD_INCLUDES}
    ${PROJECT_SOURCE_DIR}/tests)

include(GoogleTest)
gtest_discover_tests(probfd_tests)