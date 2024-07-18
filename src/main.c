#include <main.h>
#include <memory.h>
#include <multiboot.h>
#include <assert.h>
#include <graphics.h>
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

int main(uint32_t eax, uint32_t ebx) {
	assert(parse_multiboot(eax, ebx));
	irq_init();
	pit_init(60);
	keyboard_init();
	mouse_init();
	memory_init();
	enable_interrupts();
	fpu_init();
	gfx_init();
	cprintf(7, u"Kernel loaded \ue027\ue028 \ue003\ue004\n");
	cprintf(7, u"\n");
	cprintf(7, u"You are using LemonOS v%d.%d.%d.%d (%s)\n", ver_edition, ver_major, ver_minor, ver_patch, os_name16);
	acpi_init();
	pci_probe();
	apic_init();
	sleep();
}
