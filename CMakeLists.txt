cmake_minimum_required(VERSION 3.10)
project(esm)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)
set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wextra \
 -Werror -Wno-variadic-macros -Wno-missing-braces -Wno-unused-const-variable")

configure_file (
    "core/include/config.h.in"
    "core/include/config.h"
  )

if(NOT ESM_PLATFORM)
  set(ESM_PLATFORM linux)
  set(ESM_BOARD native)
endif()

include("${CMAKE_SOURCE_DIR}/Core.cmake")
include("${CMAKE_SOURCE_DIR}/platform/${ESM_PLATFORM}/CMakeLists.txt")

add_subdirectory(apps/blink)
add_subdirectory(apps/dpp)
add_subdirectory(apps/smokers)
add_subdirectory(apps/pelican)
add_subdirectory(apps/prodcons)
add_subdirectory(apps/clock)
