#ifndef INCLUDE_ESM_PLATFORM_H_
#define INCLUDE_ESM_PLATFORM_H_

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

extern uint32_t esm_global_time;

#endif /* INCLUDE_ESM_PLATFORM_H_ */
