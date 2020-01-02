
#include "config.h"
#include "keycode.h"
#include "keycode_extra.h"
#include "keyboard.h"
#include "tap_detector.h"

#include <stdint.h>
#include <stdbool.h>

#define _QWERTY (0)

#define LAYOUT( \
  k00, k01, k02, \
  k10, k11, k12, \
  k20, k21, k22  \
) \
{ \
	{ k00, k01, k02 }, \
	{ k10, k11, k12 }, \
	{ k20, k21, k22 }  \
}

const uint16_t keymaps[N_LAYERS][N_ROWS][N_COLS] = {
  [_QWERTY] = LAYOUT(
    KC_Q,    KC_W,    KC_E,
    KC_A,    KC_S,    KC_D,
    KC_Z,    KC_X,    KC_C
  ),
};