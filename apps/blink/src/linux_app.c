#include "esm/esm.h"

void app_process(char key)
{
	switch(key)
	{
	case ' ':
	{
		esm_signal_t sig = {
				.type = esm_sig_button,
				.sender = NULL,
				.receiver = blink_esm
		};
		esm_send_signal(&sig);
	}
	break;
	}
}
