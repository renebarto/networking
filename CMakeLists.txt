cmake_minimum_required(VERSION 3.5.1)

project(fdcsib-sw)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(FDCSIB_Software_Main
    VERSION 1.0.0
    DESCRIPTION "This is the project for FDCSIB-sw"
    LANGUAGES CXX
)

option(MEASURE_COVERAGE "Determine coverage" OFF)
set(DEPLOYMENT_DIR "C:/Generic" CACHE STRING "Deployment directory")

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_COLOR_MAKEFILE   OFF)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

set(CMAKE_INSTALL_PREFIX ${DEPLOYMENT_DIR} CACHE STRING "" FORCE)
set(CMAKE_STAGING_PREFIX ${DEPLOYMENT_DIR})

set(CMAKE_PROGRAM_PATH ${CMAKE_INSTALL_PREFIX}/bin)
set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INCLUDE_PATH ${CMAKE_INSTALL_PREFIX}/include)

if ("${SCRIPTS_DIR}" STREQUAL "")
    set(SCRIPTS_DIR "${CMAKE_SOURCE_DIR}/cmake")
endif()

list(APPEND CMAKE_MODULE_PATH ${SCRIPTS_DIR})

include(functions)

message(STATUS "In directory: ${CMAKE_CURRENT_SOURCE_DIR}")

if (MEASURE_COVERAGE)
message(STATUS "Configuring with coverage")
endif()

parse_msi(MSI_NUMBER MSI_VERSION_MAJOR MSI_VERSION_MINOR MSI_VERSION_LEVEL MSI_VERSION_BUILD)
message(STATUS "MSI version: ${MSI_VERSION_MAJOR}.${MSI_VERSION_MINOR}.${MSI_VERSION_LEVEL}.${MSI_VERSION_BUILD}")
if(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX TRUE)
    set(PLATFORM_NAME Linux)
endif()
if(WIN32 AND MSVC)
    set(PLATFORM_WINDOWS TRUE)
    message(STATUS "Building for Windows with MSVC")
    set(PLATFORM_NAME Windows)
elseif(PLATFORM_LINUX)
    if(PLATFORM_WRL)
        message(STATUS "Building for WindRiver Linux ${WINDRIVER_LINUX_VERSION}")
        set(PLATFORM_NAME WRLinux6)
    elseif(PLATFORM_RPI)
        message(STATUS "Building for Raspberry PI")
        set(PLATFORM_NAME RPI)
    else()
        message(STATUS "Building for Linux")
    endif()
else()
    message(STATUS "Unsupported platform " ${CMAKE_HOST_SYSTEM} "?")
endif()

set(OUTPUT_BASE_DIR ${CMAKE_SOURCE_DIR}/output/${PLATFORM_NAME})

