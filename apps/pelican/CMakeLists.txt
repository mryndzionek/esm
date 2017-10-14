set(ESM_TARGET pelican)
set(ESM_HSM ON)
add_library(${ESM_TARGET}Src STATIC "")
include("${CMAKE_SOURCE_DIR}/Core.cmake")
include("${CMAKE_SOURCE_DIR}/platform/${ESM_PLATFORM}/CMakeLists.txt")

include("${CMAKE_SOURCE_DIR}/modules/debouncer.cmake")

target_compile_definitions(${ESM_TARGET}Src
    PUBLIC
        -DESM_MAX_PRIO=2
)

target_include_directories(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
)

add_esm_executable(
    ${ESM_TARGET}
    src/pelican.c
)
target_link_libraries(${ESM_TARGET} ${ESM_TARGET}Src)
