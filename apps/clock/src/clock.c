#include "esm/hesm.h"
#include "esm/esm_timer.h"

#include <math.h>
#include <string.h>

#include "sk6812.h"
#include "keycodes.h"
#include "board.h"

#define NUM_MAJOR_STATES (5)

#define OFF (0x00000000)
#define RED (0x00AA0000)
#define GREEN (0x0000AA00)
#define ORANGE (0x00FFA000)
#define LIGHT_BLUE (0x00000008)
#define LIGHT_RED_1 (0x00010000)
#define LIGHT_RED_2 (0x000a0000)
#define LIGHT_GREEN (0x00000200)

#define HOURS_COLOR ORANGE
#define MINUTES_COLOR GREEN
#define SECONDS_COLOR RED
#define FIVE_TICK_COLOR LIGHT_RED_2
#define FIFTEEN_TICK_COLOR LIGHT_BLUE

#define INIT_LAMP_COLOR (0x00FFFFFF)

#define FIRE_COLOR (ORANGE | 0x15)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define SELF_TO_COLOR(_s) ((bmap[_s->rgb.r] << 16) | (bmap[_s->rgb.g] << 8) | (bmap[_s->rgb.b]))

extern const uint8_t bmap[256];

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
	uint8_t sn;
	uint16_t freq_hz;
	int brightness;
	size_t i;
	size_t j;
	union {
		struct
		{
			uint8_t b;
			uint8_t g;
			uint8_t r;
		} rgb;
		uint32_t c;
	};
	clock_cfg_t const *const cfg;
} clock_esm_t;

static const uint32_t sunrise_lut[] = {
	65536, 131072, 196608, 262144, 327680, 393216, 458752, 524288, 589824, 655360, 720896, 786432, 851968, 917504, 983040, 1048576, 1114112, 1179648,
	1245184, 1310720, 1376256, 1441792, 1507328, 1572864, 1638400, 1703936, 1769472, 1835008, 1900544, 1966080, 2031616, 2097152, 2162688, 2228224,
	2293760, 2359296, 2424832, 2490368, 2555904, 2621440, 2686976, 2752512, 2818048, 2883584, 2949120, 3014656, 3080192, 3145728, 3211264, 3276800,
	3342336, 3407872, 3473408, 3538944, 3604480, 3670016, 3735552, 3801088, 3866624, 3932160, 3997696, 4063232, 4128768, 4194304, 4259840, 4325376,
	4390912, 4456448, 4521984, 4587520, 4653056, 4718592, 4784128, 4849664, 4915200, 4980736, 5046272, 5111808, 5177344, 5242880, 5308416, 5373952,
	5439488, 5505024, 5570560, 5636096, 5701632, 5767168, 5832704, 5898240, 5963776, 6029312, 6094848, 6160384, 6225920, 6291456, 6356992, 6422528,
	6488064, 6553600, 6619136, 6684672, 6750208, 6815744, 6881280, 6946816, 7012352, 7077888, 7143424, 7208960, 7274496, 7340032, 7405568, 7471104,
	7536640, 7602176, 7667712, 7733248, 7798784, 7864320, 7929856, 7995392, 8060928, 8126464, 8192000, 8257536, 8323072, 8388608, 8454400, 8520192,
	8585984, 8651776, 8717568, 8783360, 8849152, 8914944, 8980736, 9046528, 9112320, 9178112, 9243904, 9309696, 9375488, 9441280, 9507072, 9572864,
	9638656, 9704448, 9770240, 9836032, 9901824, 9967616, 10033408, 10099200, 10164992, 10230784, 10296576, 10362368, 10428160, 10493952, 10559744,
	10625536, 10691328, 10757120, 10822912, 10888704, 10954496, 11020288, 11086080, 11151872, 11217664, 11283456, 11349248, 11415040, 11480832,
	11546624, 11612416, 11678208, 11744000, 11809792, 11875584, 11941376, 12007168, 12072960, 12138752, 12204544, 12270336, 12336128, 12401920,
	12467712, 12533504, 12599296, 12665088, 12730880, 12796672, 12862464, 12928256, 12994048, 13059840, 13125632, 13191424, 13257216, 13323008,
	13388800, 13454592, 13520384, 13586176, 13651968, 13717760, 13783552, 13849344, 13915136, 13980928, 14046720, 14112512, 14178304, 14244096,
	14309888, 14375680, 14441472, 14507264, 14573056, 14638848, 14704640, 14770432, 14836224, 14902016, 14967808, 15033600, 15099392, 15165184,
	15230976, 15296768, 15362560, 15428352, 15494144, 15559936, 15625728, 15691520, 15757312, 15823104, 15888896, 15954688, 16020480, 16086272,
	16152064, 16217856, 16283648, 16349440, 16415232, 16481024, 16546816, 16612608, 16678400, 16744192, 16744448, 16744448, 16744448, 16744448,
	16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448,
	16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448, 16744448,
	16744449, 16744450, 16744451, 16744452, 16744453, 16744454, 16744455, 16744456, 16744457, 16744458, 16744459, 16744460, 16744461, 16744462,
	16744463, 16744464, 16744465, 16744466, 16744467, 16744468, 16744469, 16744470, 16744471, 16744472, 16744473, 16744474, 16744475, 16744476,
	16744477, 16744478, 16744479, 16744480, 16744481, 16744482, 16744483, 16744484, 16744485, 16744486, 16744487, 16744488, 16744489, 16744490,
	16744491, 16744492, 16744493, 16744494, 16744495, 16744496, 16744497, 16744498, 16744499, 16744500, 16744501, 16744502, 16744503, 16744504,
	16744505, 16744506, 16744507, 16744508, 16744509, 16744510, 16744511, 16744512, 16744513, 16744514, 16744515, 16744516, 16744517, 16744518,
	16744519, 16744520, 16744521, 16744522, 16744523, 16744524, 16744525, 16744526, 16744527, 16744528, 16744529, 16744530, 16744531, 16744532,
	16744533, 16744534, 16744535, 16744536, 16744537, 16744538, 16744539, 16744540, 16744541, 16744542, 16744543, 16744544, 16744545, 16744546,
	16744547, 16744548, 16744549, 16744550, 16744551, 16744552, 16744553, 16744554, 16744555, 16744556, 16744557, 16744558, 16744559, 16744560,
	16744561, 16744562, 16744563, 16744564, 16744565, 16744566, 16744567, 16744568, 16744569, 16744570, 16744571, 16744572, 16744573, 16744574,
	16744575, 16744576};

