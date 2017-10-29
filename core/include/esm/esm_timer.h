#ifndef INC_ESM_TIMER_H_
#define INC_ESM_TIMER_H_

#include "esm_list.h"
#include "esm.h"

typedef struct esm_timer esm_timer_t;

struct esm_timer {
	/* Item in the global list of all timers. */
	esm_list_item_t item;
	/* The deadline when the timer expires. -1 if the timer is not active. */
	uint32_t expiry;
	esm_signal_t sig;
};

extern uint32_t esm_global_time;

#define esm_timer_enabled(tm)  ((tm)->expiry >= 0)
void esm_timer_add(esm_timer_t *timer, uint32_t delay, esm_signal_t *sig);
void esm_timer_rm(esm_timer_t *timer);
int esm_timer_next(void);
void esm_timer_fire(void);

#endif /* INC_ESM_TIMER_H_ */
