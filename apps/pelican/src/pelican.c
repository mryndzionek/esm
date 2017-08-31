#include "esm/hesm.h"
#include "esm/esm_timer.h"

ESM_THIS_FILE;

typedef struct {
} pelican_cfg_t;

typedef struct {
	hesm_t esm;
	esm_timer_t timer;
	uint8_t flash_count;
	pelican_cfg_t const *const cfg;
} pelican_esm_t;

#define GREEN_TOUT_MS	(10000UL)
#define YELLOW_TOUT_MS	(3000UL)
#define WALK_TOUT_MS	(5000UL)
#define FLASH_TOUT_MS	(1000UL)
#define FLASH_COUNT		(5UL)

#define ESM_INIT_SUB \
      (ESM_SIG_MASK(esm_sig_tmout) | \
            ESM_SIG_MASK(esm_sig_button))

ESM_COMPLEX_STATE(operational, top, 1);

ESM_COMPLEX_STATE(pedestrians, operational, 2);
ESM_COMPLEX_STATE(cars, operational, 2);

ESM_COMPLEX_STATE(green, cars, 3);
ESM_LEAF_STATE(yellow, cars, 3);

ESM_LEAF_STATE(green_no_ped, green, 4);
ESM_LEAF_STATE(green_int, green, 4);
ESM_LEAF_STATE(green_ped_wait, green, 4);

ESM_LEAF_STATE(walk, pedestrians, 3);
ESM_LEAF_STATE(flash, pedestrians, 3);

// operational
static void esm_operational_init(esm_t *const esm)
{
	(void)esm;
	ESM_TRANSITION(cars);
}

static void esm_operational_entry(esm_t *const esm)
{
	(void)esm;
	ESM_PRINTF("[%010u] [%s] CARS_RED\r\n", esm_global_time, esm->name);
	ESM_PRINTF("[%010u] [%s] PEDS_DONT_WALK\r\n", esm_global_time, esm->name);
}

static void esm_operational_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_operational_handle(esm_t *const esm, esm_signal_t *sig)
{
	(void)esm;

	switch(sig->type)
	{
	case esm_sig_button:
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

// cars
static void esm_cars_init(esm_t *const esm)
{
	(void)esm;
	ESM_TRANSITION(green);
}

static void esm_cars_entry(esm_t *const esm)
{
	(void)esm;
	ESM_PRINTF("[%010u] [%s] CARS_GREEN\r\n", esm_global_time, esm->name);
}

static void esm_cars_exit(esm_t *const esm)
{
	(void)esm;
	ESM_PRINTF("[%010u] [%s] CARS_RED\r\n", esm_global_time, esm->name);
}

static void esm_cars_handle(esm_t *const esm, esm_signal_t *sig)
{
	(void)esm;

	switch(sig->type)
	{
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

// green
static void esm_green_init(esm_t *const esm)
{
	(void)esm;
	ESM_TRANSITION(green_no_ped);
}

static void esm_green_entry(esm_t *const esm)
{
	pelican_esm_t *self = ESM_CONTAINER_OF(esm, pelican_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			GREEN_TOUT_MS, &sig);
}

static void esm_green_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_green_handle(esm_t *const esm, esm_signal_t *sig)
{
	(void)esm;

	switch(sig->type)
	{
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

// yellow
static void esm_yellow_entry(esm_t *const esm)
{
	pelican_esm_t *self = ESM_CONTAINER_OF(esm, pelican_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			YELLOW_TOUT_MS, &sig);

	ESM_PRINTF("[%010u] [%s] CARS_YELLOW\r\n", esm_global_time, esm->name);
}

static void esm_yellow_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_yellow_handle(esm_t *const esm, esm_signal_t *sig)
{
	(void)esm;

	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(pedestrians);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

// green_no_ped
static void esm_green_no_ped_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_green_no_ped_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_green_no_ped_handle(esm_t *const esm, esm_signal_t *sig)
{
	(void)esm;

	switch(sig->type)
	{
	case esm_sig_button:
		if(sig->params.key == ' ')
		{
			ESM_TRANSITION(green_ped_wait);
		}
		break;
	case esm_sig_tmout:
		ESM_TRANSITION(green_int);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

// green_int
static void esm_green_int_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_green_int_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_green_int_handle(esm_t *const esm, esm_signal_t *sig)
{
	(void)esm;

	switch(sig->type)
	{
	case esm_sig_button:
		if(sig->params.key == ' ')
		{
			ESM_TRANSITION(yellow);
		}
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

// green_ped_wait
static void esm_green_ped_wait_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_green_ped_wait_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_green_ped_wait_handle(esm_t *const esm, esm_signal_t *sig)
{
	(void)esm;

	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(yellow);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

// pedestrians
static void esm_pedestrians_init(esm_t *const esm)
{
	(void)esm;
	ESM_TRANSITION(walk);
}

static void esm_pedestrians_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_pedestrians_exit(esm_t *const esm)
{
	(void)esm;
	ESM_PRINTF("[%010u] [%s] PEDS_DONT_WALK\r\n", esm_global_time, esm->name);
}

static void esm_pedestrians_handle(esm_t *const esm, esm_signal_t *sig)
{
	(void)esm;

	switch(sig->type)
	{
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

// walk
static void esm_walk_entry(esm_t *const esm)
{
	pelican_esm_t *self = ESM_CONTAINER_OF(esm, pelican_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};
	esm_timer_add(&self->timer,
			WALK_TOUT_MS, &sig);

	ESM_PRINTF("[%010u] [%s] PEDS_WALK\r\n", esm_global_time, esm->name);
}

static void esm_walk_exit(esm_t *const esm)
{
	pelican_esm_t *self = ESM_CONTAINER_OF(esm, pelican_esm_t, esm);
	self->flash_count = FLASH_COUNT;
}

static void esm_walk_handle(esm_t *const esm, esm_signal_t *sig)
{
	(void)esm;

	switch(sig->type)
	{
	case esm_sig_tmout:
		ESM_TRANSITION(flash);
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

// flash
static void esm_flash_entry(esm_t *const esm)
{
	pelican_esm_t *self = ESM_CONTAINER_OF(esm, pelican_esm_t, esm);
	esm_signal_t sig = {
			.type = esm_sig_tmout,
			.sender = esm,
			.receiver = esm
	};

	esm_timer_add(&self->timer,
			FLASH_TOUT_MS, &sig);

	if(self->flash_count & 1UL)
	{
		ESM_PRINTF("[%010u] [%s] ** PEDS_DONT_WALK **\r\n", esm_global_time, esm->name);
	}
	else
	{
		ESM_PRINTF("[%010u] [%s]    PEDS_DONT_WALK   \r\n", esm_global_time, esm->name);
	}
}

static void esm_flash_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_flash_handle(esm_t *const esm, esm_signal_t *sig)
{
	pelican_esm_t *self = ESM_CONTAINER_OF(esm, pelican_esm_t, esm);

	switch(sig->type)
	{
	case esm_sig_tmout:
		if(--self->flash_count)
		{
			ESM_TRANSITION(self);
		}
		else
		{
			ESM_TRANSITION(cars);
		}
		break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static const pelican_cfg_t pelican_cfg = {
};

ESM_COMPLEX_REGISTER(pelican, pelican, operational, 1, 5);
