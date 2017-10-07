#include "esm/esm.h"
#ifdef ESM_HSM
#include "esm/hesm.h"
#endif
#include "esm/esm_timer.h"
#include "esm/esm_queue.h"
#include "trace.h"

#include <stdio.h>

ESM_THIS_FILE;

static uint8_t const log2lut[16] = {
		0, 1, 2, 2,
		3, 3, 3, 3,
		4, 4, 4, 4,
		4, 4, 4, 4
};

static uint8_t prio_mask;

static esm_list_t esm_signals[_ESM_MAX_PRIO];
extern esm_t * const __attribute__((weak)) __start_esm_sec;
extern esm_t * const __attribute__((weak)) __stop_esm_sec;

extern bool esm_is_tracing;

const esm_state_t esm_unhandled_state = {
		.entry = (void*)0,
		.handle = (void*)0,
		.exit = (void*)0,
		.name = "esm_unhandled",
};

#define ESM_SIGNAL(_name) #_name,
static char const * const esm_sig_name[] = {
		"alarm",
#ifdef ESM_USE_BUS
		"bus_req",
		"bus_rsp",
		"bus_ack",
#endif
		ESM_SIGNALS
};
#undef ESM_SIGNAL

static void self_entry(esm_t *const esm)
{
	esm->next_state = esm->curr_state;
	ESM_TRACE(esm, enter, esm->next_state->name);
	esm->next_state->entry(esm);
}

const esm_state_t esm_self_state = {
		.entry = self_entry,
		.handle = (void*)0,
		.exit = (void*)0,
		.name = "esm_self",
};

static void simple_process(esm_t * const esm, const esm_signal_t *const sig)
{
	if(esm->cfg->id > esm_id_trace)
	{
		ESM_TRACE(sig->receiver, receive, sig);
	}

	esm->next_state = esm->curr_state;
	esm->curr_state->handle(esm, sig);

	ESM_ASSERT_MSG(esm->next_state != &esm_unhandled_state,
			"[%010u] [%s] Unhandled signal: %s (%s)\r\n",
			esm_global_time, esm->cfg->name, esm_sig_name[sig->type], esm->curr_state->name);

	if(esm->curr_state != esm->next_state)
	{
		if(esm->cfg->id > esm_id_trace)
		{
			ESM_TRACE(esm, trans, sig);
		}

		ESM_TRACE(esm, exit, esm->curr_state->name);
		esm->curr_state->exit(esm);

		ESM_TRACE(esm, enter, esm->next_state->name);
		esm->next_state->entry(esm);
		esm->curr_state = esm->next_state;
	}

	ESM_ASSERT_MSG(esm->curr_state == esm->next_state,
			"[%010u] [%s] Transitioning from entry/exit not allowed\r\n",
			esm_global_time, esm->cfg->name);
}

#ifdef ESM_HSM
static void complex_process(esm_t * const esm, const esm_signal_t *const sig)
{
	esm_hstate_t const *start = (esm_hstate_t const *)esm->curr_state;
	esm_hstate_t const *end;

	esm->next_state = esm->curr_state;
	start->super.handle(esm, sig);

	// if signal is not handled by the current state
	// check if it's handles by any of the parent states
	while(esm->next_state == &esm_unhandled_state)
	{
		esm_hstate_t const *s = (esm_hstate_t const *)esm->curr_state;
		ESM_ASSERT_MSG(s->parent != &esm_top_state,
				"[%010u] [%s] Unhandled signal: %s (%s)\r\n",
				esm_global_time, esm->cfg->name, esm_sig_name[sig->type], esm->curr_state->name);

		esm->next_state = esm->curr_state = &s->parent->super;
		esm->curr_state->handle(esm, sig);
	}

	if(esm->next_state == &esm_self_state)
	{
		esm->next_state->entry(esm);
	}

	if(esm->curr_state != esm->next_state)
	{
		end = (esm_hstate_t const *)esm->next_state;

		if(esm->cfg->id > esm_id_trace)
		{
			ESM_TRACE(esm, trans, sig);
		}

		{
			uint8_t depth = start->depth > end->depth ? start->depth : end->depth;
			uint8_t i = 0;
			esm_hstate_t const *path[((hesm_t const * const)esm)->depth];

			while(end != start)
			{
				if(start->depth == depth)
				{
					ESM_TRACE(esm, exit, start->super.name);
					start->super.exit(esm);
					start = start->parent;
				}

				if(end->depth == depth)
				{
					path[i++] = end;
					end = end->parent;
				}
				depth--;
			}

			while(i)
			{
				start = path[--i];
				ESM_TRACE(esm, enter, start->super.name);
				start->super.entry(esm);
			}
		}

		esm->curr_state = &start->super;
		while(start->init)
		{
			esm->next_state = esm->curr_state;

			ESM_PRINTF("[%010u] [%s] Initial %s\r\n", esm_global_time, esm->cfg->name, start->super.name);

			start->init(esm);
			ESM_ASSERT(esm->curr_state != esm->next_state);

			ESM_TRACE(esm, enter, esm->next_state->name);
			esm->next_state->entry(esm);
			esm->curr_state = esm->next_state;
			start = (esm_hstate_t * const)esm->curr_state;
		}
	}
	else
	{
		esm->curr_state = &start->super;
	}
}
#endif

