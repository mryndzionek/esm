#include "esm/esm.h"
#include "esm/esm_timer.h"
#include "board.h"

ESM_THIS_FILE;

typedef struct
{

} random_exciter_cfg_t;

typedef struct
{
	esm_t esm;
	esm_timer_t timer;
	random_exciter_cfg_t const *const cfg;
} random_exciter_esm_t;

ESM_DEFINE_STATE(main);

static void esm_main_entry(esm_t *const esm)
{
    random_exciter_esm_t *self = ESM_CONTAINER_OF(esm, random_exciter_esm_t, esm);

    esm_signal_t s = {
        .type = esm_sig_tmout,
        .sender = esm,
        .receiver = esm};
    esm_timer_add(&self->timer,
                  1000UL + ESM_RANDOM(5000UL), &s);
}

static void esm_main_exit(esm_t *const esm)
{
	random_exciter_esm_t *self = ESM_CONTAINER_OF(esm, random_exciter_esm_t, esm);
	esm_timer_rm(&self->timer);
}

static void esm_main_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	switch (sig->type)
	{
	case esm_sig_tmout:
	{
        esm_signal_t s = {
            .type = esm_sig_alarm,
            .params.bcklight = {
                .row = ESM_RANDOM(N_ROWS),
                .col = ESM_RANDOM(N_COLS),
                .val = 0x80 + ESM_RANDOM(0x1FF)},
            .sender = NULL,
            .receiver = backlight_esm};

        esm_send_signal(&s);
        ESM_TRANSITION(self);
    }
	break;

	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_random_exciter_init(esm_t *const esm)
{
	ESM_TRANSITION(main);
}

static const random_exciter_cfg_t random_exciter_cfg = {

};

ESM_REGISTER(random_exciter, random_exciter, esm_gr_none, 8, 0);