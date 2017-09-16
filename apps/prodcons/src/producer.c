#include "esm/esm.h"
#include "esm/esm_timer.h"

#define MIN_DELAY_MS	(1000UL)
#define MAX_DELAY_MS	(5000UL)

ESM_THIS_FILE;

typedef struct {
	uint8_t num;
} producer_cfg_t;

typedef struct {
	esm_t esm;
	esm_timer_t timer;
	producer_cfg_t const *const cfg;
} producer_esm_t;

ESM_DEFINE_STATE(idle);
ESM_DEFINE_STATE(busy);

static void esm_idle_entry(esm_t *const esm)
{
	producer_esm_t *self = ESM_CONTAINER_OF(esm, producer_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			MIN_DELAY_MS + ESM_RANDOM(MAX_DELAY_MS - MIN_DELAY_MS), &sig);
}

static void esm_idle_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_idle_handle(esm_t *const esm, esm_signal_t *sig)
{
	(void)esm;

	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(busy);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_busy_entry(esm_t *const esm)
{
	esm_signal_t sig = {
			.type = esm_sig_request,
			.sender = esm,
			.receiver = synchronizer_esm
	};
	esm_send_signal(&sig);
}

static void esm_busy_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_busy_handle(esm_t *const esm, esm_signal_t *sig)
{
	(void)esm;

	switch(sig->type)
	{
	case esm_sig_response:
		ESM_TRANSITION(idle);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_producer_init(esm_t *const esm)
{
	ESM_TRANSITION(idle);
}

static const producer_cfg_t producer_1_cfg = {
		.num = 0
};

ESM_REGISTER(producer, producer_1, esm_gr_none, 1);

static const producer_cfg_t producer_2_cfg = {
		.num = 1
};

ESM_REGISTER(producer, producer_2, esm_gr_none, 1);
