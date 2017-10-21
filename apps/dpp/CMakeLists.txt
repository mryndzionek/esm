def_esm_app(dpp)

target_compile_definitions(${APP_NAME}
    PUBLIC
        -DESM_MAX_PRIO=1
)

target_include_directories(${APP_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(${APP_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/philo.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/table.c
)

end_esm_app()
