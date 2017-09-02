#include "esm/esm.h"
#ifdef ESM_HSM
#include "esm/hesm.h"
#endif
#include "esm/esm_timer.h"
#include "trace.h"

#include <stdio.h>

ESM_THIS_FILE;

extern esm_t * const __attribute__((weak)) __start_esm_simple;
extern esm_t * const __attribute__((weak)) __stop_esm_simple;

#ifdef ESM_HSM
extern esm_t * const __start_esm_complex;
extern esm_t * const __stop_esm_complex;
#endif

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

static uint8_t esm_sig_count;

static void self_entry(esm_t *const esm)
{
	esm->next_state = esm->curr_state;
	ESM_PRINTF("[%010u] [%s] Entering %s\r\n", esm_global_time, esm->name, esm->next_state->name);
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
	if(esm->sig_len)
	{
		esm_signal_t *sig = &esm->sig_queue[esm->sig_tail];

		if(esm->id > esm_id_trace)
		{
			ESM_DEBUG(sig->receiver, receive, sig);
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
				ESM_DEBUG(esm, trans, sig);
			}

			esm->curr_state->exit(esm);
			esm->next_state->entry(esm);
			esm->curr_state = esm->next_state;
		}

		ESM_ASSERT_MSG(esm->curr_state == esm->next_state,
				"[%010u] [%s] Transitioning from entry/exit not allowed\r\n",
				esm_global_time, esm->name);

		ESM_CRITICAL_ENTER();
		if(++esm->sig_tail == esm->sig_queue_size)
		{
			esm->sig_tail = 0;
		}
		--esm->sig_len;
		if(esm->sig_len == 0)
		{
			esm_sig_count--;
		}
		ESM_CRITICAL_EXIT();
	}
}

#ifdef ESM_HSM
static void complex_process(esm_t * const esm)
{
	if(esm->sig_len)
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
				ESM_DEBUG(esm, trans, sig);
			}

			{
				uint8_t depth = start->depth > end->depth ? start->depth : end->depth;
				uint8_t i = 0;
				esm_hstate_t const *path[((hesm_t const * const)esm)->depth];

				while(end != start)
				{
					if(start->depth == depth)
					{
						ESM_PRINTF("[%010u] [%s] Exiting %s\r\n", esm_global_time, esm->name, start->super.name);
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
					ESM_PRINTF("[%010u] [%s] Entering %s\r\n", esm_global_time, esm->name, start->super.name);
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

				ESM_PRINTF("[%010u] [%s] Entering %s\r\n", esm_global_time, esm->name, esm->next_state->name);
				esm->next_state->entry(esm);
				esm->curr_state = esm->next_state;
				start = (esm_hstate_t * const)esm->curr_state;
			}
		}
		else
		{
			esm->curr_state = &start->super;
		}

		ESM_CRITICAL_ENTER();
		if(++esm->sig_tail == esm->sig_queue_size)
		{
			esm->sig_tail = 0;
		}
		--esm->sig_len;
		if(esm->sig_len == 0)
		{
			esm_sig_count--;
		}
		ESM_CRITICAL_EXIT();
	}
}
#endif

void esm_process(void)
{
	esm_t * const * sec;

	ESM_INIT;

	for (sec = &__start_esm_simple; sec < &__stop_esm_simple; ++sec) {
		esm_t * const esm = *sec;
		ESM_DEBUG(esm, init);
		esm->curr_state->entry(esm);
	}

#ifdef ESM_HSM
	for (sec = &__start_esm_complex; sec < &__stop_esm_complex; ++sec) {
		esm_t * const esm = *sec;
		ESM_DEBUG((*sec), init);
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
	}
#endif

	while(1)
	{
		ESM_WAIT();

		while(esm_sig_count)
		{
			for (sec = &__start_esm_simple; esm_sig_count && (sec < &__stop_esm_simple); ++sec) {
				simple_process(*sec);
			}

#ifdef ESM_HSM
			for (sec = &__start_esm_complex; esm_sig_count && (sec < &__stop_esm_complex); ++sec) {
				complex_process(*sec);
			}
#endif
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
	if(sig->receiver->subscribed & (1UL << sig->type))
	{
		if(sig->receiver->sig_len)
		{
			ESM_ASSERT_MSG(sig->receiver->sig_head != sig->receiver->sig_tail,
					"Event queue for %s overrun\r\n", sig->receiver->name);
		}

		sig->receiver->sig_queue[sig->receiver->sig_head++] = *sig;
		if(sig->receiver->sig_head == sig->receiver->sig_queue_size)
		{
			sig->receiver->sig_head = 0;
		}
		if(!sig->receiver->sig_len)
		{
			esm_sig_count++;
		}
		++sig->receiver->sig_len;
		ret = true;
	}
	ESM_CRITICAL_EXIT();

	return ret;
}

void esm_broadcast_signal(esm_signal_t *sig)
{
	esm_t * const * sec;
	bool ret = false;
	for (sec = &__start_esm_simple; sec < &__stop_esm_simple; ++sec) {
		sig->receiver = *sec;
		ret |= esm_send_signal(sig);
	}
	sig->receiver = (void *)0;

	ESM_ASSERT_MSG(ret,
			"[%010u] Signal '%s' is lost\r\n",
			esm_global_time, esm_sig_name[sig->type]);
}
