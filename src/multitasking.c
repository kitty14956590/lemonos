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

size_t stack_size = 2048;

void create_process(uint16_t * name, void * eip) {
	process_t * process = malloc(sizeof(process_t) + 8);
	size_t len = ustrlen(name);
	memset(process, 0, sizeof(process_t));
	process->name = malloc(len);
	process->pid = pid_top++;
	process->stack = multitasking_alloc_stack(stack_size);
	process->esp = (uint32_t) process->stack;
	process->ebp = process->esp;
	process->eip = (uintptr_t) eip;
	process->killed = 0;
	memcpy(process->name, name, len);
	linked_add(procs, process);
	proc_count++;
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
	process->killed = 1;
	free(process->stack - process->stack_size);
	free(process->name);
	free(process);
	linked_delete(node);
	proc_count--;
	switch_task();
	enable_interrupts();
}
void switch_task() {
	if (!multitasking_done) {
		return;
	}

	uint32_t esp, ebp, eip;
	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp));
	eip = geteip();
	if (eip == 0x1234) {
		return;
	}
	current_process->eip = eip;
	current_process->esp = esp;
	current_process->ebp = ebp;
	last_process = current_process;
	multitasking_inc_taskp();
	current_process = (process_t *) linked_get(procs, taskp)->p;
	while (current_process->killed == 1) {
		multitasking_inc_taskp();
		current_process = (process_t *) linked_get(procs, taskp)->p;
	}
	eip = current_process->eip;
	esp = current_process->esp;
	ebp = current_process->ebp;
	asm_task_switch(eip, ebp, esp);
}

// testing if multitasking is working (remove me later)
void test_process() {
	int i = 0;
	cprintf(7, u"Test process 1 started\n");
	while (1) {
		i++;
		if (i > 1000) {
			i = 0;
			cprintf(7, u"Process 1: tick count %d, fps %d\n", *ticksp, fps);
		}
		asm volatile ("hlt");
	}
}

void test_process2() {
	int i = 0;
	cprintf(7, u"Test process 2 started\n");
	while (1) {
		i++;
		if (i > 1000) {
			i = 0;
			cprintf(7, u"Process 2: tick count %d, fps %d\n", *ticksp, fps);
		}
		asm volatile ("hlt");
	}
}

void multitasking_init() {
	// complete trash, just for testing
	current_process = malloc(sizeof(process_t) + 8);
	memset(current_process, 0, sizeof(process_t));
	current_process->name = malloc(16);
	current_process->pid = 0;
	current_process->stack = 0;
	current_process->killed = 0;
	pid_top++;
	proc_count++;
	procs = linked_add(procs, current_process);

	create_process(u"test", test_process);
	create_process(u"test2", test_process2);
	multitasking_done = 1;
}

void * multitasking_alloc_stack(uintptr_t size) {
	return malloc(size) + size - 8;
}

int multitasking_free_stack(void * stack, uintptr_t size) {
	return free(stack - size + 8);
}