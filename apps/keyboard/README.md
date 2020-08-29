USB HID keyboard firmware
=========================

 - configurable similarly to QMK via [keymap](src/keymap.c) file
 - timing options can be adjusted via [config.h](include/config.h)
 - multi-layers support
 - tap detection

To build this app with [real](configs/atreus50/src/keymap.c#L39) configuration run CMake with following arguments:

```sh
cmake -DCMAKE_BUILD_TYPE=Release -DESM_PLATFORM=stm32 -DESM_BOARD=bluepill \
      -DCMAKE_TOOLCHAIN_FILE=../platform/stm32/Toolchain.cmake -DKEYBOARD_NAME=atreus50 ..
```

Bootloader binary
-----------------

The keyboard app binary is suitable for [STM32duino-bootloader](https://github.com/rogerclarkmelbourne/STM32duino-bootloader).
Details of what needs to be done to a STM32CubeMX app in order to use it with the bootloader:


```diff
diff --git a/STM32F103C8Tx_FLASH.ld b/STM32F103C8Tx_FLASH.ld
index b00ae7c..3e964e8 100644
--- a/apps/keyboard/board/bluepill/STM32F103C8Tx_FLASH.ld
+++ b/apps/keyboard/board/bluepill/STM32F103C8Tx_FLASH.ld
@@ -61,7 +61,7 @@ _Min_Stack_Size = 0x400; /* required amount of stack */
 MEMORY
 {
 RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 20K
-FLASH (rx)      : ORIGIN = 0x8000000, LENGTH = 64K
+FLASH (rx)      : ORIGIN = 0x8002000, LENGTH = 64K - 0x2000^M
 }
 
 /* Define output sections */
diff --git a/system_stm32f1xx.c b/system_stm32f1xx.c
index 789b551..cf390ba 100644
--- a/apps/keyboard/board/bluepill/Src/system_stm32f1xx.c
+++ b/apps/keyboard/board/bluepill/Src/system_stm32f1xx.c
@@ -110,7 +110,7 @@
 /*!< Uncomment the following line if you need to relocate your vector Table in
      Internal SRAM. */ 
 /* #define VECT_TAB_SRAM */
-#define VECT_TAB_OFFSET  0x00000000U /*!< Vector Table base offset field. 
+#define VECT_TAB_OFFSET  0x00002000U /*!< Vector Table base offset field. ^M
                                   This value must be a multiple of 0x200. */
```

Flashing  firmware via `dfu-util`:
```sh
sleep 3; sudo dfu-util -d 1eaf:0003 -a 2 -R -D "apps/keyboard/keyboard.bin"
```

