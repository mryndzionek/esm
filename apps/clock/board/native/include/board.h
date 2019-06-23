#ifndef APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_
#define APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_

#define BOARD_LED_ON(_led_num) do { \
		(void)(_led_num); \
		ESM_PRINTF("BLINK_LED%d:ON\r\n", _led_num); \
} while(0)

#define BOARD_LED_OFF(_led_num) do { \
		(void)(_led_num); \
		ESM_PRINTF("BLINK_LED%d:OFF\r\n", _led_num); \
} while(0)

#endif /* APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_ */
