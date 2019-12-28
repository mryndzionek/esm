#ifndef INCLUDE_SIGNALS_H_
#define INCLUDE_SIGNALS_H_

#include "board.h"
#include "keyboard.h"

#define ESM_SIGNALS        \
      ESM_SIGNAL(tmout)    \
      ESM_SIGNAL(keypress) \
      ESM_SIGNAL(tap)      \
      ESM_SIGNAL(usb_tx_end)

#define ESM_IDS             \
      ESM_ID(matrix)        \
      ESM_ID(keyboard)      \
      ESM_ID(tap_detector1) \
      ESM_ID(tap_detector2)

#define ESM_GROUPS \
      ESM_GROUP(taps)

#define ESM_SIG_PARAMS               \
      union {                        \
            struct                   \
            {                        \
                  uint8_t row;       \
                  uint8_t col;       \
                  key_ev_type_e kev; \
            } key;                   \
      };

#endif /* INCLUDE_SIGNALS_H_ */