macro(setup_default_options)
    if(PLATFORM_WINDOWS)
        # Enforce CMake to use /MT and /MTd
        set(CompilerFlags
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            )
        foreach(CompilerFlag ${CompilerFlags})
            string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
        endforeach()        
        # Important! do not keep warning 4251 4275 ignored!
        # Possibly switch warning 4505 back on
        set(FLAGS_CXX
            /Wall /WX /wd4061 /wd4251 /wd4275 /wd4435 /wd4505 /wd4514 /wd4548 /wd4571 /wd4592 /wd4625 /wd4626 /wd4628 /wd4710 /wd4711 /wd4774 /wd4668 /wd5045
            /wd4820 /wd5026 /wd5027 /EHsc /Gd /GR /sdl- /Zc:wchar_t /Zc:inline /fp:precise /bigobj)
        set(FLAGS_CXX_DEBUG /Od /Gm- /Zi /RTC1 /MTd)
        set(FLAGS_CXX_RELEASE /Ox /GL- /GS /Gy /Oi /MT)
        set(FLAGS_CXX_MINSIZEREL /O1 /GL /GS /Gy /Oi /MT)
        set(FLAGS_CXX_RELWITHDEBINFO /O2 /GL /GS /Gy /Oi /Zi /MT)

        set(FLAGS_C
            /Wall /WX /wd4061 /wd4251 /wd4275 /wd4505 /wd4514 /wd4548 /wd4571 /wd4625 /wd4626 /wd4710 /wd4774
            /wd4820 /wd5026 /wd5027 /EHsc /Gd /GR /sdl- /Zc:wchar_t /Zc:inline /fp:precise)
        set(FLAGS_C_DEBUG /Od /Gm- /ZI /RTC1 /MTd)
        set(FLAGS_C_RELEASE /Ox /GL- /GS /Gy /Oi /MT)
        set(FLAGS_C_MINSIZEREL /O1 /GL /GS /Gy /Oi /MT)
        set(FLAGS_C_RELWITHDEBINFO /O2 /GL /GS /Gy /Oi /Zi /MT)

        set(DEFINES _UNICODE UNICODE _CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS WINVER=0x0A00 _WIN32_WINNT=0x0A00 PLATFORM_WINDOWS)
        set(DEFINES_DEBUG _DEBUG)
        set(DEFINES_RELEASE NDEBUG)
        set(DEFINES_MINSIZEREL NDEBUG)
        set(DEFINES_RELWITHDEBINFO NDEBUG)

        set(PLATFORM_LIBRARIES_STATIC )
        set(LINK_FLAGS /ignore:4217)
        list(APPEND LINK_LIBRARIES )
        list(APPEND LINK_DIRECTORIES )

        # Hack to force linker to target X64 machine when needed.
        if (${CMAKE_SIZEOF_VOID_P} EQUAL "8")
            set(DEFINES ${DEFINES} _AMD64_)
        else ()
            set(DEFINES ${DEFINES} _X86_)
        endif ()
        if (MEASURE_COVERAGE)
            message(STATUS "Adding options for coverage")
        endif()

    elseif(PLATFORM_LINUX AND NOT PLATFORM_WRL)
        set(FLAGS_CXX -Wall -Wextra -Werror -fPIC -fexceptions -fmessage-length=0)
        set(FLAGS_CXX_DEBUG -O0 -g)
        set(FLAGS_CXX_RELEASE -O3)
        set(FLAGS_CXX_MINSIZEREL -O3)
        set(FLAGS_CXX_RELWITHDEBINFO -O2 -g)
        if (PLATFORM_RPI)
            list(APPEND FLAGS_CXX -Wno-psabi)
        endif()

        set(FLAGS_C -Wall -Wextra -fPIC -fmessage-length=0)
        set(FLAGS_C_DEBUG -O0 -g)
        set(FLAGS_C_RELEASE -O3)
        set(FLAGS_C_MINSIZEREL -O3)
        set(FLAGS_C_RELWITHDEBINFO -O2 -g)

        set(DEFINES PLATFORM_LINUX)
        set(DEFINES_DEBUG _DEBUG)
        set(DEFINES_RELEASE NDEBUG)
        set(DEFINES_MINSIZEREL NDEBUG)
        set(DEFINES_RELWITHDEBINFO NDEBUG)

        set(PLATFORM_LIBRARIES_STATIC
            -static-libgcc
            -static-libstdc++)
        set(LINK_FLAGS )
        list(APPEND LINK_LIBRARIES )
        list(APPEND LINK_DIRECTORIES )

        if (MEASURE_COVERAGE)
            message(STATUS "Adding options for coverage")
            list(APPEND FLAGS_C -fprofile-arcs -ftest-coverage)
            list(APPEND FLAGS_CXX -fprofile-arcs -ftest-coverage)
            list(APPEND LINK_FLAGS --coverage)
            list(APPEND LINK_LIBRARIES gcov)
        endif()

    elseif(PLATFORM_WRL)
        set(ARCH cortexa9t-neon)
        set(FLAGS_CXX -fno-omit-frame-pointer -pipe -mapcs-frame -Wall -Werror -Wextra -Wno-system-headers 
            -Wno-unused-parameter -Wfloat-equal -Wpointer-arith  
            -Wwrite-strings -Wsign-compare -Wmissing-field-initializers -Wredundant-decls 
            -Winvalid-pch -Wlogical-op -Wunused-parameter
            -MD -MP -DARCH=${ARCH})
        set(FLAGS_CXX_DEBUG -O0)
        set(FLAGS_CXX_RELEASE -O2 -D__USE_STRING_INLINES)
        set(FLAGS_CXX_MINSIZEREL -O3)
        set(FLAGS_CXX_RELWITHDEBINFO -O2)

        set(FLAGS_C -fno-omit-frame-pointer -pipe -mapcs-frame -Wall -Werror -Wextra -Wno-system-headers 
            -Wno-unused-parameter -Wfloat-equal -Wpointer-arith  
            -Wwrite-strings -Wsign-compare -Wmissing-field-initializers -Wredundant-decls 
            -Winvalid-pch -Wlogical-op -Wunused-parameter
            -MD -MP -DARCH=${ARCH})

        set(FLAGS_C_DEBUG -O0)
        set(FLAGS_C_RELEASE  -O2 -D__USE_STRING_INLINES)
        set(FLAGS_C_MINSIZEREL -O3)
        set(FLAGS_C_RELWITHDEBINFO -O2)

        set(DEFINES PLATFORM_LINUX_WRL)
        set(DEFINES_DEBUG _DEBUG)
        set(DEFINES_RELEASE NDEBUG)
        set(DEFINES_MINSIZEREL NDEBUG)
        set(DEFINES_RELWITHDEBINFO NDEBUG)

        set(PLATFORM_LIBRARIES_STATIC
            -static-libgcc
            -static-libstdc++)
        set(LINK_FLAGS_DEBUG )
        set(LINK_FLAGS_RELEASE )
        list(APPEND LINK_LIBRARIES )
        list(APPEND LINK_DIRECTORIES )

        if (MEASURE_COVERAGE)
            message(STATUS "Adding options for coverage with Bullseye")
            
            # Link to Bullseye coverage library
            list(APPEND LINK_LIBRARIES cov-noThread.a)
            list(APPEND LINK_DIRECTORIES ${CMAKE_SOURCE_DIR}/IDCtrl/Lib/WRLinux6/8.14.1)

            # Prefix Bullseye compiler interceptor
            set (COVERAGE_DIR ${DEPLOYMENT_DIR})
            find_program(COVERAGE_COMPILER covc.exe PATHS "C:/Program Files/Coverage/bin" "C:/Program Files (x86)/Coverage/bin")
            message(STATUS "COVERAGE_COMPILER = ${COVERAGE_COMPILER}")
            set (CMAKE_C_COMPILER_LAUNCHER ${COVERAGE_COMPILER} -f ${COVERAGE_DIR}/FDCSW.cov -i)
            set (CMAKE_CXX_COMPILER_LAUNCHER ${COVERAGE_COMPILER} -f ${COVERAGE_DIR}/FDCSW.cov -i)
        endif()
    else()
        display_list("Unsupported platform " ${CMAKE_HOST_SYSTEM})
        return()
    endif()
