#include "esm/esm.h"
#include "esm/esm_timer.h"

#include "backlight.h"
#include "sk6812.h"

#define COLOR_1 (0x00000205)
#define COLOR_2 (0x0000050A)

ESM_THIS_FILE;

ESM_DEFINE_STATE(idle);
ESM_DEFINE_STATE(busy);

static const uint16_t seq[] = {100, 200, 50, 50, 200, 80, 100, 10, 10, 120, 50, 200};
static const uint32_t colors[] = {COLOR_1, COLOR_2, 0};

static void esm_idle_entry(esm_t *const esm)
{
    backlight_esm_t *self = ESM_CONTAINER_OF(esm, backlight_esm_t, esm);

    esm_signal_t sig = {
        .type = esm_sig_tmout,
        .sender = esm,
        .receiver = esm};

    if (self->i == 0)
    {
        self->i++;
        esm_timer_add(&self->timer, 10000 + ESM_RANDOM(20000), &sig);
    } else {
        esm_timer_add(&self->timer, seq[(self->i++) - 1], &sig);
        if (self->i > (sizeof(seq) / sizeof(seq[0])))
        {
            self->i = 0;
        }
    }
}

static void esm_idle_exit(esm_t *const esm)
{
    backlight_esm_t *self = ESM_CONTAINER_OF(esm, backlight_esm_t, esm);
    esm_timer_rm(&self->timer);
}

static void esm_idle_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    switch (sig->type)
    {
    case esm_sig_tmout:
    {
        ESM_TRANSITION(busy);
    }
    break;
    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_busy_entry(esm_t *const esm)
{
    backlight_esm_t *self = ESM_CONTAINER_OF(esm, backlight_esm_t, esm);

    for (uint8_t i = 0; i < SK6812_LEDS_NUM; i++)
    {
        sk6812_set_color(i, colors[self->i % (sizeof(colors) / sizeof(colors[0]))]);
    }

    if (self->i == 0)
    {
        for (uint8_t i = 0; i < 3; i++)
        {
            sk6812_set_color(ESM_RANDOM(SK6812_LEDS_NUM), ESM_RANDOM(5) << 16);
        }
    }

    esm_signal_t sig = {
        .type = esm_sig_tmout,
        .sender = esm,
        .receiver = esm};
    esm_timer_add(&self->timer,
                  ESM_TICKS_PER_SEC / self->cfg->freq_hz, &sig);
    board_backlight_show();
}

static void esm_busy_exit(esm_t *const esm)
{
    backlight_esm_t *self = ESM_CONTAINER_OF(esm, backlight_esm_t, esm);
    esm_timer_rm(&self->timer);
}

static void esm_busy_handle(esm_t *const esm, const esm_signal_t *const sig)
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

void esm_backlight_init(esm_t *const esm)
{
    sk6812_clear();
    ESM_TRANSITION(busy);
}
