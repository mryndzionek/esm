#ifndef INCLUDE_SIGNALS_H_
#define INCLUDE_SIGNALS_H_

#define ESM_SIGNALS \
		ESM_SIGNAL(tmout) \
		ESM_SIGNAL(request) \
		ESM_SIGNAL(response)

#define ESM_IDS \
      ESM_ID(producer_1) \
      ESM_ID(producer_2) \
      ESM_ID(synchronizer) \
      ESM_ID(consumer)

typedef struct
{
} esm_sig_params_t;

#endif /* INCLUDE_SIGNALS_H_ */
