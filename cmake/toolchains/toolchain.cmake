function (msvs::directory out-var)
    if (${out-var})
        return()
    endif()

    cmake_parse_arguments(ARG "" "VARIABLE;PATH;DOC" "" ${ARGN})
    message(CHECK_START "Searching for ${ARG_DOC}")
    # We want to get the list of options, but *not* the full path string, hence
    # the use of `RELATIVE`
    file(GLOB candidates
        LIST_DIRECTORIES YES
        RELATIVE "${ARG_PATH}"
        "${ARG_PATH}/*")

    list(SORT candidates COMPARE NATURAL ORDER DESCENDING)

    if (NOT DEFINED ${ARG_VARIABLE})
        list(GET candidates 0 ${out-var})
    else()
        foreach (candidate IN LISTS candidates)
            if ("${${ARG_VARIABLE}}" VERSION_EQUAL candidate)
                set(${out-var} "${candidate}")
                break()
            endif()
        endforeach()
    endif()

    if (NOT ${out-var})
        message(CHECK_FAIL "not found")
    else()
        message(CHECK_PASS "found : ${${out-var}}")
        set(${out-var} "${${out-var}}" CACHE INTERNAL "${out-var} value")
    endif()
endfunction()

# On MacOS, select LLVM's C++ compiler.
if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin")
    if (EXISTS "/usr/local/opt/llvm/bin/clang++")
        set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/llvm/lib/c++ -Wl,-rpath,/usr/local/opt/llvm/lib/c++")
    elseif(EXISTS "/opt/homebrew/opt/llvm/bin/clang++")
        set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/homebrew/opt/llvm/lib/c++ -Wl,-rpath,/opt/homebrew/opt/llvm/lib/c++")
    else()
        message(FATAL_ERROR "Could not determine location of LLVM's clang.")
    endif()
