#pragma once

#include <stdint.h>

enum CHARACTER_TYPES {
	FONT_LEGACY,
	FONT_BLANK,
	FONT_COMBINING,
	FONT_TRUECOLOUR = 16,
};

extern int legacy_colour[16];

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

typedef void (* font_drawer_t)(uint32_t * fb, uint32_t chr, uint32_t colour, uint32_t position);

void gfx_init();
int txt_string_draw(uint16_t * string, int x, int y, uint32_t colour, rect_2d_t * rect);
