#pragma once

#include <stdint.h>

typedef struct memory_block {
	uint32_t state;
	uint32_t size;
} memory_block_t;

// states a block can be in
enum MEMORY_STATES {
	MEMORY_NON_EXISTENT,
	MEMORY_FREE,
	MEMORY_IN_USE,
};

extern void * _kernel_end;
extern void * heap;
extern void * heap_end;

void mmap_parse();
void * phy_malloc(size_t size);
