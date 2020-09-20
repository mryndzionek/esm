
#include "config.h"
#include "keycode.h"
#include "keycode_extra.h"
#include "state_handler.h"
#include "backlight.h"

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

static const backlight_cfg_t backlight_cfg = {
    .freq_hz = 200UL};

ESM_REGISTER(backlight, backlight, esm_gr_none, 4, 0);

const state_handler_cfg_t state_handler_cfg = {
    .cfgs = {}};
