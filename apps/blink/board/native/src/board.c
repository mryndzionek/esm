#include "board.h"
#include "esm/esm.h"
#include "debouncer.h"

static bool armed = true;

void app_process(char key)
{
	if(armed)
	{
		switch(key)
		{
		case ' ':
		{
			armed = false;
			esm_signal_t sig = {
					.type = esm_sig_alarm,
					.sender = NULL,
					.receiver = debouncer_esm
			};
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
			.receiver = NULL
	};
	esm_broadcast_signal(&sig, esm_gr_blinkers);
}

static void debouncer_arm(void)
{
	armed = true;
}

static const debouncer_cfg_t debouncer_cfg = {
		.period = 30UL,
		.handle = debouncer_handle,
		.arm = debouncer_arm
};

ESM_REGISTER(debouncer, debouncer, esm_gr_none, 1, 1);
