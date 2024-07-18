#include <memory.h>
#include <math.h>
#include <multiboot.h>
#include <string.h>

void * heap;
void * heap_end;

size_t heap_length;

void mmap_parse() {
	uint64_t size = 0;
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

// physical malloc
void * malloc(size_t size) {
	size_t required_size = round32(size, 8); // round to nearest 8
	memory_block_t * current_block = (memory_block_t *) heap;
	void * current = heap;
	if ((size == 0) || ((heap + required_size + sizeof(memory_block_t)) > heap_end)) {
		return (void *) 0;
	}
	while ((current_block->state == MEMORY_FREE) || (current_block->state == MEMORY_IN_USE)) {
		if ((current_block->state == MEMORY_FREE) && (current_block->size >= required_size)) {
			current_block->state = MEMORY_IN_USE;
			return current + sizeof(memory_block_t); // reuse free block if its larger or equal to the size we want
		}
		current += current_block->size + sizeof(memory_block_t);
		current_block = (memory_block_t *) current;
		if (current > heap_end) {
			return (void *) 0;
		}
	}
	// create new block
	current_block->state = MEMORY_IN_USE;
	current_block->size = required_size;
	return ((void *) current_block) + sizeof(memory_block_t);
}

int free(void * data) {
	memory_block_t * block;
	if (data < heap || data > heap_end) {
		return 0;
	}
	block = (memory_block_t *) (data - sizeof(memory_block_t));
	if (block->state == MEMORY_IN_USE) {
		block->state = MEMORY_FREE;
		return 1;
	}
	return 0;
}

void memory_init() {
	void * p;
	mmap_parse();
	p = malloc(16);
	memcpy(p, "__phymem__", 16); // buffer overflow, but who care
}
