def_esm_app(prodcons)

include("${CMAKE_SOURCE_DIR}/modules/bus.cmake")

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
        ${CMAKE_CURRENT_SOURCE_DIR}/src/producer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/consumer.c
)

end_esm_app()
