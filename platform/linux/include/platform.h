#ifndef INCLUDE_ESM_PLATFORM_H_
#define INCLUDE_ESM_PLATFORM_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

void platform_init(void);
void platform_trace_write(uint8_t const *data, size_t size);

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

#define ESM_INIT do { \
		platform_init(); \
} while(0)

#define ESM_TICKS_PER_SEC	(1000UL)
#define ESM_WAIT() do { \
		struct timespec ts; \
		ts.tv_sec = 0; \
		ts.tv_nsec = 1000000000UL / ESM_TICKS_PER_SEC; \
		nanosleep(&ts, NULL); \
		esm_signal_t sig = { \
				.type = esm_sig_alarm, \
				.sender = (void*)0, \
				.receiver = tick_esm, \
		}; \
		esm_send_signal(&sig); \
} while(0)

#define ESM_RANDOM(_num) ((int) ((float) (_num) * random() / (RAND_MAX + 1.0)))

#define ESM_CRITICAL_ENTER()
#define ESM_CRITICAL_EXIT()

#define ESM_DEBUG_init(_p_esm, ...) do { \
		trace_init(_p_esm->id); \
		ESM_PRINTF("[%010u] [%s] Initializing\r\n", esm_global_time, _p_esm->name); \
} while (0)

#define ESM_DEBUG_trans(_p_esm, _sig, ...) do { \
		trace_trans(_p_esm->id, _sig->type, _p_esm->curr_state->name,  _p_esm->next_state->name); \
		ESM_PRINTF("[%010u] [%s] Transition %s --%s--> %s\r\n", \
				esm_global_time, \
				_p_esm->name, \
				_p_esm->curr_state->name, \
				esm_sig_name[_sig->type], \
				_p_esm->next_state->name); \
} while (0)

#define ESM_DEBUG_receive(_p_esm, _sig, ...) do { \
		trace_receive(_p_esm->id, _sig->type, _p_esm->curr_state->name); \
		ESM_PRINTF("[%010u] [%s] Receiving signal '%s' (%s)\r\n", esm_global_time, \
				_p_esm->name, esm_sig_name[_sig->type], _p_esm->curr_state->name); \
} while (0)

#define ESM_DEBUG(_p_esm, _action, ...) \
		ESM_DEBUG_##_action(_p_esm, __VA_ARGS__)

#define ESM_IDLE() do { \
} while(0)

#define ESM_TRACE_BUF_SIZE		(256)
#define ESM_TRACE_CHUNK_SIZE	(16)
#define ESM_TRACE_OUT(_data, _size) do { \
		platform_trace_write(_data, _size); \
} while(0)

#endif /* INCLUDE_ESM_PLATFORM_H_ */
