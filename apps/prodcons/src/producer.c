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
	void (* const operation)(bus_xfer_t *xfer);
} producer_cfg_t;

typedef struct {
	esm_t esm;
	esm_timer_t timer;
	producer_xfer_t xfer;
	producer_cfg_t const *const cfg;
} producer_esm_t;

ESM_DEFINE_STATE(idle);
ESM_DEFINE_STATE(busy);

static void read_sensor(bus_xfer_t *xfer)
{
	(void) xfer;
	BOARD_READ_SENSOR();

	esm_signal_t sig = {
			.type = esm_sig_request,
			.sender = xfer->receiver,
			.receiver = consumer_esm
	};
	esm_send_signal(&sig);
}

static void write_actuator(bus_xfer_t *xfer)
{
	(void) xfer;
	BOARD_WRITE_ACTUATOR();

	esm_signal_t sig = {
			.type = esm_sig_request,
			.sender = xfer->receiver,
			.receiver = consumer_esm
	};
	esm_send_signal(&sig);
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

static void esm_idle_handle(esm_t *const esm, const esm_signal_t * const sig)
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

	self->xfer.xfer.exec = self->cfg->operation;
	self->xfer.xfer.receiver = esm;

	esm_signal_t sig = {
			.type = esm_sig_request,
			.sender = esm,
			.receiver = bus_esm,
			.params = {
					.xfer = &self->xfer.xfer
			}
	};
	esm_send_signal(&sig);
}

static void esm_busy_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_busy_handle(esm_t *const esm, const esm_signal_t * const sig)
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

static const producer_cfg_t producer_1_cfg = {
		.num = 0,
		.operation = read_sensor
};

ESM_REGISTER(producer, producer_1, esm_gr_none, 1);

static const producer_cfg_t producer_2_cfg = {
		.num = 1,
		.operation = write_actuator
};

ESM_REGISTER(producer, producer_2, esm_gr_none, 1);
