#include <memory.h>
#include <math.h>
#include <multiboot.h>
#include <string.h>
#include <panic.h>

void * heap;
void * heap_end;

size_t heap_length;

void mmap_parse() {
	size_t i = 0;
	heap_length = 0;
	heap = (void *) 0;
	heap_end = (void *) 0;
	for (i = 0; i < multiboot_header->mmap_count; i += sizeof(memory_map_t)) {
		memory_map_t * mmap = (memory_map_t *) (multiboot_header->mmap_address + i);
		if (mmap->type == MMAP_BAD || mmap->type == MMAP_NON_VOLATILE) {
			continue;
		}
		if (mmap->length > heap_length) {
			heap = (void *) (uint32_t) mmap->address;
			heap_length = mmap->length;

			// cut it off if its longer than 0xffffffff
			if (heap_length > (0xffffffff - mmap->address)) {
				heap_length = (0xffffffff - mmap->address);
			}
		}
	}
	heap_end = heap + heap_length;
	if (((uint32_t) heap) == 0x100000) {
		void * kernel_end = (void *) round32((uint32_t) &_kernel_end, 64);
		heap = kernel_end + 0x100000;
		heap_length = (uint32_t) (heap_end - heap);
	}
}

static int destroy(memory_block_t * block) {
	void * p = block;
	memory_block_t * next = p + block->size + sizeof(memory_block_t);
	if (next->state != MEMORY_NON_EXISTENT) {
		return 0; // sowwy
	}
	block->state = MEMORY_NON_EXISTENT;
	block->size = 0;
	return 1;
}

// split to desired size, free created block
// - block HAS to be exactly or bigger than the desired size or underallocation will occur
static int split(memory_block_t * block, size_t size) {
	uint32_t old;
	void * p = block;
	memory_block_t * new_block;
	if (block->size < (size + 8 + sizeof(memory_block_t))) {
		return 1;
	}
	old = block->size;
	block->size = size;
	new_block = p + size + sizeof(memory_block_t);
	new_block->state = MEMORY_FREE;
	new_block->size = old - size - sizeof(memory_block_t);
	destroy(new_block);
	return 1;
}

// physical malloc
void * malloc(size_t size) {
	size_t required_size = round32(size, 8); // round to nearest 8
	memory_block_t * current_block = (memory_block_t *) heap;
	void * current = heap;
	uint64_t * next_block;
	size_t consecutive_free = 0;
	void * consecutive_start = 0;
	if ((size == 0) || ((heap + required_size + sizeof(memory_block_t)) > heap_end)) {
		return (void *) 0;
	}
	while ((current_block->state == MEMORY_FREE) || (current_block->state == MEMORY_IN_USE)) {
		if (current_block->state == MEMORY_FREE) {
			if (current_block->size >= required_size) {
				split(current_block, required_size);
				current_block->state = MEMORY_IN_USE;
				return current + sizeof(memory_block_t);
			}
			memory_block_t * next = current + current_block->size + sizeof(memory_block_t);
			if (next->state == MEMORY_NON_EXISTENT) {
				break;
			}
			consecutive_free += current_block->size + sizeof(memory_block_t);
			if (!consecutive_start) {
				consecutive_start = current;
			}
			if ((consecutive_free - sizeof(memory_block_t)) >= required_size) {
				consecutive_free -= sizeof(memory_block_t);
				current_block = consecutive_start;
				current_block->size = consecutive_free;
				split(current_block, required_size);
				current_block->state = MEMORY_IN_USE;
				return consecutive_start + sizeof(memory_block_t);
			}
		} else {
			consecutive_free = 0;
			consecutive_start = 0;
		}
		current += current_block->size + sizeof(memory_block_t);
		current_block = (memory_block_t *) current;
		if (current > heap_end || current_block->state > 3) {
			handle_error(MEMORY_CORRUPTION, 0);
			return (void *) 0;
		}
	}
	// prevent us from getting confused by left over memory (see memory_init())
	next_block = ((void *) current_block) + required_size + sizeof(memory_block_t);
	*next_block++ = 0;
	*next_block++ = 0;
	*next_block++ = 0;
	*next_block++ = 0;
	// create new block
	current_block->state = MEMORY_IN_USE;
	current_block->size = required_size;
	return ((void *) current_block) + sizeof(memory_block_t);
}

int free(void * data) {
	memory_block_t * block;
	if ((data < heap) || (data > heap_end)) {
		return 0;
	}
	block = (memory_block_t *) (data - sizeof(memory_block_t));
	if (block->state == MEMORY_IN_USE) {
		block->state = MEMORY_FREE;
		destroy(block);
		return 1;
	}
	return 0;
}

void * realloc(void * p, size_t size) {
	memory_block_t * block;
	if (!size) {
		free(p);
		return 0;
	}
	block = (memory_block_t *) (p - sizeof(memory_block_t));
	if (block->size >= size) {
		return p;
	}
	free(p);
	return malloc(size);
}

void * calloc(size_t number, size_t size) {
	void * p = malloc(number * size);
	if (!p) {
		return p;
	}
	memset(p, 0, number * size);
	return p;
}

void memory_init() {
	void * p;
	uint64_t * p2;
	mmap_parse();
	// memory doesnt get wipped on reboot, this will prevent us from getting confused by stuff left from last boot
	// (see malloc() for next half of the solution)
	p2 = heap;
	*p2 = 0;
	p = malloc(16);
	memcpy(p, "__phymem__", 16); // buffer overflow, but who care
}