
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

esm_t *keyboard_get_kev_dest(uint8_t col, uint8_t row)
{
  (void)col;
  (void)row;
  
  esm_t *e = keyboard_esm;

  esm_signal_t s = {
      .type = esm_sig_alarm,
      .params.bcklight = {
          .row = row,
          .col = col,
          .val = 0xFF},
      .sender = NULL,
      .receiver = backlight_esm};
  esm_send_signal(&s);

  return e;
}