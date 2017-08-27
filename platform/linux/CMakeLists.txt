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

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${ESM_PLATFORM}_app.c")
    target_sources(${ESM_TARGET}Src
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/src/${ESM_PLATFORM}_app.c"
    )
endif()

function(add_esm_executable EXECUTABLE_NAME)
    add_executable(${EXECUTABLE_NAME} ${ARGN})
endfunction(add_esm_executable)
