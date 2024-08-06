#include <graphics.h>
#include <input.h>
#include <multitasking.h>
#include <memory.h>
#include <linked.h>
#include <util.h>
#include <pit.h>
#include <log.h>
#include <string.h>
#include <graphics.h>
#include <stdio.h>

linked_t * procs;
process_t * current_process;
process_t * last_process;
uint32_t eips, ebps, esps;
uint64_t proc_count = 0;
uint64_t taskp = 0;
uint64_t pid_top = 0;
int multitasking_done = 0;

size_t stack_size = 64000;

// wow ! so complex !
void multitasking_default_event_handler(event_t * event) {}
int multitasking_default_kill_handler(int signal) {
	return 0;
}

process_t * create_process(uint16_t * name, void * eip) {
	process_t * process = malloc(sizeof(process_t));
	size_t len = ustrlen(name);
	memset(process, 0, sizeof(process_t));
	process->name = malloc((len * 2) + 2);
	process->pid = pid_top++;
	process->stack = multitasking_alloc_stack(stack_size);
	process->stack_size = stack_size;
	process->esp = (uint32_t) process->stack;
	process->ebp = process->esp;
	process->eip = (uintptr_t) eip;
	process->killed = 0;
	process->system = 0;
	process->windows = (void *) 0;
	process->allocs = (void *) 0;
	process->recv_event = multitasking_default_event_handler;
	process->recv_global_event = multitasking_default_event_handler;
	process->kill = multitasking_default_kill_handler;
	memcpy(process->name, name, (len * 2) + 2);
	disable_interrupts();
	linked_add(procs, process);
	proc_count++;
	enable_interrupts();
	return process;
}

int multitasking_destroy_window(linked_t * node, void * p) {
	window_t * window = node->p;
	gfx_cleanup_window(window);
}

int multitasking_destroy_alloc(linked_t * node, void * p) {
	free(node->p);
}

void cleanup_process(process_t * process) {
	// cleanup everything this process did after its been killed
	free(process->name);
	multitasking_free_stack(process->stack, process->stack_size);
	linked_chop_down(process->windows, multitasking_destroy_window, 0);
	linked_chop_down(process->allocs, multitasking_destroy_alloc, 0);
	free(process);
}

// create dead process that listens for events
void create_event_zombie(uint16_t * name, input_callback_t handler) {
	process_t * process = create_process(name, (void *) 0xdeaddead);
	process->killed = 1;
	process->recv_global_event = handler;
	process->ebp = 0xdeaddead;
	process->esp = 0xdeaddead;
	multitasking_free_stack(process->stack, process->stack_size);
	process->stack = (void *) 0xdeaddead;
	process->stack_size = 0xdeaddead;
}

void multitasking_inc_taskp() {
	taskp++;
	if (taskp >= proc_count) {
		taskp = 0;
	}
}

int multitasking_kill_callback(linked_t * node, void * p) {
	process_t * process = node->p;
	uint64_t * pid = p;
	if (process->pid == *pid) {
		return 1;
	}
	return 0;
}

int ppause(process_t * proc, int paused) {
	proc->killed = paused;
}

int pause(uint64_t pid, int paused) {
	linked_t * node = linked_find(procs, multitasking_kill_callback, &pid);
	process_t * process;
	if (!node) {
		return -1;
	}
	return ppause(node->p, paused);
}

int kill(uint64_t pid, int signal) {
	disable_interrupts();
	linked_t * node = linked_find(procs, multitasking_kill_callback, &pid);
	process_t * process;
	if (!node) {
		return -1;
	}
	if (signal == 0) {
		return 0;
	}
	taskp = 0;
	process = (process_t *) node->p;

	// dont allow process to ignore signal 9, 18, or 19, unless it is system process
	if ((signal != 9 && signal != 18 && signal != 19) || process->system) {
		if (process->kill(signal) == -1) {
			return -1;
		}
	}
	if (signal == 18 || signal == 19) {
		pause(pid, signal == 19);
		return 0;
	}
	process->killed = 1;
	cleanup_process(process);
	linked_delete(node);
	proc_count--;
	switch_task();
	enable_interrupts();
}
void switch_task() {
	if (!multitasking_done || proc_count == 1) {
		return;
	}

	uint32_t esp, ebp, eip;
	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp));
	eip = geteip();
	if (eip == 0) {
		return;
	}
	current_process->eip = eip;
	current_process->esp = esp;
	current_process->ebp = ebp;
	last_process = current_process;
	multitasking_inc_taskp();
	current_process = (process_t *) linked_get(procs, taskp)->p;
	while (current_process->killed) {
		multitasking_inc_taskp();
		current_process = (process_t *) linked_get(procs, taskp)->p;
	}
	// stacked overflow :c
	// this process may have corrupted some memory in the process but we ball
	if (multitasking_stack_overflow(current_process->stack, current_process->stack_size, current_process->esp)) {
		kill(current_process->pid, 9);
		return;
	}
	eip = current_process->eip;
	esp = current_process->esp;
	ebp = current_process->ebp;
	asm_task_switch(eip, ebp, esp);
}

void multitasking_init() {
	// complete trash, just for testing
	current_process = malloc(sizeof(process_t));
	memset(current_process, 0, sizeof(process_t));
	current_process->name = malloc(10);
	current_process->system = 1;
	current_process->killed = 0;
	current_process->recv_event = multitasking_default_event_handler;
	current_process->recv_global_event = multitasking_default_event_handler;
	current_process->kill = multitasking_default_kill_handler;
	memcpy(current_process->name, u"init", 10);
	pid_top++;
	proc_count++;
	procs = linked_add(procs, current_process);
	multitasking_done = 1;
}

void * multitasking_alloc_stack(uintptr_t size) {
	return malloc(size) + size;
}

int multitasking_free_stack(void * stack, uintptr_t size) {
	return free(stack - size);
}

// check for overflow
int multitasking_stack_overflow(void * stack, uintptr_t size, uintptr_t esp) {
	return ((uintptr_t) (stack - size)) > esp;
}