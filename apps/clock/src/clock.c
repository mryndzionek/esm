#include "esm/hesm.h"
#include "esm/esm_timer.h"
#include "board.h"

ESM_THIS_FILE;

typedef struct {
	const uint32_t delay;
	const uint8_t led_num;
} clock_cfg_t;

typedef struct {
	hesm_t esm;
	esm_timer_t timer;
	clock_cfg_t const *const cfg;
} clock_esm_t;

ESM_COMPLEX_STATE(active, top, 1);
ESM_LEAF_STATE(paused, top, 1);

ESM_LEAF_STATE(on, active, 2);
ESM_LEAF_STATE(off, active, 2);

static void esm_active_init(esm_t *const esm)
{
	(void)esm;
	ESM_TRANSITION(on);
}

static void esm_active_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_active_exit(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	esm_timer_rm(&self->timer);
}

static void esm_active_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	(void)esm;

	switch(sig->type)
	{
	case esm_sig_button:
		ESM_TRANSITION(paused);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_on_entry(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			self->cfg->delay>>3, &sig);

	BOARD_LED_ON(self->cfg->led_num);
}

static void esm_on_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_on_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(off);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_off_entry(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			self->cfg->delay, &sig);
	BOARD_LED_OFF(self->cfg->led_num);
}

static void esm_off_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_off_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(on);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_paused_entry(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	BOARD_LED_OFF(self->cfg->led_num);
}

static void esm_paused_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_paused_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	switch(sig->type)
	{
	case esm_sig_button:
		ESM_TRANSITION(active);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_clock_init(esm_t *const esm)
{
	ESM_TRANSITION(active);
}

static const clock_cfg_t clock1_cfg = {
		.delay = 300UL,
		.led_num = 0
};

ESM_COMPLEX_REGISTER(clock, clock1, esm_gr_clocks, 2, 3, 0);
