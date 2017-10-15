#ifndef APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_
#define APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_

#include "stm32f1xx_hal.h"

extern const uint16_t board_leds[];

#define BOARD_LED_ON(_led_num) \
	HAL_GPIO_WritePin(BLINK1_LED_GPIO_Port, board_leds[_led_num], GPIO_PIN_SET)

#define BOARD_LED_OFF(_led_num) \
	HAL_GPIO_WritePin(BLINK1_LED_GPIO_Port, board_leds[_led_num], GPIO_PIN_RESET)

#endif /* APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_ */
