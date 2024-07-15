#include <graphics.h>
#include <multiboot.h>

rect_2d_t root_window;

void gfx_init() {
	root_window.fb = (uint32_t *) (uint32_t) multiboot_header->framebuffer;
	root_window.cursor.x = 0;
	root_window.cursor.y = 0;
	root_window.x = 0;
	root_window.y = 0;
	root_window.size.width = multiboot_header->framebuffer_width;
	root_window.size.height = multiboot_header->framebuffer_height;
}
