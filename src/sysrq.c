#include <input.h>
#include <multitasking.h>
#include <util.h>
#include <memory.h>
#include <pit.h>
#include <stdio.h>
#include <keyboard.h>

// system request key

int sysrq_proc_callback(linked_t * node, void * p) {
	process_t * process = node->p;
	iprintf(0xffffffff, u"PID %d : ", process->pid);
	iprintf(0xffffffff, u"%s %s%s\n", process->name, process->system ? u"(System Process) " : u"", process->killed ? u"(Killed)" : u"");
	iprintf(0xffffffff, u"STACK=%r EBP=%r ESP=%r EIP=%r\n", process->stack, process->ebp, process->esp, process->eip);
}

void sysrq_handle_event(event_t * event) {
	if (event->type != EVENT_KEYBOARD) {
		return;
	}
	kbd_event_t * keyevent = (kbd_event_t *) event;
	keyboard_held_t * held = keyevent->held;
	static int do_sysrq; 

	if (held->super && keyevent->keycode == 0x56) {
		do_sysrq = 1;
		return;
	}

	if (keyevent->keycode == 0x2a || keyevent->keycode == 0x36) {
		return;
	}

	if (!held->super || !do_sysrq || keyevent->pressed == 0) {
		do_sysrq = 0;
		return;
	}
	do_sysrq = 0;
	uint16_t chr = event_to_ascii_char(keyevent);
	if (chr == u'\0') {
		return;
	}
	switch (chr) {
		case u'o':
			shutdown();
			halt();
			return;

		case u'b':
			reboot();
			halt();
			return;

		case u'p':
			pit_dump_regs = 1;
			return;

		case u'm':
			iprintf(0xffffffff, u"KERN : START=%r END=%r LEN=%u\n", &_kernel_start, &_kernel_end, &_kernel_size);
			iprintf(0xffffffff, u"HEAP : START=%r END=%r LEN=%u\n", heap, heap_end, heap_length);
			iprintf(0xffffffff, u"BLOCKS : \n");
			memory_block_t * current_block = (memory_block_t *) heap;
			void * current = heap;
			while (current_block->state != MEMORY_NON_EXISTENT) {
				iprintf(0xffffffff, u" - (memory %u @ %r%s)\n", current_block->size, current, (current_block->state == MEMORY_FREE) ? u" free" : u"");
				current += current_block->size + sizeof(memory_block_t);
				current_block = (memory_block_t *) current;
				if (current > heap_end) {
					return;
				}
			}
			return;

		case u't':
			linked_iterate(procs, sysrq_proc_callback, 0);
			return;

		case u'e':
			// e for tEsting malloc
			void * alloc1 = malloc(8);
			void * alloc2 = malloc(8);
			void * alloc3 = malloc(128);
			free(alloc1);
			free(alloc2);
			malloc(24);
			return;

		case u'y':
			// y for Your arrow key moves the cursor now
			keyboard_mouse = 1;
			return;

		case u'Y':
			keyboard_mouse = 0;
			return;

		case u'c':
			int * p;
			static int i;
			p = (int *) 0;
			*p = 0;
			i++;
			i / *p;
			return;
	}
	return;
}

void sysrq_init() {
	create_event_zombie(u"sysrqd", sysrq_handle_event);
}