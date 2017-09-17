#ifndef INCLUDE_SIGNALS_H_
#define INCLUDE_SIGNALS_H_

#define ESM_SIGNALS

#define ESM_IDS \
      ESM_ID(debug)

typedef struct
{
	uint16_t tick;
	uint8_t bin;
} esm_sig_params_t;

#endif /* INCLUDE_SIGNALS_H_ */