ESM_COMPLEX_STATE(active, top, 1);
ESM_LEAF_STATE(time, active, 2);
ESM_LEAF_STATE(fire, active, 2);
ESM_LEAF_STATE(plasma, active, 2);
ESM_LEAF_STATE(lamp, active, 2);

ESM_COMPLEX_STATE(sunrise, active, 2);
ESM_LEAF_STATE(rising, sunrise, 3);
ESM_LEAF_STATE(finished, sunrise, 3);

static void leds_set_all(uint32_t color)
{
	for (uint8_t i = 0; i < SK6812_LEDS_NUM; i++)
	{
		sk6812_set_color(i, color);
	}
}

static void render_time(ds3231_time_t const *time)
{
	uint8_t mp = time->min == 0 ? 59 : time->min - 1;

	sk6812_clear();

	for (uint8_t i = 0; i < 2 * mp; i++)
	{
		sk6812_set_color(i, LIGHT_GREEN);
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

		for (uint8_t i = 0; i < 6; i++)
		{
			if (sp & (1UL << i))
			{
				sk6812_set_color(SK6812_LEDS_NUM - 1 - i, SECONDS_COLOR);
			}
		}
	}

	// draw minutes hand
	{
		sk6812_set_color(2 * mp, MINUTES_COLOR);
		sk6812_set_color(2 * mp + 1, MINUTES_COLOR);
	}

	// draw hours
	{
		uint8_t h = time->hour % 12;
		uint8_t hp = h == 0 ? 5 * 12 - 1 : 5 * h - 1;
		sk6812_set_color(2 * hp, HOURS_COLOR);
		sk6812_set_color(2 * hp + 1, HOURS_COLOR);
	}
}

