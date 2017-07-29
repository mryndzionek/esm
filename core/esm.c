#include "esm/esm.h"
#include "esm/esm_timer.h"

#include <stdio.h>

extern esm_sec_t __start_esm_section;
extern esm_sec_t __stop_esm_section;

const esm_state_t esm_unhandled_sig = {
      .entry = (void*)0,
      .handle = (void*)0,
      .exit = (void*)0,
      .name = "esm_unhandled",
};

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
		ESM_PRINTF("Initializing esm: %s\r\n", sec->esm->name);
		sec->esm->curr_state->entry(sec->esm);
	}

	while(1)
	{
		ESM_WAIT();
		esm_global_time++;
		if(esm_timer_next() == 0)
		{
			esm_timer_fire();

			for (sec = &__start_esm_section; sec < &__stop_esm_section; ++sec) {
				ESM_PRINTF("Processing esm: %s\r\n", sec->esm->name);
			}
		}
	}
}

void esm_send_signal(signal_t *sig)
{
	esm_sec_t *sec;

	for (sec = &__start_esm_section; sec < &__stop_esm_section; ++sec) {
		if(sec->esm->subscribed & (1UL << sig->type))
		{
			ESM_PRINTF("Sending signal %d to %s:%s\r\n", sig->type, sec->esm->name, sec->esm->curr_state->name);
			sec->esm->sig_queue[sec->esm->sig_head++] = *sig;
			if(sec->esm->sig_head == sec->esm->sig_queue_len)
			{
				sec->esm->sig_head = 0;
			}
		}
	}
}
