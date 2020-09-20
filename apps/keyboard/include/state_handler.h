#ifndef MODULES_INCLUDE_STATE_HANDLER_H_
#define MODULES_INCLUDE_STATE_HANDLER_H_

#include "esm/esm.h"
#include "esm/esm_timer.h"

typedef struct
{
    int layer;
} keyboard_state_t;

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
} state_handler_cfg_t;

#endif /* MODULES_INCLUDE_STATE_HANDLER_H_ */