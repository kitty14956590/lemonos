#include <asm.h>
#include <stdint.h>
#include <irq.h>
#include <ports.h>
#include <graphics.h>
#include <stdio.h>
#include <multitasking.h>

volatile uint64_t ticks = 0;
uint64_t * ticksp;

void pit_callback(registers_t regs) {
	if (ticks & 0b1000 != 0) {
		draw_frame();
	}
	ticks++;
	outb(0x20, 0x20);
	switch_task();
}

void pit_init(uint32_t freq) {
	ticksp = (uint64_t *) &ticks;
	irq_set_handler(32, &pit_callback);
	uint32_t divisor = (uint32_t) (1193180 / freq);
	outb(0x43, 0x36);
	uint8_t low = (uint8_t) (divisor & 0xFF);
	uint8_t high = (uint8_t) ((divisor >> 8) & 0xFF);
	outb(0x40, low);
	outb(0x40, high);
}