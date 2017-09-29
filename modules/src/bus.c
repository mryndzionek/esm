#include "bus.h"

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
	bus_esm_t *self = ESM_CONTAINER_OF(esm, bus_esm_t, esm);

	if(self->cfg->req == sig->type)
	{
		self->xfer = sig->params.xfer;
		esm_list_insert(self->cfg->queue, &self->xfer->item, NULL);
		self->xfer->exec(self->xfer);
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
	bus_esm_t *self = ESM_CONTAINER_OF(esm, bus_esm_t, esm);

	if(self->cfg->rsp == sig->type)
	{
		esm_signal_t s = {
				.type = sig->type,
				.sender = sig->sender,
				.receiver = self->xfer->receiver,
		};
		esm_send_signal(&s);
		esm_list_erase(self->cfg->queue, &self->xfer->item);
		if(esm_list_empty(self->cfg->queue))
		{
			ESM_TRANSITION(idle);
		}
		else
		{
			self->xfer = ESM_CONTAINER_OF(
					esm_list_begin(self->cfg->queue), bus_xfer_t, item);
			self->xfer->exec(self->xfer);
		}
	}
	else if(self->cfg->req == sig->type)
	{
		esm_list_insert(self->cfg->queue, &sig->params.xfer->item, NULL);
	}
	else
	{
		ESM_TRANSITION(unhandled);
	}
}

void esm_bus_init(esm_t *const esm)
{
	ESM_TRANSITION(idle);
}
