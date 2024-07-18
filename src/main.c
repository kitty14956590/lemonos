#include <main.h>
#include <memory.h>
#include <multiboot.h>
#include <assert.h>
#include <graphics.h>
#include <stdio.h>
#include <version.h>
#include <fpu.h>
#include <acpi.h>
#include <util.h>
#include <apic.h>

int main(uint32_t eax, uint32_t ebx) {
	assert(parse_multiboot(eax, ebx));
	gfx_init();
	memory_init();
	fpu_init();
	acpi_init();
	apic_init();
	cprintf(7, u"Kernel loaded \ue027\ue028\n");
	cprintf(7, u"\n");
	cprintf(7, u"You are using LemonOS v%d.%d.%d.%d (%s)\n", ver_edition, ver_major, ver_minor, ver_patch, os_name16);
	cprintf(7, u"\n\n");
	cprintf(7, u"Memory Test:\n");
	cprintf(7, u" - Heap: 0x%x\n", heap);

	int count = 16;
	void * alloc;
	memory_block_t * block;
	while (count--) {
		alloc = malloc(count);
		if (!alloc) {
			continue;
		}
		block = (memory_block_t *) (alloc - sizeof(memory_block_t));

		cprintf(7, u" - (memory %u) 0x%x - 0x%x\n",
			block->size,
			block,
			alloc + block->size
		);

		free(alloc);

	}
}
