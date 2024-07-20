#include <util.h>
#include <graphics.h>
#include <asm.h>
#include <panic.h>
#include <string.h>
#include <stdint.h>

void draw_panic_screen(uint32_t colour) {
	int start = 255;
	int repeats = 1 + (root_window.size.height / 255);
	for (int i = 0; i < root_window.size.height; i++) {
		if (i < (255 * repeats)) {
				if (i % repeats == 0) {
					colour = ((start + (i / repeats) * -1) & 0xFF) << 16;
				}
				memset32(root_window.fb + (i * root_window.size.width), colour, root_window.size.width);
		} else {
			memset32(root_window.fb + (i * root_window.size.width), 0, root_window.size.width);
		}
	}
}

void panic(int error) {
	disable_interrupts();
	draw_panic_screen(0xffff0000);
	halt();
}

void handle_error(int error) {
	switch (error) {
		case MULTIBOOT_ERROR:
			// cant do anything about this
			halt();
			break;

		case SHUTDOWN_FAILURE:
		case UNKNOWN_FATAL_ERROR:
		case DEVICE_UNAVAILABLE:
		case MANUAL_PANIC: // (wanted us to to do this)
		case MEMORY_CORRUPTION:
			// or these either
			panic(error);
			break;
	}
	draw_panic_screen(0xffff0000);
}

void panic_irq(registers_t regs) {
	panic(UNKNOWN_FATAL_ERROR);
}