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

#define ESM_SIG_PARAMS \
      uint8_t num;


#endif /* INCLUDE_SIGNALS_H_ */
