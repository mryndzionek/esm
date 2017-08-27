#include "esm/esm.h"
#include "esm/esm_timer.h"
#include "trace.h"

#include <stdio.h>

ESM_THIS_FILE;

extern esm_t * const __start_esm_section;
extern esm_t * const __stop_esm_section;

extern bool esm_is_tracing;

const esm_state_t esm_unhandled_sig = {
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
	esm->next_state->entry(esm);
}

const esm_state_t esm_self_transition = {
		.entry = self_entry,
		.handle = (void*)0,
		.exit = (void*)0,
		.name = "esm_internal",
};

void esm_process(void)
{
	esm_t * const * sec;

	ESM_INIT;

	for (sec = &__start_esm_section; sec < &__stop_esm_section; ++sec) {
		esm_t * const esm = *sec;
		ESM_DEBUG(esm, init);
		esm->curr_state->entry(esm);
	}

	while(1)
	{
		ESM_WAIT();

		while(esm_sig_count)
		{
			for (sec = &__start_esm_section; sec < &__stop_esm_section; ++sec) {
				esm_t * const esm = *sec;

				if(esm->sig_len)
				{
					esm_signal_t *sig = &esm->sig_queue[esm->sig_tail];

					if(esm->id > esm_id_trace)
					{
						ESM_DEBUG(sig->receiver, receive, sig);
					}

					esm->next_state = esm->curr_state;
					esm->curr_state->handle(esm, sig);

					ESM_ASSERT_MSG(esm->next_state != &esm_unhandled_sig,
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

				if(!esm_sig_count)
				{
					break;
				}
			}
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
	for (sec = &__start_esm_section; sec < &__stop_esm_section; ++sec) {
		sig->receiver = *sec;
		ret |= esm_send_signal(sig);
	}
	sig->receiver = (void *)0;

	ESM_ASSERT_MSG(ret,
			"[%010u] Signal '%s' is lost\r\n",
			esm_global_time, esm_sig_name[sig->type]);
}
