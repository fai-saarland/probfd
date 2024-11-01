# Two configurations types are defined:
#
# * release (default)
#      -O3 optimisation, no debugging symbols, assertions inactive
# * debug
#      no optimisation, full debugging information, assertions active
#
# In all configuration types, we overwrite the default configuration to
# include "-g", allow cross compilation and switch to pedantic error
# reporting.

cmake_minimum_required(VERSION 3.22)

cmake_policy(SET CMP0091 NEW)

project(probfd
    DESCRIPTION "Probabilistic Fast Downward is an extension of the \
                 domain-independent classical planning system Fast Downward \
                 for probabilistic planning."
    LANGUAGES CXX)

# Path containing custom CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Add search component as a subproject.
add_subdirectory(src)
