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

typedef struct _bus_xfer bus_xfer_t;
struct _bus_xfer {
	void (*const tx)(bus_xfer_t *xfer);
	void (*const rx)(bus_xfer_t *xfer);
	esm_list_item_t item;
};

typedef struct
{
	bus_xfer_t *xfer;
} esm_sig_params_t;

#endif /* INCLUDE_SIGNALS_H_ */
