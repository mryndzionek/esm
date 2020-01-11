#include "board.h"
#include "esm/esm.h"

#include "debouncer.h"
#include "encoder.h"

static void chan_a_handle(esm_t *const esm, BOARD_DEBOUNCER_STATE state)
{
	(void)esm;
	(void)state;
}

static void chan_a_arm(esm_t *const esm)
{
	(void)esm;
}

static const debouncer_cfg_t deb_enc_chan_a_cfg = {
	.period = 1UL,
	.handle = chan_a_handle,
	.arm = chan_a_arm};

ESM_REGISTER(debouncer, deb_enc_chan_a, esm_gr_none, 2, 1);

static void chan_b_handle(esm_t *const esm, BOARD_DEBOUNCER_STATE state)
{
	(void)esm;
	(void)state;
}

static void chan_b_arm(esm_t *const esm)
{
	(void)esm;
}

static const debouncer_cfg_t deb_enc_chan_b_cfg = {
	.period = 1UL,
	.handle = chan_b_handle,
	.arm = chan_b_arm};

ESM_REGISTER(debouncer, deb_enc_chan_b, esm_gr_none, 2, 1);

static void enc_cw_action(void)
{

}

static void enc_ccw_action(void)
{

}

static const encoder_cfg_t encoder_cfg = {
	.cw_action = enc_cw_action,
	.ccw_action = enc_ccw_action,
};

ESM_REGISTER(encoder, encoder, esm_gr_none, 4, 1);

