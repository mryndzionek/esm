#include "esm/esm.h"
#include "esm/esm_timer.h"
#include "board.h"

#define MIN_DELAY_MS	(1000UL)
#define MAX_DELAY_MS	(5000UL)

ESM_THIS_FILE;

typedef struct {
	uint32_t speed;
	uint8_t payload[16];
	bus_xfer_t xfer;
} producer_xfer_t;

typedef struct {
	uint8_t num;
	producer_xfer_t *xfer;
} producer_cfg_t;

typedef struct {
	esm_t esm;
	esm_timer_t timer;
	producer_cfg_t const *const cfg;
} producer_esm_t;

ESM_DEFINE_STATE(idle);
ESM_DEFINE_STATE(busy);

static void prod_tx(bus_xfer_t *xfer)
{
	(void) xfer;
	BOARD_TX();

	esm_signal_t sig = {
			.type = esm_sig_request,
			.sender = NULL,
			.receiver = consumer_esm
	};
	esm_send_signal(&sig);
}

static void prod_rx(bus_xfer_t *xfer)
{
	(void) xfer;
	BOARD_RX();
}

static void esm_idle_entry(esm_t *const esm)
{
	producer_esm_t *self = ESM_CONTAINER_OF(esm, producer_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			MIN_DELAY_MS + ESM_RANDOM(MAX_DELAY_MS - MIN_DELAY_MS), &sig);
}

static void esm_idle_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_idle_handle(esm_t *const esm, esm_signal_t *sig)
{
	(void)esm;

	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(busy);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_busy_entry(esm_t *const esm)
{
	producer_esm_t *self = ESM_CONTAINER_OF(esm, producer_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_request,
			.sender = esm,
			.receiver = bus_esm,
			.params = {
					.xfer = &self->cfg->xfer->xfer
			}
	};
	esm_send_signal(&sig);
}

static void esm_busy_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_busy_handle(esm_t *const esm, esm_signal_t *sig)
{
	(void)esm;

	switch(sig->type)
	{
	case esm_sig_response:
		ESM_TRANSITION(idle);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_producer_init(esm_t *const esm)
{
	ESM_TRANSITION(idle);
}

static producer_xfer_t xfer1 = {
		.speed = 100000,
		.xfer = {
				.tx = prod_tx,
				.rx = prod_rx
		}
};

static const producer_cfg_t producer_1_cfg = {
		.num = 0,
		.xfer = &xfer1
};

ESM_REGISTER(producer, producer_1, esm_gr_none, 1);

static producer_xfer_t xfer2 = {
		.speed = 400000,
		.xfer = {
				.tx = prod_tx,
				.rx = prod_rx
		}
};

static const producer_cfg_t producer_2_cfg = {
		.num = 1,
		.xfer = &xfer2
};

ESM_REGISTER(producer, producer_2, esm_gr_none, 1);
