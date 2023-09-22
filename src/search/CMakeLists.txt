# See https://www.fast-downward.org/ForDevelopers/AddingSourceFiles
# for general information on adding source files and CMake libraries.
#
# All libraries are enabled by default and users can disable them by specifying
#    -DLIBRARY_FOO_ENABLED=FALSE
# The default behavior can be changed so all non-essential libraries are
# disabled by default by specifying
#    -DDISABLE_LIBRARIES_BY_DEFAULT=TRUE
# In that case, individual libraries can be enabled with
#    -DLIBRARY_FOO_ENABLED=TRUE
#
# Defining a new library:
#    create_fast_downward_library(
#        NAME <NAME>
#        [ HELP <HELP> ]
#        SOURCES
#            <FILE_1> [ <FILE_2> ... ]
#        [ DEPENDS <LIBRARY_NAME_1> [ <LIBRARY_NAME_2> ... ] ]
#        [ DEPENDENCY_ONLY ]
#        [ CORE_LIBRARY ]
#    )
#
# <HELP> is used to describe the cmake option, for example in ccmake.
# SOURCES lists the source files that are part of the library. Entries are
#    listed without extension. For an entry <file>, both <file>.h and <file>.cc
#    are added if the files exist.
# DEPENDS lists libraries that will be compiled as dependendies if this library
#    is enabled.
# DEPENDENCY_ONLY disables the library unless it is needed as a dependency and
#    hides the option to enable the library in cmake GUIs like ccmake.
# CORE_LIBRARY always enables the library (even if DISABLE_LIBRARIES_BY_DEFAULT
#    is used) and hides the option to disable it in CMake GUIs like ccmake.

cmake_minimum_required(VERSION 3.21)

include(common_cxx_flags)
include(macros)
include(options)

report_bitwidth()
set_up_build_types("Debug;Release")
set_up_options()

#project(downward LANGUAGES CXX)
add_library(downward_lib INTERFACE)
add_executable(downward ${CMAKE_CURRENT_SOURCE_DIR}/probfd/planner.cc)

# On Windows we have to copy all DLLs next to the generated binary.
if (WIN32)
    copy_dlls_to_binary_dir_after_build(downward)
endif()

# Collect source files needed for the active plugins.
include(DownwardFiles)
include(ProbFDFiles)

target_include_directories(downward_lib INTERFACE ${PROJECT_SOURCE_DIR}/include/search)
target_include_directories(downward PRIVATE ${PROJECT_SOURCE_DIR}/include/search)