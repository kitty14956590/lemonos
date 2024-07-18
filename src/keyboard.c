// keyboard not used yet, just for debugginfg

#include <irq.h>
#include <stdint.h>
#include <ports.h>

uint32_t keyboard_callback(registers_t regs) {
	while (inb(0x64) & 2) {}
	uint32_t keycode = inb(0x60);
	return regs.eax;
}

void keyboard_init() {
	irq_set_handler(33, &keyboard_callback);
	outb(0x64, 0xAD);
	outb(0x64, 0xA7);
	outb(0x64, 0xAE);
}