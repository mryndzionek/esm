file(GLOB_RECURSE BOARD_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/board/${ESM_BOARD}/src/*.c"
)

target_sources(${ESM_TARGET}Src
    PUBLIC
        "${BOARD_SOURCES}"
        "${CMAKE_CURRENT_LIST_DIR}/src/main.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/platform.c"
        "${CMAKE_CURRENT_LIST_DIR}/include/platform.h"
)
target_include_directories(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/${ESM_BOARD}/include"
)

function(add_esm_plat_exec EXECUTABLE_NAME)
    add_executable(${EXECUTABLE_NAME} ${ARGN})
endfunction(add_esm_plat_exec)
