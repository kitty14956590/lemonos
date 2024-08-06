#include <util.h>
#include <irq.h>
#include <graphics.h>
#include <asm.h>
#include <panic.h>
#include <string.h>
#include <stdint.h>
#include <version.h>
#include <input.h>
#include <multitasking.h>
#include <stdio.h>

void draw_panic_screen(uint32_t colour) {
	int repeats = 1 + (root_window.size.height / 255);
	for (int i = 0; i < root_window.size.height; i++) {
		if (i < (255 * repeats)) {
			if (i % repeats == 0) {
				colour = rgb_degrade(colour, 0);
			}
			memset32(root_window.fb + (i * root_window.size.width), colour, root_window.size.width);
		} else {
			memset32(root_window.fb + (i * root_window.size.width), 0, root_window.size.width);
		}
	}
}

void panic(int error, void * p) {
	disable_interrupts();
	size_2d_t size = root_window.size;
	size_2d_t vga;
	registers_t * regs = p;
	int line = 2;
	draw_panic_screen(0xffff0000);
	gfx_init_done = 0;
	vga.width = size.width / 8;
	vga.height = size.height / 16;
	background.fb = root_window.fb;
	background.size = size;
	gfx_string_draw(u"KERNEL PANIC", (vga.width / 2) - 6, 2, 0, 0xffffffff, &root_window);
	line += 3;
	gfx_string_draw(u"ERROR: ", 4, line, 0, 0xffffffff, &root_window);
	gfx_string_draw(error_name(error), 11, line, 0, 0xffffffff, &root_window);
	line += 3;
	if (p == 0) {
		gfx_string_draw(u"REGISTERS UNKNOWN", 4, line, 0, 0xffffffff, &root_window);
	} else {
		background.cursor.x = 4;
		background.cursor.y = line;
		iprintf(0xffffffff, u"EAX=%r EBX=%r ECX=%r EDX=%r", regs->eax, regs->ebx, regs->ecx, regs->edx);
		background.cursor.x = 4;
		background.cursor.y = ++line;
		iprintf(0xffffffff, u"ESI=%r EDI=%r EBP=%r ESP=%r", regs->esi, regs->edi, regs->ebp, regs->esp);
		background.cursor.x = 4;
		background.cursor.y = ++line;
		iprintf(0xffffffff, u"FLG=%r EIP=%r ERR=%r INT=%r", regs->eflags, regs->eip, regs->err_code, regs->int_no);
	}
	line += 3;
	if (multitasking_done) {
		gfx_string_draw(u"Current Process:", 4, line, 0, 0xffffffff, &root_window);
		background.cursor.x = 6;
		background.cursor.y = ++line;
		iprintf(0xffffffff, u"%d        ", current_process->pid);
		iprintf(0xffffffff, u"%s        ", current_process->name);
		iprintf(0xffffffff, u"%s", current_process->system ? u"(System Process)" : u"");
	} else {
		background.cursor.x = 4;
		background.cursor.y = line;
		iprintf(0xffffffff, u"NO MULTITASKER");
	}

	background.cursor.x = 0;
	background.cursor.y = vga.height - 1;
	iprintf(0xffffffff, u"LemonOS v%d.%d.%d.%d (%s)", ver_edition, ver_major, ver_minor, ver_patch, os_name16);
	halt();
}

uint16_t * error_name(int error) {
	switch (error) {
		default:
			return u"UNKNOWN_ERROR";
		case MANUAL_PANIC:
			return u"MANUAL_PANIC";
		case OUT_OF_MEMORY:
			return u"OUT_OF_MEMORY";
		case MEMORY_CORRUPTION:
			return u"MEMORY_CORRUPTION";
		case GENERAL_MEMORY:
			return u"GENERAL_MEMORY";
		case GENERAL_PROTECTION:
			return u"GENERAL_PROTECTION";
		case GENERAL_PAGE:
			return u"GENERAL_PAGE";
		case GENERAL_ARITHMATIC:
			return u"GENERAL_ARITHMATIC";
		case UNKNOWN_OPCODE:
			return u"UNKNOWN_OPCODE";
		case DEVICE_UNAVAILABLE:
			return u"DEVICE_UNAVAILABLE";
		case UNKNOWN_FATAL_ERROR:
			return u"UNKNOWN_FATAL_ERROR";
		case SHUTDOWN_FAILURE:
			return u"SHUTDOWN_FAILURE";
		case MULTIBOOT_ERROR:
			return u"MULTIBOOT_ERROR";
		case ACPI_FATAL_ERROR:
			return u"ACPI_FATAL_ERROR";
	}
}

void handle_error(int error, void * p) {
	switch (error) {
		case MULTIBOOT_ERROR:
			// cant do anything about this
			halt();
			break;

		default:
			panic(error, p);
			break;
	}
	draw_panic_screen(0xffff00ff);
}