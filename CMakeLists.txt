# Usage:
#         cmake -S src -B builds/release
#         cmake --build builds/release
# The first call caches settings in the build directory and reads
# them from the cache on subsequent builds. If you want to change the
# settings of some options, do _not_ change the CMakeLists.txt files.
# Instead, create a new build directory, pass -DMY_OPTION=my_value to
# cmake. Alternatively, you can use a cmake GUI like ccmake to edit
# the cache.
#
# Two build targets are defined:
#
# * release (default)
#      -O3 optimisation, debugging symbols, assertions inactive
# * debug
#      -03 optimisation, full debugging information, assertions active
#
# In all build targets, we overwrite the default configuration to
# include "-g", allow cross compilation and switch to pedantic error
# reporting.
#
# You can change the build target, by adding the parameter
#   -DCMAKE_BUILD_TYPE=type
# to the cmake call.

cmake_minimum_required(VERSION 3.21)

enable_testing()

cmake_policy(SET CMP0091 NEW)
project(probfd
    DESCRIPTION "Probabilistic Fast Downward is an extension of the \
                 domain-independent classical planning system Fast Downward \
                 for probabilistic planning."
    LANGUAGES   CXX)

# Path containing custom CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

if (MSVC)
    set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Add planner components.

# Copy the translator into the output directory.
add_custom_target(translate ALL)
add_custom_command(TARGET translate POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/src/translate
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/translate
    COMMENT "Copying translator module into output directory")

# Add search component as a subproject.
add_subdirectory(src/search)

# Add tests as a subproject.
add_subdirectory(tests)