elseif (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
    include_guard(GLOBAL)

    if (NOT CMAKE_GENERATOR MATCHES "^Visual Studio")
        if (NOT DEFINED CMAKE_SYSTEM_PROCESSOR)
            set(CMAKE_SYSTEM_PROCESSOR "${CMAKE_HOST_SYSTEM_PROCESSOR}")
        endif()

        if (NOT DEFINED CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE)
            set(CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE "x86")
            if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "AMD64")
                set(CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE "x64")
            elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "ARM64")
                set(CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE "arm64")
            endif()
        endif()

        if (NOT DEFINED CMAKE_VS_PLATFORM_NAME)
            set(CMAKE_VS_PLATFORM_NAME "x86")
            if (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
                set(CMAKE_VS_PLATFORM_NAME "x64")
            elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM64")
                set(CMAKE_VS_PLATFORM_NAME "arm64")
            endif()
        endif()
    endif()

    # Block was added in CMake 3.25, and lets us create variable scopes without
    # using a function.
    block(SCOPE_FOR VARIABLES)
        cmake_path(
            CONVERT "$ENV{ProgramFiles\(x86\)}/Microsoft Visual Studio Installer"
            TO_CMAKE_PATH_LIST vswhere.dir
            NORMALIZE)

        # This only temporarily affects the variable since we're inside a block.
        list(APPEND CMAKE_SYSTEM_PROGRAM_PATH "${vswhere.dir}")
        find_program(
            VSWHERE_EXECUTABLE
            NAMES vswhere
            DOC "Visual Studio Locator"
            PATHS ${CMAKE_CURRENT_LIST_DIR}
            REQUIRED)
    endblock()

    message(CHECK_START "Searching for Visual Studio installation")

    # Find installation path using vswhere
    execute_process(COMMAND "${VSWHERE_EXECUTABLE}"
        -nologo
        -nocolor
        -format json
        -products "*"
        -utf8
        -sort
        ENCODING UTF-8
        OUTPUT_VARIABLE candidates
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    string(JSON candidates.length LENGTH "${candidates}")

    if (candidates.length EQUAL 0)
        message(CHECK_FAIL "no products")
        # You can choose to either hard fail here, or continue
        message(FATAL_ERROR "Could not find any installation of Visual Studio.")
    endif()

    string(
        JSON candidate.install.path
        GET "${candidates}" 0 "installationPath")

    if (NOT candidate.install.path)
        message(CHECK_FAIL "no install path found")
        message(FATAL_ERROR "Could not find Visual Studio.")
    endif()

    cmake_path(
        CONVERT "${candidate.install.path}"
        TO_CMAKE_PATH_LIST candidate.install.path
        NORMALIZE)

    message(CHECK_PASS "found : ${candidate.install.path}")
    set(IZ_MSVS_INSTALL_PATH "${candidate.install.path}"
        CACHE PATH "Visual Studio Installation Path")

    # Get MSVC version
    cmake_language(CALL msvs::directory msvc.tools.version
        PATH "${IZ_MSVS_INSTALL_PATH}/VC/Tools/MSVC"
        VARIABLE IZ_MSVS_TOOLSET
        DOC "MSVC Toolset")

    # Find SDK directory via registry key
    cmake_host_system_information(
        RESULT IZ_MSVS_WINDOWS_SDK_ROOT QUERY
        WINDOWS_REGISTRY "HKLM/SOFTWARE/Microsoft/Windows Kits/Installed Roots"
        VALUE "KitsRoot10"
        VIEW BOTH
        ERROR_VARIABLE error)

    if (error)
        message(CHECK_FAIL "not found : ${error}")
        message(FATAL_ERROR "Could not find Windows SDK.")
    endif()

    cmake_path(CONVERT "${IZ_MSVS_WINDOWS_SDK_ROOT}"
        TO_CMAKE_PATH_LIST IZ_MSVS_WINDOWS_SDK_ROOT
        NORMALIZE)

    # Get Windows SDK version
    cmake_language(CALL msvs::directory IZ_MSVS_WINDOWS_SDK_VERSION
        PATH "${IZ_MSVS_WINDOWS_SDK_ROOT}Include"
        VARIABLE CMAKE_SYSTEM_VERSION
        DOC "Windows SDK")

    set(windows.sdk.host "Host${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}")
    set(windows.sdk.target "${CMAKE_VS_PLATFORM_NAME}")
    set(msvc.tools.dir "${IZ_MSVS_INSTALL_PATH}/VC/Tools/MSVC/${msvc.tools.version}")

    block(SCOPE_FOR VARIABLES)
        list(PREPEND CMAKE_SYSTEM_PROGRAM_PATH
            "${IZ_MSVS_INSTALL_PATH}/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja"
            "${msvc.tools.dir}/bin/${windows.sdk.host}/${windows.sdk.target}"
            "${IZ_MSVS_WINDOWS_SDK_ROOT}bin/${IZ_MSVS_WINDOWS_SDK_VERSION}/${windows.sdk.target}"
            "${IZ_MSVS_WINDOWS_SDK_ROOT}bin")
        find_program(CMAKE_MASM_ASM_COMPILER
            NAMES ml64 ml
            DOC "MSVC ASM Compiler")
        find_program(CMAKE_CXX_COMPILER
            NAMES cl
            DOC "MSVC C++ Compiler"
            REQUIRED)
        find_program(CMAKE_RC_COMPILER
            NAMES rc
            DOC "MSVC Resource Compiler"
            REQUIRED)
        find_program(CMAKE_C_COMPILER
            NAMES cl
            DOC "MSVC C Compiler"
            REQUIRED)
        find_program(CMAKE_LINKER
            NAMES link
            DOC "MSVC Linker"
            REQUIRED)
        find_program(CMAKE_AR
            NAMES lib
            DOC "MSVC Archiver"
            REQUIRED)
        find_program(CMAKE_MT
            NAMES mt
            DOC "MSVC Manifest Tool"
            REQUIRED)
        find_program(CMAKE_MAKE_PROGRAM
            NAMES ninja
            DOC "Ninja build system"
            REQUIRED)
    endblock()

    set(includes ucrt shared um winrt cppwinrt)
    set(libs ucrt um)

    list(TRANSFORM includes PREPEND "${IZ_MSVS_WINDOWS_SDK_ROOT}/Include/${IZ_MSVS_WINDOWS_SDK_VERSION}/")
    list(TRANSFORM libs PREPEND "${IZ_MSVS_WINDOWS_SDK_ROOT}/Lib/${IZ_MSVS_WINDOWS_SDK_VERSION}/")
    list(TRANSFORM libs APPEND "/${windows.sdk.target}")

    # We could technically set `CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES` and others,
    # but not for the library paths.
    include_directories(BEFORE SYSTEM "${msvc.tools.dir}/include" ${includes})
    link_directories(BEFORE "${msvc.tools.dir}/lib/${windows.sdk.target}" ${libs})

endif()