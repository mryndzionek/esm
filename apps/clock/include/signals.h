#ifndef INCLUDE_SIGNALS_H_
#define INCLUDE_SIGNALS_H_

#define ESM_SIGNALS      \
      ESM_SIGNAL(tmout)  \
      ESM_SIGNAL(reset)  \
      ESM_SIGNAL(button) \
      ESM_SIGNAL(remote) \
      ESM_SIGNAL(play)

#define ESM_IDS             \
      ESM_ID(debouncer)     \
      ESM_ID(nec_debouncer) \
      ESM_ID(clock1)        \
      ESM_ID(strip1)        \
      ESM_ID(player1)       \
      ESM_ID(nec1)

#define ESM_GROUPS \
      ESM_GROUP(remote)

#define ESM_SIG_PARAMS \
      uint8_t keycode;

#endif /* INCLUDE_SIGNALS_H_ */
