#ifndef INCLUDE_SIGNALS_H_
#define INCLUDE_SIGNALS_H_

#include "board.h"

#define ESM_SIGNALS \
		ESM_SIGNAL(tmout) \
		ESM_SIGNAL(button)

#define ESM_IDS \
      ESM_ID(debouncer) \
      ESM_ID(blink1) \
      ESM_ID(blink2) \
      ESM_ID(blink3)

#define ESM_GROUPS \
      ESM_GROUP(blinkers)

#define ESM_SIG_PARAMS                   \
      struct                             \
      {                                  \
            BOARD_DEBOUNCER_STATE state; \
      } debouncer;

#endif /* INCLUDE_SIGNALS_H_ */
