#pragma once

#include <stdint.h>

enum CHARACTER_TYPES {
	FONT_LEGACY,
	FONT_BLANK,
	FONT_COMBINING,
	FONT_TRUECOLOUR = 16,
};

int legacy_colour[16] = {
	0x000000,
	0x0000aa,
	0x00aa00,
	0x00aaaa,
	0xaa0000,
	0xaa00aa,
	0xaa5500,
	0xaaaaaa,
	0x555555,
	0x5555ff,
	0x55ff55,
	0x55ffff,
	0xff5555,
	0xff55ff,
	0xffff55,
	0xffffff
};

typedef struct size_2d {
	int width;
	int height;
} size_2d_t;

typedef struct vect_2d {
	int x;
	int y;
} vect_2d_t;

typedef struct _rect_2d {
	int x;
	int y;
	int id;
	vect_2d_t cursor;
	uint32_t * fb;
	size_2d_t size;
} rect_2d_t;

extern rect_2d_t root_window;
