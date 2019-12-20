def_esm_app(keyboard)

target_compile_definitions(${APP_NAME}
    PUBLIC
        -DESM_MAX_PRIO=3
)

target_include_directories(${APP_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(${APP_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/keyboard.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/matrix.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/keymap.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tap_detector.c
)

# ugly, but does the job for now
if(${ESM_PLATFORM} STREQUAL "stm32")
	target_compile_definitions(${APP_NAME}
		PUBLIC
		    -DESM_TRACE_UART=\(huart1\)
	)

	target_sources(${APP_NAME}
		PUBLIC
		    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
		    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
		    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
		    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/HID/Src/usbd_hid.c
	)

	target_include_directories(${APP_NAME}
		PUBLIC
		    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
		    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc
	)
endif()

end_esm_app()
