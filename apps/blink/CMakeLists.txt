set(ESM_TARGET blink)
set(ESM_HSM ON)
add_library(${ESM_TARGET}Src STATIC "")
include("${CMAKE_SOURCE_DIR}/Core.cmake")
include("${CMAKE_SOURCE_DIR}/platform/${ESM_PLATFORM}/CMakeLists.txt")

target_compile_definitions(${ESM_TARGET}Src
    PUBLIC
        -DESM_MAX_PRIO=2
)

target_include_directories(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_sources(${ESM_TARGET}Src
    PRIVATE
        "${CMAKE_SOURCE_DIR}/modules/src/debouncer.c"
)

add_esm_executable(
    ${ESM_TARGET}
    src/blink.c
)
target_link_libraries(${ESM_TARGET} ${ESM_TARGET}Src)
