#include "board.h"
#include "esm/esm.h"
#include "debouncer.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == BUTTON_Pin)
	{
		EXTI->IMR &= ~BUTTON_Pin;
		esm_signal_t sig = {
				.type = esm_sig_alarm,
				.sender = NULL,
				.receiver = debouncer_esm
		};
		esm_send_signal(&sig);
		__HAL_GPIO_EXTI_CLEAR_IT(BUTTON_Pin);
	}
}

static void debouncer_handle(void)
{
	if(HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) == GPIO_PIN_RESET)
	{
		esm_signal_t sig = {
				.type = esm_sig_button,
				.sender = NULL,
				.receiver = pelican_esm
		};
		esm_send_signal(&sig);
	}
}

static void debouncer_arm(void)
{
	EXTI->IMR |= BUTTON_Pin;
}

static const debouncer_cfg_t debouncer_cfg = {
		.period = 30UL,
		.handle = debouncer_handle,
		.arm = debouncer_arm
};

ESM_REGISTER(debouncer, debouncer, 1);
