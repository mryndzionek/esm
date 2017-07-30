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

#define ESM_INIT_SUB ( \
		ESM_SIG_MASK(esm_sig_tmout) | \
		ESM_SIG_MASK(esm_sig_eat) \
)

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
			ESM_RANDOM(3000UL), &sig);
}

static void esm_thinking_exit(esm_t *const esm)
{

}

static void esm_thinking_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(&esm_hungry_state);
		break;
	default:
		ESM_TRANSITION(&esm_unhandled_sig);
		break;
	}
}

static void esm_hungry_entry(esm_t *const esm)
{
	philo_esm_t *self = ESM_CONTAINER_OF(esm, philo_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_hungry,
			.sender = esm,
			.receiver = (void *)0,
			.params = self->cfg->num
	};
	esm_send_signal(&sig);
}

static void esm_hungry_exit(esm_t *const esm)
{

}

static void esm_hungry_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_eat:
		ESM_TRANSITION(&esm_eating_state);
		break;
	default:
		ESM_TRANSITION(&esm_unhandled_sig);
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
			.receiver = (void *)0,
			.params = self->cfg->num
	};
	esm_send_signal(&sig);
}

static void esm_eating_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(&esm_thinking_state);
		break;
	default:
		ESM_TRANSITION(&esm_unhandled_sig);
		break;
	}
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

ESM_REGISTER(philo, philo_1, thinking, 1);
ESM_REGISTER(philo, philo_2, thinking, 1);
ESM_REGISTER(philo, philo_3, thinking, 1);
ESM_REGISTER(philo, philo_4, thinking, 1);
ESM_REGISTER(philo, philo_5, thinking, 1);
