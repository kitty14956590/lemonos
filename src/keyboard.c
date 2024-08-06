#include <irq.h>
#include <stdint.h>
#include <ports.h>
#include <stdio.h>
#include <input.h>
#include <memory.h>
#include <input.h>
#include <graphics.h>
#include <mouse.h>

int keyboard_mouse = 0;
int next_byte = 0;
static keyboard_held_t held;

void keyboard_callback(registers_t * regs) {
	irq_ack(regs->int_no);
	while (inb(0x64) & 2) {}
	uint32_t keycode = inb(0x60);
	int pressed = 1;
	kbd_event_t * event;

	if (keycode == 224) {
		next_byte = 1; // this is multi byte sequence
		return;
	}

	if (keycode > 128) {
		keycode -= 128;
		pressed = 0;
	}

	if (next_byte) {
		next_byte = 0;
		switch (keycode) {
			default:
				if (gfx_init_done) {
					iprintf(4, u"Unknown key 0x%x\n", keycode);
				}
				return;
			case 0x38:
				keycode = 0x54; // ralt replacement
				break;
			case 0x1d:
				keycode = 0x55; // rctrl replacement
				break;
			case 0x2a:
				keycode = 0x56; // sysrq replacement
				break;
			case 0x5b:
			case 0x5c:
				keycode = 0x57; // super replacement
				break;
			case 0x48:
				keycode = 0x5a; // arrow up replacement
				break;
			case 0x4d:
				keycode = 0x5b; // arrow right replacement
				break;
			case 0x4b:
				keycode = 0x5c; // arrow left replacement
				break;
			case 0x50:
				keycode = 0x5d; // arrow down replacement
				break;
			case 0x37:
				return; // duplicate
		}
	}

	switch (keycode) {
		case 0x2a:
			held.lshift = pressed;
			break;
		case 0x36:
			held.rshift = pressed;
			break;
		case 0x38:
			held.lalt = pressed;
			break;
		case 0x54:
			held.ralt = pressed;
			held.meta = pressed;
			break;
		case 0x1d:
			held.lctrl = pressed;
			break;
		case 0x55:
			held.rctrl = pressed;
			break;
		case 0x56:
			held.sysrq = pressed;
			break;
		case 0x57:
			held.super = pressed;
			break;
	}

	if (pressed) {
		if (keycode == 0x3A) {
			held.caps = !held.caps;
		}
	}

	event = malloc(sizeof(kbd_event_t) + 8);
	event->type = EVENT_KEYBOARD;
	event->keycode = keycode;
	event->pressed = pressed;
	event->held = &held;
	broadcast_event((event_t *) event);
	free(event);
}

void keyboard_wait() {
	while (inb(0x64) & 2) {}
}

void keyboard_init() {
	irq_set_handler(33, &keyboard_callback);
	outb(0x64, 0xAD);
	keyboard_wait(); // slow peice of shit
	outb(0x64, 0xA7);
	keyboard_wait();
	outb(0x64, 0xAE);
	keyboard_wait();
}