#pragma once

#include <stdint.h>

int ustrlen(uint16_t * string);
uint32_t * memcpy32(uint32_t * dest, uint32_t * src, size_t length);
uint32_t * memset32(uint32_t * dest, uint32_t val, size_t length);
