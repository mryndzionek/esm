#ifndef INCLUDE_SIGNAL_H_
#define INCLUDE_SIGNAL_H_

#include <stdint.h>

typedef enum
{
	sig_tmout = 0,
} signal_e;

typedef struct
{
	signal_e type;
} signal_t;

#endif /* INCLUDE_SIGNAL_H_ */
