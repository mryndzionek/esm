#ifndef TRACE_INCLUDE_TRACE_H_
#define TRACE_INCLUDE_TRACE_H_

#include <stdint.h>
#include <stddef.h>

void trace_init(uint8_t esm);
void trace_trans(uint8_t esm, uint8_t sig, char const * const cs, char const * const ns);
void trace_receive(uint8_t esm, uint8_t sig, char const * const cs);
size_t trace_get(uint8_t *data, size_t bytes);

#endif /* TRACE_INCLUDE_TRACE_H_ */
