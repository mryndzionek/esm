#include "board.h"

#include "bus.h"
#include "esm/esm.h"

static void bus_request(esm_signal_t *sig)
{
	sig->receiver = consumer_esm;
	esm_send_signal(sig);
}

static void bus_respond(esm_signal_t *sig)
{
	sig->receiver = consumer_esm;
	esm_send_signal(sig);
}

static esm_queue_t bus_q = {
		.size = 2,
		.data = (esm_signal_t[2]){0},
};

static const bus_cfg_t bus_cfg = {
		.timeout = 6000UL,
		.req = esm_sig_request,
		.rsp = esm_sig_response,
		.queue = &bus_q,
		.request = bus_request,
		.respond = bus_respond
};

ESM_REGISTER(bus, bus, esm_gr_none, 1);
