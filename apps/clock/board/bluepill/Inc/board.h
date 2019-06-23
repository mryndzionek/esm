#ifndef APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_
#define APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_

#include "stm32f1xx_hal.h"

extern const uint16_t board_leds[];

#define BOARD_LED_ON(_led_num) (void)(_led_num)

#define BOARD_LED_OFF(_led_num) (void)(_led_num)

#endif /* APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_ */
