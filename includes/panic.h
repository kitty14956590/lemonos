#pragma once

#include <stdint.h>

void panic();
uint32_t panic_irq(registers_t regs);