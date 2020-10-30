#ifndef INCLUDE_SIGNALS_H_
#define INCLUDE_SIGNALS_H_

#include "board.h"

typedef enum
{
      mat_ev_down = 0,
      mat_ev_up
} mat_ev_type_e;

typedef struct
{
      esm_list_item_t item;
      uint16_t kc;
} keyitem_t;

#define ESM_SIGNALS       \
      ESM_SIGNAL(tmout)   \
      ESM_SIGNAL(matrix)  \
      ESM_SIGNAL(key)     \
      ESM_SIGNAL(pointer) \
      ESM_SIGNAL(usb_tx_end)

#define ESM_IDS             \
      ESM_ID(matrix)        \
      ESM_ID(state_handler) \
      ESM_ID(keyboard)

#define ESM_SIG_PARAMS                   \
      struct                             \
      {                                  \
            uint8_t row;                 \
            uint8_t col;                 \
            mat_ev_type_e ev;            \
      } mat;                             \
      struct                             \
      {                                  \
            keyitem_t *it;               \
            mat_ev_type_e ev;            \
      } key;                             \
      struct                             \
      {                                  \
            uint8_t btns;                \
            int8_t x;                    \
            int8_t y;                    \
      } pointer;                         \
      struct                             \
      {                                  \
            uint8_t i;                   \
            mat_ev_type_e ev;            \
      } tap;                             \
      struct                             \
      {                                  \
            BOARD_DEBOUNCER_STATE state; \
      } debouncer;                       \
      struct                             \
      {                                  \
            uint8_t chan;                \
            uint8_t state;               \
      } encoder;                         \
      struct                             \
      {                                  \
            uint8_t row;                 \
            uint8_t col;                 \
            uint16_t val;                \
      } bcklight;

#endif /* INCLUDE_SIGNALS_H_ */
