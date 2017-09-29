#include "platform.h"
#include "board.h"
#include "esm/esm.h"

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	esm_signal_t sig = {
			.type = esm_sig_alarm,
			.sender = (void*)0,
			.receiver = trace_esm,
	};
	esm_send_signal(&sig);
}

void HAL_SYSTICK_Callback(void)
{
	esm_signal_t sig = {
			.type = esm_sig_alarm,
			.sender = (void*)0,
			.receiver = tick_esm,
	};
	esm_send_signal(&sig);
#ifdef BOARD_TICK
	BOARD_TICK;
#endif
}

uint16_t platform_rnd(uint16_t range)
{
	static uint16_t lfsr = 0xACE1u;

	unsigned lsb = lfsr & 1;
	lfsr >>= 1;
	if (lsb) {
		lfsr ^= 0xB400u;
	}

	return lfsr % range;
}
