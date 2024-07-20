#pragma once

#include <stdint.h>
#include <asm.h>

typedef struct {
	uint16_t * name;
	uint64_t pid;
	void * stack; // not used when swtiching (esp is below), for freeing when done
	uintptr_t stack_size;
	uint32_t ebp, esp, eip;
	int killed;
	int system; // is system process
} process_t;

extern int multitasking_done;

void switch_task();
void multitasking_init();
void * multitasking_alloc_stack();
int multitasking_free_stack(void * stack, uintptr_t size);
int kill(uint64_t pid, int signal);
void dead_process();

void asm_task_switch(uint32_t eip, uint32_t ebp, uint32_t esp); // not really in multitasking.c
uint32_t geteip();