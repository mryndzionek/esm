#include "esm/hesm.h"
#include "esm/esm_timer.h"

#include <math.h>

#include "ds3231.h"
#include "sk6812.h"
#include "board.h"

#define NUM_MAJOR_STATES (5)

#define OFF (0x00000000)
#define RED (0x00AA0000)
#define GREEN (0x00001000)
#define ORANGE (0x00805000)
#define LIGHT_BLUE (0x00000010)
#define LIGHT_RED_1 (0x00010000)
#define LIGHT_RED_2 (0x00120000)

#define HOURS_COLOR LIGHT_RED_1
#define MINUTES_COLOR GREEN
#define SECONDS_COLOR RED
#define FIVE_TICK_COLOR LIGHT_RED_2
#define FIFTEEN_TICK_COLOR LIGHT_BLUE

#define FIRE_COLOR (ORANGE | 0x35)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

ESM_THIS_FILE;

typedef struct
{
	esm_state_t const *const states[NUM_MAJOR_STATES];
} clock_cfg_t;

typedef struct
{
	hesm_t esm;
	esm_timer_t timer;
	ds3231_time_t time;
	uint8_t i;
	uint16_t freq_hz;
	int params[16];
	clock_cfg_t const *const cfg;
} clock_esm_t;

ESM_COMPLEX_STATE(active, top, 1);
ESM_LEAF_STATE(time, active, 2);
ESM_LEAF_STATE(fire, active, 2);
ESM_LEAF_STATE(plasma, active, 2);
ESM_LEAF_STATE(off, active, 2);

ESM_COMPLEX_STATE(sunrise, active, 2);
ESM_LEAF_STATE(red, sunrise, 3);
ESM_LEAF_STATE(yellow, sunrise, 3);

static void leds_set_all(uint32_t color)
{
	for (uint8_t i = 0; i < SK6812_LEDS_NUM; i++)
	{
		sk6812_set_color(i, color);
	}
}

static void ds3231_init(void)
{
	uint8_t xferbuf[8] = {DS3231_CONTROL_REG};

	// // Turn on 1Hz RTC SQW output
	BOARD_I2C_TX(DS3231_I2C_ADDRESS, xferbuf, 1);
	BOARD_I2C_RX(DS3231_I2C_ADDRESS, &xferbuf[1], 1);

	xferbuf[1] &= ~(DS3231_CTRL_INTCN | DS3231_CTRL_RS1 | DS3231_CTRL_RS2);
	xferbuf[1] |= DS3231_CTRL_BBSQW;
	BOARD_I2C_TX(DS3231_I2C_ADDRESS, xferbuf, 2);

	// // set time - harcoded for now
	// xferbuf[0] = DS3231_TIME_CAL_REG;
	// xferbuf[1] = dectobcd(0);
	// xferbuf[2] = dectobcd(22);
	// xferbuf[3] = dectobcd(21);
	// xferbuf[4] = dectobcd(3);
	// xferbuf[5] = dectobcd(26);
	// xferbuf[6] = dectobcd(6) + 0x80;
	// xferbuf[7] = dectobcd(19);

	// BOARD_I2C_TX(DS3231_I2C_ADDRESS, xferbuf, 8);
}

static void ds3231_deinit(void)
{
	uint8_t xferbuf[2] = {DS3231_CONTROL_REG};

	// // Turn on 1Hz RTC SQW output
	BOARD_I2C_TX(DS3231_I2C_ADDRESS, xferbuf, 1);
	BOARD_I2C_RX(DS3231_I2C_ADDRESS, &xferbuf[1], 1);

	xferbuf[1] |= (DS3231_CTRL_INTCN);
	xferbuf[1] &= ~(DS3231_CTRL_BBSQW);
	BOARD_I2C_TX(DS3231_I2C_ADDRESS, xferbuf, 2);
}

static void ds3231_get_time(ds3231_time_t *time)
{
	uint8_t xferbuf[8] = {DS3231_CONTROL_REG};
	//read the time
	xferbuf[0] = DS3231_TIME_CAL_REG;
	BOARD_I2C_TX(DS3231_I2C_ADDRESS, xferbuf, 1);
	BOARD_I2C_RX(DS3231_I2C_ADDRESS, &xferbuf[1], 7);

	time->sec = bcdtodec(xferbuf[1]);
	time->min = bcdtodec(xferbuf[2]);
	time->hour = bcdtodec(xferbuf[3]);
}

