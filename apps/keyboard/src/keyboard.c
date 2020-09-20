#include <string.h>

#include "esm/esm.h"
#include "esm/esm_timer.h"
#include "rb.h"

#include "board.h"

#include "keycode.h"
#include "keycode_extra.h"

#define KEYBOARD_REPORT_SIZE (8)
#define CONSUMER_REPORT_SIZE (1)

ESM_THIS_FILE;

typedef struct
{

} keyboard_cfg_t;

typedef struct
{
    esm_t esm;
    esm_timer_t timer;
    esm_list_t pressed_keys;
    bool busy;
    keyboard_cfg_t const *const cfg;
} keyboard_esm_t;

static rb_t report_rb = {.data_ = (uint8_t[32]){0}, .capacity_ = 32};
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
    case esm_sig_key:
    {
        uint16_t kc = sig->params.key.it->kc;

        uint8_t kbd_report[KEYBOARD_REPORT_SIZE + 2] = {KEYBOARD_REPORT_SIZE + 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t consumer_report[CONSUMER_REPORT_SIZE + 2] = {CONSUMER_REPORT_SIZE + 1, 2, 0};

        esm_list_item_t *itm = &sig->params.key.it->item;

        if (sig->params.key.ev != mat_ev_up)
        {
            ESM_ASSERT((itm->next == NULL) && (itm->prev == NULL));
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
            keyitem_t *key = ESM_CONTAINER_OF(it, keyitem_t, item);
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
    ESM_TRANSITION(active);
}

static const keyboard_cfg_t keyboard_cfg = {

};

ESM_REGISTER(keyboard, keyboard, esm_gr_none, 32, 1);
