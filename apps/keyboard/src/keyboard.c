#include <string.h>

#include "esm/esm.h"
#include "esm/esm_timer.h"
#include "rb.h"

#include "board.h"

#include "keyboard.h"
#include "keycode.h"
#include "keycode_extra.h"

#define KEYBOARD_REPORT_SIZE (8)
#define CONSUMER_REPORT_SIZE (1)

ESM_THIS_FILE;

typedef struct
{
    esm_list_item_t item;
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
    keyboard_cfg_t const *const cfg;
} keyboard_esm_t;

static rb_t report_rb = {.data_ = (uint8_t[32]){0}, .capacity_ = 32};

extern const uint16_t keymaps[][N_ROWS][N_COLS];
__attribute__((weak)) uint16_t process_key_user(uint16_t keycode, key_ev_type_e kev, keyboard_state_t *const kbd)
{
    (void)kev;
    (void)kbd;

    return keycode;
}

ESM_DEFINE_STATE(active);

static void send_report(keyboard_esm_t *const self)
{
    static uint8_t report_buf[KEYBOARD_REPORT_SIZE + 1] = {0};

    if (rb_size(&report_rb))
    {
        uint8_t l;
        size_t s;

        s = rb_read(&report_rb, &l, 1);
        ESM_ASSERT(s == 1);
        ESM_ASSERT(l <= sizeof(report_buf));

        s = rb_read(&report_rb, report_buf, l);
        ESM_ASSERT(s == l);

        self->busy = board_usb_send(report_buf, l);
    }
    else
    {
        self->busy = false;
    }
}

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

    switch (sig->type)
    {
    case esm_sig_keypress:
    {
        uint8_t r = sig->params.key.row;
        uint8_t c = sig->params.key.col;
        uint16_t kc = keyboard_get_kc(c, r, &self->state);

        uint8_t kbd_report[KEYBOARD_REPORT_SIZE + 2] = {KEYBOARD_REPORT_SIZE + 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t consumer_report[CONSUMER_REPORT_SIZE + 2] = {CONSUMER_REPORT_SIZE + 1, 2, 0};

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

        uint8_t i = 4;

        while (it && (i < sizeof(kbd_report)))
        {
            hidkey_t *key = ESM_CONTAINER_OF(it, hidkey_t, item);
            kc = key->kc;

            if (IS_MOD(kc))
            {
                kbd_report[2] |= MOD_BIT(kc);
            }
            else if (IS_KEY(kc) && (i < sizeof(kbd_report)))
            {
                kbd_report[i++] = kc;
            }
            else if (IS_LSFT(kc) && (i < sizeof(kbd_report)))
            {
                kbd_report[2] |= MOD_BIT(KC_LSFT);
                kbd_report[i++] = kc & 0xFF;
            }
            else
            {
                switch (kc)
                {
                case KC_VOLU:
                    consumer_report[2] |= (1UL << 6);
                    break;

                case KC_VOLD:
                    consumer_report[2] |= (1UL << 7);
                    break;

                case KC_MPLY:
                    consumer_report[2] |= (1UL << 4);
                    break;

                case KC_MNXT:
                    consumer_report[2] |= (1UL << 0);
                    break;

                case KC_MPRV:
                    consumer_report[2] |= (1UL << 1);
                    break;

                default:
                    break;
                }
            }
            it = it->prev;
        }

        if ((sizeof(kbd_report) + sizeof(consumer_report)) <= (report_rb.capacity_ - report_rb.size_))
        {
            (void)rb_write(&report_rb, kbd_report, sizeof(kbd_report));
            (void)rb_write(&report_rb, consumer_report, sizeof(consumer_report));

            if (!self->busy)
            {
                send_report(self);
            }
        }
    }
    break;

    case esm_sig_usb_tx_end:
        send_report(self);
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
    ESM_TRANSITION(active);
}

static const keyboard_cfg_t keyboard_cfg = {

};

ESM_REGISTER(keyboard, keyboard, esm_gr_none, 32, 1);
const keyboard_state_t *const keyboard_state = &keyboard_ctx.state;
