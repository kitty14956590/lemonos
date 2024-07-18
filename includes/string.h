#pragma once

#include <stdint.h>

int ustrlen(uint16_t * string);

void * memcpy(void * dest, void * src, size_t length);
uint32_t * memcpy32(uint32_t * dest, uint32_t * src, size_t length);

uint32_t * memset32(uint32_t * dest, uint32_t val, size_t length);
uint16_t * memset16(uint16_t * dest, uint16_t val, size_t length);
void * memset(void * dest, int val, size_t length);

int memcmp(const void * str1, const void * str2, size_t count);

void ulldtoustr(uint64_t val, uint16_t * buf, int base);
void lldtoustr(int64_t val, uint16_t * buf, int base);

uint16_t * ftoustr(double f, uint16_t * buf, int precision);
char * ftoa(double f, char * buf, int precision);