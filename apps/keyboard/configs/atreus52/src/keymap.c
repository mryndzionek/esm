
#include "config.h"
#include "keycode.h"
#include "keycode_extra.h"
#include "state_handler.h"

#include <stdint.h>
#include <stdbool.h>

#define _QWERTY (0)
#define _LOWER (1)
#define _RAISE (2)
#define _MOVEMENT (3)

enum custom_keycodes
{
  LOWER = SAFE_RANGE,
  RAISE,
  MOVEMENT,
  LCTL_ESC
};

#define LAYOUT( \
  k00, k01, k02, k03, k04, k05,           k08, k09, k0a, k0b, k0c, k0d, \
  k10, k11, k12, k13, k14, k15,           k18, k19, k1a, k1b, k1c, k1d, \
  k20, k21, k22, k23, k24, k25, k26, k27, k28, k29, k2a, k2b, k2c, k2d, \
  k30, k31, k32, k33, k34, k35, k36, k37, k38, k39, k3a, k3b, k3c, k3d \
) \
{ \
	{ k00, k01, k02, k03, k04, k05, KC_NO, KC_NO, k08, k09, k0a, k0b, k0c, k0d }, \
	{ k10, k11, k12, k13, k14, k15, KC_NO, KC_NO, k18, k19, k1a, k1b, k1c, k1d }, \
	{ k20, k21, k22, k23, k24, k25, k26,   k27,   k28, k29, k2a, k2b, k2c, k2d }, \
	{ k30, k31, k32, k33, k34, k35, k36,   k37,   k38, k39, k3a, k3b, k3c, k3d } \
}

const uint16_t keymaps[N_LAYERS][N_ROWS][N_COLS] = {
  [_QWERTY] = LAYOUT(
    KC_TAB,   KC_Q,    KC_W,    KC_E,      KC_R,    KC_T,                      KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS,
    LCTL_ESC, KC_A,    KC_S,    KC_D,      KC_F,    KC_G,                      KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
    KC_LSFT,  KC_Z,    KC_X,    KC_C,      KC_V,    KC_B,   MOVEMENT, KC_RSFT, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENTER,
    KC_GRV,   KC_LCTL, KC_LALT, KC_LGUI,   LOWER,   KC_SPC, KC_LCTL,  KC_LGUI, KC_BSPC, RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
  ),

  [_LOWER] = LAYOUT(
    KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                   KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DEL,
    KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                     KC_F6,   KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE,
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  _______, _______, KC_F12,  KC_END,  _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY
  ),

  [_RAISE] = LAYOUT(
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_DEL,
    KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                     KC_F6,   KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS,
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  _______, _______, KC_F12,  KC_NUHS, KC_NUBS, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY
  ),

  [_MOVEMENT] = LAYOUT(
    KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                   KC_CIRC, KC_AMPR, KC_UP,   KC_LPRN, KC_RPRN, KC_DEL,
    KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                     KC_F6,   KC_LEFT, KC_DOWN, KC_RGHT, KC_RCBR, KC_PIPE,
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  _______, _______, KC_F12,  KC_END,  _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, KC_PGDN, KC_PGUP, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY
  ),

};

uint16_t process_key_user(uint16_t keycode, mat_ev_type_e ev, keyboard_state_t *const kbd)
{
  switch (keycode)
  {
  case LOWER:
    if (ev != mat_ev_up)
    {
      kbd->layer = _LOWER;
    }
    else
    {
      kbd->layer = _QWERTY;
    }
    break;

  case RAISE:
    if (ev != mat_ev_up)
    {
      kbd->layer = _RAISE;
    }
    else
    {
      kbd->layer = _QWERTY;
    }
    break;

  case MOVEMENT:
    if (ev != mat_ev_up)
    {
      kbd->layer = _MOVEMENT;
    }
    else
    {
      kbd->layer = _QWERTY;
    }
    break;
  }

  return keycode;
}

const state_handler_cfg_t state_handler_cfg = {
    .cfgs = {
        {.tap_tres = TAP_TRES_MS, .code = LCTL_ESC, .tap_code = KC_ESC, .press_code = KC_LCTL}}};
