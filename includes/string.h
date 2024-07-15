#pragma once

#include <stdint.h>

int ustrlen(uint16_t * string);
uint32_t * memcpy32(uint32_t * dest, uint32_t * src, size_t length);
uint32_t * memset32(uint32_t * dest, uint32_t val, size_t length);
uint16_t * memset16(uint16_t * dest, uint16_t val, size_t length);
void ulldtoustr(uint64_t val, uint16_t * buf, int base);
void lldtoustr(int64_t val, uint16_t * buf, int base);
