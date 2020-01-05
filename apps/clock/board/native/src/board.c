#include <string.h>
#include <pthread.h>

#include "board.h"
#include "esm/esm.h"
#include "debouncer.h"
#include "keycodes.h"
#include "sox.h"

ESM_THIS_FILE;

NEC_t nec1 = {
	.timingBitBoundary = 1680,
	.timingAgcBoundary = 12500,
	.type = NEC_NOT_EXTENDED,
};

struct sparams
{
	uint16_t freq;
	uint32_t dur;
};

static bool armed = true;
static uint8_t ds3231_i2c_rx_buf[8];
static sox_format_t *in, *out;
static pthread_t sthr;

void app_process(char key)
{
	switch (key)
	{
	case ' ':
	{
		if (armed)
		{
			armed = false;
			esm_signal_t sig = {
				.type = esm_sig_alarm,
				.sender = NULL,
				.receiver = debouncer_esm};
			esm_send_signal(&sig);
		}
	}
	break;

	case 'p':
	{
		esm_signal_t s = {
			.type = esm_sig_remote,
			.params.keycode = KEYCODE_PLAY,
			.sender = NULL,
			.receiver = NULL};
		esm_broadcast_signal(&s, esm_gr_remote);
	}
	break;

	case 'n':
	{
		esm_signal_t s = {
			.type = esm_sig_remote,
			.params.keycode = KEYCODE_NEXT,
			.sender = NULL,
			.receiver = NULL};
		esm_broadcast_signal(&s, esm_gr_remote);
	}
	break;
	}
}

static void debouncer_handle(esm_t *const esm, BOARD_DEBOUNCER_STATE state)
{
	(void)esm;
	(void)state;

	esm_signal_t s = {
		.type = esm_sig_button,
		.sender = NULL,
		.receiver = clock1_esm};
	esm_send_signal(&s);
}

static void debouncer_arm(esm_t *const esm)
{
	(void)esm;

	armed = true;
}

static const debouncer_cfg_t debouncer_cfg = {
	.period = 30UL,
	.handle = debouncer_handle,
	.arm = debouncer_arm};

ESM_REGISTER(debouncer, debouncer, esm_gr_none, 1, 1);

void board_nec_start(NEC_t *handle)
{
	handle->state = NEC_INIT;
}

void board_nec_stop(NEC_t *handle)
{
	(void)handle;
}

void board_i2c_tx(uint8_t addr, uint8_t const *data, uint8_t size)
{
	ESM_ASSERT(DS3231_I2C_ADDRESS == addr);
	ESM_ASSERT(size <= sizeof(ds3231_i2c_rx_buf));

	memcpy(ds3231_i2c_rx_buf, data, size);

	if (size > 1)
	{
		switch (ds3231_i2c_rx_buf[0])
		{
		case DS3231_CONTROL_REG:
			if (ds3231_i2c_rx_buf[1] & DS3231_CTRL_BBSQW)
			{
				esm_signal_t s = {
					.type = esm_sig_alarm,
					.sender = NULL,
					.receiver = ds3231_sm1_esm};
				esm_send_signal(&s);
			}
			break;

		case DS3231_TIME_CAL_REG:
			break;

		default:
			ESM_ASSERT(0);
		}
	}
}

void board_i2c_rx(uint8_t addr, uint8_t *data, uint8_t size)
{
	(void)data;
	ESM_ASSERT(DS3231_I2C_ADDRESS == addr);

	switch (ds3231_i2c_rx_buf[0])
	{
	case DS3231_CONTROL_REG:
		ESM_ASSERT(size == 1);
		data[0] = 0;
		break;

	case DS3231_TIME_CAL_REG:
		ESM_ASSERT(size == 7);
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);

		data[0] = dectobcd(tm.tm_sec);
		data[1] = dectobcd(tm.tm_min);
		data[2] = dectobcd(tm.tm_hour);
		data[3] = dectobcd(tm.tm_wday);
		data[4] = dectobcd(tm.tm_mday);
		data[5] = dectobcd(tm.tm_mon) + 0x80;
		data[6] = dectobcd(tm.tm_year);
		break;

	default:
		ESM_ASSERT(0);
	}
}

static void *_start_sound(void *data)
{
	sox_effects_chain_t *chain;
	sox_effect_t *e;
	enum sox_error_t s;
	struct sparams *p = data;
	char *args[3];
	char freq_s[16];
	char dur_s[16];
	int r;
	static bool first_run = true;

	if (first_run)
	{
		s = sox_init();
		ESM_ASSERT(s == SOX_SUCCESS);
		first_run = false;
	}

	in = sox_open_read("", NULL, NULL, "null");
	ESM_ASSERT(in);
	in->signal.channels = 1;

	out = sox_open_write("default", &in->signal, NULL, "alsa", NULL, NULL);
	ESM_ASSERT(out);

	chain = sox_create_effects_chain(&in->encoding, &out->encoding);
	ESM_ASSERT(chain);

	e = sox_create_effect(sox_find_effect("input"));
	ESM_ASSERT(e);
	args[0] = (char *)in;
	s = sox_effect_options(e, 1, args);
	ESM_ASSERT(s == SOX_SUCCESS);
	s = sox_add_effect(chain, e, &in->signal, &in->signal);
	ESM_ASSERT(s == SOX_SUCCESS);
	free(e);

	e = sox_create_effect(sox_find_effect("synth"));
	ESM_ASSERT(e);
	r = snprintf(freq_s, sizeof(freq_s), "%d", p->freq);
	ESM_ASSERT((r > 0) && (r < (int)sizeof(freq_s) - 1));

	r = snprintf(dur_s, sizeof(dur_s), "%f", (float)p->dur / 1000.0);
	ESM_ASSERT((r > 0) && (r < (int)sizeof(dur_s) - 1));

	args[0] = dur_s, args[1] = "sine", args[2] = freq_s;
	s = sox_effect_options(e, 3, args);
	ESM_ASSERT(s == SOX_SUCCESS);
	s = sox_add_effect(chain, e, &in->signal, &in->signal);
	ESM_ASSERT(s == SOX_SUCCESS);
	free(e);

	e = sox_create_effect(sox_find_effect("output"));
	ESM_ASSERT(e);
	args[0] = (char *)out;
	s = sox_effect_options(e, 1, args);
	ESM_ASSERT(s == SOX_SUCCESS);
	s = sox_add_effect(chain, e, &in->signal, &in->signal);
	ESM_ASSERT(s == SOX_SUCCESS);
	free(e);

	sox_flow_effects(chain, NULL, NULL);
	sox_delete_effects_chain(chain);
	sox_close(out);
	sox_close(in);

	pthread_exit(NULL);
}

void board_start_sound(uint16_t freq, uint32_t dur)
{
	int s;
	static struct sparams p;

	p.dur = dur > 50 ? dur - 50 : dur;
	p.freq = freq;

	s = pthread_create(&sthr, NULL, _start_sound, (void *)&p);
	ESM_ASSERT(s == 0);
}

void board_stop_sound(void)
{
	pthread_join(sthr, NULL);
}