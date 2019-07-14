cmake_minimum_required(VERSION 3.1.0)

###############################################################################
## Setup
###############################################################################

# Name
project(TemplateProject)

# CMake Build Type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Find custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 14)
if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic-errors -Werror")
endif()

###############################################################################
## Doxygen
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
## Conan
###############################################################################

set(CONAN "AUTO" CACHE STRING "conan options AUTO (conan must be in path) or MANUAL (expects conanbuildinfo.cmake in build directory)")
set(CONAN_PROFILE "default" CACHE STRING "Name of conan profile to use, uses default by default")
include(${CMAKE_MODULE_PATH}/conan.cmake)
if (${CONAN} MATCHES "AUTO")
    conan_cmake_run(CONANFILE conanfile.txt
            PROFILE ${CONAN_PROFILE}
            BASIC_SETUP CMAKE_TARGETS
            BUILD outdated)
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
    CONAN_PKG::cli11
)

add_executable(TemplateProject ${SRC_FILES} ${HDR_FILES})

###############################################################################
## Unit Tests
###############################################################################

add_executable(UnitTests tests/tests.cpp)
target_link_libraries(UnitTests CONAN_PKG::catch2)
