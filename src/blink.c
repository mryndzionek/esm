#include "esm/esm.h"
#include "esm/esm_timer.h"

#define BLINK_DELAY_MS	(1000UL)

ESM_THIS_FILE;

typedef struct {
	esm_t esm;
	esm_timer_t timer;
	uint8_t state;
} blink_esm_t;

#define ESM_INIT_SUB	(ESM_SIG_MASK(sig_tmout))

ESM_DEFINE_STATE(on);
ESM_DEFINE_STATE(off);

static void esm_on_entry(esm_t *const esm)
{
	blink_esm_t *self = ESM_CONTAINER_OF(esm, blink_esm_t, esm);
	signal_t sig = {
			.type = sig_tmout
	};
	esm_timer_add(&self->timer, BLINK_DELAY_MS, &sig);
}

static void esm_on_exit(esm_t *const esm)
{

}

static void esm_on_handle(esm_t *const esm, signal_t *sig)
{
	switch(sig->type)
	{
	case sig_tmout:
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
	signal_t sig = {
			.type = sig_tmout
	};
	esm_timer_add(&self->timer, BLINK_DELAY_MS, &sig);
}

static void esm_off_exit(esm_t *const esm)
{

}

static void esm_off_handle(esm_t *const esm, signal_t *sig)
{
	switch(sig->type)
	{
	case sig_tmout:
		ESM_TRANSITION(&esm_on_state);
		break;
	default:
		ESM_TRANSITION(&esm_unhandled_sig);
		break;
	}
}

ESM_REGISTER(blink, blink_1, off, 4);
ESM_REGISTER(blink, blink_2, off, 4);
