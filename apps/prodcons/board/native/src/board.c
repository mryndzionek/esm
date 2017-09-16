#include "board.h"
#include "esm/esm.h"
#include "synchronizer.h"

static void synchronizer_request(esm_signal_t *sig)
{
	sig->receiver = consumer_esm;
	esm_send_signal(sig);
}

static void synchronizer_respond(esm_signal_t *sig)
{
	sig->receiver = consumer_esm;
	esm_send_signal(sig);
}

static esm_queue_t synchronizer_q = {
		.size = 2,
		.data = (esm_signal_t[2]){0},
};

static const synchronizer_cfg_t synchronizer_cfg = {
		.timeout = 6000UL,
		.req = esm_sig_request,
		.rsp = esm_sig_response,
		.queue = &synchronizer_q,
		.request = synchronizer_request,
		.respond = synchronizer_respond
};

ESM_REGISTER(synchronizer, synchronizer, esm_gr_none, 1);
