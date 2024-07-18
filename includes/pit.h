#pragma once

#include <asm.h>
#include <stdint.h>

uint32_t pit_callback(registers_t regs);
void pit_init(uint32_t freq);