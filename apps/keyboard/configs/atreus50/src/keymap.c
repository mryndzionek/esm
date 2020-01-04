
#include "config.h"
#include "keycode.h"
#include "keycode_extra.h"
#include "keyboard.h"
#include "tap_detector.h"

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
  LCTL_ESC,
  LSFT_ENTER
};

#define LAYOUT( \
  k00, k01, k02, k03, k04, k05,           k06, k07, k08, k09, k0a, k0b, \
  k10, k11, k12, k13, k14, k15,           k16, k17, k18, k19, k1a, k1b, \
  k20, k21, k22, k23, k24, k25,           k26, k27, k28, k29, k2a, k2b, \
  k30, k31, k32, k33, k34, k35, km0, km1, k36, k37, k38, k39, k3a, k3b \
) \
{ \
	{ k00, k01, k02, k03, k04, k05, KC_NO, k06, k07, k08, k09, k0a, k0b }, \
	{ k10, k11, k12, k13, k14, k15, KC_NO, k16, k17, k18, k19, k1a, k1b }, \
	{ k20, k21, k22, k23, k24, k25, km1,   k26, k27, k28, k29, k2a, k2b }, \
	{ k30, k31, k32, k33, k34, k35, km0,   k36, k37, k38, k39, k3a, k3b } \
}

const uint16_t keymaps[N_LAYERS][N_ROWS][N_COLS] = {
  [_QWERTY] = LAYOUT(
    KC_TAB,   KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                      KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS,
    LCTL_ESC, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                      KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
    KC_RALT,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                      KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, LSFT_ENTER,
    KC_GRV,   KC_LCTL, KC_LALT, KC_LGUI, LOWER,   KC_SPC, MOVEMENT, KC_RSFT, KC_BSPC, RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
  ),

  [_LOWER] = LAYOUT(
    KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                   KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DEL,
    KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                     KC_F6,   KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE,
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,                    KC_F12,  KC_END,  _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY
  ),

  [_RAISE] = LAYOUT(
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_DEL,
    KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                     KC_F6,   KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS,
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,                    KC_F12,  KC_NUHS, KC_NUBS, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY
  ),

  [_MOVEMENT] = LAYOUT(
    KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                   KC_CIRC, KC_AMPR, KC_UP,   KC_LPRN, KC_RPRN, KC_DEL,
    KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                     KC_F6,   KC_LEFT, KC_DOWN, KC_RGHT, KC_RCBR, KC_PIPE,
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,                    KC_F12,  KC_END,  _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, KC_PGDN, KC_PGUP, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY
  ),

};

uint16_t process_key_user(uint16_t keycode, key_ev_type_e kev, keyboard_state_t *const kbd)
{
  switch (keycode)
  {
  case LOWER:
    if (kev != key_ev_up)
    {
      kbd->layer = _LOWER;
    }
    else
    {
      kbd->layer = _QWERTY;
    }
    break;

  case RAISE:
    if (kev != key_ev_up)
    {
      kbd->layer = _RAISE;
    }
    else
    {
      kbd->layer = _QWERTY;
    }
    break;

  case MOVEMENT:
    if (kev != key_ev_up)
    {
      kbd->layer = _MOVEMENT;
    }
    else
    {
      kbd->layer = _QWERTY;
    }
    break;

  case LCTL_ESC:
    if (kev == key_ev_tap)
    {
      keycode = KC_ESC;
    }
    else
    {
      keycode = KC_LCTL;
    }
    break;

  case LSFT_ENTER:
    if (kev == key_ev_tap)
    {
      keycode = KC_ENT;
    }
    else
    {
      keycode = KC_LSFT;
    }
    break;
  }

  return keycode;
}

esm_t *keyboard_get_kev_dest(uint8_t col, uint8_t row)
{
  uint16_t kc = keyboard_get_kc(col, row, keyboard_state);
  esm_t *e = keyboard_esm;

  if (kc == LCTL_ESC)
  {
    e = tap_detector1_esm;
  }
  else if (kc == LSFT_ENTER)
  {
    e = tap_detector2_esm;
  }
  else
  {
    esm_signal_t s = {
        .type = esm_sig_tap,
        .sender = NULL,
        .receiver = NULL};

    esm_broadcast_signal(&s, esm_gr_taps);
  }

  esm_signal_t s = {
      .type = esm_sig_alarm,
      .sender = NULL,
      .receiver = backlight_esm};
  esm_send_signal(&s);

  return e;
}

static const tap_detector_cfg_t tap_detector1_cfg = {
    .tap_tres = TAP_TRES_MS,
};
ESM_REGISTER(tap_detector, tap_detector1, esm_gr_taps, 2, 2);

static const tap_detector_cfg_t tap_detector2_cfg = {
    .tap_tres = TAP_TRES_MS,
};
ESM_REGISTER(tap_detector, tap_detector2, esm_gr_taps, 2, 2);