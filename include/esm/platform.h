#ifndef INCLUDE_ESM_PLATFORM_H_
#define INCLUDE_ESM_PLATFORM_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define ESM_PRINTF(_format, _args ... ) \
		printf(_format, ##_args)

#define ESM_RESET() {while(1) {}}

#define ESM_BUG_REPORT( _cond, _format, _args ... ) \
		ESM_PRINTF("Assert (%s) failed at %s:%d : " _format"\r\n", \
				# _cond, THIS_FILE__, __LINE__, ##_args )

#define ESM_ASSERT(_cond) do { if(!(_cond)) { \
		ESM_BUG_REPORT(_cond, ""); \
		ESM_RESET();}} while(0)

#define ESM_ASSERT_MSG( _cond, _format, _args ... ) \
		do { if(!(_cond)) { ESM_BUG_REPORT( _cond, _format, ##_args ); \
		ESM_RESET(); } } while(0)

#define ESM_TICKS_PER_SEC	(1000UL)
#define ESM_WAIT() \
		struct timespec ts; \
		ts.tv_sec = 0; \
		ts.tv_nsec = 1000000000UL / ESM_TICKS_PER_SEC; \
		nanosleep(&ts, NULL);

#define ESM_RANDOM(_num) ((int) ((float) (_num) * random() / (RAND_MAX + 1.0)))


#define ESM_DEBUG_init(_time, _p_esm, ...) \
      ESM_PRINTF("[%010u] [%s] Initializing\r\n", _time, _p_esm->name)

#define ESM_DEBUG_trans(_time, _p_esm, _sig, ...) \
      ESM_PRINTF("[%010u] [%s] Transition %s --%s--> %s\r\n", \
                 _time, \
                 _p_esm->name, \
                 _p_esm->curr_state->name, \
                 esm_sig_name[_sig->type], \
                 _p_esm->next_state->name)

#define ESM_DEBUG_receive(_time, _p_esm, _sig, ...) \
      ESM_PRINTF("[%010u] [%s] Receiving signal '%s' (%s)\r\n", _time, \
                 _p_esm->name, esm_sig_name[_sig->type], _p_esm->curr_state->name)

#define ESM_DEBUG(_p_esm, _time, _action, ...) \
      ESM_DEBUG_##_action( _time, _p_esm, __VA_ARGS__)

#define ESM_IDLE(_time) \
      ESM_PRINTF("[%010u] ------------------------------------\r\n", _time)

extern uint32_t esm_global_time;

#endif /* INCLUDE_ESM_PLATFORM_H_ */
