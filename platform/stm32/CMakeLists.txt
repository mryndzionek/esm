enable_language(ASM)

if(NOT ESM_BOARD)
    message( FATAL_ERROR "ESM_BOARD not defined" )
endif()

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

add_definitions(-DUSE_HAL_DRIVER -DSTM32F103xB -DESM_HAS_CLZ)

file(GLOB_RECURSE HAL_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/STM32F1xx_HAL_Driver/Src/*.c")

set(ESM_PLATFORM_SOURCES
    "${HAL_SOURCES}"
    "${CMAKE_CURRENT_LIST_DIR}/src/platform.c"
    "${CMAKE_CURRENT_LIST_DIR}/include/platform.h"
)

set(ESM_PLATFORM_HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/include"
    "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/STM32F1xx_HAL_Driver/Inc"
    "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy"
    "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/CMSIS/Include"
    "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/CMSIS/Device/ST/STM32F1xx/Include"
)

set(CMAKE_EXE_LINKER_FLAGS
    "-specs=nano.specs ${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${ESM_TARGET}.map,--cref -lc -lm -lnosys")

macro(def_esm_platform)
    file(GLOB_RECURSE BOARD_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/board/${ESM_BOARD}/Src/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/${ESM_BOARD}/*.s"
    )
    target_sources(${APP_NAME}
        PUBLIC
            ${BOARD_SOURCES}
    )
    target_include_directories(${APP_NAME}
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/board/${ESM_BOARD}/Inc
    )
endmacro(def_esm_platform)

macro(end_esm_platform)
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${APP_NAME}> ${APP_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${APP_NAME}> ${APP_NAME}.bin
        COMMENT "Building ${APP_NAME}.hex \nBuilding ${APP_NAME}.bin")
endmacro(end_esm_platform)
