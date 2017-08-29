target_sources(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/core/src/esm.c"
        "${CMAKE_CURRENT_LIST_DIR}/core/src/esm_internal.c"
        "${CMAKE_CURRENT_LIST_DIR}/core/src/esm_list.c"
        "${CMAKE_CURRENT_LIST_DIR}/core/src/esm_timer.c"

        "${CMAKE_CURRENT_LIST_DIR}/trace/src/trace.c"
        "${CMAKE_CURRENT_LIST_DIR}/trace/src/rb.c"
)

target_include_directories(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/core/include"
        "${CMAKE_CURRENT_LIST_DIR}/trace/include"
)
