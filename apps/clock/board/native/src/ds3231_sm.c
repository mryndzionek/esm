#include "esm/esm.h"
#include "esm/esm_timer.h"

ESM_THIS_FILE;

typedef struct
{
} ds3231_sm_cfg_t;

typedef struct
{
    esm_t esm;
    esm_timer_t timer;
    ds3231_sm_cfg_t const *const cfg;
} ds3231_sm_esm_t;

ESM_DEFINE_STATE(inactive);
ESM_DEFINE_STATE(active);

static void esm_inactive_entry(esm_t *const esm)
{
    (void)esm;
}

static void esm_inactive_exit(esm_t *const esm)
{
    (void)esm;
}

static void esm_inactive_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    (void)esm;

    switch (sig->type)
    {
    case esm_sig_alarm:
        ESM_TRANSITION(active);
        break;
    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_active_entry(esm_t *const esm)
{
    ds3231_sm_esm_t *self = ESM_CONTAINER_OF(esm, ds3231_sm_esm_t, esm);
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    ESM_ASSERT((ts.tv_nsec / 1000000UL) < 1000UL);

    esm_signal_t sig = {
        .type = esm_sig_tmout,
        .sender = esm,
        .receiver = esm};
    esm_timer_add(&self->timer,
                  1000UL - (ts.tv_nsec / 1000000UL), &sig);
}

static void esm_active_exit(esm_t *const esm)
{
    ds3231_sm_esm_t *self = ESM_CONTAINER_OF(esm, ds3231_sm_esm_t, esm);
    esm_timer_rm(&self->timer);
}

static void esm_active_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    (void)esm;

    switch (sig->type)
    {
    case esm_sig_alarm:
        break;

    case esm_sig_tmout:
    {
        esm_signal_t s = {
            .type = esm_sig_alarm,
            .sender = NULL,
            .receiver = rtc1_esm};
        esm_send_signal(&s);
        ESM_TRANSITION(self);
    }
    break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_ds3231_sm_init(esm_t *const esm)
{
    ESM_TRANSITION(inactive);
}

static const ds3231_sm_cfg_t ds3231_sm1_cfg = {};

ESM_REGISTER(ds3231_sm, ds3231_sm1, esm_gr_none, 2, 0);