#include "esm/esm.h"
#include "board.h"

ESM_THIS_FILE;

typedef struct {
} debug_cfg_t;

typedef struct {
	esm_t esm;
	char bins[DCF77_BIN_SIZE + 2]; // +2 is for line end, see 'esm_debug_init'
	uint8_t curr_bin;
	debug_cfg_t const *const cfg;
} debug_esm_t;

ESM_DEFINE_STATE(idle);

static void esm_idle_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_idle_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_idle_handle(esm_t *const esm, esm_signal_t *sig)
{
	debug_esm_t *self = ESM_CONTAINER_OF(esm, debug_esm_t, esm);

	switch(sig->type)
	{
	case esm_sig_alarm:
	{
		if(sig->params.bin == 0)
		{
			if((self->curr_bin % DCF77_BIN_EVERY) == 0)
			{
				self->bins[self->curr_bin] = '+';
			}
			else
			{
				self->bins[self->curr_bin] = '-';
			}
		}
		else if(sig->params.bin == DCF77_BIN_EVERY)
		{
			self->bins[self->curr_bin] = 'X';
		}
		else
		{
			self->bins[self->curr_bin] = '0' + sig->params.bin;
		}
		if(++self->curr_bin == DCF77_BIN_SIZE)
		{
			HAL_StatusTypeDef r = HAL_UART_Transmit_IT(&huart2, (uint8_t *)self->bins, sizeof(self->bins));
			ESM_ASSERT(r == HAL_OK);
			self->curr_bin = 0;
		}
	}
	break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_debug_init(esm_t *const esm)
{
	debug_esm_t *self = ESM_CONTAINER_OF(esm, debug_esm_t, esm);
	self->bins[DCF77_BIN_SIZE] = '\r';
	self->bins[DCF77_BIN_SIZE + 1] = '\n';
	ESM_TRANSITION(idle);
}

static const debug_cfg_t debug_cfg = {
};

ESM_REGISTER(debug, debug, esm_gr_none, 1);
