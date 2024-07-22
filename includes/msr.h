#pragma once

#include <stdint.h>

void cpu_read_msr(uint32_t ecx, uint32_t * eax, uint32_t * edx);
void cpu_write_msr(uint32_t ecx, uint32_t eax, uint32_t edx);