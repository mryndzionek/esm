set(ESM_TARGET blink)
set(ESM_HSM ON)
add_library(${ESM_TARGET}Src STATIC "")
include("${CMAKE_SOURCE_DIR}/Core.cmake")
include("${CMAKE_SOURCE_DIR}/platform/${ESM_PLATFORM}/CMakeLists.txt")

target_include_directories(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
)

add_esm_executable(
    ${ESM_TARGET}
    src/blink.c
)
target_link_libraries(${ESM_TARGET} ${ESM_TARGET}Src)
