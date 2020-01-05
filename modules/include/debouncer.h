#ifndef MODULES_INCLUDE_DEBOUNCER_H_
#define MODULES_INCLUDE_DEBOUNCER_H_

#include "esm/esm.h"
#include "esm/esm_timer.h"

#include "board.h"

typedef struct {
	uint8_t period;
	void (*handle)(esm_t *const esm, BOARD_DEBOUNCER_STATE state);
	void (*arm)(esm_t *const esm);
} debouncer_cfg_t;

typedef struct {
	esm_t esm;
	esm_timer_t timer;
	BOARD_DEBOUNCER_STATE state;
	debouncer_cfg_t const *const cfg;
} debouncer_esm_t;

void esm_debouncer_init(esm_t *const esm);

#endif /* MODULES_INCLUDE_DEBOUNCER_H_ */
