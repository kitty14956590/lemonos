#include <ports.h>
#include <serial.h>
#include <string.h>

int com1_works;
int com2_works;

int serial_init_port(uint32_t port) {
	outb(port + 1, 0x00);
	outb(port + 3, 0x80);
	outb(port + 0, 0x03);
	outb(port + 1, 0x00);
	outb(port + 3, 0x03);
	outb(port + 2, 0xC7);
	outb(port + 4, 0x0B);
	outb(port + 4, 0x1E);
	outb(port + 0, 0xAE);
	if (inb(port + 0) != 0xAE) {
		return 0;
	}
	outb(port + 4, 0x0F);
	return 1;
}

int serial_ready(int port) {
	return inb(port + 5) & 1;
}

unsigned char serial_inb(int port) {
	while (serial_ready(port) == 0);
	return inb(port);
}

int serial_busy(int port) {
	return inb(port + 5) & 0x20;
}

void serial_outb(int port, unsigned char data) {
	while (serial_busy(port) == 0) {}
	outb(port, data);
}
void serial_outw(int port, uint16_t data) {
	while (serial_busy(port) == 0) {}
	outb(port, data & 0xff);
}

void serial_outs(int port, uint16_t * data) {
	for (size_t i = 0; i < ustrlen(data); i++) {
		serial_outw(port, data[i]);
	}
}

void serial_init() {
	com1_works = 0;
	com2_works = 0;
	if (serial_init_port(COM1_PORT)) {
		com1_works = 1;
	}
	if (serial_init_port(COM2_PORT)) {
		com2_works = 1;
	}
}