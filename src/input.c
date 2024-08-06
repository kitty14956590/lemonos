#include <input.h>
#include <multitasking.h>
#include <graphics.h>
#include <linked.h>
#include <layout.h>
#include <stdio.h>

uint16_t event_to_char(kbd_event_t * event, int force_layer) {
	int layer = event->held->meta + event->held->metalock;
	int shift = event->held->lshift || event->held->rshift;
	uint16_t (* layout)[][98];
	if (force_layer != -1) {
		layer = force_layer;
	}
	layout = layout_get_layer(layer);
	return (*layout)[shift][event->keycode];
}

uint16_t event_to_ascii_char(kbd_event_t * event) {
	int shift = event->held->lshift || event->held->rshift;
	return us_qwerty_kbd[shift][event->keycode];
}

int broadcast_event_callback(linked_t * node, void * p) {
	process_t * process = node->p;
	event_t * event = p;
	process->recv_global_event(event);
}

void broadcast_event(event_t * event) {
	linked_iterate(procs, broadcast_event_callback, event);
}

void send_event(event_t * event, process_t * process) {
	process->recv_event(event);
}

// debugging
void dump_event(event_t * event) {
	if (!gfx_init_done) {
		return;
	}
	printf(u"Event:\n");
	printf(u" - Type: %d\n", event->type);
	switch (event->type) {
		default:
			printf(u" - Unknown event type\n");
			return;
		case EVENT_KEYBOARD:
			kbd_event_t * keyevent = (kbd_event_t *) event;
			printf(u" - Keycode: 0x%x\n", keyevent->keycode);
			printf(u" - Pressed: %d\n", keyevent->pressed);
			printf(u" - Held:\n");
			printf(u"    - sysrq: %d\n", keyevent->held->sysrq);
			printf(u"    - lctrl: %d\n", keyevent->held->lctrl);
			printf(u"    - rctrl: %d\n", keyevent->held->rctrl);
			printf(u"    - super: %d\n", keyevent->held->super);
			printf(u"    - meta: %d\n", keyevent->held->meta);
			printf(u"    - lalt: %d\n", keyevent->held->lalt);
			printf(u"    - ralt: %d\n", keyevent->held->ralt);
			printf(u"    - lshift: %d\n", keyevent->held->lshift);
			printf(u"    - rshift: %d\n", keyevent->held->rshift);
			printf(u"    - caps: %d\n", keyevent->held->caps);
			printf(u"    - metalock: %d\n", keyevent->held->metalock);
	}
}