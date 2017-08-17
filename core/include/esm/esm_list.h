#ifndef INC_ESM_LIST_H_
#define INC_ESM_LIST_H_


typedef struct esm_list_item esm_list_item_t;

/* Doubly-linked list. */

struct esm_list_item {
	esm_list_item_t *next;
	esm_list_item_t *prev;
};

typedef struct {
	esm_list_item_t *first;
	esm_list_item_t *last;
} esm_list_t;

void esm_list_init(esm_list_t *self);
#define esm_list_empty(self) (!((self)->first))
#define esm_list_begin(self) ((self)->first)
#define esm_list_next(it) ((it)->next)

void esm_list_insert(esm_list_t *self, esm_list_item_t *item,
		esm_list_item_t *it);

esm_list_item_t *esm_list_erase(esm_list_t *self,
		esm_list_item_t *item);

#endif /* INC_ESM_LIST_H_ */
