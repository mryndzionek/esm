#ifndef RB_H_INCLUDED
#define RB_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

typedef struct {
	size_t beg_index_, end_index_, size_;
	size_t const capacity_;
	uint8_t *data_;
} rb_t;

size_t rb_size(rb_t *rb);
size_t rb_capacity(rb_t *rb);
size_t rb_write(rb_t *rb, const uint8_t *data, size_t bytes);
size_t rb_read(rb_t *rb, uint8_t *data, size_t bytes);

#endif // RB_H_INCLUDED
