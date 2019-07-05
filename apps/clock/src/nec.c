#include "esm/esm.h"
#include "esm/esm_timer.h"

#include "board.h"

ESM_THIS_FILE;

typedef struct
{
} nec_cfg_t;

typedef struct
{
    esm_t esm;
    esm_timer_t timer;
    uint32_t delay;
    uint8_t ncmd;
    uint8_t rc;
    nec_cfg_t const *const cfg;
} nec_esm_t;

ESM_DEFINE_STATE(idle);
ESM_DEFINE_STATE(delay);

static void esm_idle_entry(esm_t *const esm)
{
    (void)esm;
    board_nec_start(&nec1);
}

static void esm_idle_exit(esm_t *const esm)
{
    (void)esm;
}

static void esm_idle_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    nec_esm_t *self = ESM_CONTAINER_OF(esm, nec_esm_t, esm);

    switch (sig->type)
    {
    case esm_sig_alarm:
    {
        NEC_t *handle = &nec1;

        for (int pos = 0; pos < 32; pos++)
        {
            int time = handle->rawTimerData[pos];
            if (time > handle->timingBitBoundary)
            {
                handle->decoded[pos / 8] |= 1 << (pos % 8);
            }
            else
            {
                handle->decoded[pos / 8] &= ~(1 << (pos % 8));
            }
        }

        uint8_t valid = 1;
        uint8_t naddr = ~handle->decoded[0];
        self->ncmd = ~handle->decoded[2];
        self->rc = 0;

        if (handle->type == NEC_NOT_EXTENDED && handle->decoded[1] != naddr)
            valid = 0;
        if (handle->decoded[3] != self->ncmd)
            valid = 0;

        if (valid)
        {
            esm_signal_t s = {
                .type = esm_sig_remote,
                .params.keycode = self->ncmd,
                .sender = NULL,
                .receiver = clock1_esm};
            esm_send_signal(&s);
            self->delay = 30UL;
            ESM_TRANSITION(delay);
        }
        else
        {
            self->ncmd = 0;
            ESM_TRANSITION(self);
        }
    }
    break;

    case esm_sig_reset:
    {
        if (self->rc == 2)
        {
            esm_signal_t s = {
                .type = esm_sig_remote,
                .params.keycode = self->ncmd,
                .sender = NULL,
                .receiver = clock1_esm};
            esm_send_signal(&s);

            self->delay = 30UL;
            ESM_TRANSITION(delay);
        }
        else
        {
            ++self->rc;
            ESM_TRANSITION(self);
        }
    }
    break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_delay_entry(esm_t *const esm)
{
    nec_esm_t *self = ESM_CONTAINER_OF(esm, nec_esm_t, esm);

    esm_signal_t sig = {
        .type = esm_sig_tmout,
        .sender = esm,
        .receiver = esm};
    esm_timer_add(&self->timer,
                  self->delay, &sig);
}

static void esm_delay_exit(esm_t *const esm)
{
    nec_esm_t *self = ESM_CONTAINER_OF(esm, nec_esm_t, esm);
    esm_timer_rm(&self->timer);
}

static void esm_delay_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    (void)esm;

    switch (sig->type)
    {
    case esm_sig_tmout:
    {
        ESM_TRANSITION(idle);
    }
    break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_nec_init(esm_t *const esm)
{
    ESM_TRANSITION(idle);
}

static const nec_cfg_t nec1_cfg = {};

ESM_REGISTER(nec, nec1, esm_gr_none, 2, 0);