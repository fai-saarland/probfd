fast_downward_set_compiler_flags()
fast_downward_set_linker_flags()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        b796f7d44681514f58a683a3a71ff17c94edb0c1 # release-1.13.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(probfd_tests test_main.cpp)

target_include_directories(probfd_tests PUBLIC ../include/search)
target_link_libraries(probfd_tests downward_lib)
target_link_libraries(probfd_tests gtest_main)

add_test(NAME example_test COMMAND example)