void esm_process(void)
{
	esm_t * const * sec;

	ESM_INIT;

	for (sec = &__start_esm_sec; sec < &__stop_esm_sec; ++sec) {
		esm_t * const esm = *sec;
		if(esm->cfg->is_cplx)
		{
#ifdef ESM_HSM
			if(esm->init)
			{
				esm->init(esm);
				ESM_ASSERT(esm->next_state);
				esm->curr_state = esm->next_state;
			}
			ESM_TRACE((*sec), init);
			esm->curr_state->entry(esm);
			esm_hstate_t const *s = (esm_hstate_t const *)esm->curr_state;

			while(s->init)
			{
				esm->next_state = esm->curr_state;
				s->init(esm);
				ESM_ASSERT(esm->curr_state != esm->next_state);
				esm->next_state->entry(esm);
				esm->curr_state = esm->next_state;
				s = (esm_hstate_t * const)esm->curr_state;
			}
#endif
		}
		else
		{
			if(esm->init)
			{
				esm->init(esm);
				ESM_ASSERT(esm->next_state);
				esm->curr_state = esm->next_state;
			}
			ESM_TRACE(esm, init);
			esm->curr_state->entry(esm);
		}
	}

	while(1)
	{
		int16_t prio;

		ESM_WAIT();

		do
		{
			if ((prio_mask & 0xF0) != 0) {
				prio = log2lut[prio_mask >> 4] + 4;
			}
			else {
				prio = log2lut[prio_mask];
			}
			ESM_ASSERT(prio);

			while(!esm_list_empty(&esm_signals[--prio]))
			{
				esm_signal_t * const sig = ESM_CONTAINER_OF(esm_list_begin(&esm_signals[prio]),
						esm_signal_t, item);
				ESM_ASSERT(sig == esm_queue_tail(&sig->receiver->queue));
				if(sig->receiver->cfg->is_cplx)
				{
#ifdef ESM_HSM
					complex_process(sig->receiver, sig);
#endif
				}
				else
				{
					simple_process(sig->receiver, sig);
				}
				ESM_CRITICAL_ENTER();
				esm_queue_pop(&sig->receiver->queue);
				esm_list_erase(&esm_signals[prio], &sig->item);
				if(esm_list_empty(&esm_signals[prio]))
				{
					prio_mask &= ~(1UL << prio);
				}
				ESM_CRITICAL_EXIT();
			}
		}
		while(prio_mask);

		if(!esm_is_tracing)
		{
			uint8_t data[ESM_TRACE_CHUNK_SIZE];
			size_t s = trace_get(data, ESM_TRACE_CHUNK_SIZE);
			if(s)
			{
				ESM_TRACE_OUT(data, s);
				esm_is_tracing = true;
			}
		}
		ESM_IDLE();
	}
}

bool esm_send_signal(esm_signal_t *sig)
{
	bool ret = false;
	ESM_CRITICAL_ENTER();
	ESM_ASSERT(sig->receiver);
	ESM_ASSERT(sig->receiver->cfg->prio < _ESM_MAX_PRIO);

	esm_list_insert(&esm_signals[sig->receiver->cfg->prio],
			&(esm_queue_head(&sig->receiver->queue))->item, NULL);
	esm_queue_push(&sig->receiver->queue, sig);
	prio_mask |= (1UL << sig->receiver->cfg->prio);

	ret = true;
	ESM_CRITICAL_EXIT();

	return ret;
}

void esm_broadcast_signal(esm_signal_t *sig, esm_group_e group)
{
	esm_t * const * sec;
	bool ret = false;
	for (sec = &__start_esm_sec; sec < &__stop_esm_sec; ++sec) {
		if((*sec)->cfg->group & group)
		{
			sig->receiver = *sec;
			ret |= esm_send_signal(sig);
		}
	}
	sig->receiver = (void *)0;

	ESM_ASSERT_MSG(ret,
			"[%010u] Signal '%s' is lost\r\n",
			esm_global_time, esm_sig_name[sig->type]);
}