static void render_time(ds3231_time_t const *time)
{
	sk6812_clear();

	// draw hours
	{
		uint8_t h = time->hour % 12;
		uint8_t hp = h == 0 ? 5 * 12 : 5 * h;
		for (uint8_t i = 0; i < 2 * hp; i++)
		{
			sk6812_set_color(i, HOURS_COLOR);
		}
	}

	// draw 5s ticks
	for (uint8_t i = 8; i < 2 * 60 + 2; i += 10)
	{
		sk6812_set_color(i, FIVE_TICK_COLOR);
		sk6812_set_color(i + 1, FIVE_TICK_COLOR);
	}

	// draw 15s ticks
	for (uint8_t i = 28; i < 2 * 60 + 2; i += 30)
	{
		sk6812_set_color(i, FIFTEEN_TICK_COLOR);
		sk6812_set_color(i + 1, FIFTEEN_TICK_COLOR);
	}

	// draw seconds hand
	{
		uint8_t sp = time->sec == 0 ? 59 : time->sec - 1;
		sk6812_set_color(2 * sp, SECONDS_COLOR);
		sk6812_set_color(2 * sp + 1, SECONDS_COLOR);
	}

	// draw minutes hand
	{
		uint8_t mp = time->min == 0 ? 59 : time->min - 1;
		sk6812_set_color(2 * mp, MINUTES_COLOR);
		sk6812_set_color(2 * mp + 1, MINUTES_COLOR);
	}
}

void fire_flicker(int rev_intensity)
{
	uint8_t r = (FIRE_COLOR >> 16) & 0xFF;
	uint8_t g = (FIRE_COLOR >> 8) & 0xFF;
	uint8_t b = (FIRE_COLOR & 0xFF);
	uint8_t lum = MAX(r, MAX(g, b)) / rev_intensity;
	for (uint16_t i = 0; i < SK6812_LEDS_NUM; i++)
	{
		int flicker = platform_rnd(lum);
		sk6812_set_rgb(i, MAX(r - flicker, 0), MAX(g - flicker, 0), MAX(b - flicker, 0));
	}
}

static uint8_t sin8(uint8_t theta)
{
	static const uint8_t b_m16_interleave[] = {0, 49, 49, 41, 90, 27, 117, 10};
	uint8_t offset = theta;

	if (theta & 0x40)
	{
		offset = (uint8_t)255 - offset;
	}
	offset &= 0x3F; // 0..63

	uint8_t secoffset = offset & 0x0F; // 0..15
	if (theta & 0x40)
		secoffset++;

	uint8_t section = offset >> 4; // 0..3
	uint8_t s2 = section * 2;
	const uint8_t *p = b_m16_interleave;
	p += s2;
	uint8_t b = *p;
	p++;
	uint8_t m16 = *p;

	uint8_t mx = (m16 * secoffset) >> 4;

	int8_t y = mx + b;
	if (theta & 0x80)
		y = -y;

	y += 128;

	return y;
}

static uint8_t cos8(uint8_t theta)
{
	return sin8(theta + 64);
}

static uint16_t beat88(uint16_t beats_per_minute_88, uint32_t timebase)
{
    return ((esm_global_time - timebase) * beats_per_minute_88 * 280) >> 16;
}

static uint16_t beat16(uint16_t beats_per_minute, uint32_t timebase)
{
    if( beats_per_minute < 256) beats_per_minute <<= 8;
    return beat88(beats_per_minute, timebase);
}

static uint8_t beat8(uint16_t beats_per_minute, uint32_t timebase)
{
    return beat16( beats_per_minute, timebase) >> 8;
}

static inline __attribute__((always_inline)) uint8_t scale8(uint8_t i, uint8_t scale)
{
	return (((uint16_t)i) * (1 + (uint16_t)(scale))) >> 8;
}

static inline __attribute__((always_inline)) uint8_t scale8_LEAVING_R1_DIRTY(uint8_t i, uint8_t scale)
{
	return (((uint16_t)i) * ((uint16_t)(scale) + 1)) >> 8;
}

