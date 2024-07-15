#include <main.h>
#include <multiboot.h>
#include <assert.h>

int main(uint32_t eax, uint32_t ebx) {
	assert(parse_multiboot(eax, ebx));
	gfx_init();
}
