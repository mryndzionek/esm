#include "esm/esm.h"
#include "esm/esm_timer.h"

#include "sk6812.h"

ESM_THIS_FILE;

typedef struct
{
} strip_cfg_t;

typedef struct
{
    esm_t esm;
    esm_timer_t timer;
    uint8_t pending;
    strip_cfg_t const *const cfg;
} strip_esm_t;

ESM_DEFINE_STATE(idle);
ESM_DEFINE_STATE(busy);

static void esm_idle_entry(esm_t *const esm)
{
    (void)esm;
}

static void esm_idle_exit(esm_t *const esm)
{
    (void)esm;
}

static void esm_idle_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    switch (sig->type)
    {
    case esm_sig_alarm:
        ESM_TRANSITION(busy);
        break;
    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_busy_entry(esm_t *const esm)
{
    strip_esm_t *self = ESM_CONTAINER_OF(esm, strip_esm_t, esm);

    self->pending = false;
    // TODO: consider relying on DMA finished IRQ instead of fixed timeout
    esm_signal_t sig = {
        .type = esm_sig_tmout,
        .sender = esm,
        .receiver = esm};
    esm_timer_add(&self->timer,
                  5, &sig);
    sk6812_show();
}

static void esm_busy_exit(esm_t *const esm)
{
    strip_esm_t *self = ESM_CONTAINER_OF(esm, strip_esm_t, esm);
    esm_timer_rm(&self->timer);
}

static void esm_busy_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    strip_esm_t *self = ESM_CONTAINER_OF(esm, strip_esm_t, esm);

    switch (sig->type)
    {
    case esm_sig_alarm:
        self->pending = true;
        break;

    case esm_sig_tmout:
    {
        if (self->pending)
        {
            ESM_TRANSITION(self);
        }
        else
        {
            ESM_TRANSITION(idle);
        }
    }
    break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_strip_init(esm_t *const esm)
{
    ESM_TRANSITION(idle);
}

static const strip_cfg_t strip1_cfg = {};

ESM_REGISTER(strip, strip1, esm_gr_none, 1, 0);