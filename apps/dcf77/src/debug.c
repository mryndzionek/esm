#include "esm/esm.h"
#include "board.h"

ESM_THIS_FILE;

typedef struct {
} debug_cfg_t;

typedef struct {
	esm_t esm;
	char bins[DCF77_BIN_SIZE + 2]; // +2 is for line end, see 'esm_debug_init'
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
		char *bin = &self->bins[sig->params.tick.tick];

		if(sig->params.tick.bin == 0)
		{
			*bin = ((sig->params.tick.tick % DCF77_BIN_EVERY) == 0) ? '+' : '-';
		}
		else if(sig->params.tick.bin == DCF77_BIN_EVERY)
		{
			*bin = 'X';
		}
		else
		{
			*bin = '0' + sig->params.tick.bin;
		}
		if(++sig->params.tick.tick == DCF77_BIN_SIZE)
		{
			BOARD_DEBUG_OUT();
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
