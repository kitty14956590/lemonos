#pragma once

#include <stdint.h>

void parallel_outb(unsigned char data);
void parallel_outw(uint16_t data);
void parallel_outs(uint16_t * data);