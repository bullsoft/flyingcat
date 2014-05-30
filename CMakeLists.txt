project(flyingcat)

cmake_minimum_required(VERSION 2.6)

SET(FLYINGCAT_VERSION "0.0.1")
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

set(LIBPHENOM_PATH "" CACHE PATH "Path for libphenom")
if (NOT LIBPHENOM_PATH)
    message(SEND_ERROR
        "Please specify libphenom path with -DLIBPHENOM_PATH=/path/to/libphenom")
endif(NOT LIBPHENOM_PATH)

# whether fc_log_debug() is available
if (CMAKE_BUILD_TYPE)
    string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
endif (CMAKE_BUILD_TYPE)
if (BUILD_TYPE STREQUAL "DEBUG" OR DEBUG_LOG)
    set(FC_DEBUG_LOG TRUE)
endif (BUILD_TYPE STREQUAL "DEBUG" OR DEBUG_LOG)

# check header files
include(CheckIncludeFiles)

# check functions
include(CheckFunctionExists)
check_function_exists(backtrace HAVE_BACKTRACE)
check_function_exists(backtrace_symbols HAVE_BACKTRACE_SYMBOLS)
check_function_exists(posix_memalign HAVE_POSIX_MEMALIGN)
check_function_exists(memalign HAVE_MEMALIGN)

include_directories(${LIBPHENOM_PATH}/include
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}/src
                   )

link_directories(${LIBPHENOM_PATH}/lib)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
#set(CMAKE_INSTALL_RPATH "${LIBPHENOM_PATH}/lib")

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(src)

