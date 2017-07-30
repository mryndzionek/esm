#include "esm/esm.h"
#include "esm/esm_timer.h"

ESM_THIS_FILE;

typedef struct {
	const uint32_t delay;
} blink_cfg_t;

typedef struct {
	esm_t esm;
	esm_timer_t timer;
	blink_cfg_t const *const cfg;
} blink_esm_t;

#define ESM_INIT_SUB	(ESM_SIG_MASK(esm_sig_tmout))

ESM_DEFINE_STATE(on);
ESM_DEFINE_STATE(off);

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

}

static void esm_on_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(&esm_off_state);
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

}

static void esm_off_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(&esm_on_state);
		break;
	default:
		ESM_TRANSITION(&esm_unhandled_sig);
		break;
	}
}

static const blink_cfg_t blink_1_cfg = {
		.delay = 3000UL
};

static const blink_cfg_t blink_2_cfg = {
		.delay = 7000UL
};

static const blink_cfg_t blink_3_cfg = {
		.delay = 11000UL
};

ESM_REGISTER(blink, blink_1, off, 1);
ESM_REGISTER(blink, blink_2, off, 1);
ESM_REGISTER(blink, blink_3, off, 1);
