enable_language(ASM)
target_compile_definitions(${ESM_TARGET}Src
    PUBLIC
        -DUSE_HAL_DRIVER -DSTM32F103xB
)

file(GLOB_RECURSE USER_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Src/*.c")
file(GLOB_RECURSE HAL_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/STM32F1xx_HAL_Driver/Src/*.c")

target_sources(${ESM_TARGET}Src
    PUBLIC
        "${USER_SOURCES}"
        "${HAL_SOURCES}"
        "${CMAKE_CURRENT_LIST_DIR}/include/platform.h"
        "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/startup_stm32f103xb.s"
)
target_include_directories(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
        "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Inc"
        "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/STM32F1xx_HAL_Driver/Inc"
        "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy"
        "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/CMSIS/Include"
        "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8/Drivers/CMSIS/Device/ST/STM32F1xx/Include"
)

set(CMAKE_EXE_LINKER_FLAGS
    "-specs=nano.specs ${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${ESM_TARGET}.map,--cref -lc -lm -lnosys")
set(HEX_FILE ${CMAKE_CURRENT_BINARY_DIR}/build/${ESM_TARGET}.hex)
set(BIN_FILE ${CMAKE_CURRENT_BINARY_DIR}/build/${ESM_TARGET}.bin)
