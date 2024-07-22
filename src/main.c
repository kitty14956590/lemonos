#include <main.h>
#include <memory.h>
#include <multiboot.h>
#include <assert.h>
#include <graphics.h>
#include <panic.h>
#include <stdio.h>
#include <version.h>
#include <fpu.h>
#include <acpi.h>
#include <util.h>
#include <apic.h>
#include <irq.h>
#include <pit.h>
#include <mouse.h>
#include <keyboard.h>
#include <pci.h>
#include <multitasking.h>
#include <serial.h>
#include <ide.h>
#include <fdc.h>

int main(uint32_t eax, uint32_t ebx) {
	assert(parse_multiboot(eax, ebx), MULTIBOOT_ERROR);
	serial_init();
	irq_init();
	pit_init(1000);
	keyboard_init();
	memory_init();
	ide_init();
	gfx_init();
	mouse_init();
	cprintf(7, u"Kernel loaded \ue027\ue028 \ue003\ue004\n");
	cprintf(7, u"\n");
	cprintf(7, u"You are using LemonOS v%d.%d.%d.%d (%s)\n", ver_edition, ver_major, ver_minor, ver_patch, os_name16);
	enable_interrupts();
	multitasking_init();
	acpi_init();
	pci_probe();
	apic_init();
	while (ticks < 1120) {}
	cprintf(7, u"fps %d\n", fps);
	sleep();
}
