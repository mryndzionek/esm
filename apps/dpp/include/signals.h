#ifndef INCLUDE_SIGNALS_H_
#define INCLUDE_SIGNALS_H_

#define ESM_SIGNALS \
		ESM_SIGNAL(tmout) \
		ESM_SIGNAL(eat) \
		ESM_SIGNAL(done) \
		ESM_SIGNAL(hungry)

#define ESM_IDS \
      ESM_ID(philo_1) \
      ESM_ID(philo_2) \
      ESM_ID(philo_3) \
      ESM_ID(philo_4) \
      ESM_ID(philo_5) \
      ESM_ID(table)

typedef struct
{
	uint8_t num;
} esm_sig_params_t;

#endif /* INCLUDE_SIGNALS_H_ */
