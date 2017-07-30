#ifndef INCLUDE_SIGNALS_H_
#define INCLUDE_SIGNALS_H_

#define ESM_SIGNALS \
		ESM_SIGNAL(tmout) \
		ESM_SIGNAL(eat) \
		ESM_SIGNAL(done) \
		ESM_SIGNAL(hungry)

typedef struct
{
	uint8_t num;
} esm_sig_params_t;

#endif /* INCLUDE_SIGNALS_H_ */
