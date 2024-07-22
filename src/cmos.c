#include <cmos.h>
#include <ports.h>
#include <stdint.h>

uint8_t cmos_read_register(uint16_t reg) {
	outb(0x70, reg);
	return inb(0x71);
}

void cmos_write_register(uint16_t reg, uint8_t value) {
	outb(0x70, reg);
	outb(0x71, value); // i think
}