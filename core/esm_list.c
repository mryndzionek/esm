#include <stddef.h>

#include "esm/esm_list.h"

void esm_list_init(esm_list_t *self)
{
	self->first = NULL;
	self->last = NULL;
}

void esm_list_insert(esm_list_t *self, esm_list_item_t *item,
		esm_list_item_t *it)
{
	item->prev = it ? it->prev : self->last;
	item->next = it;
	if(item->prev)
		item->prev->next = item;
	if(item->next)
		item->next->prev = item;
	if(!self->first || self->first == it)
		self->first = item;
	if(!it)
		self->last = item;
}

esm_list_item_t *esm_list_erase(esm_list_t *self,
		esm_list_item_t *item)
{
	esm_list_item_t *next;

	if(item->prev)
		item->prev->next = item->next;
	else
		self->first = item->next;
	if(item->next)
		item->next->prev = item->prev;
	else
		self->last = item->prev;

	next = item->next;

	item->prev = NULL;
	item->next = NULL;

	return next;
}
