#include <string.h>
#include <pthread.h>

#include "board.h"
#include "esm/esm.h"
#include "debouncer.h"
#include "backlight.h"
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

const uint32_t board_heat_colormap[256] =
    {
        0x0,
        0x70000,
        0xD0000,
        0x120000,
        0x160000,
        0x190000,
        0x1C0000,
        0x1F0000,
        0x220000,
        0x240000,
        0x260000,
        0x280000,
        0x2A0000,
        0x2C0000,
        0x2E0000,
        0x300000,
        0x320000,
        0x330000,
        0x350000,
        0x360000,
        0x380000,
        0x390000,
        0x3B0000,
        0x3C0000,
        0x3E0000,
        0x3F0000,
        0x400000,
        0x420100,
        0x430100,
        0x450100,
        0x460100,
        0x480100,
        0x490100,
        0x4B0100,
        0x4C0100,
        0x4E0100,
        0x4F0100,
        0x510100,
        0x520100,
        0x540100,
        0x550100,
        0x570100,
        0x580100,
        0x5A0100,
        0x5B0100,
        0x5D0100,
        0x5E0100,
        0x600100,
        0x610100,
        0x630100,
        0x640100,
        0x660100,
        0x670100,
        0x690100,
        0x6B0200,
        0x6C0200,
        0x6E0200,
        0x6F0200,
        0x710200,
        0x720200,
        0x740200,
        0x760200,
        0x770200,
        0x790200,
        0x7A0200,
        0x7C0200,
        0x7E0200,
        0x7F0200,
        0x810200,
        0x830200,
        0x840300,
        0x860300,
        0x870300,
        0x890300,
        0x8B0300,
        0x8C0300,
        0x8E0300,
        0x900300,
        0x910300,
        0x930300,
        0x950300,
        0x960400,
        0x980400,
        0x9A0400,
        0x9B0400,
        0x9D0400,
        0x9F0400,
        0xA00400,
        0xA20400,
        0xA40400,
        0xA50500,
        0xA70500,
        0xA90500,
        0xAB0500,
        0xAC0500,
        0xAE0500,
        0xB00600,
        0xB10600,
        0xB30600,
        0xB50600,
        0xB70600,
        0xB80600,
        0xBA0700,
        0xBC0700,
        0xBD0700,
        0xBF0700,
        0xC10700,
        0xC30800,
        0xC40800,
        0xC60800,
        0xC80800,
        0xCA0900,
        0xCB0900,
        0xCD0900,
        0xCF0900,
        0xD10A00,
        0xD20A00,
        0xD40A00,
        0xD60B00,
        0xD80B00,
        0xD90C00,
        0xDB0C00,
        0xDD0C00,
        0xDF0D00,
        0xE00D00,
        0xE20E00,
        0xE40F00,
        0xE60F00,
        0xE71000,
        0xE91100,
        0xEB1200,
        0xEC1300,
        0xEE1500,
        0xEF1800,
        0xF01A00,
        0xF11D00,
        0xF22100,
        0xF32400,
        0xF42700,
        0xF42A00,
        0xF52E00,
        0xF63100,
        0xF63400,
        0xF73700,
        0xF73A00,
        0xF73D00,
        0xF83F00,
        0xF84200,
        0xF84500,
        0xF94800,
        0xF94A00,
        0xF94D00,
        0xF94F00,
        0xFA5200,
        0xFA5400,
        0xFA5600,
        0xFA5900,
        0xFA5B00,
        0xFB5D00,
        0xFB6000,
        0xFB6200,
        0xFB6400,
        0xFB6600,
        0xFB6800,
        0xFC6A00,
        0xFC6C00,
        0xFC6E00,
        0xFC7000,
        0xFC7200,
        0xFC7400,
        0xFC7600,
        0xFC7800,
        0xFC7A00,
        0xFC7C00,
        0xFD7E00,
        0xFD8000,
        0xFD8200,
        0xFD8300,
        0xFD8500,
        0xFD8700,
        0xFD8900,
        0xFD8B00,
        0xFD8C00,
        0xFD8E00,
        0xFD9000,
        0xFD9200,
        0xFD9300,
        0xFE9500,
        0xFE9700,
        0xFE9900,
        0xFE9A00,
        0xFE9C00,
        0xFE9E00,
        0xFE9F00,
        0xFEA100,
        0xFEA300,
        0xFEA400,
        0xFEA600,
        0xFEA800,
        0xFEA900,
        0xFEAB00,
        0xFEAD00,
        0xFEAE00,
        0xFEB000,
        0xFEB100,
        0xFEB300,
        0xFEB500,
        0xFEB600,
        0xFEB800,
        0xFEB900,
        0xFEBB00,
        0xFEBD00,
        0xFFBE00,
        0xFFC000,
        0xFFC100,
        0xFFC300,
        0xFFC400,
        0xFFC600,
        0xFFC700,
        0xFFC900,
        0xFFCB00,
        0xFFCC00,
        0xFFCE00,
        0xFFCF00,
        0xFFD100,
        0xFFD200,
        0xFFD400,
        0xFFD500,
        0xFFD700,
        0xFFD800,
        0xFFDA00,
        0xFFDB00,
        0xFFDD00,
        0xFFDE00,
        0xFFE000,
        0xFFE100,
        0xFFE300,
        0xFFE400,
        0xFFE600,
        0xFFE700,
        0xFFE900,
        0xFFEA00,
        0xFFEC00,
        0xFFED00,
        0xFFEF00,
        0xFFF000,
        0xFFF200,
        0xFFF300,
        0xFFF500,
        0xFFF600,
        0xFFF800,
        0xFFF900,
        0xFFFB00,
        0xFFFC00,
        0xFFFE00,
        0xFFFF00,
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

void board_ledpos_to_xy(uint8_t p, uint8_t *xp, uint8_t *yp)
{
	*xp = p;
	*yp = 0;
}

static const backlight_cfg_t backlight_cfg = {
    .freq_hz = 10UL};

ESM_REGISTER(backlight, backlight, esm_gr_none, 8, 0);
