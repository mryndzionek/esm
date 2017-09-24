#ifndef MODULES_INCLUDE_BUS_H_
#define MODULES_INCLUDE_BUS_H_

#include "esm/esm.h"
#include "esm/esm_queue.h"
#include "esm/esm_timer.h"

typedef struct {
	uint16_t timeout;
	esm_signal_e req;
	esm_signal_e rsp;
	esm_queue_t *queue;
	void (*request)(esm_signal_t *sig);
	void (*respond)(esm_signal_t *sig);
} bus_cfg_t;

typedef struct {
	esm_t esm;
	esm_timer_t timer;
	bus_cfg_t const *const cfg;
} bus_esm_t;

void esm_bus_init(esm_t *const esm);

#endif /* MODULES_INCLUDE_BUS_H_ */
