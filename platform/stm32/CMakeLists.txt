enable_language(ASM)

if(NOT ESM_BOARD)
    message( FATAL_ERROR "ESM_BOARD not defined" )
endif()

target_compile_definitions(${ESM_TARGET}Src
    PUBLIC
        -DUSE_HAL_DRIVER -DSTM32F103xB -DESM_HAS_CLZ
)

file(GLOB_RECURSE BOARD_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/board/${ESM_BOARD}/Src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/board/${ESM_BOARD}/*.s"
)

file(GLOB_RECURSE HAL_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/STM32F1xx_HAL_Driver/Src/*.c")

target_sources(${ESM_TARGET}Src
    PUBLIC
        "${BOARD_SOURCES}"
        "${HAL_SOURCES}"
        "${CMAKE_CURRENT_LIST_DIR}/src/platform.c"
        "${CMAKE_CURRENT_LIST_DIR}/include/platform.h"
)
target_include_directories(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/${ESM_BOARD}/Inc"
        "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/STM32F1xx_HAL_Driver/Inc"
        "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy"
        "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/CMSIS/Include"
        "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/CMSIS/Device/ST/STM32F1xx/Include"
)

set(CMAKE_EXE_LINKER_FLAGS
    "-specs=nano.specs ${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${ESM_TARGET}.map,--cref -lc -lm -lnosys")

function(add_esm_plat_exec EXECUTABLE_NAME)
    add_executable(${EXECUTABLE_NAME} ${ARGN})
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${EXECUTABLE_NAME}> ${EXECUTABLE_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${EXECUTABLE_NAME}> ${EXECUTABLE_NAME}.bin
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")
endfunction(add_esm_plat_exec)
