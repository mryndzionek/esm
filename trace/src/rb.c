#include "rb.h"

#include <string.h>

size_t rb_size(rb_t *rb)
{
	return rb->size_;
}

size_t rb_capacity(rb_t *rb)
{
	return rb->capacity_;
}

size_t rb_write(rb_t *rb, const uint8_t *data, size_t bytes)
{
	if (bytes == 0) return 0;

	size_t capacity = rb->capacity_;

	if (bytes <= capacity - rb->end_index_)
	{
		memcpy(rb->data_ + rb->end_index_, data, bytes);
		rb->end_index_ += bytes;
		if (rb->end_index_ == capacity) rb->end_index_ = 0;
	}
	else
	{
		size_t size_1 = capacity - rb->end_index_;
		memcpy(rb->data_ + rb->end_index_, data, size_1);
		size_t size_2 = bytes - size_1;
		memcpy(rb->data_, data + size_1, size_2);
		rb->end_index_ = size_2;
	}

	if(bytes > (capacity - rb->size_))
	{
		rb->beg_index_ = rb->end_index_;
		rb->size_ = capacity;
	} else {
		rb->size_ += bytes;
	}
	return bytes;
}

size_t rb_read(rb_t *rb, uint8_t *data, size_t bytes)
{
	if (bytes == 0) return 0;

	size_t capacity = rb->capacity_;
	size_t bytes_to_read = bytes > rb->size_ ? rb->size_ : bytes;

	if (bytes_to_read <= capacity - rb->beg_index_)
	{
		memcpy(data, rb->data_ + rb->beg_index_, bytes_to_read);
		rb->beg_index_ += bytes_to_read;
		if (rb->beg_index_ == capacity) rb->beg_index_ = 0;
	}
	else
	{
		size_t size_1 = capacity - rb->beg_index_;
		memcpy(data, rb->data_ + rb->beg_index_, size_1);
		size_t size_2 = bytes_to_read - size_1;
		memcpy(data + size_1, rb->data_, size_2);
		rb->beg_index_ = size_2;
	}

	rb->size_ -= bytes_to_read;
	return bytes_to_read;
}
