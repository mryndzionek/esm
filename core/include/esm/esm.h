#ifndef INCLUDE_ESM_ESM_H_
#define INCLUDE_ESM_ESM_H_

#include <stdbool.h>
#include <stddef.h>
#include "platform.h"
#include "signals.h"

#define ESM_CONTAINER_OF(ptr, type, field) \
		((type *)(((char *)(ptr)) - offsetof(type, field)))

#define ESM_THIS_FILE \
		static char const THIS_FILE__[] = __FILE__

#define ESM_DEFINE_STATE(_name) \
		static void esm_##_name##_entry(esm_t *const esm); \
		static void esm_##_name##_handle(esm_t *const esm, esm_signal_t *sig); \
		static void esm_##_name##_exit(esm_t *const esm); \
		static const esm_state_t esm_##_name##_state = { \
				.entry = esm_##_name##_entry, \
				.handle = esm_##_name##_handle, \
				.exit = esm_##_name##_exit, \
				.name = #_name, \
		}

#define ESM_REGISTER(_type, _name, _init, _sigq_size) \
		static _type##_esm_t _name##_ctx = { \
				.esm = { \
						.name = #_name, \
						.id = esm_id_##_name, \
						.subscribed = ESM_INIT_SUB, \
						.curr_state = &esm_##_init##_state, \
						.sig_queue_size = _sigq_size, \
						.sig_queue = (esm_signal_t[_sigq_size]){0}, \
		}, \
		.cfg = &_name##_cfg \
		}; \
		esm_t * const _name##_esm \
		__attribute((__section__("esm_simple"))) \
		__attribute((__used__)) = &_name##_ctx.esm;

#define ESM_TRANSITION(_state) do { \
		ESM_ASSERT(esm); \
		esm->next_state = (esm_state_t *)_state; \
} while(0)

#define ESM_SUBSCRIBE(_sig) esm->subscribed |= (1UL << _sig)
#define ESM_UNSUBSCRIBE(_sig) esm->subscribed &= ~(1UL << _sig)

#define ESM_SIG_MASK(_sig) (1UL << _sig)

#define ESM_SIGNAL(_name) esm_sig_##_name,
typedef enum
{
	esm_sig_alarm = 0,
	ESM_SIGNALS
} esm_signal_e;
#undef ESM_SIGNAL

#define ESM_ID(_name) esm_id_##_name,
typedef enum {
	esm_id_tick = 0,
	esm_id_trace,
	ESM_IDS
} esm_id_e;
#undef ESM_ID

typedef struct _esm esm_t;

#define ESM_ID(_name) extern esm_t * const _name##_esm;
extern esm_t * const tick_esm;
extern esm_t * const trace_esm;
ESM_IDS
#undef ESM_ID

typedef struct
{
	esm_signal_e type;
	esm_t *receiver;
	esm_t *sender;
	esm_sig_params_t params;
} esm_signal_t;

typedef struct {
	char const *const name;
	void (*entry)(esm_t *const esm);
	void (*handle)(esm_t *const esm, esm_signal_t *sig);
	void (*exit)(esm_t *const esm);
} esm_state_t;

struct _esm {
	char const *const name;
	const uint8_t id;
	uint32_t subscribed;
	esm_state_t const *curr_state;
	esm_state_t const *next_state;
	const uint8_t sig_queue_size;
	uint8_t sig_len;
	uint8_t sig_head;
	uint8_t sig_tail;
	esm_signal_t *sig_queue;
};

extern const esm_state_t esm_unhandled_sig;
extern const esm_state_t esm_self_transition;

void esm_process(void);
void esm_broadcast_signal(esm_signal_t *sig);
bool esm_send_signal(esm_signal_t *sig);

#endif /* INCLUDE_ESM_ESM_H_ */
