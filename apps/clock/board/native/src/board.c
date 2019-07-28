#include "board.h"
#include "esm/esm.h"
#include "debouncer.h"

NEC_t nec1 = {
	.timingBitBoundary = 1680,
	.timingAgcBoundary = 12500,
	.type = NEC_NOT_EXTENDED,
};

static bool armed = true;

void app_process(char key)
{
	if (armed)
	{
		switch (key)
		{
		case ' ':
		{
			armed = false;
			esm_signal_t sig = {
				.type = esm_sig_alarm,
				.sender = NULL,
				.receiver = debouncer_esm};
			esm_send_signal(&sig);
		}
		break;
		}
	}
}

static void debouncer_handle(void)
{
	esm_signal_t sig = {
		.type = esm_sig_button,
		.sender = NULL,
		.receiver = clock1_esm};
	esm_send_signal(&sig);
}

static void debouncer_arm(void)
{
	armed = true;
}

static const debouncer_cfg_t debouncer_cfg = {
	.period = 30UL,
	.handle = debouncer_handle,
	.arm = debouncer_arm};

ESM_REGISTER(debouncer, debouncer, esm_gr_none, 1, 1);

void board_nec_start(NEC_t *handle)
{
	handle->state = NEC_INIT;
}

void board_nec_stop(NEC_t *handle)
{
	(void)handle;
}
