if(NOT KEYBOARD_NAME)
	set(KEYBOARD_NAME "test")
endif()

set(ESM_APP_BOARD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/configs/${KEYBOARD_NAME}/board)

def_esm_app(keyboard)

include("${CMAKE_CURRENT_SOURCE_DIR}/configs/${KEYBOARD_NAME}/keyboard.cmake")

target_compile_definitions(${APP_NAME}
    PUBLIC
        -DESM_MAX_PRIO=3
)

target_include_directories(${APP_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/configs/${KEYBOARD_NAME}/include
)

target_sources(${APP_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/keyboard.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/matrix.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/state_handler.c
        ${CMAKE_CURRENT_SOURCE_DIR}/configs/${KEYBOARD_NAME}/src/keymap.c
)

# ugly, but does the job for now
if(${ESM_PLATFORM} STREQUAL "stm32")

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
