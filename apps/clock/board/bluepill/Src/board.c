#include "board.h"
#include "esm/esm.h"
#include "debouncer.h"

static const NEC_HW_CTX_t nechwctx = {
	.timerHandle = &htim2,
	.timerChannel = TIM_CHANNEL_1,
};

NEC_t nec1 = {
	.hwctx = &nechwctx,
	.timingBitBoundary = 1680,
	.timingAgcBoundary = 12500,
	.type = NEC_NOT_EXTENDED,
};

void NEC_TIM_IC_CaptureCallback(NEC_t *handle)
{
	HAL_TIM_IC_Stop_DMA(handle->hwctx->timerHandle, handle->hwctx->timerChannel);
	if (handle->state == NEC_INIT)
	{
		if (handle->rawTimerData[1] < handle->timingAgcBoundary)
		{
			esm_signal_t sig = {
				.type = esm_sig_reset,
				.sender = NULL,
				.receiver = nec1_esm};
			esm_send_signal(&sig);
		}
		else
		{
			handle->state = NEC_AGC_OK;
			HAL_TIM_IC_Start_DMA(handle->hwctx->timerHandle, handle->hwctx->timerChannel,
								 (uint32_t *)handle->rawTimerData, 32);
		}
	}
	else if (handle->state == NEC_AGC_OK)
	{
		esm_signal_t sig = {
			.type = esm_sig_alarm,
			.sender = NULL,
			.receiver = nec1_esm};
		esm_send_signal(&sig);
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim2)
	{
		NEC_TIM_IC_CaptureCallback(&nec1);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == RTC_IRQ_Pin)
	{
		esm_signal_t sig = {
			.type = esm_sig_alarm,
			.sender = NULL,
			.receiver = rtc1_esm};
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

static void debouncer_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	if (HAL_GPIO_ReadPin(PUSHBUTTON_GPIO_Port, PUSHBUTTON_Pin) == GPIO_PIN_RESET)
	{
		esm_signal_t s = {
			.type = esm_sig_button,
			.sender = NULL,
			.receiver = clock1_esm};
		esm_send_signal(&s);
	}
}

static void debouncer_arm(esm_t *const esm)
{
	EXTI->IMR |= PUSHBUTTON_Pin;
}

static const debouncer_cfg_t debouncer_cfg = {
	.period = 30UL,
	.handle = debouncer_handle,
	.arm = debouncer_arm};

ESM_REGISTER(debouncer, debouncer, esm_gr_none, 1, 1);

void board_nec_start(NEC_t *handle)
{
	handle->state = NEC_INIT;
	HAL_TIM_IC_Start_DMA(handle->hwctx->timerHandle, handle->hwctx->timerChannel,
						 (uint32_t *)handle->rawTimerData, 2);
}

void board_nec_stop(NEC_t *handle)
{
	HAL_TIM_IC_Stop_DMA(handle->hwctx->timerHandle, handle->hwctx->timerChannel);
}