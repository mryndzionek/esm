#include "esm/esm.h"
#include "esm/esm_timer.h"

#define N_SMOKERS		(3)

ESM_THIS_FILE;

typedef struct {
} table_cfg_t;

typedef struct {
	esm_t esm;
	esm_timer_t timer;
    esm_t *philo[N_SMOKERS];
    uint8_t fork[N_SMOKERS];
    uint8_t is_hungry[N_SMOKERS];
	table_cfg_t const *const cfg;
} table_esm_t;

ESM_DEFINE_STATE(idle);
ESM_DEFINE_STATE(requesting);

static const char *const resource2str[] = {
		"tobacco",
		"paper",
		"matches"
};

static void esm_idle_entry(esm_t *const esm)
{
	table_esm_t *self = ESM_CONTAINER_OF(esm, table_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			1000UL + ESM_RANDOM(5000UL), &sig);
}

static void esm_idle_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_idle_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(requesting);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_requesting_entry(esm_t *const esm)
{
	smoker_resource_e resource_1 = ESM_RANDOM(N_SMOKERS);
	smoker_resource_e resource_2 = (resource_1 + ESM_RANDOM(2) + 1) % N_SMOKERS;

	ESM_ASSERT(resource_1 <= matches);
	ESM_ASSERT(resource_2 <= matches);
	ESM_ASSERT(resource_1 != resource_2);

	ESM_PRINTF("[%010u] [%s] Requesting '%s' and '%s'\r\n",
			esm_global_time,
			esm->name,
			resource2str[resource_1],
			resource2str[resource_2]);

	esm_signal_t sig = {
			.type = esm_sig_request,
			.sender = esm,
			.receiver = (void *)0,
			.params.resource = resource_1
	};

	esm_broadcast_signal(&sig);

	sig.params.resource = resource_2;
	esm_broadcast_signal(&sig);
}

static void esm_requesting_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_requesting_handle(esm_t *const esm, esm_signal_t *sig)
{
	switch(sig->type)
	{
	case esm_sig_done:
		ESM_TRANSITION(idle);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static const table_cfg_t table_cfg = {
};

ESM_REGISTER(table, table, idle, N_SMOKERS);
