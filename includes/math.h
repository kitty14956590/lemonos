#pragma once

#include <stdint.h>

typedef struct vect_2d {
	int x;
	int y;
} vect_2d_t;

uint32_t round32(uint32_t x, uint32_t y);
