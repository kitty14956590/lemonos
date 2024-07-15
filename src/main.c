#include <main.h>
#include <multiboot.h>
#include <assert.h>
#include <graphics.h>

int main(uint32_t eax, uint32_t ebx) {
	assert(parse_multiboot(eax, ebx));
	gfx_init();
	txt_string_draw(u"testing", 0, 0, 7, &root_window);
}
