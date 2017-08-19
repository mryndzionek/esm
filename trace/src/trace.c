#include "trace.h"
#include "esm/esm_timer.h"
#include "rb.h"

#define END_FLAG           (0x7E)
#define ESCAPE_BYTE        (0x7D)

#define ENCODE_NUM_1(_b, _v) \
		if (((_v & 0xFF) ^ ESCAPE_BYTE) && ((_v & 0xFF) ^ END_FLAG)) { \
			*(_b++) = _v; \
		} else { \
			*(_b++) = ESCAPE_BYTE; \
			*(_b++) = _v ^ 0x20; \
		}

#define ENCODE_NUM_2(_b, _v) \
		ENCODE_NUM_1(_b, _v); \
		_v >>= 8; \
		ENCODE_NUM_1(_b, (_v));

#define ENCODE_NUM_4(_b, _v) \
		ENCODE_NUM_2(_b, (_v)); \
		_v >>= 8; \
		ENCODE_NUM_2(_b, (_v));

#define crc_init()
#define crc_deinit()
#define crc_setup uint8_t crc = 0
#define crc_update(_data) (crc += (_data))
#define crc_finish() (crc = ~crc)
#define crc_get() (crc)

static rb_t _rb = { .data_ = (uint8_t[ESM_TRACE_BUF_SIZE]){0}, .capacity_ = ESM_TRACE_BUF_SIZE};
static uint8_t trace_fsm_counter;

static uint8_t _crc(uint8_t *data, uint8_t len)
{
	uint8_t i;
	crc_setup;

	for(i = 0; i < len; i++)
	{
		if(data[i] == ESCAPE_BYTE)
		{
			crc_update(data[++i] ^ 0x20);
		}
		else
		{
			crc_update(data[i]);
		}
	}
	return crc_get();
}

static uint8_t *_add_header(uint8_t *b, uint8_t uid)
{
	uint32_t ts = esm_global_time;

	ENCODE_NUM_1(b, trace_fsm_counter);
	trace_fsm_counter++;
	ENCODE_NUM_1(b, uid);
	ENCODE_NUM_4(b, ts);

	return b;
}

static uint8_t *_add_str(uint8_t *bb, char const * s)
{
	uint8_t i = 16;
	uint8_t *b = bb + 1;

	do
	{
		ENCODE_NUM_1(b, *s);
		--i;
	} while(*(++s) && i);
	*bb = 16 - i;

	return b;
}

void trace_init(uint8_t esm)
{
	uint8_t tmp[15+2], *b = tmp;
	uint8_t crc = 0;

	b = _add_header(b, 0);
	ENCODE_NUM_1(b, esm);
	crc_init();
	crc = _crc(tmp, b-tmp);
	crc_finish();
	crc_deinit();
	ENCODE_NUM_1(b, crc);
	*(b++) = END_FLAG;
	(void)rb_write(&_rb, tmp, b-tmp);
}

void trace_trans(uint8_t esm, uint8_t sig, char const * const cs, char const * const ns)
{
	uint8_t tmp[15+4+32+32], *b = tmp;
	uint8_t crc = 0;

	b = _add_header(b, 1);
	ENCODE_NUM_1(b, esm);
	ENCODE_NUM_1(b, sig);
	b = _add_str(b, cs);
	b = _add_str(b, ns);
	crc_init();
	crc = _crc(tmp, b-tmp);
	crc_finish();
	crc_deinit();
	ENCODE_NUM_1(b, crc);
	*(b++) = END_FLAG;
	(void)rb_write(&_rb, tmp, b-tmp);
}

void trace_receive(uint8_t esm, uint8_t sig, char const * const cs)
{
	uint8_t tmp[15+4+32], *b = tmp;
	uint8_t crc = 0;

	b = _add_header(b, 2);
	ENCODE_NUM_1(b, esm);
	ENCODE_NUM_1(b, sig);
	b = _add_str(b, cs);
	crc_init();
	crc = _crc(tmp, b-tmp);
	crc_finish();
	crc_deinit();
	ENCODE_NUM_1(b, crc);
	*(b++) = END_FLAG;
	(void)rb_write(&_rb, tmp, b-tmp);
}

size_t trace_get(uint8_t *data, size_t bytes)
{
	return rb_read(&_rb, data, bytes);
}
