#ifndef MODULES_INCLUDE_TAP_DETECTOR_H_
#define MODULES_INCLUDE_TAP_DETECTOR_H_

#include "esm/esm.h"
#include "esm/esm_timer.h"

typedef struct
{
    uint32_t tap_tres;
    uint16_t code;
    uint16_t tap_code;
    uint16_t press_code;    
} tap_cfg_t;

typedef struct
{
    tap_cfg_t cfgs[NUM_TAPS];
} tap_detector_cfg_t;

typedef struct
{
    int layer;
} keyboard_state_t;

typedef struct
{
    esm_timer_t timer;
    uint8_t col;
    uint8_t row;
    bool is_active;
} tap_state_t;

typedef struct
{
    esm_t esm;
    tap_state_t tap_s[NUM_TAPS];
    keyboard_state_t kbd_s;
    tap_detector_cfg_t const *const cfg;
} tap_detector_esm_t;

void esm_tap_detector_init(esm_t *const esm);

#endif /* MODULES_INCLUDE_TAP_DETECTOR_H_ */