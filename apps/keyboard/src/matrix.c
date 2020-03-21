#include "esm/esm.h"
#include "esm/esm_timer.h"
#include "board.h"

#define SCAN_PERIOD_MS (ESM_TICKS_PER_SEC / SCAN_FREQ_HZ)
#define DEBOUNCE_THRESHOLD (DEBOUNCE_MS / SCAN_PERIOD_MS)

ESM_THIS_FILE;

typedef struct
{
	uint16_t dc; // debounce counter
	bool pressed;
} key_state_t;

typedef struct
{

} matrix_cfg_t;

typedef struct
{
	esm_t esm;
	esm_timer_t timer;
	bool keys[N_COLS][N_ROWS];
	key_state_t matrix[N_COLS][N_ROWS];
	matrix_cfg_t const *const cfg;
} matrix_esm_t;

__attribute__((weak)) esm_t *keyboard_get_kev_dest(uint8_t col, uint8_t row)
{
	(void)col;
	(void)row;
	return keyboard_esm;
}

ESM_DEFINE_STATE(scanning);

static void esm_scanning_entry(esm_t *const esm)
{
	matrix_esm_t *self = ESM_CONTAINER_OF(esm, matrix_esm_t, esm);

	esm_signal_t s = {
		.type = esm_sig_tmout,
		.sender = esm,
		.receiver = esm};
	esm_timer_add(&self->timer, SCAN_PERIOD_MS, &s);
}

static void esm_scanning_exit(esm_t *const esm)
{
	matrix_esm_t *self = ESM_CONTAINER_OF(esm, matrix_esm_t, esm);
	esm_timer_rm(&self->timer);
}

static void esm_scanning_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	matrix_esm_t *self = ESM_CONTAINER_OF(esm, matrix_esm_t, esm);

	switch (sig->type)
	{
	case esm_sig_tmout:
	{
		board_read_matrix(&self->keys);

		// debounce and detect key events
		for (uint8_t j = 0; j < N_COLS; j++)
		{
			for (uint8_t i = 0; i < N_ROWS; i++)
			{
				key_state_t *ks = &self->matrix[j][i];

				if ((ks->pressed == false) &&
					self->keys[j][i] && (ks->dc < DEBOUNCE_THRESHOLD))
				{
					ks->dc++;
					if (ks->dc == DEBOUNCE_THRESHOLD)
					{
						esm_t *dest = (esm_t *)keyboard_get_kev_dest(j, i);
						ks->pressed = true;
						esm_signal_t s = {
							.type = esm_sig_keypress,
							.params.key = {
								.row = i,
								.col = j,
								.kev = key_ev_down},
							.sender = NULL,
							.receiver = dest};
						esm_send_signal(&s);
					}
				}
				else if (ks->pressed &&
						 (self->keys[j][i] == false) && (ks->dc > 0))
				{
					ks->dc--;
					if (ks->dc == 0)
					{
						ks->pressed = false;

						esm_t *dest = (esm_t *)keyboard_get_kev_dest(j, i);
						esm_signal_t s = {
							.type = esm_sig_keypress,
							.params.key = {
								.row = i,
								.col = j,
								.kev = key_ev_up},
							.sender = NULL,
							.receiver = dest};
						esm_send_signal(&s);
					}
				}
			}
		}
		ESM_TRANSITION(self);
	}
	break;

	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_matrix_init(esm_t *const esm)
{
	// turn of state changes tracing for this SM
	esm->trace_off = true;

	ESM_TRANSITION(scanning);
}

static const matrix_cfg_t matrix_cfg = {

};

ESM_REGISTER(matrix, matrix, esm_gr_none, 2, 0);
