#include "esm/esm.h"
#include "esm/esm_timer.h"

ESM_THIS_FILE;

typedef struct {
	uint8_t num;
} philo_cfg_t;

typedef struct {
	esm_t esm;
	esm_timer_t timer;
	philo_cfg_t const *const cfg;
} philo_esm_t;

ESM_DEFINE_STATE(thinking);
ESM_DEFINE_STATE(hungry);
ESM_DEFINE_STATE(eating);

static void esm_thinking_entry(esm_t *const esm)
{
	philo_esm_t *self = ESM_CONTAINER_OF(esm, philo_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			1000UL + ESM_RANDOM(5000UL), &sig);
}

static void esm_thinking_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_thinking_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(hungry);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_hungry_entry(esm_t *const esm)
{
	philo_esm_t *self = ESM_CONTAINER_OF(esm, philo_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_hungry,
			.sender = esm,
			.receiver = table_esm,
			.params.num = self->cfg->num
	};
	esm_send_signal(&sig);
}

static void esm_hungry_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_hungry_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_eat:
		ESM_TRANSITION(eating);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_eating_entry(esm_t *const esm)
{
	philo_esm_t *self = ESM_CONTAINER_OF(esm, philo_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			ESM_RANDOM(10000UL), &sig);
}

static void esm_eating_exit(esm_t *const esm)
{
	philo_esm_t *self = ESM_CONTAINER_OF(esm, philo_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_done,
			.sender = esm,
			.receiver = table_esm,
			.params = self->cfg->num
	};
	esm_send_signal(&sig);
}

static void esm_eating_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(thinking);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_philo_init(esm_t *const esm)
{
	ESM_TRANSITION(thinking);
}

static const philo_cfg_t philo_1_cfg = {
		.num = 0
};

static const philo_cfg_t philo_2_cfg = {
		.num = 1
};

static const philo_cfg_t philo_3_cfg = {
		.num = 2
};

static const philo_cfg_t philo_4_cfg = {
		.num = 3
};

static const philo_cfg_t philo_5_cfg = {
		.num = 4
};

ESM_REGISTER(philo, philo_1, common, 1);
ESM_REGISTER(philo, philo_2, common, 1);
ESM_REGISTER(philo, philo_3, common, 1);
ESM_REGISTER(philo, philo_4, common, 1);
ESM_REGISTER(philo, philo_5, common, 1);
