#include <asm.h>
#include <stdint.h>
#include <irq.h>
#include <ports.h>
#include <util.h>
#include <graphics.h>
#include <stdio.h>
#include <multitasking.h>

volatile uint64_t ticks = 0;
uint64_t * ticksp;
int fps = 0;
int fps_tmp = 0;

void pit_callback(registers_t regs) {
	ticks++;
	if (ticks % 1000 == 0) {
		fps = fps_tmp;
		fps_tmp = 0;
	}
	if (ticks % 16 == 0) {
		fps_tmp++;
		draw_frame();
	}
	outb(0x20, 0x20);
	switch_task();
	disable_interrupts();
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