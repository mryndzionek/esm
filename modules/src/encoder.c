#include "encoder.h"

ESM_THIS_FILE;

ESM_DEFINE_STATE(active);

#define START (0x0)
#define CW_Step1 (0x1)
#define CW_Step2 (0x2)
#define CW_Step3 (0x3)
#define CCW_Step1 (0x4)
#define CCW_Step2 (0x5)
#define CCW_Step3 (0x6)

#define CW_Flag (0b10000000)
#define CCW_Flag (0b01000000)

static const uint8_t transition_table[7][4] = {
	{START, CCW_Step1, CW_Step1, START},
	{CW_Step2, START, CW_Step1, START},
	{CW_Step2, CW_Step3, CW_Step1, START},
	{CW_Step2, CW_Step3, START, START | CW_Flag},
	{CCW_Step2, CCW_Step1, START, START},
	{CCW_Step2, CCW_Step1, CCW_Step3, START},
	{CCW_Step2, START, CCW_Step3, START | CCW_Flag}};

static void esm_active_entry(esm_t *const esm)
{
	(void)esm;
}

static void esm_active_exit(esm_t *const esm)
{
	(void)esm;
}

static void esm_active_handle(esm_t *const esm, const esm_signal_t *const sig)
{
	encoder_esm_t *self = ESM_CONTAINER_OF(esm, encoder_esm_t, esm);

	switch (sig->type)
	{
	case esm_sig_alarm:
	{
		uint8_t mask = (1UL << sig->params.encoder.chan);

		if (sig->params.encoder.state)
		{
			self->chanls |= mask;
		}
		else
		{
			self->chanls &= ~mask;
		}

		self->state = transition_table[self->state & 0b00000111][self->chanls];

		if ((self->state & CW_Flag) && (self->cfg->cw_action))
		{
			self->cfg->cw_action();
		}
		else if ((self->state & CCW_Flag) && (self->cfg->ccw_action))
		{
			self->cfg->ccw_action();
		}
	}
	break;

	default:
		ESM_TRANSITION(unhandled);
		break;
	}
}

void esm_encoder_init(esm_t *const esm)
{
	ESM_TRANSITION(active);
}
