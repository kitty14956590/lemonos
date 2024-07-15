#include <main.h>
#include <memory.h>
#include <multiboot.h>
#include <assert.h>
#include <graphics.h>
#include <stdio.h>
#include <version.h>

int main(uint32_t eax, uint32_t ebx) {
	assert(parse_multiboot(eax, ebx));
	gfx_init();
	mmap_parse();
	cprintf(7, u"Le noyeau est charge \1!\n");
	cprintf(7, u"\n");
	cprintf(7, u"Vous utilisez CitronOS v%d.%d.%d.%d (%s)\n", ver_edition, ver_major, ver_minor, ver_patch, os_name16);
	cprintf(7, u"\n\n");
	cprintf(7, u"Test de memoire:\n");
	cprintf(7, u" - Pile: 0x%x\n", heap);

	int count = 16;
        void * alloc;
        memory_block_t * block;
	while (count--) {
		alloc = phy_malloc(count);
		if (!alloc) {
			continue;
		}
		block = (memory_block_t *) (alloc - sizeof(memory_block_t));

		cprintf(7, u" - (memoire %u) 0x%x - 0x%x\n",
			block->size,
			block,
			alloc + block->size
		);

		phy_free(alloc);

	}
}
