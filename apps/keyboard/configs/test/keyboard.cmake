target_compile_definitions(${APP_NAME}
    PUBLIC
        -DSK6812_LEDS_NUM=60
)

target_include_directories(${APP_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/lib/include
)

target_sources(${APP_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/lib/src/sk6812.c
)

