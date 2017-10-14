target_sources(${ESM_TARGET}Src
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/core/src/esm.c"
        "${CMAKE_CURRENT_LIST_DIR}/core/src/esm_internal.c"
        "${CMAKE_CURRENT_LIST_DIR}/core/src/esm_list.c"
        "${CMAKE_CURRENT_LIST_DIR}/core/src/esm_queue.c"
        "${CMAKE_CURRENT_LIST_DIR}/core/src/esm_timer.c"

        "${CMAKE_CURRENT_LIST_DIR}/trace/src/trace.c"
        "${CMAKE_CURRENT_LIST_DIR}/trace/src/rb.c"
)

if(${ESM_HSM})
    target_sources(${ESM_TARGET}Src
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/core/src/hesm.c"
    )

    target_compile_definitions(${ESM_TARGET}Src
        PRIVATE
            -DESM_HSM
    )
endif()

target_include_directories(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/core/include"
        "${CMAKE_CURRENT_LIST_DIR}/trace/include"
        "${CMAKE_CURRENT_LIST_DIR}/modules/include"
)
