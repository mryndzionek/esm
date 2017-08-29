#include "esm/hesm.h"
#include "esm/esm_timer.h"

ESM_THIS_FILE;

typedef struct {
	const uint32_t delay;
} blink_cfg_t;

typedef struct {
	hesm_t esm;
	esm_timer_t timer;
	blink_cfg_t const *const cfg;
} blink_esm_t;

#define ESM_INIT_SUB	(ESM_SIG_MASK(esm_sig_tmout) | \
      ESM_SIG_MASK(esm_sig_pause))

ESM_COMPLEX_STATE(active, top);
ESM_LEAF_STATE(on, active);
ESM_LEAF_STATE(off, active);
ESM_LEAF_STATE(paused, top);

static void esm_active_init(esm_t *const esm)
{
	(void)esm;
}

static void esm_active_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_active_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_active_handle(esm_t *const esm, esm_signal_t *sig)
{
	(void)esm;
	(void)sig;
}

static void esm_on_entry(esm_t *const esm)
{
	blink_esm_t *self = ESM_CONTAINER_OF(esm, blink_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			self->cfg->delay, &sig);
}

static void esm_on_exit(esm_t *const esm)
{
	blink_esm_t *self = ESM_CONTAINER_OF(esm, blink_esm_t, esm);
	esm_timer_rm(&self->timer);
}

static void esm_on_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(&esm_off_state);
		break;
	case esm_sig_pause:
		ESM_TRANSITION(&esm_paused_state);
		break;
	default:
		ESM_TRANSITION(&esm_unhandled_sig);
		break;
	}
}

static void esm_off_entry(esm_t *const esm)
{
	blink_esm_t *self = ESM_CONTAINER_OF(esm, blink_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			self->cfg->delay, &sig);
}

static void esm_off_exit(esm_t *const esm)
{
	blink_esm_t *self = ESM_CONTAINER_OF(esm, blink_esm_t, esm);
	esm_timer_rm(&self->timer);
}

static void esm_off_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(&esm_on_state);
		break;
	case esm_sig_pause:
		ESM_TRANSITION(&esm_paused_state);
		break;
	default:
		ESM_TRANSITION(&esm_unhandled_sig);
		break;
	}
}

static void esm_paused_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_paused_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_paused_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_pause:
		ESM_TRANSITION(&esm_on_state);
		break;
	default:
		ESM_TRANSITION(&esm_unhandled_sig);
		break;
	}
}

static const blink_cfg_t blink_cfg = {
		.delay = 3000UL
};

ESM_COMPLEX_REGISTER(blink, blink, active, 1);
