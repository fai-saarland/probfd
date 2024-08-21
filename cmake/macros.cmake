include_guard(GLOBAL)

include(CMakeParseArguments)

function(set_up_build_types allowedBuildTypes)
    get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(isMultiConfig)
        # Set the possible choices for multi-config generators (like Visual
        # Studio Projects) that choose the build type at build time.
        set(CMAKE_CONFIGURATION_TYPES "${allowedBuildTypes}"
            CACHE STRING "Supported build types: ${allowedBuildTypes}" FORCE)
    else()
        # Set the possible choices for programs like ccmake.
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${allowedBuildTypes}")
        if(NOT CMAKE_BUILD_TYPE)
            message(STATUS "Defaulting to release build.")
            set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
        elseif(NOT CMAKE_BUILD_TYPE IN_LIST allowedBuildTypes)
            message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}. "
                "Supported build types: ${allowedBuildTypes}")
        endif()
    endif()
endfunction()

macro(report_bitwidth)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
        message(STATUS "Building for 32-bit.")
    elseif(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        message(STATUS "Building for 64-bit.")
    else()
        message(FATAL_ERROR, "Could not determine bitwidth.")
    endif()
endmacro()

function(add_existing_sources_to_list _HEADERS_LIST_VAR _SOURCES_LIST_VAR)
    set(_HEADER_FILES)
    set(_SOURCE_FILES)
    foreach(SOURCE_FILE ${${_SOURCES_LIST_VAR}})
        get_filename_component(_SOURCE_FILE_DIR ${SOURCE_FILE} PATH)
        get_filename_component(_SOURCE_FILE_NAME ${SOURCE_FILE} NAME_WE)
        get_filename_component(_SOURCE_FILE_EXT ${SOURCE_FILE} EXT)
        if (_SOURCE_FILE_DIR)
            set(_SOURCE_FILE_DIR "${_SOURCE_FILE_DIR}/")
        endif()
        if (EXISTS "${PROJECT_SOURCE_DIR}/include/search/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.h")
            list(APPEND _HEADER_FILES "${PROJECT_SOURCE_DIR}/include/search/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.h")
        endif()
        if (EXISTS "${PROJECT_SOURCE_DIR}/src/search/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.cc")
            list(APPEND _SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/search/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.cc")
        endif()
    endforeach()
    set(${_HEADERS_LIST_VAR} ${_HEADER_FILES} PARENT_SCOPE)
    set(${_SOURCES_LIST_VAR} ${_SOURCE_FILES} PARENT_SCOPE)
endfunction()

function(add_existing_test_sources_to_list _HEADERS_LIST_VAR _SOURCES_LIST_VAR)
    set(_HEADER_FILES)
    set(_SOURCE_FILES)
    foreach(SOURCE_FILE ${${_SOURCES_LIST_VAR}})
        get_filename_component(_SOURCE_FILE_DIR ${SOURCE_FILE} PATH)
        get_filename_component(_SOURCE_FILE_NAME ${SOURCE_FILE} NAME_WE)
        get_filename_component(_SOURCE_FILE_EXT ${SOURCE_FILE} EXT)
        if (_SOURCE_FILE_DIR)
            set(_SOURCE_FILE_DIR "${_SOURCE_FILE_DIR}/")
        endif()
        if (EXISTS "${PROJECT_SOURCE_DIR}/include/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.h")
            list(APPEND _HEADER_FILES "${PROJECT_SOURCE_DIR}/include/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.h")
        endif()
        if (EXISTS "${PROJECT_SOURCE_DIR}/src/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.cc")
            list(APPEND _SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.cc")
        endif()
    endforeach()
    set(${_HEADERS_LIST_VAR} ${_HEADER_FILES} PARENT_SCOPE)
    set(${_SOURCES_LIST_VAR} ${_SOURCE_FILES} PARENT_SCOPE)
endfunction()

function(create_fast_downward_library)
    create_library(TARGET probfd FLAGS common_cxx_flags FIND_SOURCES add_existing_sources_to_list ${ARGV})
endfunction()

function(create_probfd_library)
    create_library(TARGET probfd FLAGS common_cxx_flags FIND_SOURCES add_existing_sources_to_list ${ARGV})
endfunction()

function(create_test_library)
    create_library(TARGET probfd_tests FLAGS test_cxx_flags FIND_SOURCES add_existing_test_sources_to_list ${ARGV})
endfunction()

function(create_library)
    set(_OPTIONS DEPENDENCY_ONLY CORE_LIBRARY)
    set(_ONE_VALUE_ARGS NAME HELP TARGET FLAGS FIND_SOURCES)
    set(_MULTI_VALUE_ARGS SOURCES DEPENDS)
    cmake_parse_arguments(_LIBRARY "${_OPTIONS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN})
    # Check mandatory arguments.
    if(NOT _LIBRARY_NAME)
        message(FATAL_ERROR "create_library: 'NAME' argument required.")
    endif()
    if(NOT _LIBRARY_SOURCES)
        message(FATAL_ERROR "create_library: 'SOURCES' argument required.")
    endif()

    cmake_language(CALL ${_LIBRARY_FIND_SOURCES} _LIBRARY_HEADERS _LIBRARY_SOURCES)

    if (NOT _LIBRARY_CORE_LIBRARY AND NOT _LIBRARY_DEPENDENCY_ONLY)
        # Decide whether the library should be enabled by default.
        if (DISABLE_LIBRARIES_BY_DEFAULT)
            set(_OPTION_DEFAULT FALSE)
        else()
            set(_OPTION_DEFAULT TRUE)
        endif()
        string(TOUPPER ${_LIBRARY_NAME} _LIBRARY_NAME_UPPER)
        option(LIBRARY_${_LIBRARY_NAME_UPPER}_ENABLED ${_LIBRARY_HELP} ${_OPTION_DEFAULT})
    endif()

    if (_LIBRARY_SOURCES)
        add_library(${_LIBRARY_NAME} OBJECT ${_LIBRARY_HEADERS} ${_LIBRARY_SOURCES})
        target_link_libraries(${_LIBRARY_NAME} PUBLIC ${_LIBRARY_FLAGS})
        foreach(lib ${_LIBRARY_DEPENDS})
            target_link_libraries(${_LIBRARY_NAME} PUBLIC iface_${lib})
        endforeach()

        add_library(iface_${_LIBRARY_NAME} INTERFACE)
        target_link_libraries(iface_${_LIBRARY_NAME} INTERFACE ${_LIBRARY_NAME} $<TARGET_OBJECTS:${_LIBRARY_NAME}>)
    else()
        add_library(iface_${_LIBRARY_NAME} INTERFACE ${_LIBRARY_HEADERS})
        target_link_libraries(iface_${_LIBRARY_NAME} INTERFACE ${_LIBRARY_FLAGS})

        foreach(lib ${_LIBRARY_DEPENDS})
            target_link_libraries(iface_${_LIBRARY_NAME} INTERFACE iface_${lib})
        endforeach()
    endif()

    if ((_LIBRARY_CORE_LIBRARY OR LIBRARY_${_LIBRARY_NAME_UPPER}_ENABLED) AND _LIBRARY_TARGET)
        target_link_libraries(${_LIBRARY_TARGET} PUBLIC iface_${_LIBRARY_NAME})
    endif()
endfunction()

function(copy_dlls_to_binary_dir_after_build _TARGET_NAME)
    set(have_runtime_dlls
        $<BOOL:$<TARGET_RUNTIME_DLLS:${_TARGET_NAME}>>
    )

    set(command
        ${CMAKE_COMMAND} -E copy
        $<TARGET_RUNTIME_DLLS:${_TARGET_NAME}>
        $<TARGET_FILE_DIR:${_TARGET_NAME}>
    )

    add_custom_command(TARGET ${_TARGET_NAME} POST_BUILD
        COMMAND "$<${have_runtime_dlls}:${command}>"
        COMMAND_EXPAND_LISTS
    )
endfunction()
