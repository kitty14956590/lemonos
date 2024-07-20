#include <acpi.h>
#include <ports.h>
#include <util.h>

void reboot() {
	if (acpi_working && acpi_reboot()) {
		return;
	}
	uint8_t wait = 0x02;
	while (wait & 0x02) {
		wait = inb(0x64);
	}
	outb(0x64, 0xFE);
	halt();
}

void shutdown() {
	if (acpi_working && acpi_shutdown()) {
		return;
	}
	halt();
}

void disable_interrupts() {
	asm ("cli");
}
void enable_interrupts() {
	asm ("sti");
}

void __attribute__((optimize("O0"))) halt() {
	for (;;) {
		asm volatile ("cli\nhlt\n");
	}
}

void __attribute__((optimize("O0"))) sleep() {
	for (;;) {
		asm volatile ("hlt\n");
	}
}
