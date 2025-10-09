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
        if (EXISTS "${PROJECT_SOURCE_DIR}/include/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.hpp")
            message(WARNING "File with file extension .hpp detected: ${PROJECT_SOURCE_DIR}/include/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.hpp.")
        endif()
        if (EXISTS "${PROJECT_SOURCE_DIR}/include/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.h")
            list(APPEND _HEADER_FILES "${PROJECT_SOURCE_DIR}/include/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.h")
        endif()
        if (EXISTS "${PROJECT_SOURCE_DIR}/src/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.cpp")
            message(WARNING "File with file extension .cpp detected: ${PROJECT_SOURCE_DIR}/src/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.cpp.")
        endif()
        if (EXISTS "${PROJECT_SOURCE_DIR}/src/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.cc")
            list(APPEND _SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/${_SOURCE_FILE_DIR}${_SOURCE_FILE_NAME}.cc")
        endif()
    endforeach()
    set(${_HEADERS_LIST_VAR} ${_HEADER_FILES} PARENT_SCOPE)
    set(${_SOURCES_LIST_VAR} ${_SOURCE_FILES} PARENT_SCOPE)
endfunction()

function(create_library)
    set(_OPTIONS CORE_LIBRARY)
    set(_ONE_VALUE_ARGS NAME HELP TARGET)
    set(_MULTI_VALUE_ARGS SOURCES DEPENDS)
    cmake_parse_arguments(_LIBRARY "${_OPTIONS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN})
    # Check mandatory arguments.
    if(NOT _LIBRARY_NAME)
        message(FATAL_ERROR "create_library: 'NAME' argument required.")
    endif()
    if(NOT _LIBRARY_SOURCES)
        message(FATAL_ERROR "create_library: 'SOURCES' argument required.")
    endif()

    add_existing_sources_to_list(_LIBRARY_HEADERS _LIBRARY_SOURCES)

    if (_LIBRARY_SOURCES)
        add_library(${_LIBRARY_NAME}_obj OBJECT ${_LIBRARY_HEADERS}
        ${_LIBRARY_SOURCES})
        target_link_libraries(${_LIBRARY_NAME}_obj PUBLIC common_cxx_flags)
        foreach(lib ${_LIBRARY_DEPENDS})
            target_link_libraries(${_LIBRARY_NAME}_obj PUBLIC ${lib})
        endforeach()

        add_library(${_LIBRARY_NAME} INTERFACE)
        target_link_libraries(${_LIBRARY_NAME} INTERFACE ${_LIBRARY_NAME}_obj
        $<TARGET_OBJECTS:${_LIBRARY_NAME}_obj>)
    else()
        add_library(${_LIBRARY_NAME} INTERFACE ${_LIBRARY_HEADERS})
        target_link_libraries(${_LIBRARY_NAME} INTERFACE common_cxx_flags)

        foreach(lib ${_LIBRARY_DEPENDS})
            target_link_libraries(${_LIBRARY_NAME} INTERFACE ${lib})
        endforeach()
    endif()

    # Link with target
    if (_LIBRARY_TARGET)
        if (_LIBRARY_CORE_LIBRARY)
            target_link_libraries(${_LIBRARY_TARGET} PUBLIC ${_LIBRARY_NAME})
        else()
            # Decide whether the library should be enabled by default.
            if (DISABLE_LIBRARIES_BY_DEFAULT)
                set(_OPTION_DEFAULT FALSE)
            else()
                set(_OPTION_DEFAULT TRUE)
            endif()

            # Add option to enable library
            string(TOUPPER ${_LIBRARY_NAME} _LIBRARY_NAME_UPPER)
            option(LIBRARY_${_LIBRARY_NAME_UPPER}_ENABLED ${_LIBRARY_HELP} ${_OPTION_DEFAULT})

            if (LIBRARY_${_LIBRARY_NAME_UPPER}_ENABLED)
                target_link_libraries(${_LIBRARY_TARGET} PUBLIC ${_LIBRARY_NAME})
            endif()
        endif()
    endif()
endfunction()

function(copy_runtime_dlls_post_build _TARGET_NAME)
    add_custom_command(TARGET ${_TARGET_NAME} POST_BUILD
        COMMAND
        "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:${_TARGET_NAME}>"
        "$<TARGET_RUNTIME_DLLS:${_TARGET_NAME}>"
        COMMAND_EXPAND_LISTS
    )
endfunction()