void fire_flicker(int rev_intensity, uint32_t c)
{
	uint8_t r = (c >> 16) & 0xFF;
	uint8_t g = (c >> 8) & 0xFF;
	uint8_t b = (c & 0xFF);
	uint8_t lum = MAX(r, MAX(g, b)) / rev_intensity;
	for (uint16_t i = 0; i < SK6812_LEDS_NUM; i++)
	{
		int flicker = ESM_RANDOM(lum);
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
	int thisPhase = beatsin8(2, -64, 64);
	int thatPhase = beatsin8(3, -64, 64);

	for (int k = 0; k < SK6812_LEDS_NUM; k++)
	{
		int colorIndex = cubicwave8((k * 2) + thisPhase) / 2 + cos8((k * 1) + thatPhase) / 2;
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
	self->sn = 0;
	esm->next_state = self->cfg->states[self->sn];
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
		self->sn = (self->sn + 1) > NUM_MAJOR_STATES - 1 ? 0 : (self->sn + 1);
		esm->next_state = self->cfg->states[self->sn];
	}
	break;

	case esm_sig_remote:
	{
		switch(sig->params.keycode)
		{
			case KEYCODE_CH_PLUS:
				self->sn = (self->sn + 1) >= NUM_MAJOR_STATES ? 0 : (self->sn + 1);
				esm->next_state = self->cfg->states[self->sn];
				break;

			case KEYCODE_CH_MINUS:
				self->sn = (self->sn - 1) >= 0 ? (self->sn - 1) : NUM_MAJOR_STATES - 1;
				esm->next_state = self->cfg->states[self->sn];
				break;

			case KEYCODE_CH:
				self->sn = 0;
				esm->next_state = self->cfg->states[self->sn];
				break;

			case KEYCODE_PLUS: {
				self->brightness = (self->brightness + 8) >= 255 ? 255 : (self->brightness + 8);
				sk6812_set_brightness(self->brightness);
			}
			break;

			case KEYCODE_MINUS:
			{
				self->brightness = (self->brightness - 8) <= 0 ? 0 : (self->brightness - 8);
				sk6812_set_brightness(self->brightness);
			}
			break;

			case KEYCODE_EQ:
			{
				self->brightness = 0;
				sk6812_set_brightness(self->brightness);
			}
			break;

			case KEYCODE_100:
			{
				self->brightness = 128;
				sk6812_set_brightness(self->brightness);
			}
			break;

			case KEYCODE_200:
			{
				self->brightness = 255;
				sk6812_set_brightness(self->brightness);
			}
			break;

			case KEYCODE_1:
			{
				self->rgb.r = self->rgb.r < (255 - 8) ? self->rgb.r + 8 : 255;
			}
			break;

			case KEYCODE_4:
			{
				self->rgb.r = self->rgb.r > 8 ? self->rgb.r - 8 : 0;
			}
			break;

			case KEYCODE_2:
			{
				self->rgb.g = self->rgb.g < (255 - 8) ? self->rgb.g + 8 : 255;
			}
			break;

			case KEYCODE_5:
			{
				self->rgb.g = self->rgb.g > 8 ? self->rgb.g - 8 : 0;
			}
			break;

			case KEYCODE_3:
			{
				self->rgb.b = self->rgb.b < (255 - 8) ? self->rgb.b + 8 : 255;
			}
			break;

			case KEYCODE_6:
			{
				self->rgb.b = self->rgb.b > 8 ? self->rgb.b - 8 : 0;
			}
			break;
		}
	}
	break;

	case esm_sig_rtc:
		memcpy(&self->time, sig->params.time, sizeof(ds3231_time_t));
		if (sig->params.time->alarm)
		{
			ESM_TRANSITION(sunrise);
		}
		break;

	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_time_entry(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	{
		esm_signal_t s = {
			.type = esm_sig_rtc,
			.params.time = &self->time,
			.sender = NULL,
			.receiver = esm};
		esm_send_signal(&s);
	}
}

static void esm_time_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_time_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	(void)esm;

	switch (sig->type)
	{
	case esm_sig_rtc:
	{
		render_time(sig->params.time);
		{
			esm_signal_t s = {
				.type = esm_sig_alarm,
				.sender = NULL,
				.receiver = strip1_esm};
			esm_send_signal(&s);
		}
	}

	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_sunrise_init(esm_t *const esm)
{
	(void)esm;
	ESM_TRANSITION(rising);
}

static void esm_sunrise_entry(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);

	sk6812_set_brightness(255);

	self->i = 0;
	self->j = 0;
	leds_set_all(sunrise_lut[self->i++]);
	esm_signal_t s = {
		.type = esm_sig_alarm,
		.sender = NULL,
		.receiver = strip1_esm};
	esm_send_signal(&s);
}

static void esm_sunrise_exit(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	sk6812_set_brightness(self->brightness);
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

static void esm_rising_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_rising_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_rising_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);

	switch (sig->type)
	{
	case esm_sig_rtc:
	{
		self->j = (self->j + 1) % 4;
		if (self->j == 0)
			{
				if (self->i == (sizeof(sunrise_lut) / sizeof(sunrise_lut[0])))
				{
					ESM_TRANSITION(finished);
				}
				else
				{
					leds_set_all(sunrise_lut[self->i++]);
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

static void esm_finished_entry(esm_t *const esm)
{
	(void)esm;
	esm_signal_t s = {
		.type = esm_sig_play,
		.sender = NULL,
		.receiver = player1_esm};
	esm_send_signal(&s);
}

static void esm_finished_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_finished_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	(void)esm;

	switch (sig->type)
	{
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_fire_entry(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);

	self->freq_hz = 10UL + ESM_RANDOM(25);
	self->c = FIRE_COLOR;

	esm_signal_t sig = {
		.type = esm_sig_tmout,
		.sender = esm,
		.receiver = esm};
	esm_send_signal(&sig);
}

static void esm_fire_exit(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	esm_timer_rm(&self->timer);
}

static void esm_fire_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	static float intensity = 1.7;

	switch (sig->type)
	{

	case esm_sig_remote:
	{
		switch (sig->params.keycode)
		{
		case KEYCODE_8:
			if (intensity < 6)
			{
				intensity++;
			}
			break;

		case KEYCODE_7:
			if (intensity > 2)
			{
				intensity--;
			}
			break;

		default:
			ESM_TRANSITION(unhandled);
			break;
		}
		break;
	}
	break;

	case esm_sig_tmout:
	{
		fire_flicker(intensity, SELF_TO_COLOR(self));
		{
			esm_signal_t s = {
				.type = esm_sig_alarm,
				.sender = NULL,
				.receiver = strip1_esm};
			esm_send_signal(&s);
		}
		{
			esm_signal_t s = {
				.type = esm_sig_tmout,
				.sender = esm,
				.receiver = esm};
			esm_timer_add(&self->timer,
						  1000UL / self->freq_hz, &s);
		}
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

	self->freq_hz = 25UL;

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

static void esm_lamp_entry(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	self->freq_hz = 10UL;
	self->c = INIT_LAMP_COLOR;

	esm_signal_t sig = {
		.type = esm_sig_tmout,
		.sender = esm,
		.receiver = esm};
	esm_send_signal(&sig);
}

static void esm_lamp_exit(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);
	esm_timer_rm(&self->timer);
}

static void esm_lamp_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);

	switch (sig->type)
	{

	case esm_sig_tmout:
	{
		leds_set_all(SELF_TO_COLOR(self));
		{
			esm_signal_t s = {
				.type = esm_sig_alarm,
				.sender = NULL,
				.receiver = strip1_esm};
			esm_send_signal(&s);
		}
		{
			esm_signal_t s = {
				.type = esm_sig_tmout,
				.sender = esm,
				.receiver = esm};
			esm_timer_add(&self->timer,
						  1000UL / self->freq_hz, &s);
		}
	}
	break;

	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_clock_init(esm_t *const esm)
{
	clock_esm_t *self = ESM_CONTAINER_OF(esm, clock_esm_t, esm);

	self->brightness = 255;
	sk6812_set_brightness(self->brightness);
	sk6812_clear();
	board_nec_start(&nec1);

	ESM_TRANSITION(active);
}

static const clock_cfg_t clock1_cfg = {
	.states = {
		(esm_state_t const *const) & esm_time_state,
		(esm_state_t const *const) & esm_sunrise_state,
		(esm_state_t const *const) & esm_fire_state,
		(esm_state_t const *const) & esm_plasma_state,
		(esm_state_t const *const) & esm_lamp_state,
	}};

ESM_COMPLEX_REGISTER(clock, clock1, esm_gr_remote, 4, 4, 0);
