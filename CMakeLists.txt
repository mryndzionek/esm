cmake_minimum_required(VERSION 3.5)
project(esm)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)
set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wextra \
 -Werror -Wno-variadic-macros -Wno-missing-braces")

configure_file (
    "core/include/config.h.in"
    "core/include/config.h"
  )

if(NOT ESM_PLATFORM)
  set(ESM_PLATFORM linux)
endif()

file(GLOB ESM_SOURCES
    "core/src/*.c"
    "platform/${ESM_PLATFORM}/src/*.c"
    "trace/src/*.c"
    )

include_directories("core/include")
include_directories("platform/${ESM_PLATFORM}/include")
include_directories("trace/include")

add_subdirectory(apps/blink)
add_subdirectory(apps/dpp)
add_subdirectory(apps/smokers)
