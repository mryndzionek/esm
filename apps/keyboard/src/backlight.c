#include "esm/esm.h"
#include "esm/esm_timer.h"

#include "sk6812.h"

#define GREEN (0x0000BB00)
#define LIGHT_RED (0x00080000)

#define COLOR_BG LIGHT_RED
#define COLOR_FG GREEN

ESM_THIS_FILE;

typedef struct
{
    uint16_t freq_hz;
} backlight_cfg_t;

typedef struct
{
    esm_t esm;
    esm_timer_t timer;
    uint8_t pos;
    backlight_cfg_t const *const cfg;
} backlight_esm_t;

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
    backlight_esm_t *self = ESM_CONTAINER_OF(esm, backlight_esm_t, esm);

    if (self->pos == 0)
    {
        self->pos = SK6812_LEDS_NUM;
    }

    esm_signal_t sig = {
        .type = esm_sig_tmout,
        .sender = esm,
        .receiver = esm};
    esm_timer_add(&self->timer,
                  ESM_TICKS_PER_SEC / self->cfg->freq_hz, &sig);
    if (--self->pos)
    {
        sk6812_set_color(self->pos, COLOR_FG);
    }
    else
    {
        sk6812_set_color(self->pos, COLOR_BG);
    }

    sk6812_show();
}

static void esm_busy_exit(esm_t *const esm)
{
    backlight_esm_t *self = ESM_CONTAINER_OF(esm, backlight_esm_t, esm);
    esm_timer_rm(&self->timer);
}

static void esm_busy_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    backlight_esm_t *self = ESM_CONTAINER_OF(esm, backlight_esm_t, esm);

    switch (sig->type)
    {
    case esm_sig_tmout:
    {
        sk6812_set_color(self->pos, COLOR_BG);
        if (self->pos > 0)
        {
            ESM_TRANSITION(self);
        }
        else
        {
            ESM_TRANSITION(idle);
        }
    }
    break;

    case esm_sig_alarm:
        sk6812_set_color(self->pos, COLOR_BG);
        self->pos = SK6812_LEDS_NUM - 1;
        break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_backlight_init(esm_t *const esm)
{
    sk6812_clear();
    ESM_TRANSITION(busy);
}

static const backlight_cfg_t backlight_cfg = {
    .freq_hz = 200UL};

ESM_REGISTER(backlight, backlight, esm_gr_none, 2, 0);