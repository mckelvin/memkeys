cmake_minimum_required(VERSION 2.8)

find_program(CMAKE_CXX_COMPILER NAMES g++-4.8.1 g++-4.8 g++-4.7.2 g++-4.7.1 g++-4.7 g++)
find_program(CMAKE_C_COMPILER NAMES gcc-4.8.1 gcc-4.8 gcc-4.7.2 gcc-4.7.1 gcc-4.7 gcc)

project(memkeys)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 2)
set(VERSION_PATCH 0)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
                    OUTPUT_VARIABLE GCC_VERSION)
    if(GCC_VERSION VERSION_LESS 4.6)
        message(FATAL_ERROR "GCC version >= 4.6 required")
    endif()

    set(WARN_FLAGS "-Wall -Wextra -Wno-unused-parameter")
    set(CMAKE_C_FLAGS "-pthread ${WARN_FLAGS} ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-std=c++0x -pthread ${WARN_FLAGS} ${CMAKE_CXX_FLAGS}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(WARN_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wno-format-invalid-specifier")
    set(CMAKE_C_FLAGS "-pthread ${WARN_FLAGS} ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-std=c++11 -pthread ${WARN_FLAGS} ${CMAKE_CXX_FLAGS}")
endif()
set(CMAKE_C_FLAGS_DEBUG "-DDEBUG ${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG ${CMAKE_CXX_FLAGS_DEBUG}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

include_directories(${CMAKE_SOURCE_DIR}/src)

try_compile(
    HAVE_ATOMIC
    ${memkeys_BINARY_DIR} ${CMAKE_MODULE_PATH}/check_atomic.cpp
    OUTPUT_VARIABLE OUTPUT
)

if(HAVE_ATOMIC)
    add_definitions(-DHAVE_ATOMIC)
endif()
add_subdirectory(src)


add_subdirectory(gtest)
enable_testing()
add_subdirectory(tests)