endmacro()

macro(convert_default_options)
    set(COMPILER_DEFINITIONS_DEBUG ${DEFINES} ${DEFINES_DEBUG})
    set(COMPILER_DEFINITIONS_RELEASE ${DEFINES} ${DEFINES_RELEASE})
    set(COMPILER_DEFINITIONS_MINSIZEREL ${DEFINES} ${DEFINES_MINSIZEREL})
    set(COMPILER_DEFINITIONS_RELWITHDEBINFO ${DEFINES} ${DEFINES_RELWITHDEBINFO})

    set(COMPILER_OPTIONS_CXX_DEBUG ${FLAGS_CXX} ${FLAGS_CXX_DEBUG})
    set(COMPILER_OPTIONS_CXX_RELEASE ${FLAGS_CXX} ${FLAGS_CXX_RELEASE})
    set(COMPILER_OPTIONS_CXX_MINSIZEREL ${FLAGS_CXX} ${FLAGS_CXX_MINSIZEREL})
    set(COMPILER_OPTIONS_CXX_RELWITHDEBINFO ${FLAGS_CXX} ${FLAGS_CXX_RELWITHDEBINFO})

    set(COMPILER_OPTIONS_C_DEBUG ${FLAGS_C} ${FLAGS_C_DEBUG})
    set(COMPILER_OPTIONS_C_RELEASE ${FLAGS_C} ${FLAGS_C_RELEASE})
    set(COMPILER_OPTIONS_C_MINSIZEREL ${FLAGS_C} ${FLAGS_C_MINSIZEREL})
    set(COMPILER_OPTIONS_C_RELWITHDEBINFO ${FLAGS_C} ${FLAGS_C_RELWITHDEBINFO})

    set(LINKER_OPTIONS_DEBUG ${LINK_FLAGS} ${LINK_FLAGS_DEBUG})
    set(LINKER_OPTIONS_RELEASE ${LINK_FLAGS} ${LINK_FLAGS_RELEASE})
    set(LINKER_OPTIONS_MINSIZEREL ${LINK_FLAGS} ${LINK_FLAGS_MINSIZEREL})
    set(LINKER_OPTIONS_RELWITHDEBINFO ${LINK_FLAGS} ${LINK_FLAGS_RELWITHDEBINFO})

    set(LINKER_LIBRARIES ${LINK_LIBRARIES})

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(COMPILER_DEFINITIONS ${COMPILER_DEFINITIONS_DEBUG})
        set(COMPILER_OPTIONS_CXX ${COMPILER_OPTIONS_CXX_DEBUG})
        set(COMPILER_OPTIONS_C ${COMPILER_OPTIONS_C_DEBUG})
        set(LINKER_OPTIONS ${LINKER_OPTIONS_DEBUG})
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set(COMPILER_DEFINITIONS ${COMPILER_DEFINITIONS_RELEASE})
        set(COMPILER_OPTIONS_CXX ${COMPILER_OPTIONS_CXX_RELEASE})
        set(COMPILER_OPTIONS_C ${COMPILER_OPTIONS_CXX_RELEASE})
        set(LINKER_OPTIONS ${LINKER_OPTIONS_RELEASE})
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
        set(COMPILER_DEFINITIONS ${COMPILER_DEFINITIONS_MINSIZEREL})
        set(COMPILER_OPTIONS_CXX ${COMPILER_OPTIONS_CXX_MINSIZEREL})
        set(COMPILER_OPTIONS_C ${COMPILER_OPTIONS_C_MINSIZEREL})
        set(LINKER_OPTIONS ${LINKER_OPTIONS_MINSIZEREL})
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        set(COMPILER_DEFINITIONS ${COMPILER_DEFINITIONS_RELWITHDEBINFO})
        set(COMPILER_OPTIONS_CXX ${COMPILER_OPTIONS_CXX_RELWITHDEBINFO})
        set(COMPILER_OPTIONS_C ${COMPILER_OPTIONS_C_RELWITHDEBINFO})
        set(LINKER_OPTIONS ${LINKER_OPTIONS_RELWITHDEBINFO})
    else()
        message(FATAL_ERROR "Invalid build type: " ${CMAKE_BUILD_TYPE})
    endif()

    if(CMAKE_VERBOSE_MAKEFILE)
        display_list("COMPILER_DEFINITIONS_DEBUG            : " ${COMPILER_DEFINITIONS_DEBUG})
        display_list("COMPILER_DEFINITIONS_RELEASE          : " ${COMPILER_DEFINITIONS_RELEASE})
        display_list("COMPILER_DEFINITIONS_MINSIZEREL       : " ${COMPILER_DEFINITIONS_MINSIZEREL})
        display_list("COMPILER_DEFINITIONS_RELWITHDEBINFO   : " ${COMPILER_DEFINITIONS_RELWITHDEBINFO})

        display_list("COMPILER_OPTIONS_CXX_DEBUG            : " ${COMPILER_OPTIONS_CXX_DEBUG})
        display_list("COMPILER_OPTIONS_CXX_RELEASE          : " ${COMPILER_OPTIONS_CXX_RELEASE})
        display_list("COMPILER_OPTIONS_CXX_MINSIZEREL       : " ${COMPILER_OPTIONS_CXX_MINSIZEREL})
        display_list("COMPILER_OPTIONS_CXX_RELWITHDEBINFO   : " ${COMPILER_OPTIONS_CXX_RELWITHDEBINFO})

        display_list("COMPILER_OPTIONS_C_DEBUG              : " ${COMPILER_OPTIONS_C_DEBUG})
        display_list("COMPILER_OPTIONS_C_RELEASE            : " ${COMPILER_OPTIONS_C_RELEASE})
        display_list("COMPILER_OPTIONS_C_MINSIZEREL         : " ${COMPILER_OPTIONS_C_MINSIZEREL})
        display_list("COMPILER_OPTIONS_C_RELWITHDEBINFO     : " ${COMPILER_OPTIONS_C_RELWITHDEBINFO})

        display_list("LINKER_OPTIONS_DEBUG                  : " ${LINKER_OPTIONS_DEBUG})
        display_list("LINKER_OPTIONS_RELEASE                : " ${LINKER_OPTIONS_RELEASE})
        display_list("LINKER_OPTIONS_MINSIZEREL             : " ${LINKER_OPTIONS_MINSIZEREL})
        display_list("LINKER_OPTIONS_RELWITHDEBINFO         : " ${LINKER_OPTIONS_RELWITHDEBINFO})

        display_list("Active COMPILER_DEFINITIONS           : " ${COMPILER_DEFINITIONS})
        display_list("Active COMPILER_OPTIONS_CXX           : " ${COMPILER_OPTIONS_CXX})
        display_list("Active COMPILER_OPTIONS_C             : " ${COMPILER_OPTIONS_C})
        display_list("Active LINKER_OPTIONS                 : " ${LINKER_OPTIONS})
        display_list("Active LINKER_LIBRARIES               : " ${LINKER_LIBRARIES})
        display_list("Active LINK_DIRECTORIES               : " ${LINK_DIRECTORIES})
    endif()
