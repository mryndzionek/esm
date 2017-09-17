#include "esm/esm.h"
#include "board.h"

#define BINS_PER_10MS					(DCF77_BIN_SIZE / 100UL)
#define BINS_PER_50MS					(5 * BINS_PER_10MS)
#define BINS_PER_100MS					(10 * BINS_PER_10MS)
#define BINS_PER_200MS					(20 * BINS_PER_10MS)
#define TICKS_TO_DRIFT_ONE_TICK     	(300000UL)
#define _NN								((DCF77_BIN_EVERY * TICKS_TO_DRIFT_ONE_TICK) / ESM_TICKS_PER_SEC)

ESM_THIS_FILE;

typedef struct {
} phase_cfg_t;

typedef struct {
	esm_t esm;
	uint16_t bins[DCF77_BIN_SIZE];
	uint8_t phase;
	int32_t max;
	int32_t noise_max;
	phase_cfg_t const *const cfg;
} phase_esm_t;

ESM_DEFINE_STATE(idle);

static uint16_t wrap(const uint16_t value) {
	// faster modulo function which avoids division
	uint16_t result = value;
	while (result >= DCF77_BIN_SIZE) {
		result-= DCF77_BIN_SIZE;
	}
	return result;
}

static void esm_idle_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_idle_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_idle_handle(esm_t *const esm, esm_signal_t *sig)
{
	phase_esm_t *self = ESM_CONTAINER_OF(esm, phase_esm_t, esm);

	switch(sig->type)
	{
	case esm_sig_alarm:
	{
		static int32_t integral = 0;
		static int32_t running_max = 0;
		static uint16_t running_max_index = 0;
		static int32_t running_noise_max = 0;

		uint16_t *bin = &self->bins[sig->params.tick.tick];
		const uint16_t ck_start_tick  = wrap(sig->params.tick.tick+((10-2)*BINS_PER_100MS));
		const uint16_t ck_middle_tick = wrap(sig->params.tick.tick+((10-1)*BINS_PER_100MS));

		if(*bin < _NN)
		{
			*bin += sig->params.tick.bin;
		}

		if(*bin > (DCF77_BIN_EVERY - sig->params.tick.bin))
		{
			*bin -= DCF77_BIN_EVERY - sig->params.tick.bin;
		}

		if (integral > running_max) {
			running_max = integral;
			running_max_index = ck_start_tick;
		}
		if (sig->params.tick.tick == wrap(self->phase + 2*BINS_PER_200MS)) {
			running_noise_max = integral;
		}

		if(sig->params.tick.tick == 0)
		{
			self->max = running_max;
			self->noise_max = running_noise_max;
			self->phase = running_max_index;
			running_max = 0;
		}

		integral -= (int32_t)self->bins[ck_start_tick]*2;
		integral += (int32_t)self->bins[ck_middle_tick];
		integral += (int32_t)*bin;

		{
			static uint16_t bins_to_go = 0;

			if (bins_to_go == 0) {
				if (wrap((DCF77_BIN_SIZE + sig->params.tick.tick - self->phase)) <= BINS_PER_100MS ||
						wrap((DCF77_BIN_SIZE + self->phase - sig->params.tick.tick)) <= BINS_PER_10MS ) {
					bins_to_go = BINS_PER_200MS + 2*BINS_PER_10MS;
				}
			}

			if (bins_to_go > 0) {
				--bins_to_go;
				{
					static uint16_t count = 0;
					static dcf77_data_e decoded_data = 0;

					count += sig->params.tick.bin;
					if (bins_to_go > BINS_PER_100MS) {
						if (bins_to_go == BINS_PER_100MS + 1) {
							decoded_data = count > (DCF77_BIN_EVERY * BINS_PER_50MS) ? dcf77_zero : dcf77_sync_mark;
							count = 0;
						}
					} else {
						if (bins_to_go == 0) {
							decoded_data += count > (DCF77_BIN_EVERY * BINS_PER_50MS) ? 1 : 0;
							count = 0;

							esm_signal_t sig = {
									.type = esm_sig_dcf77_data,
									.sender = (void*)0,
									.receiver = led_esm,
									.params.dcf77_data.data = decoded_data
							};
							esm_send_signal(&sig);
						}
					}
				}
			}
		}
	}
	break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_phase_init(esm_t *const esm)
{
	ESM_TRANSITION(idle);
}

static const phase_cfg_t phase_cfg = {
};

ESM_REGISTER(phase, phase, esm_gr_none, 1);
