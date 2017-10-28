#ifndef CORE_INCLUDE_UTIL_H_
#define CORE_INCLUDE_UTIL_H_

#define LIKELY(condition) __builtin_expect((condition), 1)
#define UNLIKELY(condition) __builtin_expect((condition), 0)

#endif /* CORE_INCLUDE_UTIL_H_ */
