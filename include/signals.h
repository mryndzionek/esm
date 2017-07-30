#ifndef INCLUDE_SIGNALS_H_
#define INCLUDE_SIGNALS_H_

typedef enum
{
	esm_sig_tmout = 0,
	esm_sig_eat,
	esm_sig_done,
	esm_sig_hungry,
} esm_signal_e;

typedef struct
{
	uint8_t num;
} esm_sig_params_t;

#endif /* INCLUDE_SIGNALS_H_ */
