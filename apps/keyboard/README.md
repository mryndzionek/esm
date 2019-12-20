USB HID keyboard firmware
=========================

 - configurable similarly to QMK via keymap file
 - multi-layer support
 - tap detection


Bootloader binary
-----------------

```diff
diff --git a/apps/keyboard/board/bluepill/STM32F103C8Tx_FLASH.ld b/apps/keyboard/board/bluepill/STM32F103C8Tx_FLASH.ld
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
diff --git a/apps/keyboard/board/bluepill/Src/system_stm32f1xx.c b/apps/keyboard/board/bluepill/Src/system_stm32f1xx.c
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

TODO
----

 - move different keyboard configurations to separate locations


