#include "tap_detector.h"
#include "config.h"

ESM_THIS_FILE;

ESM_DEFINE_STATE(idle);
ESM_DEFINE_STATE(active);

static void esm_idle_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_idle_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_idle_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	tap_detector_esm_t *self = ESM_CONTAINER_OF(esm, tap_detector_esm_t, esm);

	switch (sig->type)
	{
	case esm_sig_keypress:
		if (sig->params.key.kev == key_ev_down)
		{
			self->params = sig->params;
			ESM_TRANSITION(active);
		} else {
			esm_signal_t s = *sig;
			s.receiver = keyboard_esm;
			esm_send_signal(&s);
		}
		break;

	case esm_sig_tap:
	break;

	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

static void esm_active_entry(esm_t *const esm)
{
	tap_detector_esm_t *self = ESM_CONTAINER_OF(esm, tap_detector_esm_t, esm);

	esm_signal_t s = {
		.type = esm_sig_tmout,
		.sender = esm,
		.receiver = esm};
	esm_timer_add(&self->timer, self->cfg->tap_tres, &s);
}

static void esm_active_exit(esm_t *const esm)
{
	tap_detector_esm_t *self = ESM_CONTAINER_OF(esm, tap_detector_esm_t, esm);
	esm_timer_rm(&self->timer);
}

static void esm_active_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	tap_detector_esm_t *self = ESM_CONTAINER_OF(esm, tap_detector_esm_t, esm);

	switch (sig->type)
	{
	case esm_sig_tmout:
	case esm_sig_tap:
	{
		esm_signal_t s = {
			.type = esm_sig_keypress,
			.params = self->params,
			.sender = NULL,
			.receiver = keyboard_esm};
		esm_send_to_front(&s);
		ESM_TRANSITION(idle);
	}
	break;

	case esm_sig_keypress:
		if (sig->params.key.kev == key_ev_up)
		{
			{
				self->params.key.kev = key_ev_tap;
				esm_signal_t s = {
					.type = esm_sig_keypress,
					.params = self->params,
					.sender = NULL,
					.receiver = keyboard_esm};
				esm_send_signal(&s);
			}
			{
				self->params.key.kev = key_ev_up;
				esm_signal_t s = {
					.type = esm_sig_keypress,
					.params = self->params,
					.sender = NULL,
					.receiver = keyboard_esm};
				esm_send_signal(&s);
			}
		}
		else
		{
			esm_signal_t s = *sig;
			s.receiver = keyboard_esm;
			esm_send_signal(&s);
		}
		ESM_TRANSITION(idle);
		break;
		
	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

void esm_tap_detector_init(esm_t *const esm)
{
	ESM_TRANSITION(idle);
}