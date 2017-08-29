#include "esm/hesm.h"

const esm_hstate_t esm_top_state = {
		.super = {
				.entry = (void*)0,
				.handle = (void*)0,
				.exit = (void*)0,
				.name = "esm_top",
		}
};