static uint8_t beatsin8(uint16_t beats_per_minute, uint8_t lowest, uint8_t highest)
{
	uint32_t timebase = 0;
	uint8_t phase_offset = 0;
	uint8_t beat = beat8(beats_per_minute, timebase);
	uint8_t beatsin = sin8(beat + phase_offset);
	uint8_t rangewidth = highest - lowest;
	uint8_t scaledbeat = scale8(beatsin, rangewidth);
	uint8_t result = lowest + scaledbeat;
	return result;
}

static uint8_t ease8InOutCubic(uint8_t i)
{
    uint8_t ii  = scale8_LEAVING_R1_DIRTY(  i, i);
    uint8_t iii = scale8_LEAVING_R1_DIRTY( ii, i);

    uint16_t r1 = (3 * (uint16_t)(ii)) - ( 2 * (uint16_t)(iii));

    /* the code generated for the above *'s automatically
       cleans up R1, so there's no need to explicitily call
       cleanup_R1(); */

    uint8_t result = r1;

    if( r1 & 0x100 ) {
        result = 255;
    }
    return result;
}

static uint8_t triwave8(uint8_t in)
{
	if (in & 0x80)
	{
		in = 255 - in;
	}
	uint8_t out = in << 1;
	return out;
}

static uint8_t cubicwave8(uint8_t in)
{
    return ease8InOutCubic(triwave8( in));
}

static uint32_t color_wheel(uint8_t pos)
{
	pos = 255 - pos;
	if (pos < 85)
	{
		return ((uint32_t)(255 - pos * 3) << 16) | ((uint32_t)(0) << 8) | (pos * 3);
	}
	else if (pos < 170)
	{
		pos -= 85;
		return ((uint32_t)(0) << 16) | ((uint32_t)(pos * 3) << 8) | (255 - pos * 3);
	}
	else
	{
		pos -= 170;
		return ((uint32_t)(pos * 3) << 16) | ((uint32_t)(255 - pos * 3) << 8) | (0);
	}
}

#define qsuba(x, b) ((x > b) ? x - b : 0)
#define pproc(x, b) ((x > b) ? x - b : 2)
static void plasma(void)
{
	int thisPhase = beatsin8(4, -64, 64);
	int thatPhase = beatsin8(5, -64, 64);

	for (int k = 0; k < SK6812_LEDS_NUM; k++)
	{
		int colorIndex = cubicwave8((k * 3) + thisPhase) / 2 + cos8((k * 4) + thatPhase) / 2;
		int thisBright = qsuba(colorIndex, beatsin8(7, 0, 128));

		uint32_t c = color_wheel(colorIndex % 256);
		uint8_t r = (c >> 16) & 0xFF;
		uint8_t g = (c >> 8) & 0xFF;
		uint8_t b = c & 0xFF;

		sk6812_set_rgb(k, r, pproc(g, thisBright), pproc(b, thisBright));
	}
}

static void esm_active_init(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	self->freq_hz = 10UL;
	self->i = 0;
	//esm->next_state = self->cfg->states[self->i];
	ESM_TRANSITION(plasma);
}

static void esm_active_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_active_exit(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	esm_timer_rm(&self->timer);
}

static void esm_active_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);

	switch (sig->type)
	{

	case esm_sig_button:
	{
		self->i = (self->i + 1) % NUM_MAJOR_STATES;
		esm->next_state = self->cfg->states[self->i];
	}
	break;

	case esm_sig_alarm:
		break;

	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_time_entry(esm_t *const esm)
{
	(void)esm;
	ds3231_init();
}

static void esm_time_exit(esm_t *const esm)
{
	(void)esm;
	ds3231_deinit();
}

static void esm_time_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);

	switch (sig->type)
	{
	case esm_sig_alarm:
	{
		ds3231_get_time(&self->time);
		// alarm - hardcoded for now to 04:00
		if ((self->time.hour == 4) && (self->time.min == 0))
		{
			ESM_TRANSITION(sunrise);
		}
		else
		{
			render_time(&self->time);
			esm_signal_t s = {
				.type = esm_sig_alarm,
				.sender = NULL,
				.receiver = strip1_esm};
			esm_send_signal(&s);
		}
	}
	break;

	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_sunrise_init(esm_t *const esm)
{
	(void)esm;
	ESM_TRANSITION(red);
}

static void esm_sunrise_entry(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);

	ds3231_init();
	self->params[0] = 0;
	sk6812_clear();
	esm_signal_t s = {
		.type = esm_sig_alarm,
		.sender = NULL,
		.receiver = strip1_esm};
	esm_send_signal(&s);
}

