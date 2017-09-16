#include "esm/esm.h"
#ifdef ESM_HSM
#include "esm/hesm.h"
#endif
#include "esm/esm_timer.h"
#include "trace.h"

#include <stdio.h>

ESM_THIS_FILE;

static esm_list_t esm_signals = {0};
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

static void simple_process(esm_t * const esm)
{
	esm_signal_t *sig = &esm->sig_queue[esm->sig_tail];

	if(esm->id > esm_id_trace)
	{
		ESM_TRACE(sig->receiver, receive, sig);
	}

	esm->next_state = esm->curr_state;
	esm->curr_state->handle(esm, sig);

	ESM_ASSERT_MSG(esm->next_state != &esm_unhandled_state,
			"[%010u] [%s] Unhandled signal: %s (%s)\r\n",
			esm_global_time, esm->name, esm_sig_name[sig->type], esm->curr_state->name);

	if(esm->curr_state != esm->next_state)
	{
		if(esm->id > esm_id_trace)
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
			esm_global_time, esm->name);
}

#ifdef ESM_HSM
static void complex_process(esm_t * const esm)
{
	esm_signal_t *sig = &esm->sig_queue[esm->sig_tail];
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
				esm_global_time, esm->name, esm_sig_name[sig->type], esm->curr_state->name);

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

		if(esm->id > esm_id_trace)
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

			ESM_PRINTF("[%010u] [%s] Initial %s\r\n", esm_global_time, esm->name, start->super.name);

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
		if(esm->is_cplx)
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
		ESM_WAIT();

		while(!esm_list_empty(&esm_signals))
		{
			esm_signal_t * const sig = ESM_CONTAINER_OF(esm_list_begin(&esm_signals),
					esm_signal_t, item);
			if(sig->receiver->is_cplx)
			{
#ifdef ESM_HSM
				complex_process(sig->receiver);
#endif
			}
			else
			{
				simple_process(sig->receiver);
			}
			ESM_CRITICAL_ENTER();
			if(++sig->receiver->sig_tail == sig->receiver->sig_queue_size)
			{
				sig->receiver->sig_tail = 0;
			}
			--sig->receiver->sig_len;
			esm_list_erase(&esm_signals, &sig->item);
			ESM_CRITICAL_EXIT();
		}
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
	if(sig->receiver->sig_len)
	{
		ESM_ASSERT_MSG(sig->receiver->sig_head != sig->receiver->sig_tail,
				"Event queue for %s overrun\r\n", sig->receiver->name);
	}

	sig->receiver->sig_queue[sig->receiver->sig_head] = *sig;
	esm_list_insert(&esm_signals, &sig->receiver->sig_queue[sig->receiver->sig_head++].item, NULL);
	if(sig->receiver->sig_head == sig->receiver->sig_queue_size)
	{
		sig->receiver->sig_head = 0;
	}
	++sig->receiver->sig_len;
	ret = true;
	ESM_CRITICAL_EXIT();

	return ret;
}

void esm_broadcast_signal(esm_signal_t *sig, esm_group_e group)
{
	esm_t * const * sec;
	bool ret = false;
	for (sec = &__start_esm_sec; sec < &__stop_esm_sec; ++sec) {
		if((*sec)->group & group)
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
