
cmake_minimum_required(VERSION 2.8)
project(esm)

enable_testing()

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)
set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wextra \
 -Werror -Wno-variadic-macros -Wno-missing-braces")

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/core/include/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/core/include/config.h"
  )

if(NOT ESM_PLATFORM)
  set(ESM_PLATFORM linux)
endif()

file(GLOB ESM_SOURCES
    "${CMAKE_SOURCE_DIR}/core/src/*.c"
    "${CMAKE_SOURCE_DIR}/platform/${ESM_PLATFORM}/src/*.c"
    "${CMAKE_SOURCE_DIR}/trace/src/*.c"
    )

include_directories("${CMAKE_SOURCE_DIR}/core/include")
include_directories("${CMAKE_SOURCE_DIR}/platform/${ESM_PLATFORM}/include")
include_directories("${CMAKE_SOURCE_DIR}/trace/include")

add_subdirectory(apps/blink)
add_subdirectory(apps/dpp)
add_subdirectory(apps/smokers)
