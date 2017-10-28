#include "esm/esm_queue.h"

ESM_THIS_FILE;

void esm_queue_push(esm_queue_t *self, esm_signal_t *sig)
{
	ESM_ASSERT(self->len < self->size);

	self->data[self->head++] = *sig;
	if(self->head == self->size)
	{
		self->head = 0;
	}
	++self->len;
}
