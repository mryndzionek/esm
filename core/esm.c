#include "esm/esm.h"
#include "esm/esm_timer.h"

#include <stdio.h>

ESM_THIS_FILE;

extern esm_sec_t __start_esm_section;
extern esm_sec_t __stop_esm_section;
uint32_t esm_global_time;

const esm_state_t esm_unhandled_sig = {
		.entry = (void*)0,
		.handle = (void*)0,
		.exit = (void*)0,
		.name = "esm_unhandled",
};

#define ESM_SIGNAL(_name) #_name,
static char const * const esm_sig_name[] ={
		ESM_SIGNALS
};
#undef ESM_SIGNAL

static uint32_t esm_sig_mask;

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
	esm_sec_t *sec;

	ESM_INIT;

	for (sec = &__start_esm_section; sec < &__stop_esm_section; ++sec) {
		esm_t *esm = sec->esm;
		ESM_DEBUG(esm, esm_global_time, init);
		sec->esm->curr_state->entry(esm);
	}

	while(1)
	{
		ESM_WAIT();

		if(esm_timer_next() == 0)
		{
			esm_timer_fire();
		}
		if(esm_sig_mask)
		{
			while(esm_sig_mask)
			{
				for (sec = &__start_esm_section; sec < &__stop_esm_section; ++sec) {
					esm_t *esm = sec->esm;
					if(esm->sig_len)
					{
						ESM_CRITICAL_ENTER();
						esm_signal_t *sig = &esm->sig_queue[esm->sig_tail++];
						if(esm->sig_tail == esm->sig_queue_size)
						{
							esm->sig_tail = 0;
						}
						--esm->sig_len;
						if(esm->sig_len == 0)
						{
							esm_sig_mask &= ~(1UL << esm->id);
						}
						ESM_CRITICAL_EXIT();

						esm->next_state = esm->curr_state;
						esm->curr_state->handle(esm, sig);

						ESM_ASSERT_MSG(esm->next_state != &esm_unhandled_sig,
								"[%010u] [%s] Unhandled signal: %d (%s)\r\n",
								esm_global_time, esm->name, sig->type, esm->curr_state->name);

						if(esm->curr_state != esm->next_state)
						{
							ESM_DEBUG(esm, esm_global_time, trans, sig);

							esm->curr_state->exit(esm);
							esm->next_state->entry(esm);
							esm->curr_state = esm->next_state;
						}

						ESM_ASSERT_MSG(esm->curr_state == esm->next_state,
								"[%010u] [%s] Transitioning from entry/exit not allowed\r\n",
								esm_global_time, esm->name);
					}
				}
			}
			ESM_IDLE(esm_global_time);
		}
	}
}

void esm_send_signal_from_isr(esm_signal_t *sig)
{
	ESM_CRITICAL_ENTER();
	ESM_ASSERT(sig->receiver);
	if(sig->receiver->subscribed & (1UL << sig->type))
	{
		ESM_DEBUG(sig->receiver, esm_global_time, receive, sig);

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
		++sig->receiver->sig_len;
		esm_sig_mask |= (1UL << sig->receiver->id);
	}
	ESM_CRITICAL_EXIT();
}

void esm_send_signal(esm_signal_t *sig)
{
	esm_sec_t *sec;

	if(sig->receiver)
	{
		esm_send_signal_from_isr(sig);
	}
	else
	{
		for (sec = &__start_esm_section; sec < &__stop_esm_section; ++sec) {
			sig->receiver = sec->esm;
			esm_send_signal_from_isr(sig);
		}
		sig->receiver = (void *)0;
	}

	ESM_ASSERT_MSG(esm_sig_mask != 0,
			"[%010u] Signal '%s' is lost\r\n",
			esm_global_time, esm_sig_name[sig->type]);
}
