include(CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/platform/${ESM_PLATFORM}/stm32f103c8/STM32F103C8Tx_FLASH.ld)
set(COMMON_FLAGS "-mcpu=cortex-m3 -mthumb -ffunction-sections -fdata-sections -g -fno-common -fmessage-length=0")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99 -Wno-unused-parameter")
set(CMAKE_EXE_LINKER_FLAGS "-mcpu=cortex-m3 -mthumb -Wl,-gc-sections -T ${LINKER_SCRIPT}")
