#include "encoder.h"

ESM_THIS_FILE;

ESM_DEFINE_STATE(idle);
ESM_DEFINE_STATE(active);

static void esm_idle_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_idle_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_idle_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	(void)esm;

	switch(sig->type)
	{
	case esm_sig_alarm:
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_active_entry(esm_t *const esm)
{
	encoder_esm_t *self = ESM_CONTAINER_OF(esm, encoder_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			100, &sig);
}

static void esm_active_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_active_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	(void)esm;

	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(idle);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

void esm_encoder_init(esm_t *const esm)
{
	ESM_TRANSITION(idle);
}
