#include <asm.h>
#include <stdint.h>
#include <irq.h>
#include <ports.h>
#include <graphics.h>
#include <stdio.h>

uint64_t ticks = 0;

uint32_t pit_callback(registers_t regs) {
	ticks++;
	draw_frame();
	return regs.eax;
}

void pit_init(uint32_t freq) {
	irq_set_handler(32, &pit_callback);
	uint32_t divisor = 1193180 / freq;
	outb(0x43, 0x36);
	uint8_t low = (uint8_t) (divisor & 0xFF);
	uint8_t high = (uint8_t) ((divisor >> 8) & 0xFF);
	outb(0x40, low);
	outb(0x40, high);
}