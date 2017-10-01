#ifndef INCLUDE_ESM_ESM_H_
#define INCLUDE_ESM_ESM_H_

#include <stdbool.h>
#include <stddef.h>
#include "esm_list.h"
#include "platform.h"
#include "signals.h"

#ifndef ESM_MAX_PRIO
#define ESM_MAX_PRIO	(1)
#endif

#define ESM_CONTAINER_OF(ptr, type, field) \
		((type *)(((char *)(ptr)) - offsetof(type, field)))

#define ESM_THIS_FILE \
		static char const THIS_FILE__[] = __FILE__

#define ESM_DEFINE_STATE(_name) \
		static void esm_##_name##_entry(esm_t *const esm); \
		static void esm_##_name##_handle(esm_t *const esm, const esm_signal_t * const sig); \
		static void esm_##_name##_exit(esm_t *const esm); \
		static const esm_state_t esm_##_name##_state = { \
				.entry = esm_##_name##_entry, \
				.handle = esm_##_name##_handle, \
				.exit = esm_##_name##_exit, \
				.name = #_name, \
		}

#define ESM_REGISTER(_type, _name, _group, _sigq_size) \
        static const esm_cfg_t esm_##_name##_cfg = { \
            .name = #_name, \
            .id = esm_id_##_name, \
            .is_cplx = false, \
            .group = _group, \
		}; \
		static _type##_esm_t _name##_ctx = { \
				.esm = { \
						.cfg = &esm_##_name##_cfg, \
						.init = esm_##_type##_init, \
						.queue = { \
						      .size = _sigq_size, \
						      .data = (esm_signal_t[_sigq_size]){0}, \
				}\
		}, \
		.cfg = &_name##_cfg \
		}; \
		esm_t * const _name##_esm \
		__attribute((__section__("esm_sec"))) \
		__attribute((__used__)) = &_name##_ctx.esm;

#define ESM_TRANSITION(_s) do { \
		ESM_ASSERT(esm); \
		esm->next_state = (esm_state_t const * const)&esm_##_s##_state; \
} while(0)

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

typedef struct
{
	char const *const name;
	const uint8_t id;
	const bool is_cplx;
	const uint8_t group;
	const uint16_t prio;
} esm_cfg_t;

typedef struct _esm esm_t;

typedef struct _bus_xfer bus_xfer_t;
struct _bus_xfer {
	esm_t *receiver;
	void (*exec)(bus_xfer_t *xfer);
	esm_list_item_t item;
};

#define ESM_ID(_name) extern esm_t * const _name##_esm;
extern esm_t * const tick_esm;
extern esm_t * const trace_esm;
ESM_IDS
#undef ESM_ID

#define ESM_GROUP(_name) esm_gr_##_name = (1UL << __COUNTER__),
typedef enum {
	esm_gr_none = 0,
	ESM_GROUPS
} esm_group_e;
#undef ESM_GROUP

#ifndef ESM_SIG_PARAMS
#define ESM_SIG_PARAMS
#endif

typedef struct
{
	ESM_SIG_PARAMS
} esm_sig_params_t;

typedef struct
{
	esm_signal_e type;
	esm_t *receiver;
	esm_t *sender;
	esm_sig_params_t params;
	esm_list_item_t item;
} esm_signal_t;

typedef struct {
	const uint8_t size;
	uint8_t len;
	uint8_t head;
	uint8_t tail;
	esm_signal_t *data;
} esm_queue_t;

typedef struct {
	char const *const name;
	void (*entry)(esm_t *const esm);
	void (*handle)(esm_t *const esm, const esm_signal_t * const sig);
	void (*exit)(esm_t *const esm);
} esm_state_t;

struct _esm {
	const esm_cfg_t *const cfg;
	void (*init)(esm_t *const esm);
	esm_state_t const *curr_state;
	esm_state_t const *next_state;
	esm_queue_t queue;
};

extern const esm_state_t esm_unhandled_state;
extern const esm_state_t esm_self_state;

void esm_process(void);
void esm_broadcast_signal(esm_signal_t *sig, esm_group_e group);
bool esm_send_signal(esm_signal_t *sig);

#endif /* INCLUDE_ESM_ESM_H_ */