endmacro()

macro(set_config_dir)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(CONFIG_DIR Debug)
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release" OR
           ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel" OR 
           ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        set(CONFIG_DIR NonDebug)
    else()
        message(FATAL_ERROR "Invalid build type: " ${CMAKE_BUILD_TYPE})
    endif()
endmacro()

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()

set_config_dir()

if (PLATFORM_WINDOWS)
    set(CMAKE_USE_WIN32_THREADS_INIT TRUE)
    find_package(Threads REQUIRED)
else()
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()

setup_default_options()
convert_default_options()

add_custom_target(install-components)
add_custom_target(uninstall-components)

set(THIS_LIST_DIR ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

function(add_uninstall_target TARGET MAIN_TARGET)
    if (NOT PLATFORM_WINDOWS)
        configure_file(
            ${THIS_LIST_DIR}/uninstall.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake
            IMMEDIATE @ONLY)
        add_custom_target(uninstall_${TARGET}
            COMMAND ${CMAKE_COMMAND} -DCOMPONENT=${TARGET} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake
            DEPENDS ${TARGET}
            COMMENT "Uninstalling ${TARGET}")
        add_dependencies(${MAIN_TARGET} uninstall_${TARGET})
    endif()
endfunction()

function(add_install_target TARGET MAIN_TARGET)
    add_custom_target(install_${TARGET}
        COMMAND ${CMAKE_COMMAND} -DCOMPONENT=${TARGET} -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
        DEPENDS ${TARGET}
        COMMENT "Installing ${TARGET}")
    add_dependencies(${MAIN_TARGET} install_${TARGET})
endfunction()

message(STATUS "Setup google test")
# to prevent a warning with google test, set the default value for policy CMP0048
message(STATUS "In directory: ${CMAKE_SOURCE_DIR}/../Externals/Philips.3rdParty.gtest/googletest-release-1.8.0")
set( CMAKE_POLICY_DEFAULT_CMP0048 NEW )
add_subdirectory(code/external)

message(STATUS "Setup code tree")
add_subdirectory(code)
