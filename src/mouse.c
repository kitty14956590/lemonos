#include <math.h>
#include <graphics.h>
#include <ports.h>
#include <asm.h>
#include <irq.h>
#include <panic.h>

int8_t mouse_bytes[3];
uint8_t mouse_cycle = 0;
int32_t mouse_y = 0;
int32_t mouse_x = 0;
int left_click = 0;
int right_click = 0;

void mouse_handler(registers_t regs) {
	uint8_t status = inb(0x64);
	int8_t mouse_in = inb(0x60);
	if (status & 0x20) {
		switch (mouse_cycle) {
			case 0:
				mouse_bytes[0] = mouse_in;
				if (!(mouse_in & 0x08)) {
					return;
				}
				++mouse_cycle;
				break;
			case 1:
				mouse_bytes[1] = mouse_in;
				++mouse_cycle;
				break;
			case 2:
				mouse_bytes[2] = mouse_in;
				if (mouse_bytes[0] & 0x80 || mouse_bytes[0] & 0x40) {
					break;
				}
				mouse_x += mouse_bytes[1];
				mouse_y -= mouse_bytes[2];
				if (mouse_x < 0) {
						mouse_x = 0;
				} else if (mouse_x > root_window.size.width - 2) {
						mouse_x = root_window.size.width - 2;
				}
				if (mouse_y < 0) {
						mouse_y = 0;
				} else if (mouse_y > root_window.size.height - 2) {
						mouse_y = root_window.size.height - 2;
				}
				left_click = (mouse_bytes[0] & 0x01);
				right_click = (mouse_bytes[0] & 0x02) >> 1;
				if (right_click && mouse_x == 0) {
					panic();
				}
				cursor.x = mouse_x;
				cursor.y = mouse_y;
				mouse_cycle = 0;
				break;
		}
	}
}


void mouse_sleep(uint8_t a_type) {
	uint32_t timeout = 100000;
	if (!a_type) {
		while (--timeout) {
			if ((inb(0x64) & 0x01) == 1) {
					return;
			}
		}
		return;
	} else {
		while (--timeout) {
				if (!((inb(0x64) & 0x02))) {
						return;
				}
		}
		return;
	}
}

void mouse_write(uint8_t write) {
	mouse_sleep(1);
	outb(0x64, 0xD4);
	mouse_sleep(1);
	outb(0x60, write);
}
uint8_t mouse_read() {
	mouse_sleep(0);
	uint32_t t = inb(0x60);
	return t;
}

void mouse_init() {
	uint8_t status;
	mouse_x = cursor.x;
	mouse_y = cursor.y;
	irq_set_handler(44, &mouse_handler);
	mouse_sleep(1);
	outb(0x64, 0xA8);
	mouse_sleep(1);
	outb(0x64, 0x20);
	mouse_sleep(0);
	status = inb(0x60) | 2;
	mouse_sleep(1);
	outb(0x64, 0x60);
	mouse_sleep(1);
	outb(0x60, status);
	mouse_write(0xF6);
	mouse_read();
	mouse_write(0xF4);
	mouse_read();
}