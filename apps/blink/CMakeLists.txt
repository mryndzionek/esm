def_esm_app(blink)

set(ESM_HSM ON)
include("${CMAKE_SOURCE_DIR}/modules/debouncer.cmake")

target_compile_definitions(${APP_NAME}
    PUBLIC
        -DESM_MAX_PRIO=2
)

target_include_directories(${APP_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(${APP_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/blink.c
)

end_esm_app()
