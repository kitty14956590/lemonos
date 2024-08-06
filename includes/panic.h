#pragma once

#include <stdint.h>
#include <asm.h>

enum {
	MANUAL_PANIC,
	OUT_OF_MEMORY,
	MEMORY_CORRUPTION,
	GENERAL_MEMORY,
	GENERAL_PROTECTION,
	GENERAL_PAGE,
	GENERAL_ARITHMATIC,
	UNKNOWN_OPCODE,
	DEVICE_UNAVAILABLE,
	UNKNOWN_FATAL_ERROR,
	SHUTDOWN_FAILURE,
	MULTIBOOT_ERROR,
	ACPI_FATAL_ERROR,
	SEGMENTATION_FAULT,
};

void panic(int error, void * p);
void panic_irq(registers_t regs);
void handle_error(int error, void * p);
uint16_t * error_name(int error);