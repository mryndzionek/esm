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
				.receiver = pelican_esm,
				.params.key = key
		};
		esm_send_signal(&sig);
	}
	break;
	}
}
