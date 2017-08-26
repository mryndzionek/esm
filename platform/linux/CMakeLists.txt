target_sources(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/src/main.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/platform.c"
        "${CMAKE_CURRENT_LIST_DIR}/include/platform.h"
)
target_include_directories(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
)

function(add_esm_executable EXECUTABLE_NAME)
    add_executable(${EXECUTABLE_NAME} ${ARGN})
endfunction(add_esm_executable)
