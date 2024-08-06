#include <asm.h>
#include <stdint.h>
#include <stdio.h>
#include <irq.h>
#include <ports.h>
#include <util.h>
#include <graphics.h>
#include <stdio.h>
#include <input.h>
#include <multitasking.h>

volatile uint64_t ticks = 0;
uint64_t * ticksp;
uint32_t pit_freq;
int pit_dump_regs = 0;

static int frame_divisor;

// todo: fix me !!
void pit_callback(registers_t * regs) {
	irq_ack(regs->int_no);
	if (pit_dump_regs) {
		pit_dump_regs = 0;
		iprintf(0xffffffff, u"EAX=%r EBX=%r ECX=%r EDX=%r\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
		iprintf(0xffffffff, u"ESI=%r EDI=%r EBP=%r ESP=%r\n", regs->esi, regs->edi, regs->ebp, regs->esp);
		iprintf(0xffffffff, u"FLG=%r EIP=%r ERR=%r INT=%r\n", regs->eflags, regs->eip, regs->err_code, regs->int_no);
	}
	ticks++;
	if (ticks % frame_divisor == 0) {
		draw_frame();
	}
	switch_task();
	disable_interrupts();
}

void pit_init(uint32_t freq) {
	uint32_t divisor = (uint32_t) (1193180 / freq);
	ticksp = (uint64_t *) &ticks;
	pit_freq = freq;
	frame_divisor = freq / 60;
	irq_set_handler(32, &pit_callback);
	outb(0x43, 0x36);
	outb(0x40, divisor & 0xff);
	outb(0x40, (divisor >> 8) & 0xff);
}