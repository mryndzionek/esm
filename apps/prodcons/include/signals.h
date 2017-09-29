#ifndef INCLUDE_SIGNALS_H_
#define INCLUDE_SIGNALS_H_

#define ESM_SIGNALS \
		ESM_SIGNAL(tmout) \
		ESM_SIGNAL(request) \
		ESM_SIGNAL(response)

#define ESM_IDS \
      ESM_ID(producer_1) \
      ESM_ID(producer_2) \
      ESM_ID(bus) \
      ESM_ID(consumer)

#define ESM_SIG_PARAMS \
	bus_xfer_t *xfer;

#endif /* INCLUDE_SIGNALS_H_ */
