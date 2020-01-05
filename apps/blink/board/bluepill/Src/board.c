#include "board.h"
#include "esm/esm.h"
#include "debouncer.h"

const uint16_t board_leds[3] =
{
		BLINK1_LED_Pin,
		BLINK2_LED_Pin,
		BLINK3_LED_Pin
};

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == BLINK_BUTTON_Pin)
	{
		int s = HAL_GPIO_ReadPin(BLINK_BUTTON_GPIO_Port, BLINK_BUTTON_Pin);
		EXTI->IMR &= ~BLINK_BUTTON_Pin;
		esm_signal_t sig = {
				.type = esm_sig_alarm,
				.params.debouncer.state = s,
				.sender = NULL,
				.receiver = debouncer_esm
		};
		esm_send_signal(&sig);
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
	}
}

static void debouncer_handle(esm_t *const esm, BOARD_DEBOUNCER_STATE state)
{
	if(HAL_GPIO_ReadPin(BLINK_BUTTON_GPIO_Port, BLINK_BUTTON_Pin) == state)
	{
		esm_signal_t s = {
				.type = esm_sig_button,
				.sender = NULL,
				.receiver = blink1_esm
		};
		esm_broadcast_signal(&s, esm_gr_blinkers);
	}
}

static void debouncer_arm(esm_t *const esm)
{
	EXTI->IMR |= BLINK_BUTTON_Pin;
}

static const debouncer_cfg_t debouncer_cfg = {
		.period = 30UL,
		.handle = debouncer_handle,
		.arm = debouncer_arm
};

ESM_REGISTER(debouncer, debouncer, esm_gr_none, 1, 1);
