#include "esm/esm.h"
#include "esm/esm_timer.h"
#include "trace.h"

ESM_THIS_FILE;

bool esm_is_tracing;

void empty_en_ex(esm_t *const esm)
{
	(void)esm;
}

static void tick_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	switch(sig->type)
	{
	case esm_sig_alarm:
		esm_global_time++;
		if(esm_timer_next() == 0)
		{
			esm_timer_fire();
		}
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

const esm_state_t tick = {
		.entry = empty_en_ex,
		.handle = tick_handle,
		.exit = empty_en_ex,
		.name = "tick",
};

static esm_t esm_tick = {
		.name = "esm_tick",
		.id = esm_id_tick,
		.curr_state = &tick,
		.queue = {
				.size = 1,
				.data = (esm_signal_t[1]){0}
		}
};
esm_t * const tick_esm
__attribute((__section__("esm_simple")))
__attribute((__used__)) = &esm_tick;

static uint8_t data[ESM_TRACE_CHUNK_SIZE];
static void trace_handle(esm_t *const esm, const esm_signal_t * const sig)
{
	switch(sig->type)
	{
	case esm_sig_alarm:
	{
		size_t s = trace_get(data, ESM_TRACE_CHUNK_SIZE);
		if(s)
		{
			ESM_TRACE_OUT(data, s);
		}
		else
		{
			esm_is_tracing = false;
		}
	}
	break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

const esm_state_t trace = {
		.entry = empty_en_ex,
		.handle = trace_handle,
		.exit = empty_en_ex,
		.name = "trace",
};

static esm_t esm_trace = {
		.name = "esm_trace",
		.id = esm_id_trace,
		.curr_state = &trace,
		.queue = {
				.size = 2,
				.data = (esm_signal_t[2]){0}
		}
};
esm_t * const trace_esm
__attribute((__section__("esm_simple")))
__attribute((__used__)) = &esm_trace;
