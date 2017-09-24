#ifndef CORE_INCLUDE_ESM_HESM_H_
#define CORE_INCLUDE_ESM_HESM_H_

#include "esm/esm.h"

#define ESM_COMPLEX_STATE(_name, _parent, _depth) \
      static void esm_##_name##_entry(esm_t *const esm); \
      static void esm_##_name##_handle(esm_t *const esm, const esm_signal_t * const sig); \
      static void esm_##_name##_exit(esm_t *const esm); \
      static void esm_##_name##_init(esm_t *const esm); \
      static const esm_hstate_t esm_##_name##_state = { \
            .super = { \
                  .entry = esm_##_name##_entry, \
                  .handle = esm_##_name##_handle, \
                  .exit = esm_##_name##_exit, \
                  .name = #_name, \
      }, \
      .parent = &esm_##_parent##_state, \
      .init = esm_##_name##_init, \
      .depth = _depth \
      }

#define ESM_LEAF_STATE(_name, _parent, _depth) \
      static void esm_##_name##_entry(esm_t *const esm); \
      static void esm_##_name##_handle(esm_t *const esm, const esm_signal_t * const sig); \
      static void esm_##_name##_exit(esm_t *const esm); \
      static const esm_hstate_t esm_##_name##_state = { \
            .super = { \
                  .entry = esm_##_name##_entry, \
                  .handle = esm_##_name##_handle, \
                  .exit = esm_##_name##_exit, \
                  .name = #_name, \
      }, \
      .parent = &esm_##_parent##_state, \
      .init = NULL, \
	  .depth = _depth \
      }

#define ESM_COMPLEX_REGISTER(_type, _name, _group, _sigq_size, _depth) \
      static _type##_esm_t _name##_ctx = { \
            .esm = { \
                  .super = { \
                        .name = #_name, \
                        .id = esm_id_##_name, \
						.is_cplx = true, \
                        .group = _group, \
                        .init = esm_##_type##_init, \
                        .queue = { \
                              .size = _sigq_size, \
                              .data = (esm_signal_t[_sigq_size]){0}, \
                  }\
            }, \
            .depth = _depth, \
      }, \
      .cfg = &_name##_cfg \
      }; \
      esm_t * const _name##_esm \
      __attribute((__section__("esm_sec"))) \
      __attribute((__used__)) = (esm_t *)&_name##_ctx.esm;

typedef struct _hesmstate esm_hstate_t;

typedef struct {
	esm_t super;
	const uint8_t depth;
} hesm_t;

struct _hesmstate {
	esm_state_t super;
	struct _hesmstate const * const parent;
	void (*init)(esm_t *const esm);
	const uint8_t depth;
};

extern const esm_hstate_t esm_top_state;

#endif /* CORE_INCLUDE_ESM_HESM_H_ */
