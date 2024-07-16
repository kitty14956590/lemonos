#include <fpu.h>
#include <stdint.h>

// assume FPU exists, sorry NexGen!

void fpu_init() {
	size_t t;
	asm volatile (
		"clts\n"
		"mov %%cr0, %0\n"

		: "=r"(t)
	);
	t &= ~(1 << 2);
	t |= 0b10;
	asm volatile (
		"mov %1, %%cr0\n"
		"mov %%cr4, %0\n"

		: "=r"(t)
		: "r"(t)
	);
	t |= 3 << 9;
	asm volatile (
		"mov %0, %%cr4\n"
		"fninit\n"

		 :: "r"(t)
	);
}
