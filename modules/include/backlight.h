#ifndef MODULES_INCLUDE_BACKLIGHT_H_
#define MODULES_INCLUDE_BACKLIGHT_H_

#include "esm/esm.h"
#include "esm/esm_timer.h"

typedef struct
{
    uint16_t freq_hz;
} backlight_cfg_t;

typedef struct
{
    esm_t esm;
    esm_timer_t timer;
    uint8_t i;
    backlight_cfg_t const *const cfg;
} backlight_esm_t;

void esm_backlight_init(esm_t *const esm);

#endif /* MODULES_INCLUDE_BACKLIGHT_H_ */
