#ifndef INCLUDE_KEYBOARD_H_
#define INCLUDE_KEYBOARD_H_

#include <stdint.h>

typedef enum
{
    key_ev_down = 0,
    key_ev_tap,
    key_ev_up
} key_ev_type_e;

typedef struct
{
    int layer;
} keyboard_state_t;

// this state is shared to enable keycode lookup (via below function)
// so that some actions can be determined according to keycode
// instead of coordinates in matrix
extern const keyboard_state_t *const keyboard_state;

uint16_t keyboard_get_kc(uint8_t col, uint8_t row, const keyboard_state_t *const kbd);

#endif /* INCLUDE_KEYBOARD_H_ */
