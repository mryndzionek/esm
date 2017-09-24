#include "board.h"

#include "bus.h"
#include "esm/esm.h"

static esm_list_t bus_q;

static const bus_cfg_t bus_cfg = {
		.timeout = 6000UL,
		.req = esm_sig_request,
		.rsp = esm_sig_response,
		.queue = &bus_q,
};

ESM_REGISTER(bus, bus, esm_gr_none, 1);
