#ifndef CORE_INCLUDE_ESM_ESM_PRIVATE_H_
#define CORE_INCLUDE_ESM_ESM_PRIVATE_H_

#include "esm/esm.h"

struct _esm {
	char const *const name;
	const uint8_t id;
	const bool is_cplx;
	const uint8_t group;
	void (*init)(esm_t *const esm);
	esm_state_t const *curr_state;
	esm_state_t const *next_state;
	esm_queue_t queue;
};

#endif /* CORE_INCLUDE_ESM_ESM_PRIVATE_H_ */
