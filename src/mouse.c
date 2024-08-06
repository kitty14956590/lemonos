#include <math.h>
#include <graphics.h>
#include <ports.h>
#include <asm.h>
#include <irq.h>
#include <panic.h>
#include <input.h>
#include <multitasking.h>
#include <keyboard.h>
#include <memory.h>

int8_t mouse_bytes[3];
uint8_t mouse_cycle = 0;
int32_t mouse_y = 0;
int32_t mouse_x = 0;
static mouse_held_t held;
int left_click = 0;
int right_click = 0;
int middle_click = 0;

void clip_mouse() {
	if (mouse_x < -1) {
			mouse_x = -1;
	} else if (mouse_x > root_window.size.width - 2) {
			mouse_x = root_window.size.width - 2;
	}
	if (mouse_y < 0) {
			mouse_y = 0;
	} else if (mouse_y > root_window.size.height - 2) {
			mouse_y = root_window.size.height - 2;
	}
}

void mouse_handler(registers_t * regs) {
	irq_ack(regs->int_no);
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
				mouse_event_t * event;
				int32_t old_mouse_y = mouse_y;
				int32_t old_mouse_x = mouse_x;
				mouse_bytes[2] = mouse_in;
				// gcc some how makes shit code that takes forever if you dont add != 0
				if (mouse_bytes[0] & 0x80 || mouse_bytes[0] & 0x40) {
					break;
				}
				mouse_x += mouse_bytes[1];
				mouse_y -= mouse_bytes[2];
				clip_mouse();
				held.left = (mouse_bytes[0] & 0x01);
				held.right = (mouse_bytes[0] & 0x02) >> 1;
				held.middle = (mouse_bytes[0] & 0x04) >> 2;
				cursor.x = mouse_x;
				cursor.y = mouse_y;
				event = malloc(sizeof(mouse_event_t));
				event->type = EVENT_MOUSE;
				event->x = mouse_x;
				event->y = mouse_y;
				event->delta_x = mouse_bytes[1];
				event->delta_y = mouse_bytes[2];
				event->bdelta_x = mouse_x - old_mouse_x;
				event->bdelta_y = -(mouse_y - old_mouse_y);
				event->held = &held;
				broadcast_event((event_t *) event);
				free(event);
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

void mouse_sample_rate(int rate) {
	outb(0x64, 0xd4);
	outb(0x60, 0xf3);
	mouse_sleep(0);
	inb(0x60);
	outb(0x64, 0xd4);
	outb(0x60, rate);
	mouse_sleep(0);
	inb(0x60);
}

int mouse_cmd(int cmd, int data, int hasdata) {
	outb(0x64, 0xd4);
	outb(0x60, cmd);
	mouse_sleep(0);
	inb(0x60);
}

void keyboard_mouse_handle_event(event_t * event) {
	if (event->type != EVENT_KEYBOARD) {
		return;
	}
	kbd_event_t * keyevent = (kbd_event_t *) event;
	keyboard_held_t * keyheld = keyevent->held;
	mouse_event_t * mouseevent;
	int32_t old_mouse_y = 0;
	int32_t old_mouse_x = 0;
	static int left_held = 0;
	static int right_held = 0;
	static int up_held = 0;
	static int down_held = 0;

	if ((keyevent->keycode >= 0x5a && keyevent->keycode <= 0x5d) && keyboard_mouse) {
		int amount = (keyheld->lshift || keyheld->rshift) ? 4 : ((keyheld->lctrl) ? 16 : 8);
		switch (keyevent->keycode) {
			case 0x5a:
				up_held = keyevent->pressed;
				break;
			case 0x5b:
				right_held = keyevent->pressed;
				break;
			case 0x5c:
				left_held = keyevent->pressed;
				break;
			case 0x5d:
				down_held = keyevent->pressed;
				break;
		}
		held.left = keyheld->rctrl;
		// there HAS to be a better way to do this????
		mouseevent = malloc(sizeof(mouse_event_t));
		mouseevent->type = EVENT_MOUSE;
		mouseevent->held = &held;
		old_mouse_y = mouse_y;
		old_mouse_x = mouse_x;
		if (right_held) {
			mouseevent->delta_x += amount;
			mouse_x += amount;
		}
		if (left_held) {
			mouseevent->delta_x -= amount;
			mouse_x -= amount;
		}
		if (up_held) {
			mouseevent->delta_y -= amount;
			mouse_y -= amount;
		}
		if (down_held) {
			mouseevent->delta_y += amount;
			mouse_y += amount;
		}
		clip_mouse();
		cursor.x = mouse_x;
		cursor.y = mouse_y;
		mouseevent->x = mouse_x;
		mouseevent->y = mouse_y;
		mouseevent->bdelta_x = mouse_x - old_mouse_x;
		mouseevent->bdelta_y = -(mouse_y - old_mouse_y);
		broadcast_event((event_t *) mouseevent);
		free(mouseevent);
		return;
	}
}

void enable_z_axis() {
	mouse_sample_rate(200);
	mouse_sample_rate(100);
	mouse_sample_rate(80);
}

void mouse_init() {
	uint8_t status;
	mouse_x = cursor.x;
	mouse_y = cursor.y;
	irq_set_handler(44, &mouse_handler);
	mouse_sleep(1);
	outb(0x64, 0xa8);
	mouse_sleep(1);
	outb(0x64, 0x20);
	mouse_sleep(0);
	status = inb(0x60) | 2;
	mouse_sleep(1);
	outb(0x64, 0x60);
	mouse_sleep(1);
	outb(0x60, status);
	mouse_write(0xf6);
	mouse_read();
	mouse_write(0xf4);
	mouse_read();
}

void mouse_late_init() {
	create_event_zombie(u"moused", keyboard_mouse_handle_event);
}