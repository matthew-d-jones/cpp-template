cmake_minimum_required(VERSION 3.9.0)

###############################################################################
## Setup (mainly compiler options)
###############################################################################

# Name
project(TemplateProject LANGUAGES CXX)

# Make a Debug build the default
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Find custom cmake modules
set(LOCAL_CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH "${LOCAL_CMAKE_MODULE_PATH}/sanitizers-cmake/cmake" ${LOCAL_CMAKE_MODULE_PATH})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Set the CMake standard, can set to 98, 11, 14, or 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags to be strict about warnings
if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -pedantic-errors -Werror")
elseif (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w4")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
endif()

# Use gold linker if available on system (it is faster than default GNU linker)
if (UNIX AND NOT APPLE)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE ld_version)
    if ("${ld_version}" MATCHES "GNU gold")
        message(STATUS "Using gold linker")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
    endif()
endif()

# Enable link-time optimisation, for performance gains and smaller executable size
# Typically takes quite a long time and affects debugging, so we'll only enable it for release builds
if(CMAKE_BUILD_TYPE MATCHES Release)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result OUTPUT output)
    if(result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        # CMake specifies -flto=thin when using clang compiler which automatically parallelises,
        # but if we are using gcc then we have to explicitly tell it to parallelise
        if(CMAKE_COMPILER_IS_GNUCXX)
            include(ProcessorCount)
            ProcessorCount(NCores) # Returns 0 if it fails
            if(NOT NCores EQUAL 0)
                message(STATUS "Parallelising link-time optimisation over ${NCores} CPU cores")
                list(APPEND CMAKE_C_COMPILE_OPTIONS_IPO "-flto=${NCores}")
                list(APPEND CMAKE_CXX_COMPILE_OPTIONS_IPO "-flto=${NCores}")
            endif()
        endif()
        message(STATUS "Using link-time optimisation")
    else()
        message(STATUS "Link-time optimisation is not supported: ${output}")
    endif()
endif(CMAKE_BUILD_TYPE MATCHES Release)

###############################################################################
## Sanitisers
###############################################################################

find_package(Sanitizers)
# Add sanitisers to a target like this:
# add_sanitizers(some_lib)
# add_sanitizers(some_exe)
# And enable them with CMake settings:
# SANITIZE_ADDRESS, SANITIZE_MEMORY, SANITIZE_THREAD or SANITIZE_UNDEFINED
# for example -DSANITIZE_ADDRESS=On

###############################################################################
## Create target to build documentation (Doxygen)
###############################################################################

set(BUILD_DOCS TRUE)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if (DOXYGEN_FOUND AND BUILD_DOCS)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(docs
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM
            )
endif()

###############################################################################
## Get dependencies (with Conan)
###############################################################################

set(CONAN "AUTO" CACHE STRING "conan options AUTO (conan must be in path) or MANUAL (expects conanbuildinfo.cmake in build directory)")
set(CONAN_PROFILE "default" CACHE STRING "Name of conan profile to use, uses default by default")
include(${LOCAL_CMAKE_MODULE_PATH}/conan.cmake)
if (${CONAN} MATCHES "AUTO")
    conan_cmake_run(CONANFILE conanfile.txt
            PROFILE ${CONAN_PROFILE}
            BASIC_SETUP CMAKE_TARGETS
            BUILD outdated
            KEEP_RPATHS)
elseif (${CONAN} MATCHES "MANUAL" )
    if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
        include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
        conan_basic_setup(TARGETS)
    else()
        message(FATAL_ERROR "CONAN set to MANUAL but no file named conanbuildinfo.cmake found in build directory")
    endif()
else()
    message(FATAL_ERROR "Unrecognised option for CONAN, use AUTO or MANUAL")
endif()

###############################################################################
## Create target for the main executable
###############################################################################

set(SRC_FILES
    src/main.cpp
    # add any additional source files here
)

set(HDR_FILES
    # add any additional header files here
)

set(LIBRARIES
    CONAN_PKG::fmt
    CONAN_PKG::spdlog
    CONAN_PKG::CLI11
)

add_executable(TemplateProject ${SRC_FILES} ${HDR_FILES})
target_link_libraries(TemplateProject ${LIBRARIES})
add_sanitizers(TemplateProject)

###############################################################################
## Create target for unit tests
###############################################################################

add_executable(UnitTests tests/tests.cpp)
target_link_libraries(UnitTests CONAN_PKG::catch2)
