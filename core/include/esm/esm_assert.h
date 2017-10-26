#ifndef CORE_INCLUDE_ESM_ESM_ASSERT_H_
#define CORE_INCLUDE_ESM_ESM_ASSERT_H_

#define ESM_RESET() {ESM_CRITICAL_ENTER(); while(1) {}}

#define ESM_BUG_REPORT( _cond, _format, _args ... ) \
		ESM_PRINTF("Assert (%s) failed at %s:%d : " _format"\r\n", \
				# _cond, THIS_FILE__, __LINE__, ##_args )

#define ESM_ASSERT(_cond) do { if(!(_cond)) { \
		ESM_BUG_REPORT(_cond, ""); \
		ESM_RESET();}} while(0)

#define ESM_ASSERT_MSG(_cond, _format, _args ... ) \
		do { if(!(_cond)) { ESM_BUG_REPORT( _cond, _format, ##_args ); \
		ESM_RESET(); } } while(0)

#define ESM_ASSERT_COMPILE(_cond) \
		extern int esm_assert_compile[(_cond) ? 1 : -1]

#endif /* CORE_INCLUDE_ESM_ESM_ASSERT_H_ */
