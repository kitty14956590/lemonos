#include <linked.h>
#include <memory.h>

linked_t * linked_leaf(linked_t * bottom) {
	linked_t * top = bottom;
	while (top->next) {
		top = top->next;
	}
	return top;
}

linked_t * linked_branch(linked_t * node) {
	linked_t * p = node;
	while (p->back) {
		p = p->back;
	}
	return p;
}

int linked_count(linked_t * bottom) {
	if (!bottom) {
		return 0;
	}
	linked_t * current = bottom;
	int i = 0;
	while (current) {
		current = current->next;
		i++;
	}
	return i;
}

linked_t * linked_add(linked_t * bottom, void * p) {
	if (!bottom) {
		linked_t * new = malloc(sizeof(linked_t) + 8);
		new->p = p;
		return new;
	}
	linked_t * new = malloc(sizeof(linked_t) + 8);
	linked_t * leaf;
	if (!new) {
		return 0;
	}
	leaf = linked_leaf(bottom);
	new->p = p;
	new->next = 0;
	new->back = leaf;
	leaf->next = new;
	return bottom;
}

linked_t * linked_get(linked_t * bottom, int index) {
	if (!bottom) {
		return 0;
	}
	linked_t * current = bottom;
	int i = 0;
	while (current) {
		if (i == index) {
			return current;
		}
		current = current->next;
		i++;
	}
	return 0;
}

linked_t * linked_remove(linked_t * bottom, int index) {
	return linked_delete(linked_get(bottom, index));
}

linked_t * linked_delete(linked_t * node) {
	linked_t * branch = linked_branch(node);
	if (node->back) {
		node->back->next = node->next;
	}
	if (node->next) {
		node->next->back = node->back;
	}
	free(node);
	return branch == node ? 0 : branch;
}

void linked_iterate(linked_t * bottom, linked_callback_t callback) {
	if (!bottom) {
		return;
	}
	linked_t * node = bottom;
	while (node) {
		callback(node);
		node = node->next;
	}
}