#include "board.h"
#include "esm/esm.h"

#include "debouncer.h"
#include "encoder.h"

#include "trace.h"

typedef struct
{
	GPIO_TypeDef *port;
	uint16_t pin;
} pin_desc_t;

static const pin_desc_t cols[N_COLS] = {
	{COL1_OUT_GPIO_Port, COL1_OUT_Pin},
	{COL2_OUT_GPIO_Port, COL2_OUT_Pin},
	{COL3_OUT_GPIO_Port, COL3_OUT_Pin},
};

static const pin_desc_t rows[N_ROWS] = {
	{ROW1_IN_GPIO_Port, ROW1_IN_Pin},
	{ROW2_IN_GPIO_Port, ROW2_IN_Pin},
	{ROW3_IN_GPIO_Port, ROW3_IN_Pin},
};

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == ENC1_CHAN_A_Pin)
	{
		int s = HAL_GPIO_ReadPin(ENC1_CHAN_A_GPIO_Port, ENC1_CHAN_A_Pin);
		EXTI->IMR &= ~ENC1_CHAN_A_Pin;
		esm_signal_t sig = {
			.type = esm_sig_alarm,
			.params.debouncer.state = s,
			.sender = NULL,
			.receiver = deb_enc_chan_a_esm};
		esm_send_signal(&sig);
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
	}
	else if (GPIO_Pin == ENC1_CHAN_B_Pin)
	{
		int s = HAL_GPIO_ReadPin(ENC1_CHAN_B_GPIO_Port, ENC1_CHAN_B_Pin);
		EXTI->IMR &= ~ENC1_CHAN_B_Pin;
		esm_signal_t sig = {
			.type = esm_sig_alarm,
			.params.debouncer.state = s,
			.sender = NULL,
			.receiver = deb_enc_chan_b_esm};
		esm_send_signal(&sig);
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
	}
}

void board_read_matrix(bool (*const matrix)[N_COLS][N_ROWS])
{
	for (uint8_t j = 0; j < N_COLS; j++)
	{
		HAL_GPIO_WritePin(cols[j].port, cols[j].pin, GPIO_PIN_SET);
		for (uint8_t i = 0; i < N_ROWS; i++)
		{
			if (HAL_GPIO_ReadPin(rows[i].port, rows[i].pin) == GPIO_PIN_SET)
			{
				(*matrix)[j][i] = true;
			}
			else
			{
				(*matrix)[j][i] = false;
			}
		}
		HAL_GPIO_WritePin(cols[j].port, cols[j].pin, GPIO_PIN_RESET);
	}
}

static void chan_a_handle(esm_t *const esm, BOARD_DEBOUNCER_STATE state)
{
	int s = HAL_GPIO_ReadPin(ENC1_CHAN_A_GPIO_Port, ENC1_CHAN_A_Pin);

	uint8_t data[2] = {0, s};
	trace_data(data, sizeof(data));
}

static void chan_a_arm(esm_t *const esm)
{
	EXTI->IMR |= ENC1_CHAN_A_Pin;
}

static const debouncer_cfg_t deb_enc_chan_a_cfg = {
	.period = 2UL,
	.handle = chan_a_handle,
	.arm = chan_a_arm};

ESM_REGISTER(debouncer, deb_enc_chan_a, esm_gr_none, 2, 1);

static void chan_b_handle(esm_t *const esm, BOARD_DEBOUNCER_STATE state)
{
	int s = HAL_GPIO_ReadPin(ENC1_CHAN_B_GPIO_Port, ENC1_CHAN_B_Pin);

	uint8_t data[2] = {1, s};
	trace_data(data, sizeof(data));
}

static void chan_b_arm(esm_t *const esm)
{
	EXTI->IMR |= ENC1_CHAN_B_Pin;
}

static const debouncer_cfg_t deb_enc_chan_b_cfg = {
	.period = 2UL,
	.handle = chan_b_handle,
	.arm = chan_b_arm};

ESM_REGISTER(debouncer, deb_enc_chan_b, esm_gr_none, 2, 1);

static const encoder_cfg_t encoder_cfg = {};

ESM_REGISTER(encoder, encoder, esm_gr_none, 1, 1);
