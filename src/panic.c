#include <util.h>
#include <graphics.h>
#include <asm.h>
#include <string.h>
#include <stdint.h>

void panic() {
	memset32(root_window.fb, 0xffff0000, root_window.size.width * root_window.size.height);
	halt();
}

uint32_t panic_irq(registers_t regs) {
	panic();
}