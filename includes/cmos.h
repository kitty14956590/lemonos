#pragma once

#include <stdint.h>

uint8_t cmos_read_register(uint16_t reg);
void cmos_write_register(uint16_t reg, uint8_t value);