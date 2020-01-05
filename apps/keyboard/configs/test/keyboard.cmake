set(ESM_APP_IDS ${ESM_APP_IDS} backlight
                               deb_enc_chan_a
                               deb_enc_chan_b
                               encoder)

include("${CMAKE_SOURCE_DIR}/modules/debouncer.cmake")
include("${CMAKE_SOURCE_DIR}/modules/encoder.cmake")

target_compile_definitions(${APP_NAME}
    PUBLIC
        -DSK6812_LEDS_NUM=24
)

target_include_directories(${APP_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/lib/include
)

target_sources(${APP_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/lib/src/sk6812.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/backlight.c
)

if(${ESM_PLATFORM} STREQUAL "stm32")

	target_compile_definitions(${APP_NAME}
		PUBLIC
		    -DESM_TRACE_UART=\(huart1\)
	)
endif()
