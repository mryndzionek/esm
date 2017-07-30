#include "esm/esm.h"
#include "esm/esm_timer.h"

#include <stdio.h>

ESM_THIS_FILE;

extern esm_sec_t __start_esm_section;
extern esm_sec_t __stop_esm_section;

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

	for (sec = &__start_esm_section; sec < &__stop_esm_section; ++sec) {
		esm_t *esm = sec->esm;
		ESM_PRINTF("[%010u] [%s] Initializing\r\n", esm_global_time, esm->name);
		sec->esm->curr_state->entry(esm);
	}

	while(1)
	{
		ESM_WAIT();
		esm_global_time++;
		if(esm_timer_next() == 0)
		{
			esm_timer_fire();

			do
			{
				for (sec = &__start_esm_section; sec < &__stop_esm_section; ++sec) {
					esm_t *esm = sec->esm;
					if(esm->sig_len)
					{
						esm_signal_t *sig = &esm->sig_queue[esm->sig_tail++];
						if(esm->sig_tail == esm->sig_queue_size)
						{
							esm->sig_tail = 0;
						}
						--esm->sig_len;
						esm_sig_mask &= ~(1UL << esm->id);

						esm->next_state = esm->curr_state;
						esm->curr_state->handle(esm, sig);

						ESM_ASSERT_MSG(esm->next_state != &esm_unhandled_sig,
								"[%010u] [%s] Unhandled signal: %d (%s)\r\n",
								esm_global_time, esm->name, sig->type, esm->curr_state->name);

						if(esm->curr_state != esm->next_state)
						{
							ESM_PRINTF("[%010u] [%s] Transition %s --%s--> %s\r\n",
									esm_global_time,
									esm->name,
									esm->curr_state->name,
									esm_sig_name[sig->type],
									esm->next_state->name);

							esm->curr_state->exit(esm);
							esm->next_state->entry(esm);
							esm->curr_state = esm->next_state;
						}

						ESM_ASSERT_MSG(esm->curr_state == esm->next_state,
								"[%010u] [%s] Transitioning from entry/exit not allowed\r\n",
								esm_global_time, esm->name);
					}
				}
			} while(esm_sig_mask);
			ESM_PRINTF("[%010u] ------------------------------------\r\n", esm_global_time);
		}
	}
}

static void _send(esm_t *const esm, esm_signal_t *sig)
{
	if(esm->subscribed & (1UL << sig->type))
	{
		ESM_PRINTF("[%010u] [%s] Receiving signal '%s' (%s)\r\n", esm_global_time,
				esm->name, esm_sig_name[sig->type], esm->curr_state->name);

		if(esm->sig_len)
		{
			ESM_ASSERT_MSG(esm->sig_head != esm->sig_tail,
					"Event queue for %s overrun\r\n", esm->name);
		}

		esm->sig_queue[esm->sig_head++] = *sig;
		if(esm->sig_head == esm->sig_queue_size)
		{
			esm->sig_head = 0;
		}
		++esm->sig_len;
		esm_sig_mask |= (1UL << esm->id);
	}
}

void esm_send_signal(esm_signal_t *sig)
{
	esm_sec_t *sec;

	if(sig->receiver)
	{
		_send(sig->receiver, sig);
	}
	else
	{
		for (sec = &__start_esm_section; sec < &__stop_esm_section; ++sec) {
			_send(sec->esm, sig);
		}
	}
}
