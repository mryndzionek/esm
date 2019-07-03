#include "board.h"
#include "esm/esm.h"
#include "debouncer.h"
#include "NEC_Decode.h"

static NEC nec;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == RTC_IRQ_Pin)
	{
		esm_signal_t sig = {
			.type = esm_sig_alarm,
			.sender = NULL,
			.receiver = clock1_esm};
		esm_send_signal(&sig);
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
	}
	else if (GPIO_Pin == PUSHBUTTON_Pin)
	{
		EXTI->IMR &= ~PUSHBUTTON_Pin;
		esm_signal_t sig = {
			.type = esm_sig_alarm,
			.sender = NULL,
			.receiver = debouncer_esm};
		esm_send_signal(&sig);
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim2)
	{
		NEC_TIM_IC_CaptureCallback(&nec);
	}
}

static void debouncer_handle(void)
{
	if (HAL_GPIO_ReadPin(PUSHBUTTON_GPIO_Port, PUSHBUTTON_Pin) == GPIO_PIN_RESET)
	{
		esm_signal_t sig = {
			.type = esm_sig_button,
			.sender = NULL,
			.receiver = clock1_esm};
		esm_send_signal(&sig);
	}
}

static void debouncer_arm(void)
{
	EXTI->IMR |= PUSHBUTTON_Pin;
}

static const debouncer_cfg_t debouncer_cfg = {
	.period = 30UL,
	.handle = debouncer_handle,
	.arm = debouncer_arm};

ESM_REGISTER(debouncer, debouncer, esm_gr_none, 1, 1);

void board_init(void)
{
	nec.timerHandle = &htim2;

	nec.timerChannel = TIM_CHANNEL_1;
	nec.timerChannelActive = HAL_TIM_ACTIVE_CHANNEL_1;

	nec.timingBitBoundary = 1680;
	nec.timingAgcBoundary = 12500;
	nec.type = NEC_NOT_EXTENDED;

	NEC_Read(&nec);
}