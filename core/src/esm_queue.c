#include "esm/esm_queue.h"

ESM_THIS_FILE;

void esm_queue_push(esm_queue_t *self, esm_signal_t *sig)
{
	if(self->len)
	{
		ESM_ASSERT_MSG(self->head != self->tail,
				"Event queue for %s overrun\r\n", sig->receiver->cfg->name);
	}

	self->data[self->head++] = *sig;
	if(self->head == self->size)
	{
		self->head = 0;
	}
	++self->len;
}

void esm_queue_pop(esm_queue_t *self)
{
	if(++self->tail == self->size)
	{
		self->tail = 0;
	}
	--self->len;
}
