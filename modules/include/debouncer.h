#ifndef MODULES_INCLUDE_DEBOUNCER_H_
#define MODULES_INCLUDE_DEBOUNCER_H_

#include "esm/esm.h"
#include "esm/esm_timer.h"

typedef struct {
	uint8_t period;
	void (*handle)(void);
	void (*arm)(void);
} debouncer_cfg_t;

typedef struct {
	esm_t esm;
	esm_timer_t timer;
	debouncer_cfg_t const *const cfg;
} debouncer_esm_t;

void esm_debouncer_init(esm_t *const esm);

#endif /* MODULES_INCLUDE_DEBOUNCER_H_ */
