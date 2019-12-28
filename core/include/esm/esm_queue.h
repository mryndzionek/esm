#ifndef CORE_INCLUDE_ESM_ESM_QUEUE_H_
#define CORE_INCLUDE_ESM_ESM_QUEUE_H_

#include "esm/esm.h"

#define esm_queue_tail(_self) &(_self)->data[(_self)->tail]
#define esm_queue_head(_self) &(_self)->data[(_self)->head]

void esm_queue_push(esm_queue_t *self, esm_signal_t *sig);
void esm_queue_push_back(esm_queue_t *self, esm_signal_t *sig);
void esm_queue_pop(esm_queue_t *self);

#endif /* CORE_INCLUDE_ESM_ESM_QUEUE_H_ */
