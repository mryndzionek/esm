set(ESM_CORE_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/core/src/esm.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/src/esm_internal.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/src/esm_list.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/src/esm_queue.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/src/esm_timer.c"

    "${CMAKE_CURRENT_LIST_DIR}/trace/src/trace.c"
    "${CMAKE_CURRENT_LIST_DIR}/trace/src/rb.c"
)

set(ESM_CORE_HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/core/include"
    "${CMAKE_CURRENT_LIST_DIR}/trace/include"
    "${CMAKE_CURRENT_LIST_DIR}/modules/include"
)

macro(def_esm_app APPLICATION_NAME)
    set(APP_NAME ${APPLICATION_NAME})
    add_executable(${APP_NAME} "")
    target_sources(${APP_NAME}
        PUBLIC
            ${ESM_PLATFORM_SOURCES}
            ${ESM_CORE_SOURCES}
    )
    target_include_directories(${APP_NAME}
        PUBLIC
            ${ESM_PLATFORM_HEADERS}
            ${ESM_CORE_HEADERS}
    )
    def_esm_platform()
endmacro(def_esm_app)

macro(end_esm_app)
    if(${ESM_HSM})
        target_sources(${APP_NAME}
            PUBLIC
                "${CMAKE_SOURCE_DIR}/core/src/hesm.c"
        )

        target_compile_definitions(${APP_NAME}
            PRIVATE
                -DESM_HSM
        )
    endif()

    set(mSigs "")
    foreach(ms ${ESM_APP_SIGNALS})
        list(APPEND mSigs "ESM_SIGNAL(${ms})")
    endforeach(ms)
    string(REPLACE ";" " " mSigsStr "${mSigs}")
    target_compile_definitions(${APP_NAME}
        PUBLIC
            -DESM_APP_SIGNALS=${mSigsStr}
    )

    set(mIds "")
    foreach(ms ${ESM_APP_IDS})
        list(APPEND mIds "ESM_ID(${ms})")
    endforeach(ms)
    string(REPLACE ";" " " mIdsStr "${mIds}")
    target_compile_definitions(${APP_NAME}
        PUBLIC
            -DESM_APP_IDS=${mIdsStr}
    )

    set(mGrs "")
    foreach(ms ${ESM_APP_GROUPS})
        list(APPEND mGrs "ESM_GROUP(${ms})")
    endforeach(ms)
    string(REPLACE ";" " " mGrsStr "${mGrs}")
    target_compile_definitions(${APP_NAME}
        PUBLIC
            -DESM_APP_GROUPS=${mGrsStr}
    )
    end_esm_platform()

    unset(APP_NAME)
endmacro(end_esm_app)
