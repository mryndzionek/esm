#include "esm/esm_timer.h"
#include "signal.h"
#include "util.h"

ESM_THIS_FILE;

uint32_t esm_global_time;
static esm_list_t esm_timers = {0};

void esm_timer_add(esm_timer_t *timer, uint32_t delay, esm_signal_t *sig) {
	timer->expiry = esm_global_time+delay;
	timer->sig = *sig;
	/* Move the timer into the right place in the ordered list
       of existing timers. TODO: This is an O(n) operation! */
	esm_list_item_t *it = esm_list_begin(&esm_timers);
	while(it) {
		esm_timer_t *tm = ESM_CONTAINER_OF(it, esm_timer_t, item);
		/* If multiple timers expire at the same moment they will be fired
           in the order they were created in (> rather than >=). */
		if(tm->expiry > timer->expiry)
			break;
		it = esm_list_next(it);
	}
	ESM_CRITICAL_ENTER();
	esm_list_insert(&esm_timers, &timer->item, it);
	ESM_CRITICAL_EXIT();
}

void esm_timer_rm(esm_timer_t *timer) {
	ESM_CRITICAL_ENTER();
	if(UNLIKELY(!(timer->item.next || timer->item.prev) &&
			(&timer->item != esm_timers.first)))
	{
		return;
	}
	else
	{
		esm_list_erase(&esm_timers, &timer->item);
	}
	ESM_CRITICAL_EXIT();
}

int esm_timer_next(void) {
	if(esm_list_empty(&esm_timers))
		return -1;
	uint32_t expiry = ESM_CONTAINER_OF(esm_list_begin(&esm_timers),
			esm_timer_t, item)->expiry;
	return (int) (esm_global_time >= expiry ? 0 : expiry - esm_global_time);
}

void esm_timer_fire(void) {
	/* Avoid getting current time if there are no timers anyway. */
	if(esm_list_empty(&esm_timers))
		return;

	while(!esm_list_empty(&esm_timers)) {
		esm_timer_t *tm = ESM_CONTAINER_OF(
				esm_list_begin(&esm_timers), esm_timer_t, item);
		if(tm->expiry > esm_global_time)
			break;
		ESM_CRITICAL_ENTER();
		esm_list_erase(&esm_timers, esm_list_begin(&esm_timers));
		esm_send_signal(&tm->sig);
	}
}
