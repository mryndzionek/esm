#include "synchronizer.h"

ESM_THIS_FILE;

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
	synchronizer_esm_t *self = ESM_CONTAINER_OF(esm, synchronizer_esm_t, esm);

	if(self->cfg->req == sig->type)
	{
		self->cfg->request(sig);
		ESM_TRANSITION(busy);
	}
	else
	{
		ESM_TRANSITION(unhandled);
	}
}

static void esm_busy_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_busy_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_busy_handle(esm_t *const esm, esm_signal_t *sig)
{
	synchronizer_esm_t *self = ESM_CONTAINER_OF(esm, synchronizer_esm_t, esm);

	if(self->cfg->rsp == sig->type)
	{
		self->cfg->respond(sig);
		ESM_TRANSITION(idle);
	}
	else if(self->cfg->req == sig->type)
	{
		esm_queue_push(self->cfg->queue, sig);
	}
	else
	{
		ESM_TRANSITION(unhandled);
	}
}

void esm_synchronizer_init(esm_t *const esm)
{
	ESM_TRANSITION(idle);
}
