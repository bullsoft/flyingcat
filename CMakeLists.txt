# Copyright (C) 2014 Shang Yuanchun <idealities@gmail.com>

project(flyingcat)

cmake_minimum_required(VERSION 2.6)

message(STATUS "Running cmake version ${CMAKE_VERSION}")

set(FLYINGCAT_VERSION "0.0.1")
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# set(LIBPHENOM_PATH "" CACHE PATH "Path for libphenom")
# if (NOT LIBPHENOM_PATH)
#     message(SEND_ERROR
#         "Please specify libphenom path with -DLIBPHENOM_PATH=/path/to/libphenom")
# endif(NOT LIBPHENOM_PATH)

# whether fc_log_debug() is available
if (CMAKE_BUILD_TYPE)
    string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
endif (CMAKE_BUILD_TYPE)
if (BUILD_TYPE STREQUAL "DEBUG" OR DEBUG_LOG)
    set(FC_DEBUG_LOG TRUE)
    add_definitions(-Wall)
endif (BUILD_TYPE STREQUAL "DEBUG" OR DEBUG_LOG)

if (LOG_PATH)
    set(FC_LOG_PATH TRUE)
endif (LOG_PATH)
if (CONF_PATH)
    set(FC_CONF_PATH TRUE)
endif (CONF_PATH)
if (PID_PATH)
    set(FC_PID_PATH TRUE)
endif (PID_PATH)

# check header files
include(CheckIncludeFiles)

# check functions
include(CheckFunctionExists)
check_function_exists(backtrace HAVE_BACKTRACE)
check_function_exists(backtrace_symbols HAVE_BACKTRACE_SYMBOLS)
check_function_exists(posix_memalign HAVE_POSIX_MEMALIGN)
check_function_exists(memalign HAVE_MEMALIGN)
check_function_exists(getopt_long HAVE_GETOPT_LONG)
check_function_exists(getifaddrs HAVE_GETIFADDRS)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}/src
                   )

if (LIBPHENOM_PATH)
    include_directories(${LIBPHENOM_PATH}/include)
    link_directories(${LIBPHENOM_PATH}/lib)
endif (LIBPHENOM_PATH)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# set(CMAKE_INSTALL_RPATH "${LIBPHENOM_PATH}/lib")

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-D_GNU_SOURCE)
endif (CMAKE_COMPILER_IS_GNUCC)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(src)