static void esm_sunrise_exit(esm_t *const esm)
{
	(void)esm;
	ds3231_deinit();
}

static void esm_sunrise_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	(void)esm;

	switch (sig->type)
	{
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_red_entry(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	self->params[1] = 0;
}

static void esm_red_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_red_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);

	switch (sig->type)
	{
	case esm_sig_alarm:
	{
		self->params[0] = (self->params[0] + 1) % 3;
		if (self->params[0] == 0)
		{
			if (self->params[1] < 255)
			{
				self->params[1]++;
				leds_set_all(((uint32_t)self->params[1]) << 16);
				esm_signal_t s = {
					.type = esm_sig_alarm,
					.sender = NULL,
					.receiver = strip1_esm};
				esm_send_signal(&s);
			}
			else
			{
				ESM_TRANSITION(yellow);
			}
		}
	}
	break;

	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_yellow_entry(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	self->params[1] = 0;
}

static void esm_yellow_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_yellow_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);

	switch (sig->type)
	{
	case esm_sig_alarm:
	{
		self->params[0] = (self->params[0] + 1) % 3;
		if (self->params[0] == 0)
		{
			if (self->params[1] < 128)
			{
				self->params[1]++;
				leds_set_all(((uint32_t)self->params[1] << 8) | (0x00FF0000));
				esm_signal_t s = {
					.type = esm_sig_alarm,
					.sender = NULL,
					.receiver = strip1_esm};
				esm_send_signal(&s);
			}
		}
	}
	break;

	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_fire_entry(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);

	self->freq_hz = 10UL + platform_rnd(25);

	esm_signal_t sig = {
		.type = esm_sig_tmout,
		.sender = esm,
		.receiver = esm};
	esm_timer_add(&self->timer,
				  1000UL / self->freq_hz, &sig);
}

static void esm_fire_exit(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	esm_timer_rm(&self->timer);
}

static void esm_fire_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	(void)esm;

	switch (sig->type)
	{
	case esm_sig_tmout:
	{
		fire_flicker(1.7);
		esm_signal_t s = {
			.type = esm_sig_alarm,
			.sender = NULL,
			.receiver = strip1_esm};
		esm_send_signal(&s);
		ESM_TRANSITION(self);
	}
	break;

	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_plasma_entry(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);

	self->freq_hz = 30UL;

	esm_signal_t sig = {
		.type = esm_sig_tmout,
		.sender = esm,
		.receiver = esm};
	esm_timer_add(&self->timer,
				  1000UL / self->freq_hz, &sig);
}

static void esm_plasma_exit(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	esm_timer_rm(&self->timer);
}

static void esm_plasma_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	(void)esm;

	switch (sig->type)
	{
	case esm_sig_tmout:
	{
		plasma();
		esm_signal_t s = {
			.type = esm_sig_alarm,
			.sender = NULL,
			.receiver = strip1_esm};
		esm_send_signal(&s);
		ESM_TRANSITION(self);
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
	sk6812_clear();
	esm_signal_t s = {
		.type = esm_sig_alarm,
		.sender = NULL,
		.receiver = strip1_esm};
	esm_send_signal(&s);
}

static void esm_off_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_off_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	(void)esm;

	switch (sig->type)
	{
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_clock_init(esm_t *const esm)
{
	sk6812_clear();
	ds3231_deinit();

	ESM_TRANSITION(active);
}

static const clock_cfg_t clock1_cfg = {
	.states = {
		(esm_state_t const *const) & esm_time_state,
		(esm_state_t const *const) & esm_sunrise_state,
		(esm_state_t const *const) & esm_fire_state,
		(esm_state_t const *const) & esm_plasma_state,
		(esm_state_t const *const) & esm_off_state,
	}};

ESM_COMPLEX_REGISTER(clock, clock1, esm_gr_clocks, 3, 4, 0);
