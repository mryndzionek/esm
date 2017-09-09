#include "esm/esm.h"
#include "esm/esm_timer.h"

#define N_PHILO		(5)

#define RIGHT(n_) ((uint8_t)(((n_) + (N_PHILO - 1U)) % N_PHILO))
#define LEFT(n_)  ((uint8_t)(((n_) + 1U) % N_PHILO))
#define FREE      ((uint8_t)0)
#define USED      ((uint8_t)1)

ESM_THIS_FILE;

typedef struct {
} table_cfg_t;

typedef struct {
	esm_t esm;
	esm_timer_t timer;
    esm_t *philo[N_PHILO];
    uint8_t fork[N_PHILO];
    uint8_t is_hungry[N_PHILO];
	table_cfg_t const *const cfg;
} table_esm_t;

ESM_DEFINE_STATE(serving);

static void esm_serving_entry(esm_t *const esm)
{
	table_esm_t *self = ESM_CONTAINER_OF(esm, table_esm_t, esm);
	(void)self;
}

static void esm_serving_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_serving_handle(esm_t *const esm, esm_signal_t *sig)
{
	table_esm_t *self = ESM_CONTAINER_OF(esm, table_esm_t, esm);

	switch(sig->type)
	{
	case esm_sig_hungry:
	{
		uint8_t n = sig->params.num;
		ESM_ASSERT((n < N_PHILO) &&
				(self->is_hungry[n] == 0U));
		uint8_t m = LEFT(n);

		if ((self->fork[m] == FREE) && (self->fork[n] == FREE)) {
			self->fork[m] = USED;
			self->fork[n] = USED;
			esm_signal_t _sig = {
					.type = esm_sig_eat,
					.sender = esm,
					.receiver = sig->sender,
			};
			esm_send_signal(&_sig);
		}
		else {
			self->is_hungry[n] = 1U;
			self->philo[n] = sig->sender;
		}
	}
	break;
	case esm_sig_done:
	{
		uint8_t n = sig->params.num;
		ESM_ASSERT((n < N_PHILO) &&
				(self->is_hungry[n] == 0U));
		uint8_t m = LEFT(n);
		ESM_ASSERT((self->fork[n] == USED) && (self->fork[m] == USED));

		self->fork[m] = FREE;
		self->fork[n] = FREE;
		m = RIGHT(n);

		if ((self->is_hungry[m] != 0U) && (self->fork[m] == FREE)) {
			self->fork[n] = USED;
			self->fork[m] = USED;
			self->is_hungry[m] = 0U;

			esm_signal_t _sig = {
					.type = esm_sig_eat,
					.sender = esm,
					.receiver = self->philo[m],
			};
			esm_send_signal(&_sig);
			self->philo[m] = (void *)0;
		}
		m = LEFT(n);
		n = LEFT(m);
		if ((self->is_hungry[m] != 0U) && (self->fork[n] == FREE)) {
			self->fork[m] = USED;
			self->fork[n] = USED;
			self->is_hungry[m] = 0U;

			esm_signal_t _sig = {
					.type = esm_sig_eat,
					.sender = esm,
					.receiver = self->philo[m],
			};
			esm_send_signal(&_sig);
			self->philo[m] = (void *)0;
		}
	}
	break;
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_table_init(esm_t *const esm)
{
	ESM_TRANSITION(serving);
}

static const table_cfg_t table_cfg = {
};

ESM_REGISTER(table, table, esm_gr_none, N_PHILO);
