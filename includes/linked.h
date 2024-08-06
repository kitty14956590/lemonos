#pragma once

typedef struct linked {
	void * p;
	struct linked * next;
	struct linked * back;
} linked_t;

typedef struct slinked {
	void * p;
	struct slinked * next;
} slinked_t;

typedef int (* linked_callback_t)(linked_t * node, void * pass);

linked_t * linked_leaf(linked_t * bottom);
linked_t * linked_branch(linked_t * node);
int linked_count(linked_t * bottom);
linked_t * linked_add(linked_t * bottom, void * p);
linked_t * linked_get(linked_t * bottom, int index);
linked_t * linked_remove(linked_t * bottom, int index);
linked_t * linked_delete(linked_t * node);
void linked_iterate(linked_t * bottom, linked_callback_t callback, void * pass);
linked_t * linked_find(linked_t * bottom, linked_callback_t callback, void * pass);
linked_t * linked_find_back(linked_t * bottom, linked_callback_t callback, void * pass);
void linked_chop_down(linked_t * bottom, linked_callback_t destroy, void * pass);