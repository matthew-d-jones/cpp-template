cmake_minimum_required(VERSION 3.9.0)

###############################################################################
## Setup (mainly compiler options)
###############################################################################

# Name
project(TemplateProject)

# Make a Debug build the default
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Find custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 14)

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

# Enable link-time optimisation
# Can be slow and affect debugging, so only enabled for release builds
if (CMAKE_BUILD_TYPE EQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result OUTPUT output)
    if(result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "IPO is not supported: ${output}")
    endif()
endif (CMAKE_BUILD_TYPE EQUAL "Release")

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
include(${CMAKE_MODULE_PATH}/conan.cmake)
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
## Executable
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

###############################################################################
## Unit Tests
###############################################################################

add_executable(UnitTests tests/tests.cpp)
target_link_libraries(UnitTests CONAN_PKG::catch2)
