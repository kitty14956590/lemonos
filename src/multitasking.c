#include <multitasking.h>
#include <memory.h>
#include <linked.h>
#include <util.h>
#include <pit.h>
#include <log.h>
#include <string.h>
#include <stdio.h>

linked_t * procs;
process_t * current_process;
uint32_t eips, ebps, esps;
uint64_t proc_count = 0;
uint64_t taskp = 0;
int multitasking_done = 0;

extern uint32_t stack2_top;

void multitasking_inc_taskp() {
	taskp++;
	if (taskp == proc_count) {
		taskp = 0;
	}
}

void save_registers();
void load_registers();

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
	multitasking_inc_taskp();
	current_process = (process_t *) linked_get(procs, taskp)->p;
	eip = current_process->eip;
	esp = current_process->esp;
	ebp = current_process->ebp;
	asm_task_switch(eip, ebp, esp);
}

/*
void switch_task() {
	uint32_t eip, ebp, esp;
	asm volatile ("cli");
	save_registers();
	eip = geteip();
	if (current_process->tswitch) {
		load_registers();
		asm volatile ("sti");
		current_process->tswitch = 0;
		return;
	}
	current_process->tswitch = 1;
	multitasking_inc_taskp();
	current_process->eip = eip;
	current_process = (process_t *) (linked_get(procs, taskp)->p);
	eips = current_process->eip;
	ebps = current_process->ebp;
	esps = current_process->esp;
	asm volatile("mov %0, %%esp; mov %1, %%ebp; mov %2, %%eax; sti; jmp *%%eax" :: "r"(esps), "r"(ebps), "r"(eips));
}
*/

// testing if multitasking is working (remove me later)
void test_process() {
	uint64_t mytick = 0;
	int i = 0;
	cprintf(7, u"Test process 1 started\n");
	while (1) {
		if (mytick < *ticksp) {
			mytick = *ticksp;
			i++;
		}
		asm volatile("nop");
		if (i > 60) {
			i = 0;
			cprintf(7, u"Process 1: tick count %d\n", *ticksp);
		}
	}
}

void test_process2() {
	uint64_t mytick = 0;
	int i = 0;
	cprintf(7, u"Test process 2 started\n");
	while (1) {
		if (mytick < *ticksp) {
			mytick = *ticksp;
			i++;
		}
		asm volatile("nop");
		if (i > 60) {
			i = 0;
			cprintf(7, u"Process 2: tick count %d\n", *ticksp);
		}
	}
}

void multitasking_init() {
	// complete trash, just for testing
	process_t * test_proc;
	process_t * test_proc2;
	current_process = malloc(sizeof(process_t) + 8);
	memset(current_process, 0, sizeof(process_t));
	current_process->name = malloc(16);
	current_process->pid = 0;
	current_process->stack = 0;
	current_process->tswitch = 0;

	test_proc = malloc(sizeof(process_t) + 8);
	memset(test_proc, 0, sizeof(process_t));
	test_proc->name = malloc(16);
	test_proc->pid = 0;
	test_proc->stack = multitasking_alloc_stack(8000000);
	test_proc->esp = (uint32_t) test_proc->stack;
	test_proc->ebp = test_proc->esp;
	test_proc->eip = (uint32_t) test_process;
	test_proc->tswitch = 0;

	test_proc2 = malloc(sizeof(process_t) + 8);
	memset(test_proc2, 0, sizeof(process_t));
	test_proc2->name = malloc(16);
	test_proc2->pid = 0;
	test_proc2->stack = multitasking_alloc_stack(8000000);
	test_proc2->esp = (uint32_t) test_proc2->stack;
	test_proc2->ebp = test_proc2->esp;
	test_proc2->eip = (uint32_t) test_process2;
	test_proc2->tswitch = 0;

	procs = linked_add(procs, current_process);
	linked_add(procs, test_proc);
	linked_add(procs, test_proc2);
	proc_count = 3;
	multitasking_done = 1;
}

void * multitasking_alloc_stack(uintptr_t size) {
	return malloc(size) + size - 8;
}

int multitasking_free_stack(void * stack, uintptr_t size) {
	return free(stack - size + 8);
}