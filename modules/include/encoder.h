#ifndef MODULES_INCLUDE_ENCODER_H_
#define MODULES_INCLUDE_ENCODER_H_

#include "esm/esm.h"
#include "esm/esm_timer.h"

typedef struct {

} encoder_cfg_t;

typedef struct {
	esm_t esm;
	esm_timer_t timer;
	encoder_cfg_t const *const cfg;
} encoder_esm_t;

void esm_encoder_init(esm_t *const esm);

#endif /* MODULES_INCLUDE_ENCODER_H_ */
