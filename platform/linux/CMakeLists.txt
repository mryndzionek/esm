if(NOT ESM_BOARD)
    message( FATAL_ERROR "ESM_BOARD not defined" )
endif()

set(CMAKE_C_FLAGS_DEBUG "-Wall -Wextra \
-O2 -Wshadow -Wformat=2 -Wfloat-equal \
-Wlogical-op -Wcast-qual -Wno-variadic-macros \
-Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC \
-D_FORTIFY_SOURCE=2 -fsanitize=address \
-fsanitize=undefined -fno-sanitize-recover \
-fstack-protector"
)

set(ESM_PLATFORM_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/src/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/src/platform.c"
    "${CMAKE_CURRENT_LIST_DIR}/include/platform.h"
)

set(ESM_PLATFORM_HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

macro(def_esm_platform)
    file(GLOB_RECURSE BOARD_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/board/${ESM_BOARD}/src/*.c"
    )
    target_sources(${APP_NAME}
        PUBLIC
            ${BOARD_SOURCES}
    )
    target_include_directories(${APP_NAME}
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/board/${ESM_BOARD}/include"
    )
endmacro(def_esm_platform)

macro(end_esm_platform)
endmacro(end_esm_platform)
