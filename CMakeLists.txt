cmake_minimum_required(VERSION 2.8)

project(memkeys)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 2)
set(VERSION_PATCH 0)

execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if(GCC_VERSION VERSION_LESS 4.5)
    message(FATAL_ERROR "GCC version >= 4.5 required")
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None Debug Release." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

set(COMMON_FLAGS "-Wall -Wextra -Wno-unused-parameter -pedantic -pthread")

set(CMAKE_CXX_FLAGS_DEBUG "-std=c++0x -g3 -O0 ${COMMON_FLAGS}" CACHE STRING "CXX DEBUG FLAGS" FORCE)
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 ${COMMON_FLAGS}" CACHE STRING "C DEBUG FLAGS" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++0x -DNDEBUG -O2 ${COMMON_FLAGS}" CACHE STRING "CXX RELEASE FLAGS" FORCE)
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O2 ${COMMON_FLAGS}" CACHE STRING "C RELEASE FLAGS" FORCE)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

include_directories(${CMAKE_SOURCE_DIR}/src)

try_compile(
    HAVE_ATOMIC
    ${memkeys_BINARY_DIR} ${CMAKE_MODULE_PATH}/check_atomic.cpp
    COMPILE_DEFINITIONS -std=c++0x
    OUTPUT_VARIABLE OUTPUT
)

if(HAVE_ATOMIC)
    add_definitions(-DHAVE_ATOMIC)
endif()
add_subdirectory(src)


add_subdirectory(gtest)
enable_testing()
add_subdirectory(tests)
