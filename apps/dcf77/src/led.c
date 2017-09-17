#include "esm/esm.h"
#include "esm/esm_timer.h"
#include "board.h"

#define DCF77_ZERO_DELAY_MS	(50UL)
#define DCF77_ONE_DELAY_MS	(200UL)
#define DCF77_SYNC_DELAY_MS	(500UL)

ESM_THIS_FILE;

typedef struct {
} led_cfg_t;

typedef struct {
	esm_t esm;
	esm_timer_t timer;
	led_cfg_t const *const cfg;
} led_esm_t;

ESM_DEFINE_STATE(idle);
ESM_DEFINE_STATE(busy);

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
	led_esm_t *self = ESM_CONTAINER_OF(esm, led_esm_t, esm);

	switch(sig->type)
	{
	case esm_sig_dcf77_data:
	{
		esm_signal_t _sig = {
				.type = esm_sig_tmout,
				.sender = esm,
				.receiver = esm
		};

		switch(sig->params.dcf77_data.data)
		{
		case dcf77_zero:
			esm_timer_add(&self->timer,
					DCF77_ZERO_DELAY_MS, &_sig);
			ESM_TRANSITION(busy);
			break;

		case dcf77_one:
			esm_timer_add(&self->timer,
					DCF77_ONE_DELAY_MS, &_sig);
			ESM_TRANSITION(busy);
			break;

		case dcf77_sync_mark:
			esm_timer_add(&self->timer,
					DCF77_SYNC_DELAY_MS, &_sig);
			ESM_TRANSITION(busy);
			break;
		default:
			break;
		}
	}
	break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_busy_entry(esm_t *const esm)
{
	(void)esm;
	BOARD_LED_FILTERED_ON();
}

static void esm_busy_exit(esm_t *const esm)
{
	(void)esm;
	BOARD_LED_FILTERED_OFF();
}

static void esm_busy_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
	{
		ESM_TRANSITION(idle);
	}
	break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}
static void esm_led_init(esm_t *const esm)
{
	ESM_TRANSITION(idle);
}

static const led_cfg_t led_cfg = {
};

ESM_REGISTER(led, led, esm_gr_none, 1);
