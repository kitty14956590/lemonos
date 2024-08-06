#pragma once

#include <input.h>
#include <stdint.h>
#include <asm.h>
#include <input.h>
#include <linked.h>

typedef int (* kill_callback_t)(int signal);

typedef struct {
	uint16_t * name;
	uint64_t pid;
	void * stack; // not used when swtiching (esp is below), for freeing when done
	uintptr_t stack_size;
	uint32_t ebp, esp, eip;
	int killed;
	int system; // is system process
	// for cleaning up after lazy processes
	linked_t * windows; // windows created
	linked_t * allocs; // tracks allocations made
	input_callback_t recv_event;
	input_callback_t recv_global_event;
	kill_callback_t kill; // called when you tell a process to kill (not called for signal 9)
} process_t;

extern int multitasking_done;
extern process_t * current_process;
extern linked_t * procs;

process_t * create_process(uint16_t * name, void * eip);
void create_event_zombie(uint16_t * name, input_callback_t handler);
void switch_task();
void multitasking_init();
void * multitasking_alloc_stack();
int multitasking_stack_overflow(void * stack, uintptr_t size, uintptr_t esp);
int multitasking_free_stack(void * stack, uintptr_t size);
int kill(uint64_t pid, int signal);
void dead_process();

void asm_task_switch(uint32_t eip, uint32_t ebp, uint32_t esp); // not really in multitasking.c
uint32_t geteip();