#include "esm/esm.h"
#include "esm/esm_timer.h"

#include "backlight.h"
#include "sk6812.h"

#define DIFF_SKEWED(_a, _b) (((_a) > (_b)) ? ((_a) - (_b)) : 0)

ESM_THIS_FILE;

ESM_DEFINE_STATE(idle);
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

static void esm_idle_entry(esm_t *const esm)
{
    (void)esm;
}

static void esm_idle_exit(esm_t *const esm)
{
    (void)esm;
    board_backlight_show();
}

static void esm_idle_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    backlight_esm_t *self = ESM_CONTAINER_OF(esm, backlight_esm_t, esm);
    switch (sig->type)
    {
    case esm_sig_alarm:
    {
        grid[self->i ^ 1][sig->params.bcklight.col + 1][sig->params.bcklight.row + 1] +=
            grid[self->i ^ 1][sig->params.bcklight.col + 1][sig->params.bcklight.row + 1] > 0x1FF ? 0 : sig->params.bcklight.val;
        ESM_TRANSITION(main);
    }
    break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
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
        board_backlight_show();
        self->i ^= 1;
        ESM_TRANSITION(self);
    }
    break;

    case esm_sig_alarm:
    {
        if (sig->params.bcklight.val == 0)
        {
            ESM_TRANSITION(idle);
        }
        else
        {
            grid[self->i ^ 1][sig->params.bcklight.col + 1][sig->params.bcklight.row + 1] +=
                grid[self->i ^ 1][sig->params.bcklight.col + 1][sig->params.bcklight.row + 1] > 0x1FF ? 0 : sig->params.bcklight.val;
        }
    }
    break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

void esm_backlight_init(esm_t *const esm)
{
    sk6812_set_brightness(255);
    ESM_TRANSITION(idle);
}