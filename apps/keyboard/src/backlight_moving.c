#include "esm/esm.h"
#include "esm/esm_timer.h"

#include "backlight.h"
#include "sk6812.h"

#define BLUE (0x000000FF)
#define GREEN (0x0000BB00)
#define LIGHT_RED (0x00050000)
#define RED (0x00BB0000)

#define COLOR_BG LIGHT_RED
#define COLOR_FG BLUE

ESM_THIS_FILE;

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

    if (self->i == 0)
    {
        self->i = SK6812_LEDS_NUM;
    }

    esm_signal_t sig = {
        .type = esm_sig_tmout,
        .sender = esm,
        .receiver = esm};
    esm_timer_add(&self->timer,
                  ESM_TICKS_PER_SEC / self->cfg->freq_hz, &sig);
    if (--self->i)
    {
        sk6812_set_color(self->i, COLOR_FG);
    }
    else
    {
        sk6812_set_color(self->i, COLOR_BG);
    }

    board_backlight_show();
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
        sk6812_set_color(self->i, COLOR_BG);
        if (self->i > 0)
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
        sk6812_set_color(self->i, COLOR_BG);
        self->i = SK6812_LEDS_NUM - 1;
        break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

void esm_backlight_init(esm_t *const esm)
{
    sk6812_clear();
    ESM_TRANSITION(busy);
}
