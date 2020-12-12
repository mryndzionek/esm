target_compile_definitions(${APP_NAME}
    PUBLIC
        -DSK6812_LEDS_NUM=24
        -DNUM_TAPS=1
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
