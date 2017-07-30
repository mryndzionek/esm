#include "esm/esm.h"
#include "esm/esm_timer.h"

ESM_THIS_FILE;

typedef struct {
	smoker_resource_e resource;
} smoker_cfg_t;

typedef struct {
	esm_t esm;
	esm_timer_t timer;
	uint8_t has;
	smoker_cfg_t const *const cfg;
} smoker_esm_t;

#define ESM_INIT_SUB ( \
      ESM_SIG_MASK(esm_sig_tmout) | \
      ESM_SIG_MASK(esm_sig_request) | \
      ESM_SIG_MASK(esm_sig_done) \
)

ESM_DEFINE_STATE(idle);
ESM_DEFINE_STATE(smoking);

static void esm_idle_entry(esm_t *const esm)
{
	smoker_esm_t *self = ESM_CONTAINER_OF(esm, smoker_esm_t, esm);
	self->has = 1UL << self->cfg->resource;
}

static void esm_idle_exit(esm_t *const esm)
{
	smoker_esm_t *self = ESM_CONTAINER_OF(esm, smoker_esm_t, esm);
}

static void esm_idle_handle(esm_t *const esm, esm_signal_t *sig)
{
	smoker_esm_t *self = ESM_CONTAINER_OF(esm, smoker_esm_t, esm);

	switch(sig->type)
	{
	case esm_sig_request:
		self->has |= 1UL << sig->params.resource;
		if((self->has & 0x07) == 0x07)
		{
			ESM_TRANSITION(&esm_smoking_state);
		}
		break;
	case esm_sig_done:
		ESM_TRANSITION(&esm_self_transition);
		break;
	default:
		ESM_TRANSITION(&esm_unhandled_sig);
		break;
	}
}

static void esm_smoking_entry(esm_t *const esm)
{
	smoker_esm_t *self = ESM_CONTAINER_OF(esm, smoker_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			1000UL + ESM_RANDOM(5000UL), &sig);
}

static void esm_smoking_exit(esm_t *const esm)
{
	smoker_esm_t *self = ESM_CONTAINER_OF(esm, smoker_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_done,
			.sender = esm,
			.receiver = (void *)0,
	};
	esm_send_signal(&sig);
}

static void esm_smoking_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(&esm_idle_state);
		break;
	default:
		ESM_TRANSITION(&esm_unhandled_sig);
		break;
	}
}

static const smoker_cfg_t tobacco_smoker_cfg = {
		.resource = tobacco
};

static const smoker_cfg_t paper_smoker_cfg = {
		.resource = paper
};

static const smoker_cfg_t matches_smoker_cfg = {
		.resource = matches
};

ESM_REGISTER(smoker, tobacco_smoker, idle, 2);
ESM_REGISTER(smoker, paper_smoker, idle, 2);
ESM_REGISTER(smoker, matches_smoker, idle, 2);
