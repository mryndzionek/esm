set(ESM_APP_IDS ${ESM_APP_IDS} backlight)

target_compile_definitions(${APP_NAME}
    PUBLIC
        -DSK6812_LEDS_NUM=24
        -DNUM_TAPS=3
)

target_sources(${APP_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/backlight_simple.c
        ${CMAKE_SOURCE_DIR}/lib/src/sk6812.c
)

target_include_directories(${APP_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/lib/include
)

if(${ESM_PLATFORM} STREQUAL "stm32")

	target_compile_definitions(${APP_NAME}
		PUBLIC
		    -DESM_TRACE_DISABLE
	)
endif()
