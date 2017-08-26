#ifndef INCLUDE_ESM_PLATFORM_H_
#define INCLUDE_ESM_PLATFORM_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

void platform_init(void);
void platform_trace_write(uint8_t const *data, size_t size);

#define ESM_PRINTF(_format, _args ... )

#define ESM_RESET() {while(1) {}}

#define ESM_BUG_REPORT( _cond, _format, _args ... )

#define ESM_ASSERT(_cond) do { if(!(_cond)) { \
		ESM_BUG_REPORT(_cond, ""); \
		ESM_RESET();}} while(0)

#define ESM_ASSERT_MSG( _cond, _format, _args ... ) \
		do { if(!(_cond)) { ESM_BUG_REPORT( _cond, _format, ##_args ); \
		ESM_RESET(); } } while(0)

#define ESM_INIT do { \
} while(0)

#define ESM_TICKS_PER_SEC	(1000UL)
#define ESM_WAIT() do { \
		esm_signal_t sig = { \
				.type = esm_sig_alarm, \
				.sender = (void*)0, \
				.receiver = tick_esm, \
		}; \
		esm_send_signal(&sig); \
} while(0)

#define ESM_RANDOM(_num) (0)

#define ESM_CRITICAL_ENTER()
#define ESM_CRITICAL_EXIT()

#define ESM_DEBUG_init(_p_esm, ...) do { \
		trace_init(_p_esm->id); \
} while (0)

#define ESM_DEBUG_trans(_p_esm, _sig, ...) do { \
		trace_trans(_p_esm->id, _sig->type, _p_esm->curr_state->name,  _p_esm->next_state->name); \
} while (0)

#define ESM_DEBUG_receive(_p_esm, _sig, ...) do { \
		trace_receive(_p_esm->id, _sig->type, _p_esm->curr_state->name); \
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
