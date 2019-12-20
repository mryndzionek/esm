#ifndef MODULES_INCLUDE_TAP_DETECTOR_H_
#define MODULES_INCLUDE_TAP_DETECTOR_H_

#include "esm/esm.h"
#include "esm/esm_timer.h"

typedef struct
{
    uint32_t tap_tres;
} tap_detector_cfg_t;

typedef struct
{
    esm_t esm;
    esm_timer_t timer;
    esm_sig_params_t params;
    tap_detector_cfg_t const *const cfg;
} tap_detector_esm_t;

void esm_tap_detector_init(esm_t *const esm);

#endif /* MODULES_INCLUDE_TAP_DETECTOR_H_ */