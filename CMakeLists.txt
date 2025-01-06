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

# If the Doxyfile exists, add a custom target to build the documentation.
if (EXISTS ${CMAKE_SOURCE_DIR}/docs/Doxyfile)
    # check if Doxygen is installed
    message(CHECK_START "Looking for Doxygen")

    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        # set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        # set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        get_target_property(doxygen_dir Doxygen::doxygen IMPORTED_LOCATION)

        # request to configure the file
        # configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message(CHECK_PASS "found : ${doxygen_dir} (version ${DOXYGEN_VERSION})")
        message(VERBOSE "Adding documentation target.")
        set(DOXYGEN_OUT ${CMAKE_SOURCE_DIR}/docs/Doxyfile)

        # note the option ALL which allows to build the docs together with the application
        doxygen_add_docs(docs
            CONFIG_FILE ${CMAKE_SOURCE_DIR}/docs/Doxyfile
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs)
    else (DOXYGEN_FOUND)
        message(CHECK_PASS "not found")
        message(VERBOSE "Documentation target not added because doxygen is not
        installed.")
    endif (DOXYGEN_FOUND)
else ()
    message(VERBOSE "Documentation target not added because Doxyfile is missing.")
endif ()

# Add search component as a subproject.
add_subdirectory(src)
