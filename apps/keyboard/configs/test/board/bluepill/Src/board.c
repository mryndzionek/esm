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
	static esm_signal_t sig = {
		.type = esm_sig_alarm,
		.params.debouncer.state = 0,
		.sender = NULL,
		.receiver = NULL};

	if (GPIO_Pin == ENC1_CHAN_A_Pin)
	{
		CLEAR_BIT(EXTI->IMR, ENC1_CHAN_A_Pin);
		sig.receiver = deb_enc_chan_a_esm;
		esm_send_signal(&sig);
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
	}
	else if (GPIO_Pin == ENC1_CHAN_B_Pin)
	{
		CLEAR_BIT(EXTI->IMR, ENC1_CHAN_B_Pin);
		sig.receiver = deb_enc_chan_b_esm;
		esm_send_signal(&sig);
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
	}
}

bool board_usb_send(uint8_t *data, uint16_t len)
{
	if (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED)
	{
		return false;
	}
	USBD_HID_SendReport(&hUsbDeviceFS, data, len);
	
	return true;
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
	esm_signal_t sig = {
		.type = esm_sig_alarm,
		.params.encoder = {.chan = 0, .state = HAL_GPIO_ReadPin(GPIOA, ENC1_CHAN_A_Pin)},
		.sender = NULL,
		.receiver = encoder_esm};
	esm_send_signal(&sig);
}

static void chan_a_arm(esm_t *const esm)
{
	SET_BIT(EXTI->IMR, ENC1_CHAN_A_Pin);
}

static const debouncer_cfg_t deb_enc_chan_a_cfg = {
	.period = 1UL,
	.handle = chan_a_handle,
	.arm = chan_a_arm};

ESM_REGISTER(debouncer, deb_enc_chan_a, esm_gr_none, 2, 1);

static void chan_b_handle(esm_t *const esm, BOARD_DEBOUNCER_STATE state)
{
	esm_signal_t sig = {
		.type = esm_sig_alarm,
		.params.encoder = {.chan = 1, .state = HAL_GPIO_ReadPin(GPIOA, ENC1_CHAN_B_Pin)},
		.sender = NULL,
		.receiver = encoder_esm};
	esm_send_signal(&sig);
}

static void chan_b_arm(esm_t *const esm)
{
	SET_BIT(EXTI->IMR, ENC1_CHAN_B_Pin);
}

static const debouncer_cfg_t deb_enc_chan_b_cfg = {
	.period = 1UL,
	.handle = chan_b_handle,
	.arm = chan_b_arm};

ESM_REGISTER(debouncer, deb_enc_chan_b, esm_gr_none, 2, 1);

static void enc_cw_action(void)
{
	esm_signal_t s = {
		.type = esm_sig_matrix,
		.params.mat = {
			.row = 0,
			.col = 0,
			.ev = mat_ev_down},
		.sender = NULL,
		.receiver = keyboard_esm};
	esm_send_signal(&s);
	s.params.mat.ev = mat_ev_up;
	esm_send_signal(&s);
}

static void enc_ccw_action(void)
{
	esm_signal_t s = {
		.type = esm_sig_matrix,
		.params.mat = {
			.row = 1,
			.col = 0,
			.ev = mat_ev_down},
		.sender = NULL,
		.receiver = keyboard_esm};
	esm_send_signal(&s);
	s.params.mat.ev = mat_ev_up;
	esm_send_signal(&s);
}

static const encoder_cfg_t encoder_cfg = {
	.cw_action = enc_cw_action,
	.ccw_action = enc_ccw_action,
};

ESM_REGISTER(encoder, encoder, esm_gr_none, 4, 1);
