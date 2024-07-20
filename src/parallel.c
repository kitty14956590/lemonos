#include <parallel.h>
#include <ports.h>
#include <string.h>
#include <stdint.h>

void parallel_outb(unsigned char data) {
	unsigned char control;
	while (!inb(0x379) & 0x80) {}
	outb(0x378, data);
	control = inb(0x37A);
	outb(0x37A, control | 1);
	outb(0x37A, control);
	while (!inb(0x379) & 0x80) {}
}

void parallel_outw(uint16_t data) {
	parallel_outb(data & 0xff);
	parallel_outb((data >> 8) & 0xff);
}

void parallel_outs(uint16_t * data) {
	for (size_t i = 0; i < ustrlen(data); i++) {
		parallel_outb(data[i] & 0xff);
	}
}