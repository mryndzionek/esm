#include "state_handler.h"
#include "config.h"
#include "keycode.h"

ESM_THIS_FILE;

typedef struct
{
    esm_timer_t timer;
    uint8_t col;
    uint8_t row;
    bool is_active;
} tap_state_t;

typedef struct
{
    esm_t esm;
    tap_state_t tap_s[NUM_TAPS];
    keyboard_state_t kbd_s;
    state_handler_cfg_t const *const cfg;
} state_handler_esm_t;

ESM_DEFINE_STATE(active);

static keyitem_t keyitems[N_COLS][N_ROWS];

extern const uint16_t keymaps[][N_ROWS][N_COLS];

__attribute__((weak)) uint16_t process_key_user(uint16_t keycode, mat_ev_type_e ev, keyboard_state_t *const kbd)
{
	(void)ev;
	(void)kbd;

	return keycode;
}

static uint16_t keyboard_get_kc(state_handler_esm_t *self, uint8_t col, uint8_t row, mat_ev_type_e ev)
{
	int sl;
	uint16_t kc;

	sl = self->kbd_s.layer;

	do
	{
		if (sl < 0)
		{
			kc = keymaps[BASE_LAYER][row][col];
			break;
		}
		kc = keymaps[sl--][row][col];
	} while (kc == KC_TRNS);

	kc = process_key_user(kc, ev, &self->kbd_s);
	if (self->kbd_s.layer < 0)
	{
		self->kbd_s.layer = 0;
	}
	else if (self->kbd_s.layer >= N_COLS)
	{
		self->kbd_s.layer = N_COLS - 1;
	}

	return kc;
}

static int is_tap(const state_handler_cfg_t *cfg, uint16_t kc)
{
	int ret = -1;

	for (uint8_t i = 0; i < NUM_TAPS; i++)
	{
		if (cfg->cfgs[i].code == kc)
		{
			ret = i;
			break;
		}
	}
	return ret;
}

static void send_key(uint16_t kc, keyitem_t *it, mat_ev_type_e ev)
{
	it->kc = kc;

	esm_signal_t s = {
		.type = esm_sig_key,
		.params.key = {
			.it = it,
			.ev = ev},
		.sender = NULL,
		.receiver = keyboard_esm};
	esm_send_signal(&s);
}

static void schedule(esm_t *const esm, uint8_t i, uint16_t kc, uint8_t col, uint8_t row)
{
	state_handler_esm_t *self = ESM_CONTAINER_OF(esm, state_handler_esm_t, esm);
	keyitems[col][row].kc = kc;
	self->tap_s[i].col = col;
	self->tap_s[i].row = row;

	esm_signal_t s = {
		.type = esm_sig_tmout,
		.params.tap = {
			.i = i,
			.it = &keyitems[col][row],
			.ev = mat_ev_down},
		.sender = esm,
		.receiver = esm};
	esm_timer_add(&self->tap_s[i].timer, self->cfg->cfgs[i].tap_tres, &s);
}

static void esm_active_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_active_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_active_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	state_handler_esm_t *self = ESM_CONTAINER_OF(esm, state_handler_esm_t, esm);

	switch (sig->type)
	{
	case esm_sig_tmout:
	{
		self->tap_s[sig->params.tap.i].is_active = false;
		send_key(sig->params.tap.it->kc, sig->params.tap.it, mat_ev_down);
	}
	break;

	case esm_sig_matrix:
	{
		uint16_t kc = keyboard_get_kc(self, sig->params.mat.col, sig->params.mat.row, sig->params.mat.ev);
		int i = is_tap(self->cfg, kc);

		if (i >= 0)
		{
			if (sig->params.mat.ev == mat_ev_up)
			{
				keyitem_t *it = &keyitems[sig->params.mat.col][sig->params.mat.row];
				if(self->tap_s[i].is_active)
				{
					esm_timer_rm(&self->tap_s[i].timer);
					send_key(self->cfg->cfgs[i].tap_code, it, mat_ev_down);
					send_key(self->cfg->cfgs[i].tap_code, it, mat_ev_up);
					self->tap_s[i].is_active = false;
				} else {
					send_key(self->cfg->cfgs[i].tap_code, it, mat_ev_up);
				}
			}
			else
			{
				ESM_ASSERT(!self->tap_s[i].is_active);
				self->tap_s[i].is_active = true;
				schedule(esm, i, self->cfg->cfgs[i].press_code,
						 sig->params.mat.col, sig->params.mat.row);
			}
		}
		else
		{
			for (uint8_t j = 0; j < NUM_TAPS; j++)
			{
				tap_state_t s = self->tap_s[j];
				if (s.is_active)
				{
					esm_timer_rm(&s.timer);
					s.is_active = false;
					send_key(self->cfg->cfgs[j].press_code,
							 &keyitems[s.col][s.row], mat_ev_down);
				}
			}
			keyitem_t *it = &keyitems[sig->params.mat.col][sig->params.mat.row];
			send_key(kc, it, sig->params.mat.ev);
		}
	}
	break;

	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

void esm_state_handler_init(esm_t *const esm)
{
	state_handler_esm_t *self = ESM_CONTAINER_OF(esm, state_handler_esm_t, esm);

	self->kbd_s.layer = BASE_LAYER;
	ESM_TRANSITION(active);
}

extern const state_handler_cfg_t state_handler_cfg;
ESM_REGISTER(state_handler, state_handler, esm_gr_none, 4, 2);
