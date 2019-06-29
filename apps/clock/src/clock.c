#include "esm/hesm.h"
#include "esm/esm_timer.h"

#include <math.h>

#include "ds3231.h"
#include "sk6812.h"
#include "board.h"

#define NUM_MAJOR_STATES (4)

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

static void fire_flicker(int rev_intensity)
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

static void esm_active_init(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	self->freq_hz = 10UL;
	self->i = 0;
	ESM_TRANSITION(time);
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
		(esm_state_t const *const) & esm_off_state,
	}};

ESM_COMPLEX_REGISTER(clock, clock1, esm_gr_clocks, 3, 4, 0);
