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
#include <input.h>
#include <multitasking.h>
#include <serial.h>
#include <ide.h>
#include <fdc.h>
#include <cpuspeed.h>
#include <layout.h>
#include <sysrq.h>

int main(uint32_t eax, uint32_t ebx) {
	assert(parse_multiboot(eax, ebx), MULTIBOOT_ERROR, 0);
	memory_init();
	serial_init();
	irq_init();
	pit_init(1000);
	layout_init();
	keyboard_init();
	ide_init();
	gfx_init();
	mouse_init();
	enable_interrupts();
	cprintf(7, u"Kernel loaded \ue027\ue028 \ue003\ue004\n");
	cprintf(7, u"\n");
	cprintf(7, u"You are using LemonOS v%d.%d.%d.%d (%s)\n", ver_edition, ver_major, ver_minor, ver_patch, os_name16);
	multitasking_init();
	mouse_late_init();
	gfx_late_init();
	sysrq_init();
	pci_probe();
	acpi_init();
	acpi_shutdown();
	sleep();
}
