#include "debouncer.h"

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
	debouncer_esm_t *self = ESM_CONTAINER_OF(esm, debouncer_esm_t, esm);

	switch(sig->type)
	{
	case esm_sig_alarm:
	    self->state = sig->params.debouncer.state;
		ESM_TRANSITION(active);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_active_entry(esm_t *const esm)
{
	debouncer_esm_t *self = ESM_CONTAINER_OF(esm, debouncer_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			self->cfg->period, &sig);
}

static void esm_active_exit(esm_t *const esm)
{
	debouncer_esm_t *self = ESM_CONTAINER_OF(esm, debouncer_esm_t, esm);
	self->cfg->arm(esm);
}

static void esm_active_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	debouncer_esm_t *self = ESM_CONTAINER_OF(esm, debouncer_esm_t, esm);

	switch(sig->type)
	{
	case esm_sig_tmout:
		self->cfg->handle(esm, self->state);
		ESM_TRANSITION(idle);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

void esm_debouncer_init(esm_t *const esm)
{
	ESM_TRANSITION(idle);
}
