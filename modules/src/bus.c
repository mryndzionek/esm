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

static void esm_idle_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	bus_esm_t *self = ESM_CONTAINER_OF(esm, bus_esm_t, esm);

	switch(sig->type)
	{
	case esm_sig_bus_req:
		self->xfer = sig->params.xfer;
		esm_list_insert(self->cfg->queue, &self->xfer->item, NULL);
		self->xfer->exec(self->xfer);
		ESM_TRANSITION(busy);
		break;

	default:
		ESM_TRANSITION(unhandled);
		break;
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

static void esm_busy_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	bus_esm_t *self = ESM_CONTAINER_OF(esm, bus_esm_t, esm);

	switch(sig->type)
	{
	case esm_sig_bus_rsp:
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
	break;

	case esm_sig_bus_req:
		esm_list_insert(self->cfg->queue, &sig->params.xfer->item, NULL);
		break;

	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

void esm_bus_init(esm_t *const esm)
{
	ESM_TRANSITION(idle);
}
