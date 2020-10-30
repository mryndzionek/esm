target_compile_definitions(${APP_NAME}
    PUBLIC
        -DNUM_TAPS=2
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
