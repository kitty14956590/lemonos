#include <msr.h>
#include <stdint.h>

void cpu_read_msr(uint32_t ecx, uint32_t * eax, uint32_t * edx) {
	asm volatile (
		"rdmsr"

		: "=a"(*eax), "=d"(*edx)
		: "c"(ecx)
	);
}

void cpu_write_msr(uint32_t ecx, uint32_t eax, uint32_t edx) {
	asm volatile (
		"wrmsr"

		:: "a"(eax), "d"(edx), "c"(ecx)
	);
}