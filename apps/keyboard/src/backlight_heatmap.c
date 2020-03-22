#include "esm/esm.h"
#include "esm/esm_timer.h"

#include "sk6812.h"

#define POKE_VAL (0xFF)
#define FLCKR_VAL (0x1FF)

#define DIFF_SKEWED(_a, _b) (((_a) > (_b)) ? ((_a) - (_b)) : 0)

ESM_THIS_FILE;

typedef struct
{
    uint16_t freq_hz;
} backlight_cfg_t;

typedef struct
{
    esm_t esm;
    esm_timer_t timer;
    uint8_t i;
    backlight_cfg_t const *const cfg;
} backlight_esm_t;

ESM_DEFINE_STATE(main);

static uint16_t grid[2][N_COLS + 2][N_ROWS + 2];

static uint16_t avg8(uint16_t g[N_COLS + 2][N_ROWS + 2], uint8_t x, uint8_t y)
{
    uint32_t v = ((uint32_t)g[x - 1][y] + g[x + 1][y] +
                  g[x][y - 1] + g[x][y + 1] +
                  g[x - 1][y - 1] + g[x + 1][y + 1] +
                  g[x - 1][y + 1] + g[x + 1][y - 1]) /
                 8;
    return v > 0xFFFF ? 0xFFFF : v;
}

static uint16_t transfer(uint16_t g[N_COLS + 2][N_ROWS + 2], uint8_t x, uint8_t y)
{
    uint32_t v = (uint32_t)DIFF_SKEWED(g[x][y - 1], g[x][y]) +
    DIFF_SKEWED(g[x][y + 1], g[x][y]) +
    DIFF_SKEWED(g[x - 1][y], g[x][y]) +
    DIFF_SKEWED(g[x + 1][y], g[x][y]) +
    DIFF_SKEWED(g[x - 1][y - 1], g[x][y]) +
    DIFF_SKEWED(g[x + 1][y - 1], g[x][y]) +
    DIFF_SKEWED(g[x - 1][y + 1], g[x][y]) +
    DIFF_SKEWED(g[x + 1][y + 1], g[x][y]);
    return v > 0xFFFF ? 0xFFFF : v;
}

static void esm_main_entry(esm_t *const esm)
{
    backlight_esm_t *self = ESM_CONTAINER_OF(esm, backlight_esm_t, esm);

    esm_signal_t sig = {
        .type = esm_sig_tmout,
        .sender = esm,
        .receiver = esm};
    esm_timer_add(&self->timer,
                  ESM_TICKS_PER_SEC / self->cfg->freq_hz, &sig);
}

static void esm_main_exit(esm_t *const esm)
{
    backlight_esm_t *self = ESM_CONTAINER_OF(esm, backlight_esm_t, esm);
    esm_timer_rm(&self->timer);
}

static void esm_main_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    backlight_esm_t *self = ESM_CONTAINER_OF(esm, backlight_esm_t, esm);

    switch (sig->type)
    {
    case esm_sig_tmout:
    {
        uint8_t x, y;
        uint16_t v;

        for (y = 1; y < N_ROWS + 1; y++)
        {
            for (x = 1; x < N_COLS + 1; x++)
            {
                grid[self->i][x][y] = DIFF_SKEWED(grid[self->i ^ 1][x][y], 30) +
                                      (transfer(grid[self->i ^ 1], x, y) / 8);
            }
        }

        for (uint8_t i = 0; i < SK6812_LEDS_NUM; i++)
        {
            board_ledpos_to_xy(i, &x, &y);
            v = avg8(grid[self->i], x + 1, y + 1);
            sk6812_set_color(i, board_heat_colormap[v > 0xFF ? 0xFF : v]);
        }
        sk6812_show();
        self->i ^= 1;
        ESM_TRANSITION(self);
    }
    break;

    case esm_sig_alarm:
    {
        uint8_t x = sig->params.key.col;
        uint8_t y = sig->params.key.row;

        if (sig->sender)
        {
            grid[self->i ^ 1][x + 1][y + 1] = FLCKR_VAL;
        }
        else
        {
            grid[self->i ^ 1][x + 1][y + 1] += grid[self->i ^ 1][x + 1][y + 1] > 0x1FF ? 0 : POKE_VAL;
        }
    }
    break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_backlight_init(esm_t *const esm)
{
    backlight_esm_t *self = ESM_CONTAINER_OF(esm, backlight_esm_t, esm);

    for (uint8_t y = 1; y < N_ROWS + 1; y++)
    {
        for (uint8_t x = 1; x < N_COLS + 1; x++)
        {
            grid[self->i ^ 1][x][y] = 0x3FF;
        }
    }

    sk6812_set_brightness(255);
    ESM_TRANSITION(main);
}

static const backlight_cfg_t backlight_cfg = {
    .freq_hz = 40UL};

ESM_REGISTER(backlight, backlight, esm_gr_none, 8, 0);
