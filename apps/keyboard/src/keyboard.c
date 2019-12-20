#include <string.h>

#include "esm/esm.h"
#include "esm/esm_timer.h"
#include "board.h"
#include "trace.h"

#include "keyboard.h"
#include "keycode.h"
#include "keycode_extra.h"

#define PRESS_REPORT_SIZE (8)

ESM_THIS_FILE;

typedef struct
{
    esm_list_item_t item;
    uint8_t row;
    uint8_t col;
    uint16_t kc;
} hidkey_t;

typedef struct
{

} keyboard_cfg_t;

typedef struct
{
    esm_t esm;
    esm_timer_t timer;
    hidkey_t keys[N_COLS][N_ROWS];
    esm_list_t pressed_keys;
    keyboard_state_t state;
    bool busy;
    bool retry;
    keyboard_cfg_t const *const cfg;
} keyboard_esm_t;

extern const uint16_t keymaps[][N_ROWS][N_COLS];
__attribute__((weak)) uint16_t process_key_user(uint16_t keycode, key_ev_type_e kev, keyboard_state_t *const kbd)
{
    (void)kev;
    (void)kbd;

    return keycode;
}

ESM_DEFINE_STATE(active);

uint16_t keyboard_get_kc(uint8_t col, uint8_t row, const keyboard_state_t *const kbd)
{
    int sl;
    uint16_t kc;

    sl = kbd->layer;

    do
    {
        if (sl < 0)
        {
            kc = keymaps[BASE_LAYER][row][col];
            break;
        }
        kc = keymaps[sl--][row][col];
    } while (kc == KC_TRNS);

    return kc;
}

static void esm_active_entry(esm_t *const esm)
{
    (void)esm;
}

static void esm_active_exit(esm_t *const esm)
{
    (void)esm;
}

static void esm_active_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    keyboard_esm_t *self = ESM_CONTAINER_OF(esm, keyboard_esm_t, esm);
    static uint8_t press_report[PRESS_REPORT_SIZE] = {0};

    switch (sig->type)
    {
    case esm_sig_keypress:
    {
        uint8_t r = sig->params.key.row;
        uint8_t c = sig->params.key.col;
        uint16_t kc = keyboard_get_kc(c, r, &self->state);

        esm_list_item_t *itm = &self->keys[c][r].item;
        kc = process_key_user(kc, sig->params.key.kev, &self->state);

        if (self->state.layer < 0)
        {
            self->state.layer = 0;
        }
        else if (self->state.layer >= N_COLS)
        {
            self->state.layer = N_COLS - 1;
        }

        if (sig->params.key.kev != key_ev_up)
        {
            ESM_ASSERT((itm->next == NULL) && (itm->prev == NULL));
            self->keys[c][r].kc = kc;
            esm_list_insert(&self->pressed_keys, itm, NULL);
        }
        else
        {
            esm_list_erase(&self->pressed_keys, itm);
        }

        // construct HID report
        esm_list_item_t *it = (&self->pressed_keys)->last;
        memset(press_report, 0x00, PRESS_REPORT_SIZE);

        uint8_t i = 2;
        while (it)
        {
            hidkey_t *key = ESM_CONTAINER_OF(it, hidkey_t, item);
            kc = key->kc;

            if (IS_MOD(kc))
            {
                press_report[0] |= MOD_BIT(kc);
            }
            else if (IS_ANY(kc) && (i < PRESS_REPORT_SIZE))
            {
                press_report[i++] = kc;
            }
            else if (IS_LSFT(kc) && (i < PRESS_REPORT_SIZE))
            {
                press_report[0] |= MOD_BIT(KC_LSFT);
                press_report[i++] = kc & 0xFF;
            }
            it = it->prev;
        }

        if (!self->busy)
        {
            trace_data((uint8_t const *)press_report, PRESS_REPORT_SIZE);
            board_usb_send(press_report, PRESS_REPORT_SIZE);
            self->busy = true;
        } else {
            self->retry = true;
        }
    }
    break;

    case esm_sig_usb_tx_end:
        if (self->retry)
        {
            trace_data((uint8_t const *)press_report, PRESS_REPORT_SIZE);
            board_usb_send(press_report, PRESS_REPORT_SIZE);
            self->retry = false;
        }
        else
        {
            self->busy = false;
        }
        break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_keyboard_init(esm_t *const esm)
{
    keyboard_esm_t *self = ESM_CONTAINER_OF(esm, keyboard_esm_t, esm);

    self->state.layer = BASE_LAYER;
    
    for (uint8_t j = 0; j < N_COLS; j++)
    {
        for (uint8_t i = 0; i < N_ROWS; i++)
        {
            self->keys[j][i].col = j;
            self->keys[j][i].row = i;
        }
    }

    ESM_TRANSITION(active);
}

static const keyboard_cfg_t keyboard_cfg = {

};

ESM_REGISTER(keyboard, keyboard, esm_gr_none, 4, 2);
const keyboard_state_t *const keyboard_state = &keyboard_ctx.state;
