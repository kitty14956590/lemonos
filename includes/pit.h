#pragma once

#include <asm.h>
#include <stdint.h>

extern volatile uint64_t ticks;
extern uint64_t * ticksp;

uint32_t pit_callback(registers_t regs);
void pit_init(uint32_t freq);