#include "esm/hesm.h"
#include "esm/esm_timer.h"

#include "keycodes.h"
#include "mmml_player.h"
#include "mmml-data.h"
#include "board.h"

#define SAMP_BUF_SIZE (1 * 1024)

ESM_THIS_FILE;

typedef struct
{
} player_cfg_t;

typedef struct
{
    hesm_t esm;
    mmml_player_t player;
    uint8_t i, track, in_track;
    bool finished;
    uint8_t buf[2][SAMP_BUF_SIZE];
    esm_timer_t timer;
    player_cfg_t const *const cfg;
} player_esm_t;

ESM_COMPLEX_STATE(base, top, 1);
ESM_LEAF_STATE(off, base, 2);
ESM_LEAF_STATE(on, base, 2);

static void esm_base_init(esm_t *const esm)
{
    (void)esm;
    ESM_TRANSITION(off);
}

static void esm_base_entry(esm_t *const esm)
{
    (void)esm;
}

static void esm_base_exit(esm_t *const esm)
{
    (void)esm;
}

static void esm_base_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    player_esm_t *self = ESM_CONTAINER_OF(esm, player_esm_t, esm);
    (void)self;

    switch (sig->type)
    {
    case esm_sig_remote:
    {
        switch (sig->params.keycode)
        {
        case KEYCODE_PLAY:
        {
            esm_signal_t s = {
                .type = esm_sig_play,
                .params.play = {.track = self->in_track},
                .sender = NULL,
                .receiver = esm};
            esm_send_signal(&s);
        }
        break;

        case KEYCODE_NEXT:
        {
            self->in_track = self->in_track == (int)(NUM_ALARMS - 1) ? 0 : self->in_track + 1;
            mmml_player_init(&self->player, Tracks[self->in_track]);
        }
        break;

        case KEYCODE_PREV:
        {
            self->in_track = self->in_track == 0 ? (uint8_t)(NUM_ALARMS - 1) : self->in_track - 1;
            mmml_player_init(&self->player, Tracks[self->in_track]);
        }
        break;
        }
    }
    break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_off_entry(esm_t *const esm)
{
    (void)esm;
}

static void esm_off_exit(esm_t *const esm)
{
    (void)esm;
}

static void esm_off_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    player_esm_t *self = ESM_CONTAINER_OF(esm, player_esm_t, esm);

    switch (sig->type)
    {
    case esm_sig_play:
        self->track = sig->params.play.track;
        ESM_TRANSITION(on);
        break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_on_entry(esm_t *const esm)
{
    player_esm_t *self = ESM_CONTAINER_OF(esm, player_esm_t, esm);

    mmml_player_init(&self->player, Tracks[self->track]);
    mmml_player_gen_samples(&self->player, self->buf[0], SAMP_BUF_SIZE);
    mmml_player_gen_samples(&self->player, self->buf[1], SAMP_BUF_SIZE);
    BOARD_PLAY(self->buf[self->i], SAMP_BUF_SIZE);
    self->i ^= 0x01;
    self->finished = 0;
}

static void esm_on_exit(esm_t *const esm)
{
    (void)esm;
    BOARD_PAUSE();
}

static void esm_on_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    player_esm_t *self = ESM_CONTAINER_OF(esm, player_esm_t, esm);
    (void)self;

    switch (sig->type)
    {
    case esm_sig_alarm:
        if (self->finished)
        {
            ESM_TRANSITION(off);
        }
        else
        {
            BOARD_PLAY(self->buf[self->i], SAMP_BUF_SIZE);
            self->i ^= 0x01;
            self->finished = mmml_player_gen_samples(&self->player, self->buf[self->i], SAMP_BUF_SIZE);
        }
        break;

    case esm_sig_play:
        ESM_TRANSITION(off);
        break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_player_init(esm_t *const esm)
{
    player_esm_t *self = ESM_CONTAINER_OF(esm, player_esm_t, esm);
    self->track = 0;
    ESM_TRANSITION(off);
}

static const player_cfg_t player1_cfg = {};

ESM_COMPLEX_REGISTER(player, player1, esm_gr_remote, 2, 3, 0);