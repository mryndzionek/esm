#ifndef INCLUDE_SIGNALS_H_
#define INCLUDE_SIGNALS_H_

#include "board.h"
#include "ds3231.h"

// keep those in sync with the `Tracks` array in `mmml-data.h`
#define NUM_ALARMS (4)
#define HOUR_CHIME_NUM (4)
#define HALF_HOUR_CHIME_NUM (5)

#define ESM_SIGNALS      \
      ESM_SIGNAL(tmout)  \
      ESM_SIGNAL(reset)  \
      ESM_SIGNAL(button) \
      ESM_SIGNAL(remote) \
      ESM_SIGNAL(play)   \
      ESM_SIGNAL(rtc)

#define ESM_IDS             \
      ESM_ID(debouncer)     \
      ESM_ID(clock1)        \
      ESM_ID(strip1)        \
      ESM_ID(player1)       \
      ESM_ID(nec1)          \
      ESM_ID(rtc1)          \
      ESM_ID(backlight) 

#define ESM_GROUPS \
      ESM_GROUP(remote)

#define ESM_SIG_PARAMS                   \
      uint8_t keycode;                   \
      ds3231_time_t const *time;         \
      struct                             \
      {                                  \
            BOARD_DEBOUNCER_STATE state; \
      } debouncer;                       \
      struct                             \
      {                                  \
            uint8_t row;                 \
            uint8_t col;                 \
            uint16_t val;                \
      } bcklight;                        \
      struct                             \
      {                                  \
            uint8_t track;               \
      } play;  

#endif /* INCLUDE_SIGNALS_H_ */
